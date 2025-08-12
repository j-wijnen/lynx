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

#include "SwiftHardening.h"

namespace lynx
{

registerMooseObject("LynxApp", SwiftHardening);

InputParameters
SwiftHardening::validParams()
{
  InputParameters params = HardeningBase<false>::validParams();

  // Option to set names of consumed properties
  params.addRequiredParam<Real>("initial_yield_stress", "The initial yield stress");
  params.addRequiredParam<Real>("hardening_modulus", "Hardening modulus");
  params.addRequiredParam<Real>("hardening_exponent", "The hardening exponent");
  
  return params;
}

SwiftHardening::SwiftHardening(const InputParameters & params)
  : HardeningBase<false>(params),
  _initial_yield_stress(getParam<Real>("initial_yield_stress")),
  _hardening_modulus(getParam<Real>("hardening_modulus")),
  _hardening_exponent(getParam<Real>("hardening_exponent"))
{
}

Real
SwiftHardening::getYieldStress(Real plastic_multiplier)
{
  return _hardening_modulus * std::pow(
    std::pow(_initial_yield_stress/_hardening_modulus, 1./_hardening_exponent)
    + plastic_multiplier, _hardening_exponent);
}

Real 
SwiftHardening::getDerivative(Real plastic_multiplier)
{
  return _hardening_exponent * _hardening_modulus * std::pow(
    std::pow(_initial_yield_stress/_hardening_modulus, 1.0/_hardening_exponent)
    + plastic_multiplier, _hardening_exponent - 1.0);
}

} // end namespace