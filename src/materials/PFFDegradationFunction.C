#include "PFFDegradationFunction.h"


registerMooseObject("LynxApp", PFFDegradationFunction);

InputParameters
PFFDegradationFunction::validParams()
{
  InputParameters params = DerivativeMaterialInterface<Material>::validParams();
  params.addClassDescription("Compute phase-field fracture degradation functions");

  // Coupled variable
  params.addRequiredCoupledVar("variable", "Name of the damage variable.");

  // Declared property names
  params.addParam<MaterialPropertyName>("pff_degradation_name", "pff_degradation",
                                        "The name of the material degradation property.");

  // Parameters
  params.addParam<Real>("epsilon", 1e-7, "Constant for numerical stability.");

  return params;
}

PFFDegradationFunction::PFFDegradationFunction(
    const InputParameters & parameters
)
  : DerivativeMaterialInterface<Material>(parameters),

  // Coupled damage variable
  _u_name(coupledName("variable")),
  _u(coupledValue("variable")),

  // Degradation function
  _g(declareProperty<Real>("pff_degradation_name")),
  _dg_du(declarePropertyDerivative<Real>("pff_degradation", _u_name)),
  _d2g_du2(declarePropertyDerivative<Real>("pff_degradation", _u_name, _u_name)),
  
  // Parameters
  _epsilon(getParam<Real>("epsilon"))
{
}

void
PFFDegradationFunction::computeQpProperties()
{
  Real u_term = (1.0 - _u[_qp]);
  Real e_term = (1.0 - _epsilon);

  _g[_qp] = u_term * u_term * e_term + _epsilon;
  _dg_du[_qp] = -2.0 * u_term * e_term;
  _d2g_du2[_qp] =  2.0 * e_term;
}
