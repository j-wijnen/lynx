#include "ExampleCoupledNeumannBC.h"

registerMooseObject("LynxApp", ExampleCoupledNeumannBC);


InputParameters
ExampleCoupledNeumannBC::validParams()
{
  InputParameters params = IntegratedBC::validParams();

  params.addParam<Real>("alpha", 1.0, "Value multiplied by the coupled value");
  params.addRequiredCoupledVar("coupled_variable", "Value on the boundary");
  return params;
}


ExampleCoupledNeumannBC::ExampleCoupledNeumannBC(
  const InputParameters & params
)
  : IntegratedBC(params),
    _alpha(getParam<Real>("alpha")),
    _coupled_var(coupledValue("coupled_variable"))
{
}


Real
ExampleCoupledNeumannBC::computeQpResidual()
{
  return -_test[_i][_qp] * _alpha * _coupled_var[_qp];
}