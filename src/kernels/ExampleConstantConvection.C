#include "ExampleConstantConvection.h"

registerMooseObject("LynxApp", ExampleConstantConvection);

InputParameters
ExampleConstantConvection::validParams()
{
  InputParameters params = Kernel::validParams();
  params.addClassDescription("Convection example 2 of tutorial");
  params.addRequiredParam<RealVectorValue>("velocity", "Velocity vector");
  return params;
}

ExampleConstantConvection::ExampleConstantConvection(const InputParameters & params)
  : Kernel(params), _v(getParam<RealVectorValue>("velocity"))
{
}

Real
ExampleConstantConvection::computeQpResidual()
{
  return _test[_i][_qp] * (_v * _grad_u[_qp]);
}

Real
ExampleConstantConvection::computeQpJacobian()
{
  return _test[_i][_qp] * (_v * _grad_phi[_j][_qp]);
}