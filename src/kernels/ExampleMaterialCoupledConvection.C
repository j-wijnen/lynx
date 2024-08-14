#include "ExampleMaterialCoupledConvection.h"

registerMooseObject("LynxApp", ExampleMaterialCoupledConvection);

InputParameters
ExampleMaterialCoupledConvection::validParams()
{
  InputParameters params = Kernel::validParams();
  params.addClassDescription("Convection example of tutorial");
  return params;
}

ExampleMaterialCoupledConvection::ExampleMaterialCoupledConvection(
  const InputParameters & params
)
  : Kernel(params),
   _grad_v(getMaterialProperty<RealGradient>("convection_velocity"))
{
}

Real
ExampleMaterialCoupledConvection::computeQpResidual()
{
  return _test[_i][_qp] * (_grad_v[_qp] * _grad_u[_qp]);
}

Real
ExampleMaterialCoupledConvection::computeQpJacobian()
{
  return _test[_i][_qp] * (_grad_v[_qp] * _grad_phi[_j][_qp]);
}