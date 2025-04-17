#include "FractureEnergyAux.h"

registerMooseObject("LynxApp", FractureEnergyAux);

InputParameters
FractureEnergyAux::validParams()
{
  InputParameters params = AuxKernel::validParams();
  params.addClassDescription("Compute the PF fracture energy");

  params.addRequiredCoupledVar("damage_variable", "Coupled damage variable");
  params.addParam<MaterialPropertyName>("fracture_toughness", "fracture_toughness", 
    "Critical energy release rate");

  return params;
}

FractureEnergyAux::FractureEnergyAux(const InputParameters & parameters)
  : AuxKernel(parameters),
   _damage(coupledValue("damage_variable")),
   _fracture_toughness(getMaterialProperty<Real>("fracture_toughness"))
{
}

Real
FractureEnergyAux::computeValue()
{
  return _fracture_toughness[_qp] * _damage[_qp];
}
