#include "IsotropicPlasticStress.h"
#include "ElasticityTensorTools.h"
#include "MooseEnum.h"
#include "MooseTensorUtils.h"

using ElasticityTensorTools::getIsotropicShearModulus;

registerMooseObject("LynxApp", IsotropicPlasticStress);

InputParameters
IsotropicPlasticStress::validParams()
{
  InputParameters params = ComputeStressBase::validParams();
  params.addClassDescription("This material computes the stress for small strain"
    "isotropic von Mises plasticity.");
    
  MooseEnum hardeningLaw("NONE LINEAR POWERLAW VARIABLE_POWERLAW", "VARIABLE_SWIFT", "NONE");
  params.addParam<MooseEnum>("hardening_law", hardeningLaw, "Strain formulation");
  params.addParam<Real>("initial_yield_stress", 0.0, "The initial yield stress of the material.");
  params.addParam<Real>("hardening_modulus", 0.0, "The hardenings modulus (linear hardening)");
  params.addParam<Real>("hardening_exponent", 0.0, "The hardening exponent (powerlaw hardening)");
  params.addParam<Real>("tolerance", 1e-8, "Tolerance of internal loop.");
  params.addParam<MaterialPropertyName>("initial_yield_stress_name", "initial_yield_stress", "Property name of yield stress");
  params.addParam<MaterialPropertyName>("hardening_modulus_name", "hardening_modulus", "Property name of hardening modulus");
  params.addParam<MaterialPropertyName>("hardening_exponent_name", "hardening_exponent", "Property name of hardening exponent");

  return params;
}

IsotropicPlasticStress::IsotropicPlasticStress(
    const InputParameters & parameters
)
  : ComputeStressBase(parameters),
    GuaranteeConsumer(this),

  // Consumed material properties
  _elasticity_tensor(getMaterialProperty<RankFourTensor>(_base_name + "elasticity_tensor")),

  // Declared material properties
  _plastic_strain(declareProperty<RankTwoTensor>(_base_name + "plastic_strain")),
  _plastic_multiplier(declareProperty<Real>(_base_name + "plastic_multiplier")),
  _yield_stress(declareProperty<Real>(_base_name + "yield_stress")),

  // Stateful properties
  _plastic_strain_old(getMaterialPropertyOld<RankTwoTensor>(_base_name + "plastic_strain")),
  _plastic_multiplier_old(getMaterialPropertyOld<Real>(_base_name + "plastic_multiplier")),

  // Yield parameters
  _hardening_law(getParam<MooseEnum>("hardening_law")),
  _initial_yield_stress(getParam<Real>("initial_yield_stress")),
  _hardening_modulus(getParam<Real>("hardening_modulus")),
  _hardening_exponent(getParam<Real>("hardening_exponent")),
  _tolerance(getParam<Real>("tolerance")),

  // Yield parameters for variable properties
  _initial_yield_stress_prop(getOptionalMaterialProperty<Real>("initial_yield_stress_name")),
  _hardening_modulus_prop(getOptionalMaterialProperty<Real>("hardening_modulus_name")),
  _hardening_exponent_prop(getOptionalMaterialProperty<Real>("hardening_exponent_name"))
{
  // Assign hardening law
  switch(_hardening_law)
  {
    case 0: // NONE
      if(!parameters.isParamSetByUser("initial_yield_stress"))
        mooseError("`yield_stress` is not set");
      _hardening = std::make_unique<LinearHardening>(_initial_yield_stress, 0.0);
      break;
    case 1: // LINEAR
      if(!parameters.isParamSetByUser("initial_yield_stress"))
        mooseError("`yield_stress` is not set");
      if(!parameters.isParamSetByUser("hardening_modulus"))
        mooseError("`hardening_modulus` is not set");
      _hardening = std::make_unique<LinearHardening>(_initial_yield_stress, _hardening_modulus);
      break;
    case 2: // POWERLAW
      if(!parameters.isParamSetByUser("initial_yield_stress"))
        mooseError("`yield_stress` is not set");
      if(!parameters.isParamSetByUser("hardening_modulus"))
        mooseError("`hardening_modulus` is not set");
      if(!parameters.isParamSetByUser("hardening_exponent"))
        mooseError("`hardening_exponent` is not set");
      _hardening = std::make_unique<PowerLawHardening>(_initial_yield_stress, _hardening_modulus, _hardening_exponent);
      break;
    case 3: // VARIABLE_POWERLAW
      _hardening = std::make_unique<VariablePowerLawHardening>(
        _initial_yield_stress_prop,  _hardening_modulus_prop, _hardening_exponent_prop, _qp);
      break;
    case 4: // VARIABLE_SWIFT
      _hardening = std::make_unique<VariableSwiftHardening>(
        _initial_yield_stress_prop,  _hardening_modulus_prop, _hardening_exponent_prop, _qp);
      break;
  }

}

void IsotropicPlasticStress::initialSetup()
{
  ComputeStressBase::initialSetup();

  // Check if the material is isotropic
  if(!hasGuaranteedMaterialProperty(_base_name + "elasticity_tensor", Guarantee::ISOTROPIC))
    mooseError("The elasticity tensor must be isotropic.");
 }

void
IsotropicPlasticStress::initQpStatefulProperties()
{
  ComputeStressBase::initQpStatefulProperties();

  _plastic_strain[_qp].zero();
  _plastic_multiplier[_qp] = 0.0;
}

void 
IsotropicPlasticStress::computeQpStress()
{
  Real shear_modulus = getIsotropicShearModulus(_elasticity_tensor[_qp]);

  // Elastic trial state
  _elastic_strain[_qp] = _mechanical_strain[_qp] - _plastic_strain_old[_qp];
  _stress[_qp] = _elasticity_tensor[_qp] * _elastic_strain[_qp];
  _Jacobian_mult[_qp] = _elasticity_tensor[_qp];

  RankTwoTensor stress_dev = _stress[_qp].deviatoric();
  Real stress_eq = _sqrt32 * std::sqrt(stress_dev.doubleContraction(stress_dev));

  // Check for yielding
  _yield_stress[_qp] = _hardening->getValue(_plastic_multiplier_old[_qp]);
  if(stress_eq > _yield_stress[_qp])
  {
    // Return map
    Real dplastic_mult = computeReturnMap(stress_eq);

    // Update strains, stress, jacobian
    RankTwoTensor N = _sqrt32 * stress_dev / stress_eq;

    _plastic_strain[_qp] = _plastic_strain_old[_qp] + _sqrt32 * dplastic_mult * N;
    _plastic_multiplier[_qp] = _plastic_multiplier_old[_qp] + dplastic_mult;
    _elastic_strain[_qp] = _mechanical_strain[_qp] - _plastic_strain[_qp];

    _stress[_qp] = _elasticity_tensor[_qp] * _elastic_strain[_qp];

    _Jacobian_mult[_qp] += 6. * shear_modulus*shear_modulus * (
      - dplastic_mult / stress_eq * IdentityFourDev
      + (dplastic_mult / stress_eq + 1. / computeReturnDerivative(dplastic_mult)) 
      * N.outerProduct(N));
  }
}

Real
IsotropicPlasticStress::computeReturnMap(Real trial_stress)
{
  Real dplastic_mult = 0.0;
  Real dplastic_mult_prev;
  Real r;
  int iter = 0;

  while(true)
  {
    _yield_stress[_qp] = _hardening->getValue(_plastic_multiplier_old[_qp] + dplastic_mult);
    r = computeReturnResidual(trial_stress, dplastic_mult);

    if (abs(r) < _tolerance * _yield_stress[_qp])
      return dplastic_mult;
    else if (iter > 30)
      mooseException("Plastic return map not converging");

    dplastic_mult_prev = dplastic_mult;
    dplastic_mult -= r / computeReturnDerivative(dplastic_mult); 

    // Prevent negative plastic multiplier
    if (dplastic_mult < 0.0)
      dplastic_mult = 0.5 * dplastic_mult_prev;

    ++iter;
  }
}

Real
IsotropicPlasticStress::computeReturnResidual(Real trial_stress, 
                                              Real dplastic_mult)
{
  return trial_stress - 3. * getIsotropicShearModulus(_elasticity_tensor[_qp]) * dplastic_mult
    - _yield_stress[_qp];
}

Real
IsotropicPlasticStress::computeReturnDerivative(Real dplastic_mult)
{
  return - 3. * getIsotropicShearModulus(_elasticity_tensor[_qp])
    - _hardening->getDerivative(_plastic_multiplier_old[_qp] + dplastic_mult);
}