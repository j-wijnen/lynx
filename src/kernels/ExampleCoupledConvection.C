#include "ExampleCoupledConvection.h"

registerMooseObject("LynxApp", ExampleCoupledConvection);

InputParameters
ExampleCoupledConvection::validParams()
{
  InputParameters params = Kernel::validParams();
  params.addClassDescription("Convection example 3 of tutorial");
  params.addRequiredCoupledVar("coupled_variable", "Coupled variable");
  return params;
}

ExampleCoupledConvection::ExampleCoupledConvection(const InputParameters & params)
  : Kernel(params), _grad_v(coupledGradient("coupled_variable"))
{
}

Real
ExampleCoupledConvection::computeQpResidual()
{
  return _test[_i][_qp] * (_grad_v[_qp] * _grad_u[_qp]);
}

Real
ExampleCoupledConvection::computeQpJacobian()
{
  return _test[_i][_qp] * (_grad_v[_qp] * _grad_phi[_j][_qp]);
}