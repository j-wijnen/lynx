#include "PhaseFieldFractureAT2.h"

registerMooseObject("LynxApp", PhaseFieldFractureAT2);


InputParameters
PhaseFieldFractureAT2::validParams()
{
  InputParameters params = DerivativeMaterialInterface<Kernel>::validParams();

  // Option to set names of consumed properties
  params.addParam<MaterialPropertyName>("length_scale", "length_scale", 
                                        "Crack regularization length scale");
  params.addParam<MaterialPropertyName>("fracture_toughness", "fracture_toughness", 
                                        "Critical energy release rate");
  params.addParam<MaterialPropertyName>("pff_driving_force", "pff_driving_force",
                                        "The name of the fracture driving force property.");

  return params;
}


PhaseFieldFractureAT2::PhaseFieldFractureAT2(
  const InputParameters & params
)
  : DerivativeMaterialInterface<Kernel>(params),

  // Consumed properties
  _length_scale(getMaterialProperty<Real>("length_scale")),
  _fracture_toughness(getMaterialProperty<Real>("fracture_toughness")),
  _pff_driving_force(getMaterialProperty<Real>("pff_driving_force")),
  _dpff_driving_force_du(getMaterialPropertyDerivative<Real>("pff_driving_force", _var.name()))
{
}

Real
PhaseFieldFractureAT2::computeQpResidual()
{
  return _test[_i][_qp] * _fracture_toughness[_qp] / _length_scale[_qp] * _u[_qp]
    + _grad_test[_i][_qp] * _fracture_toughness[_qp] * _length_scale[_qp] * _grad_u[_qp]
    - _test[_i][_qp] * _pff_driving_force[_qp];
}


Real
PhaseFieldFractureAT2::computeQpJacobian()
{
  return _test[_i][_qp] * _fracture_toughness[_qp] / _length_scale[_qp] * _phi[_j][_qp]
    + _grad_test[_i][_qp] * _fracture_toughness[_qp] * _length_scale[_qp] * _grad_phi[_j][_qp]
    - _test[_i][_qp] * _dpff_driving_force_du[_qp] * _phi[_j][_qp];
}