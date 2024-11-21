#include "PhaseFieldFracture.h"

registerMooseObject("LynxApp", PhaseFieldFracture);


InputParameters
PhaseFieldFracture::validParams()
{
  InputParameters params = DerivativeMaterialInterface<Kernel>::validParams();

  // Parameters
  params.addRequiredParam<Real>("length_scale", "The length scale parameter.");

  // Option to set names of consumed properties
  params.addParam<MaterialPropertyName>("crack_driving_force_name", "crack_driving_force",
                                        "The name of the crack driving force property.");

  return params;
}


PhaseFieldFracture::PhaseFieldFracture(
  const InputParameters & params
)
  : DerivativeMaterialInterface<Kernel>(params),

  // Parameters
  _length_scale(getParam<Real>("length_scale")),

  // Consumed properties
  _crack_driving_force(getMaterialProperty<Real>("crack_driving_force_name")),
  _dcrack_driving_force_dphi(getMaterialPropertyDerivative<Real>("crack_driving_force_name", _var.name()))
{
}


Real
PhaseFieldFracture::computeQpResidual()
{
  return _test[_i][_qp] * (_u[_qp] / _length_scale + _crack_driving_force[_qp]) 
    + _length_scale * _grad_test[_i][_qp] * _grad_u[_qp];
}


Real
PhaseFieldFracture::computeQpJacobian()
{
  return (1. / _length_scale + _dcrack_driving_force_dphi[_qp]) * _test[_i][_qp] * _phi[_j][_qp]
    + _length_scale * _grad_test[_i][_qp] * _grad_phi[_j][_qp];
}