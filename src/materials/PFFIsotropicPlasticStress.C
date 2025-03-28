#include "PFFIsotropicPlasticStress.h"

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
  _beta(getParam<Real>("beta"))
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

  // Trial elastic strain
  _elastic_strain[_qp] = _mechanical_strain[_qp] - _plastic_strain_old[_qp];
  Real elastic_strain_vol = _elastic_strain[_qp].trace();
  RankTwoTensor elastic_strain_dev = _elastic_strain[_qp].deviatoric();

  // Trial stress
  Real stress_vol = g * _bulk_modulus * elastic_strain_vol;
  RankTwoTensor stress_dev = 2. * g * _shear_modulus * elastic_strain_dev;
  Real stress_eq = std::sqrt(1.5 * stress_dev.doubleContraction(stress_dev));

  // Elastic jacobian
  _Jacobian_mult[_qp] = g * _bulk_modulus * IdentityTwoTwo 
    + 2. * g * _shear_modulus * IdentityFourDev;

  // Obtain yield stress
  FunctionValue yield_stress = getYieldStress(_plastic_strain_eq_old[_qp]);

  // Check for yielding
  if(stress_eq > gp * yield_stress.value)
  {
    Real r, dr_dpeeq;
    Real dpeeq = 0.0;
    unsigned int iter = 0;

    // Return map
    while(true)
    {
      r = stress_eq - gp * yield_stress.value - 3. * g * _shear_modulus * dpeeq;

      // Check convergence
      if(abs(r) < _tolerance * gp * yield_stress.value)
        break;
      else if(iter > 50)
        mooseException("Plasticity not converging");

      dr_dpeeq = - gp * yield_stress.derivative - 3. * g * _shear_modulus;
      dpeeq -= r / dr_dpeeq; 
      ++iter;

      yield_stress = getYieldStress(_plastic_strain_eq_old[_qp] + dpeeq);
    }

    // Plastic strain direction
    RankTwoTensor N = std::sqrt(1.5) * stress_dev / stress_eq;

    // Update tangent (before updating stress_eq)
    _Jacobian_mult[_qp] += 6. * g*g * _shear_modulus*_shear_modulus * (
      - dpeeq / stress_eq * IdentityFourDev
      + (dpeeq / stress_eq - 1. / (3. * g * _shear_modulus + gp * yield_stress.derivative)) 
      * N.outerProduct(N)
    );

    // Update plastic strains
    _plastic_strain[_qp] = _plastic_strain_old[_qp] + std::sqrt(1.5) * dpeeq * N;
    _plastic_strain_eq[_qp] = _plastic_strain_eq_old[_qp] + dpeeq;

    // Update elastic strains
    _elastic_strain[_qp] = _mechanical_strain[_qp] - _plastic_strain[_qp];
    elastic_strain_dev = _elastic_strain[_qp].deviatoric();

    // Update deviatoric stress
    stress_eq = gp * yield_stress.value;
    stress_dev = std::sqrt(2./3.) * stress_eq * N;

    // Plastic dissipated energy
    _plastic_energy[_qp] = _plastic_energy_old[_qp] + yield_stress.value * dpeeq;
  }

  // Final stress
  _stress[_qp] = stress_vol * Identity + stress_dev;

  // Elastic strain energy
  _elastic_energy[_qp] = std::max(
    0.5 * _bulk_modulus * (elastic_strain_vol * elastic_strain_vol)
      + _shear_modulus * (elastic_strain_dev.doubleContraction(elastic_strain_dev)),
    _elastic_energy_old[_qp]
  ); 

  // Total energy contribution to fracture
  _pff_energy[_qp] = _elastic_energy[_qp] + _beta * _plastic_energy[_qp];
}