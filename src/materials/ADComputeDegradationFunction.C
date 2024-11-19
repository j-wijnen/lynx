#include "ADComputeDegradationFunction.h"


registerMooseObject("SolidMechanicsApp", ADComputeDegradationFunction);

InputParameters
ADComputeDegradationFunction::validParams()
{
  InputParameters params = ADMaterial::validParams();
  params.addClassDescription("Compute phase-field fracture stress using elasticity for small strains");
  
  params.addRequiredCoupledVar("variable", "Name of the damage variable.");

  // Declared property names
  params.addParam<MaterialPropertyName>("degradation_name", "degradation",
                                        "The name of the material degradation property.");
  params.addParam<MaterialPropertyName>("ddegradationdphi_name", "ddegradationdphi",
                                        "The name of the derivative of the material degradation property.");

  // Parameters
  params.addParam<Real>("epsilon", 1e-7, "Constant for numerical stability.");

  return params;
}

ADComputeDegradationFunction::ADComputeDegradationFunction(
    const InputParameters & parameters
)
  : ADMaterial(parameters),

  _phi(adCoupledValue("variable")),

  _degradation(declareADProperty<Real>("degradation_name")),
  _ddegradationdphi(declareADProperty<Real>("ddegradationdphi_name")),

  _epsilon(getParam<Real>("epsilon"))
{
}

void
ADComputeDegradationFunction::computeQpProperties()
{
  _degradation[_qp] = (1.0  - _phi[_qp]) * (1.0 - _phi[_qp]) * (1.0 - _epsilon) + _epsilon;
  _ddegradationdphi[_qp] = -2.0 * (1.0 - _phi[_qp]) * (1.0 - _epsilon);
}
