//* This file is part of Lynx, 
//* an open-source application for the simulation  
//* of mechanics and multi-physics problems
//* https://github.com/j-wijnen/lynx
//*
//* Lynx is powered by the MOOSE Framework
//* https://www.mooseframework.org
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "MaterialFromVariable.h"

namespace lynx {

registerMooseObject("LynxApp", MaterialFromVariable);

InputParameters
MaterialFromVariable::validParams()
{
  InputParameters params = Material::validParams();
  params.addClassDescription("Transfers a variable to a material property");
  
  // Coupled variables
  params.addRequiredCoupledVar("coupled_variable", "Name of the damage variable.");

  // Consumed property names
  params.addRequiredParam<MaterialPropertyName>("property", 
    "The name of the material degradation property.");

  return params;
}

MaterialFromVariable::MaterialFromVariable(const InputParameters & parameters)
  : Material(parameters),

  // Coupled variable
  _var(coupledValue("coupled_variable")),

  // Declared properties
  _prop(declareProperty<Real>(getParam<MaterialPropertyName>("property")))
{
}

void 
MaterialFromVariable::initQpStatefulProperties()
{
  _prop[_qp] = _var[_qp];
}

void
MaterialFromVariable::computeQpProperties()
{
  _prop[_qp] = _var[_qp];
}

} // end namespace