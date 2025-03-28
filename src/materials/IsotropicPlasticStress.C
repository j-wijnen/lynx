#include "IsotropicPlasticStress.h"
#include "MooseEnum.h"
#include "MooseTensor.h"

registerMooseObject("LynxApp", IsotropicPlasticStress);

InputParameters
IsotropicPlasticStress::validParams()
{
  InputParameters params = ComputeStressBase::validParams();
  params.addClassDescription("This material computes the stress for small strain"
    "isotropic von Mises plasticity.");

  params.addParam<Real>("youngs_modulus", 0.0, "The Young's modulus of the material.");
  params.addParam<Real>("poissons_ratio", 0.0, "The Poisson ratio of the material.");
  params.addParam<Real>("bulk_modulus", 0.0, "The bulk modulus of the material.");
  params.addParam<Real>("shear_modulus", 0.0, "The shear modulus of the material.");

  MooseEnum hardeningLaw("NONE LINEAR POWERLAW", "NONE");
  params.addParam<MooseEnum>("hardening_law", hardeningLaw, "Strain formulation");
  params.addRequiredParam<Real>("yield_stress", "The initial yield stress of the material.");
  params.addParam<Real>("hardening_modulus", 0.0, "The hardenings modulus (linear hardening)");
  params.addParam<Real>("hardening_exponent", 0.0, "The hardening exponent (powerlaw hardening)");
  params.addParam<Real>("tolerance", 1e-10, "Tolerance of internal loop.");
  return params;
}

IsotropicPlasticStress::IsotropicPlasticStress(
    const InputParameters & parameters
)
  : ComputeStressBase(parameters),

  // Declared material properties
  _plastic_strain(declareProperty<RankTwoTensor>(_base_name + "plastic_strain")),
  _plastic_strain_eq(declareProperty<Real>(_base_name + "equivalent_plastic_strain")),

  // Stateful properties
  _plastic_strain_old(getMaterialPropertyOld<RankTwoTensor>(_base_name + "plastic_strain")),
  _plastic_strain_eq_old(getMaterialPropertyOld<Real>(_base_name + "equivalent_plastic_strain")),

  // Lame constants
  _youngs_modulus(getParam<Real>("youngs_modulus")),
  _poissons_ratio(getParam<Real>("poissons_ratio")),
  _bulk_modulus(getParam<Real>("bulk_modulus")),
  _shear_modulus(getParam<Real>("shear_modulus")),
  _isset_youngs_modulus(parameters.isParamSetByUser("youngs_modulus")),
  _isset_poissons_ratio(parameters.isParamSetByUser("poissons_ratio")),
  _isset_bulk_modulus(parameters.isParamSetByUser("bulk_modulus")),
  _isset_shear_modulus(parameters.isParamSetByUser("shear_modulus")),

  _hardening_law(getParam<MooseEnum>("hardening_law")),
  _yield_stress0(getParam<Real>("yield_stress")),
  _hardening_modulus(getParam<Real>("hardening_modulus")),
  _hardening_exponent(getParam<Real>("hardening_exponent")),
  _tolerance(getParam<Real>("tolerance"))
{
  computeLameConstants();

  // Check hardening parameters
  switch(_hardening_law)
  {
    case 0: // NONE
      break;
    case 1: // LINEAR
      if(!parameters.isParamSetByUser("hardening_modulus"))
        mooseError("`hardening_modulus` is not set");
      break;
    case 2: // POWERLAW
      if(!parameters.isParamSetByUser("hardening_exponent"))
        mooseError("`hardening_exponent` is not set");
      break;
  }
}

void
IsotropicPlasticStress::initQpStatefulProperties()
{
  ComputeStressBase::initQpStatefulProperties();

  _plastic_strain[_qp].zero();
  _plastic_strain_eq[_qp] = 0.0;
}

void 
IsotropicPlasticStress::computeQpStress()
{
  // Trial elastic strain
  _elastic_strain[_qp] = _mechanical_strain[_qp] - _plastic_strain_old[_qp];
  Real elastic_strain_vol = _elastic_strain[_qp].trace();
  RankTwoTensor elastic_strain_dev = _elastic_strain[_qp].deviatoric();

  // Trial stress
  Real stress_vol = _bulk_modulus * elastic_strain_vol;
  RankTwoTensor stress_dev = 2. * _shear_modulus * elastic_strain_dev;
  Real stress_eq = std::sqrt(1.5 * stress_dev.doubleContraction(stress_dev));

  // Elastic jacobian
  _Jacobian_mult[_qp] = _bulk_modulus * IdentityTwoTwo + 2. * _shear_modulus * IdentityFourDev;

  // Obtain yield stress
  FunctionValue yield_stress = getYieldStress(_plastic_strain_eq_old[_qp]);

  // Check for yielding
  if(stress_eq > yield_stress.value)
  {
    Real r, dr_dpeeq;
    Real dpeeq = 0.0;
    unsigned int iter = 0;

    // Return map
    while(true)
    {
      r = stress_eq - yield_stress.value - 3. * _shear_modulus * dpeeq;

      // Check convergence
      if (abs(r) < _tolerance * yield_stress.value)
        break;
      else if (iter > 20)
        mooseException("IsotropicPlasticStress: Plasticity not converging");

      dr_dpeeq = - yield_stress.derivative - 3. * _shear_modulus;
      dpeeq -= r / dr_dpeeq; 

      yield_stress = getYieldStress(_plastic_strain_eq_old[_qp] + dpeeq);

      ++iter;
    }

    // Plastic strain direction
    RankTwoTensor N = std::sqrt(1.5) * stress_dev / stress_eq;

    // Update strains
    _plastic_strain[_qp] = _plastic_strain_old[_qp] + std::sqrt(1.5) * dpeeq * N;
    _plastic_strain_eq[_qp] = _plastic_strain_eq_old[_qp] + dpeeq;
    _elastic_strain[_qp] = _mechanical_strain[_qp] - _plastic_strain[_qp];

    // Update tangent (before updating stress_eq)
    _Jacobian_mult[_qp] += 6. * _shear_modulus * _shear_modulus * (
      - dpeeq / stress_eq * IdentityFourDev
      + (dpeeq / stress_eq - 1. / (3. * _shear_modulus + yield_stress.derivative)) 
      * N.outerProduct(N)
    );

    // Update deviatoric stress
    stress_eq = yield_stress.value;
    stress_dev = std::sqrt(2./3.) * stress_eq * N;
  }

  _stress[_qp] = stress_vol * Identity + stress_dev;
}

void 
IsotropicPlasticStress::computeLameConstants()
{
  int nset = static_cast<int>(_isset_youngs_modulus) + static_cast<int>(_isset_poissons_ratio)
    +static_cast<int>(_isset_bulk_modulus) + static_cast<int>(_isset_shear_modulus);
  
  if(nset != 2)
    mooseError("Exactly two Lame constants have to be specified.");
  
  if(_isset_youngs_modulus && _isset_poissons_ratio)
  {
    _bulk_modulus = _youngs_modulus / (3. * (1. - 2. * _poissons_ratio));
    _shear_modulus = _youngs_modulus / (2. * (1. + _poissons_ratio));
  }
  else if(_isset_youngs_modulus && _isset_bulk_modulus)
  {
    _poissons_ratio = (3. * _bulk_modulus - _youngs_modulus) / (6. * _bulk_modulus);
    _shear_modulus = 3. * _bulk_modulus * _youngs_modulus / (9. * _bulk_modulus - _youngs_modulus);
  }
  else if(_isset_youngs_modulus && _isset_shear_modulus)
  {
    _poissons_ratio = _youngs_modulus / (2.*_shear_modulus) - 1.;
    _bulk_modulus = _youngs_modulus * _shear_modulus / (3. * (3. * _shear_modulus - _youngs_modulus));
  }
  else if(_isset_poissons_ratio && _isset_bulk_modulus)
  {
    _youngs_modulus = 3. * _bulk_modulus * (1. - 2. * _poissons_ratio);
    _shear_modulus = 3 * _bulk_modulus * (1. - 2. * _poissons_ratio) / (2. * (1. + _poissons_ratio));
  }
  else if(_isset_poissons_ratio && _isset_shear_modulus)
  {
    _youngs_modulus = 2. * _shear_modulus * (1. + _poissons_ratio);
    _bulk_modulus = 2. * _shear_modulus * (1. + _poissons_ratio) / (2. * (1. - 2. * _poissons_ratio));
  }
  else if(_isset_bulk_modulus && _isset_shear_modulus)
  {
    _youngs_modulus = 9. * _bulk_modulus * _shear_modulus / (3. * _bulk_modulus + _shear_modulus);
    _poissons_ratio = (3. * _bulk_modulus - 2. * _shear_modulus) / (2. * (3. * _bulk_modulus + _shear_modulus));
  }
}

FunctionValue
IsotropicPlasticStress::getYieldStress(Real plastic_strain_eq)
{
  FunctionValue yield_stress;

  switch(_hardening_law)
  {
    case 0 : // NONE
      yield_stress.value = _yield_stress0;
      yield_stress.derivative = 0.0;
      break;
    case 1: // LINEAR
      yield_stress.value =  _yield_stress0 + _hardening_modulus * plastic_strain_eq;
      yield_stress.derivative = _hardening_modulus;
      break;
    case 2: // POWERLAW
      Real term = 1. + _youngs_modulus / _yield_stress0 * plastic_strain_eq;
      yield_stress.value = _yield_stress0 * std::pow(term, _hardening_exponent);
      yield_stress.derivative = _youngs_modulus * _hardening_exponent 
        * std::pow(term, _hardening_exponent - 1.);
      break;
  }

  return yield_stress;
}