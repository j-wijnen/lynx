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

#include "VariablePowerLawHardening.h"

namespace lynx
{

registerMooseObject("LynxApp", VariablePowerLawHardening);

InputParameters
VariablePowerLawHardening::validParams()
{
  InputParameters params = HardeningBase<false>::validParams();

  // Option to set names of consumed properties
  params.addParam<MaterialPropertyName>("initial_yield_stress", "initial_yield_stress", 
                                        "The name of the initial yield stress property");
  params.addParam<MaterialPropertyName>("youngs_modulus", "youngs_modulus", 
                                        "The name of the Young's modulus property");
  params.addParam<MaterialPropertyName>("hardening_exponent", "hardening_exponent", 
                                        "The name of the hardening exponent property.");
  
  return params;
}

VariablePowerLawHardening::VariablePowerLawHardening(const InputParameters & params)
  : HardeningBase<false>(params),
  _initial_yield_stress(getMaterialProperty<Real>("initial_yield_stress")),
  _youngs_modulus(getMaterialProperty<Real>("youngs_modulus")),
  _hardening_exponent(getMaterialProperty<Real>("hardening_exponent"))
{
}

Real
VariablePowerLawHardening::getYieldStress(Real plastic_multiplier)
{
  return _initial_yield_stress[*_qp] * std::pow(
    1. + _youngs_modulus[*_qp] / _initial_yield_stress[*_qp] * plastic_multiplier, 
    _hardening_exponent[*_qp]);
}

Real 
VariablePowerLawHardening::getDerivative(Real plastic_multiplier)
{
  return _youngs_modulus[*_qp] * _hardening_exponent[*_qp] * std::pow(
    1. + _youngs_modulus[*_qp] / _initial_yield_stress[*_qp] * plastic_multiplier,
     _hardening_exponent[*_qp] - 1.);
}

} // end namespace