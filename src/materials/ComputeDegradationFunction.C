#include "ComputeDegradationFunction.h"


registerMooseObject("SolidMechanicsApp", ComputeDegradationFunction);

InputParameters
ComputeDegradationFunction::validParams()
{
  InputParameters params = DerivativeMaterialInterface<Material>::validParams();
  params.addClassDescription("Compute phase-field fracture stress using elasticity for small strains");

  // Coupled variable
  params.addRequiredCoupledVar("variable", "Name of the damage variable.");

  // Declared property names
  params.addParam<MaterialPropertyName>("degradation_name", "degradation",
                                        "The name of the material degradation property.");

  // Parameters
  params.addParam<Real>("epsilon", 1e-7, "Constant for numerical stability.");

  return params;
}

ComputeDegradationFunction::ComputeDegradationFunction(
    const InputParameters & parameters
)
  : DerivativeMaterialInterface<Material>(parameters),

  _phi_name(coupledName("variable")),
  _phi(coupledValue("variable")),

  _degradation(declareProperty<Real>("degradation_name")),
  _ddegradationdphi(declarePropertyDerivative<Real>("degradation", _phi_name)),
  _d2degradationd2phi(declarePropertyDerivative<Real>("degradation", _phi_name, _phi_name)),

  _epsilon(getParam<Real>("epsilon"))
{
}

void
ComputeDegradationFunction::computeQpProperties()
{
  _degradation[_qp] = (1.0  - _phi[_qp]) * (1.0 - _phi[_qp]) * (1.0 - _epsilon) + _epsilon;
  _ddegradationdphi[_qp] = -2.0 * (1.0 - _phi[_qp]) * (1.0 - _epsilon);
  _d2degradationd2phi[_qp] =  2.0 * (1.0 - _epsilon);
}
