#include "FractureEnergyAT2Aux.h"

registerMooseObject("LynxApp", FractureEnergyAT2Aux);

InputParameters
FractureEnergyAT2Aux::validParams()
{
  InputParameters params = AuxKernel::validParams();
  params.addClassDescription("Compute the PF fracture EnergyAT2");

  params.addRequiredCoupledVar("damage_variable", "Coupled damage variable");
  params.addParam<MaterialPropertyName>("fracture_toughness", "fracture_toughness", "Critical EnergyAT2 release rate");
  params.addParam<MaterialPropertyName>("length_scale", "length_scale", "PFF length scale");

  return params;
}

FractureEnergyAT2Aux::FractureEnergyAT2Aux(const InputParameters & parameters)
  : AuxKernel(parameters),
   _damage(coupledValue("damage_variable")),
   _grad_damage(coupledGradient("damage_variable")),
   _fracture_toughness(getMaterialProperty<Real>("fracture_toughness")),
   _length_scale(getMaterialProperty<Real>("length_scale"))
{
}

Real
FractureEnergyAT2Aux::computeValue()
{
  return 0.5 * _fracture_toughness[_qp] / _length_scale[_qp] * (
    _damage[_qp]*_damage[_qp] 
    + _length_scale[_qp]*_length_scale[_qp] * _grad_damage[_qp]*_grad_damage[_qp]);
}
