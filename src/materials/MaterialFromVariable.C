#include "MaterialFromVariable.h"

registerMooseObject("SolidMechanicsApp", MaterialFromVariable);

InputParameters
MaterialFromVariable::validParams()
{
  InputParameters params = Material::validParams();
  params.addClassDescription("Transfers a variable to a material property");
  
 
  // Coupled variables
  params.addRequiredCoupledVar("variable", "Name of the damage variable.");

  // Consumed property names
  params.addRequiredParam<MaterialPropertyName>("property", 
    "The name of the material degradation property.");

  return params;
}

MaterialFromVariable::MaterialFromVariable(
    const InputParameters & parameters
)
  : Material(parameters),

  // Coupled variable
  _var(coupledValue("variable")),

  // Declared properties
  _prop(declareProperty<Real>(getParam<MaterialPropertyName>("property")))
{
}

void
MaterialFromVariable::computeQpProperties()
{
  _prop[_qp] = _var[_qp];
}
