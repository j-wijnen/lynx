#include "FractureEnergyAux.h"

registerMooseObject("LynxApp", FractureEnergyAux);

InputParameters
FractureEnergyAux::validParams()
{
  InputParameters params = AuxKernel::validParams();
  params.addClassDescription("Compute the PF fracture energy");

  params.addRequiredCoupledVar("damage_variable", "Coupled damage variable");
  params.addParam<MaterialPropertyName>("fracture_toughness", "fracture_toughness", "Critical energy release rate");
  params.addParam<MaterialPropertyName>("length_scale", "length_scale", "PFF length scale");

  return params;
}

FractureEnergyAux::FractureEnergyAux(const InputParameters & parameters)
  : AuxKernel(parameters),
   _damage(coupledValue("damage_variable")),
   _grad_damage(coupledGradient("damage_variable")),
   _fracture_toughness(getMaterialProperty<Real>("fracture_toughness")),
   _length_scale(getMaterialProperty<Real>("length_scale"))
{
}

Real
FractureEnergyAux::computeValue()
{
  return 0.5 * _fracture_toughness[_qp] / _length_scale[_qp] * (
    _damage[_qp]*_damage[_qp] 
    + _length_scale[_qp]*_length_scale[_qp] * _grad_damage[_qp]*_grad_damage[_qp]);
}
