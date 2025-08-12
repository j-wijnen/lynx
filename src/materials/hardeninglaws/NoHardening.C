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

#include "NoHardening.h"

namespace lynx
{

registerMooseObject("LynxApp", NoHardening);
registerMooseObject("LynxApp", ADNoHardening);

template <bool is_ad>
InputParameters
NoHardeningTempl<is_ad>::validParams()
{
  InputParameters params = HardeningBase<is_ad>::validParams();

  // Option to set names of consumed properties
  params.addRequiredParam<Real>("yield_stress", "The yield stress");
  
  return params;
}

template <bool is_ad>
NoHardeningTempl<is_ad>::NoHardeningTempl(const InputParameters & params)
  : HardeningBase<is_ad>(params),
  _yield_stress(this->template getParam<Real>("yield_stress"))
{
}

template <bool is_ad>
GenericReal<is_ad>
NoHardeningTempl<is_ad>::getYieldStress(GenericReal<is_ad> plastic_multiplier)
{
  return _yield_stress;
}

template <bool is_ad>
GenericReal<is_ad>
NoHardeningTempl<is_ad>::getDerivative(GenericReal<is_ad> plastic_multiplier)
{
  return 0.0;
}

template class NoHardeningTempl<false>;
template class NoHardeningTempl<true>;

} // end namespace