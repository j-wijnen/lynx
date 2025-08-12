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

#include "LinearHardening.h"

namespace lynx
{

registerMooseObject("LynxApp", LinearHardening);
registerMooseObject("LynxApp", ADLinearHardening);

template <bool is_ad>
InputParameters
LinearHardeningTempl<is_ad>::validParams()
{
  InputParameters params = HardeningBase<is_ad>::validParams();

  // Option to set names of consumed properties
  params.addRequiredParam<Real>("initial_yield_stress", "The yield stress");
  params.addRequiredParam<Real>("hardening_modulus", "The hardening modulus");
  
  return params;
}

template <bool is_ad>
LinearHardeningTempl<is_ad>::LinearHardeningTempl(const InputParameters & params)
  : HardeningBase<is_ad>(params),
  _initial_yield_stress(this->template getParam<Real>("initial_yield_stress")),
  _hardening_modulus(this->template getParam<Real>("hardening_modulus"))
{
}

template <bool is_ad>
GenericReal<is_ad>
LinearHardeningTempl<is_ad>::getYieldStress(GenericReal<is_ad> plastic_multiplier)
{
  return _initial_yield_stress + _hardening_modulus * plastic_multiplier;
}

template <bool is_ad>
GenericReal<is_ad> 
LinearHardeningTempl<is_ad>::getDerivative(GenericReal<is_ad> plastic_multiplier)
{
  return _hardening_modulus;
}

} // end namespace