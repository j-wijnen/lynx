#include "ExampleCoupledDirichletBC.h"

registerMooseObject("LynxApp", ExampleCoupledDirichletBC);


InputParameters
ExampleCoupledDirichletBC::validParams()
{
  InputParameters params = NodalBC::validParams();

  params.addParam<Real>("alpha", 1.0, "Value multiplied by the coupled value");
  params.addRequiredCoupledVar("coupled_variable", "Value on the boundary");
  return params;
}


ExampleCoupledDirichletBC::ExampleCoupledDirichletBC(
  const InputParameters & params
)
  : NodalBC(params),
  _alpha(getParam<Real>("alpha")), 
  _coupled_var(coupledValue("coupled_variable"))
{
}


Real
ExampleCoupledDirichletBC::computeQpResidual()
{
  return _u[_qp] - (_alpha * _coupled_var[_qp]);
}