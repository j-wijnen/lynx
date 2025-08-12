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

#pragma once

#include "HardeningBase.h"

namespace lynx
{

/**
 * Implements perfect plasticity (no hardening)
 */
template <bool is_ad>
class NoHardeningTempl : public HardeningBase<is_ad>
{
public:
  static InputParameters validParams();

  NoHardeningTempl(const InputParameters & params);

  virtual GenericReal<is_ad> getYieldStress(GenericReal<is_ad> plastic_multiplier) override;

  virtual GenericReal<is_ad> getDerivative(GenericReal<is_ad> plastic_multiplier) override;

protected:
  const Real _yield_stress;  
};

typedef NoHardeningTempl<false> NoHardening;
typedef NoHardeningTempl<true> ADNoHardening;

} // end namespace