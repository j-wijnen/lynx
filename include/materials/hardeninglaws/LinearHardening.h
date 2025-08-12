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
 * Implements linear hardening
 * sy = sy0 + h * lambda
 */
template <bool is_ad>
class LinearHardeningTempl : public HardeningBase<is_ad>
{
public:
  static InputParameters validParams();

  LinearHardeningTempl(const InputParameters & params);

  virtual GenericReal<is_ad> getYieldStress(GenericReal<is_ad> plastic_multiplier) override;

  virtual GenericReal<is_ad> getDerivative(GenericReal<is_ad> plastic_multiplier) override;

protected:
  const Real _initial_yield_stress;  
  const Real _hardening_modulus;  
};

typedef LinearHardeningTempl<false> LinearHardening;
typedef LinearHardeningTempl<true> ADLinearHardening;

} // end namespace