#include "PFFIsotropicPlasticStress.h"
#include "ElasticityTensorTools.h"
#include "MooseTensorUtils.h"

using ElasticityTensorTools::getIsotropicShearModulus;
using ElasticityTensorTools::getIsotropicBulkModulus;
using lynx::MooseTensorUtils::Identity;
using lynx::MooseTensorUtils::IdentityFourSymDev;
using lynx::MooseTensorUtils::IdentityTwoTwo;

namespace lynx 
{

registerMooseObject("LynxApp", PFFIsotropicPlasticStress);

InputParameters
PFFIsotropicPlasticStress::validParams()
{
  InputParameters params = IsotropicPlasticStress::validParams();
  params.addClassDescription("This material computes the stress for small strain"
    "phase field fracture with isotropic von Mises plasticity.");

  // Consumed property names
  params.addParam<MaterialPropertyName>("pff_degradation_name", "pff_degradation",
    "The name of the material degradation property.");

  // Declared property names
  params.addParam<MaterialPropertyName>("pff_energy_name", "pff_energy",
    "The name of the energy driving fracture.");

  // parameters
  params.addParam<Real>("beta", 0.0, "Fraction of plastic dissipation contributing to damage.");
  params.addParam<bool>("energy_split", true, "Split energy in damaging and non-damaging hydrostatic parts.");
  params.addParam<bool>("consistent_split", false, "Use split on stress calculation (variational consistent).");
  
  return params;
}

PFFIsotropicPlasticStress::PFFIsotropicPlasticStress(
    const InputParameters & parameters
)
  : IsotropicPlasticStress(parameters),

  // Consumed properties
  _g(getOptionalMaterialProperty<Real>("pff_degradation_name")),

  // Declared material properties
  _pff_energy(declareProperty<Real>("pff_energy_name")),
  _elastic_energy(declareProperty<Real>("pff_elastic_energy")),
  _plastic_energy(declareProperty<Real>("pff_plastic_energy")),

  // Stateful material properties
  _elastic_energy_old(getMaterialPropertyOld<Real>("pff_elastic_energy")),
  _plastic_energy_old(getMaterialPropertyOld<Real>("pff_plastic_energy")),

  // Parameters
  _beta(getParam<Real>("beta")),
  _energy_split(getParam<bool>("energy_split")),
  _consistent_split(getParam<bool>("consistent_split"))
{
}

void
PFFIsotropicPlasticStress::initQpStatefulProperties()
{
  IsotropicPlasticStress::initQpStatefulProperties();

  _elastic_energy[_qp] = 0.0;
  _plastic_energy[_qp] = 0.0;
}

void 
PFFIsotropicPlasticStress::computeQpStress()
{
  // Obtain degradation functions
  Real g = _g ? _g[_qp] : 1.0;
  Real gp = _beta * g + 1. - _beta;

  // Elastic parameters
  Real bulk_modulus = getIsotropicBulkModulus(_elasticity_tensor[_qp]);
  Real shear_modulus = getIsotropicShearModulus(_elasticity_tensor[_qp]);

  // Trial elastic strain
  _elastic_strain[_qp] = _mechanical_strain[_qp] - _plastic_strain_old[_qp];
  Real volumetric_strain = _elastic_strain[_qp].trace();
  RankTwoTensor elastic_strain_dev = _elastic_strain[_qp].deviatoric();

  // Volumetric part
  Real stress_vol;
  if (!_energy_split || !_consistent_split || volumetric_strain > 0.0)
  {
    stress_vol = g * bulk_modulus * volumetric_strain;
    _Jacobian_mult[_qp] = g * bulk_modulus * IdentityTwoTwo;
  }
  else 
  {
    stress_vol = bulk_modulus * volumetric_strain;
    _Jacobian_mult[_qp] = bulk_modulus * IdentityTwoTwo;
  }

  // Elastic deviatoric part
  RankTwoTensor stress_dev = 2. * g * shear_modulus * elastic_strain_dev;
  Real stress_eq = _sqrt32 * std::sqrt(stress_dev.doubleContraction(stress_dev));

  _Jacobian_mult[_qp] += 2.0 * g * shear_modulus * IdentityFourSymDev;

  // Check for yielding
  _yield_stress[_qp] = _hardening_law->getYieldStress(_plastic_multiplier_old[_qp]);
  if(stress_eq > gp * _yield_stress[_qp])
  {
    // Return map
    Real dplastic_mult = computeReturnMap(stress_eq);
    
    // Update strains, stress, jacobian
    RankTwoTensor N = _sqrt32 * stress_dev / stress_eq;

    // Update relevant strains, stresses and jacobian
    _plastic_strain[_qp] = _plastic_strain_old[_qp] + _sqrt32 * dplastic_mult * N;
    _plastic_multiplier[_qp] = _plastic_multiplier_old[_qp] + dplastic_mult;
    _elastic_strain[_qp] = _mechanical_strain[_qp] - _plastic_strain[_qp];

    elastic_strain_dev = _elastic_strain[_qp].deviatoric();
    stress_dev = 2. * g * shear_modulus * elastic_strain_dev;

    _Jacobian_mult[_qp] += 6. * g*g * shear_modulus*shear_modulus * (
      - dplastic_mult / stress_eq * IdentityFourSymDev
      + (dplastic_mult / stress_eq + 1. / computeReturnDerivative(dplastic_mult)) 
      * N.outerProduct(N));

    // Plastic energy
    // More general version of $yield_stress * dplastic_mult$, (needed for viscoplasticity)
    _plastic_energy[_qp] = _plastic_energy_old[_qp] + stress_dev.doubleContraction(_sqrt32 * dplastic_mult * N) / gp;
  }
  // Final stress
  _stress[_qp] = stress_vol * Identity + stress_dev;

  // Elastic energy split (hydrostatic-deviatoric)
  _elastic_energy[_qp] = shear_modulus * (elastic_strain_dev.doubleContraction(elastic_strain_dev));
  if (!_energy_split || volumetric_strain > 0.0)
    _elastic_energy[_qp] += 0.5 * bulk_modulus * volumetric_strain * volumetric_strain;
  _elastic_energy[_qp] = std::max(_elastic_energy[_qp], _elastic_energy_old[_qp]);

  // Total energy contributing to damage
  _pff_energy[_qp] = _elastic_energy[_qp] + _beta * _plastic_energy[_qp];
}

Real
PFFIsotropicPlasticStress::computeReturnResidual(Real trial_stress, 
                                                 Real dplastic_mult)
{
  Real g = _g ? _g[_qp] : 1.0;
  Real gp = _beta * g + 1. - _beta;

  return trial_stress 
    - 3. * g * getIsotropicShearModulus(_elasticity_tensor[_qp]) * dplastic_mult
    - gp * _yield_stress[_qp];
}

Real
PFFIsotropicPlasticStress::computeReturnDerivative(Real dplastic_mult)
{
  Real g = _g ? _g[_qp] : 1.0;
  Real gp = _beta * g + 1. - _beta;

  return - 3. * g * getIsotropicShearModulus(_elasticity_tensor[_qp])
    - gp * _hardening_law->getDerivative(_plastic_multiplier_old[_qp] + dplastic_mult);
}

} // end namespace