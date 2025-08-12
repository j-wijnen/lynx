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

#include "PowerLawHardening.h"

namespace lynx
{

registerMooseObject("LynxApp", PowerLawHardening);
registerMooseObject("LynxApp", ADPowerLawHardening);

template <bool is_ad>
InputParameters
PowerLawHardeningTempl<is_ad>::validParams()
{
  InputParameters params = HardeningBase<is_ad>::validParams();

  // Option to set names of consumed properties
  params.addRequiredParam<Real>("initial_yield_stress", "The initial yield stress");
  params.addRequiredParam<Real>("youngs_modulus", "The youngs modulus");
  params.addRequiredParam<Real>("hardening_exponent", "The hardening exponent");
  
  return params;
}

template <bool is_ad>
PowerLawHardeningTempl<is_ad>::PowerLawHardeningTempl(const InputParameters & params)
  : HardeningBase<is_ad>(params),
  _initial_yield_stress(this->template getParam<Real>("initial_yield_stress")),
  _youngs_modulus(this->template getParam<Real>("youngs_modulus")),
  _hardening_exponent(this->template getParam<Real>("hardening_exponent"))
{
}

template <bool is_ad>
GenericReal<is_ad>
PowerLawHardeningTempl<is_ad>::getYieldStress(GenericReal<is_ad> plastic_multiplier)
{
  return _initial_yield_stress * std::pow(
    1. + _youngs_modulus / _initial_yield_stress * plastic_multiplier, 
    _hardening_exponent);
}

template <bool is_ad>
GenericReal<is_ad>
PowerLawHardeningTempl<is_ad>::getDerivative(GenericReal<is_ad> plastic_multiplier)
{
  return _youngs_modulus * _hardening_exponent * std::pow(
    1. + _youngs_modulus / _initial_yield_stress * plastic_multiplier,
     _hardening_exponent - 1.);
}

} // end namespace