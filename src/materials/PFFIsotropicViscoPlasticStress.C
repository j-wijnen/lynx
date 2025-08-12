#include "PFFIsotropicViscoPlasticStress.h"
#include "MooseEnum.h"
#include "ElasticityTensorTools.h"

using ElasticityTensorTools::getIsotropicShearModulus;

namespace lynx
{

registerMooseObject("LynxApp", PFFIsotropicViscoPlasticStress);

InputParameters
PFFIsotropicViscoPlasticStress::validParams()
{
  InputParameters params = PFFIsotropicPlasticStress::validParams();
  params.addClassDescription("This material computes the stress for small strain"
    "isotropic von Mises plasticity.");

  params.addRequiredParam<Real>("viscosity", "The plastic viscocity of the material.");
  params.addParam<Real>("rate_sensitivity", 1.0, "The strain rate sensitivity.");

  MooseEnum viscosity_type("PERZYNA PERIC", "PERZYNA");
  params.addParam<MooseEnum>("viscosity_type", viscosity_type, "The type of viscous model to use.");

  return params;
}

PFFIsotropicViscoPlasticStress::PFFIsotropicViscoPlasticStress(const InputParameters & parameters)
  : PFFIsotropicPlasticStress(parameters),
  _viscosity(getParam<Real>("viscosity")),
  _rate_sensitivity(getParam<Real>("rate_sensitivity")),
  _viscosity_type(getParam<MooseEnum>("viscosity_type"))
{
  // Set the ViscoPlasticLaw
  if (!parameters.isParamSetByUser("rate_sensitivity"))
    _viscosity_law = std::make_unique<ViscoPlasticLaw>(_viscosity);
  else if (_viscosity_type == "PERZYNA")
    _viscosity_law = std::make_unique<PerzynaViscoPlasticLaw>(_viscosity, _rate_sensitivity);
  else if (_viscosity_type == "PERIC")
    _viscosity_law = std::make_unique<PericViscoPlasticLaw>(_viscosity, _rate_sensitivity);
}

Real  
PFFIsotropicViscoPlasticStress::computeReturnResidual(Real trial_stress,
                                                   Real dplastic_mult)
{
  Real g = _g ? _g[_qp] : 1.0;
  Real gp = _beta * g + 1. - _beta;

  return trial_stress 
    - 3. * g * getIsotropicShearModulus(_elasticity_tensor[_qp]) * dplastic_mult
    - gp * _viscosity_law->getStress(dplastic_mult, _dt, _yield_stress[_qp]);
}

Real 
PFFIsotropicViscoPlasticStress::computeReturnDerivative(Real dplastic_mult)
{
  Real g = _g ? _g[_qp] : 1.0;
  Real gp = _beta * g + 1. - _beta;

  Real yield_stress_derivative = _hardening_law->getDerivative(_plastic_multiplier_old[_qp] + dplastic_mult);
  return - 3. * g * getIsotropicShearModulus(_elasticity_tensor[_qp])
    - gp * _viscosity_law->getDerivative(dplastic_mult, _dt, _yield_stress[_qp], yield_stress_derivative);
}

} // end namespace