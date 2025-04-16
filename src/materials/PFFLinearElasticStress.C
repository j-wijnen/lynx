#include "PFFLinearElasticStress.h"
#include "ElasticityTensorTools.h"
#include "MooseTensorUtils.h"

using ElasticityTensorTools::getIsotropicShearModulus;
using ElasticityTensorTools::getIsotropicBulkModulus;

registerMooseObject("LynxApp", PFFLinearElasticStress);

InputParameters
PFFLinearElasticStress::validParams()
{
  InputParameters params = ComputeStressBase::validParams();
  params.addClassDescription("Compute the phase-field fracture stress using elasticity for small strains");

  // Consumed property names
  params.addParam<MaterialPropertyName>("pff_degradation_name", "pff_degradation",
                                        "The name of the material degradation property.");

  // Declared property names
  params.addParam<MaterialPropertyName>("pff_energy_name", "pff_energy",
                                        "The name of the energy driving fracture.");

  // Parameters  
  params.addParam<bool>("consistent_split", false, "Use split on stress consistent with energy.");

  return params;
}

PFFLinearElasticStress::PFFLinearElasticStress(
    const InputParameters & parameters
)
  : ComputeStressBase(parameters),
    GuaranteeConsumer(this),

  // Consumed properties
  _elasticity_tensor(getMaterialProperty<RankFourTensor>(_base_name + "elasticity_tensor")),
  _g(getOptionalMaterialProperty<Real>("pff_degradation_name")),

  // Declared properties
  _pff_energy(declareProperty<Real>("pff_energy_name")),

  // Stateful properties
  _pff_energy_old(getMaterialPropertyOld<Real>("pff_energy_name")),

  // Parameters
  _consistent_split(getParam<bool>("consistent_split"))
{
}

void PFFLinearElasticStress::initialSetup()
{
  ComputeStressBase::initialSetup();

  // Check if the material is isotropic
  if(!hasGuaranteedMaterialProperty(_base_name + "elasticity_tensor", Guarantee::ISOTROPIC))
    mooseError("The elasticity tensor must be isotropic.");
}

void
PFFLinearElasticStress::initQpStatefulProperties()
{
  _pff_energy[_qp] = 0.0;
}

void
PFFLinearElasticStress::computeQpStress()
{
  // Obtain degradation function
  Real g = _g ? _g[_qp] : 1.0;

  // Elasticity parameters
  Real shear_modulus = getIsotropicShearModulus(_elasticity_tensor[_qp]);
  Real bulk_modulus = getIsotropicBulkModulus(_elasticity_tensor[_qp]);

  // Elastic strains
  _elastic_strain[_qp] = _mechanical_strain[_qp];
  Real volumetric_strain = _elastic_strain[_qp].trace();
  RankTwoTensor deviatoric_strain = _elastic_strain[_qp].deviatoric();

  // Volumetric part
  if (!_consistent_split || volumetric_strain > 0.0)
  {
    _stress[_qp] = g * bulk_modulus * volumetric_strain * Identity;
    _Jacobian_mult[_qp] = g * bulk_modulus * IdentityTwoTwo;
  }
  else
  {
    _stress[_qp] = bulk_modulus * volumetric_strain * Identity;
    _Jacobian_mult[_qp] = bulk_modulus * IdentityTwoTwo;
  }

  // Deviatoric part
  _stress[_qp] += 2.0 * g * shear_modulus * deviatoric_strain;
  _Jacobian_mult[_qp] += 2.0 * g * shear_modulus * IdentityFourDev;
  
  // Energy
  _pff_energy[_qp] = shear_modulus * deviatoric_strain.doubleContraction(deviatoric_strain);

  if (volumetric_strain > 0.0)
    _pff_energy[_qp] += 0.5 * bulk_modulus * volumetric_strain * volumetric_strain;

  _pff_energy[_qp] = std::max(_pff_energy[_qp], _pff_energy_old[_qp]);
}
