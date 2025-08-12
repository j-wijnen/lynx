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
 * Implements Swift hardening:
 * sy = h * (eps0 + eps)^n
 * eps0 = (sy0/A)^(1/n)
 */

class SwiftHardening : public HardeningBase<false>
{
public:
  static InputParameters validParams();

  SwiftHardening(const InputParameters & params);

  virtual Real getYieldStress(Real plastic_multiplier) override;

  virtual Real getDerivative(Real plastic_multiplier) override;

protected:
  const Real _initial_yield_stress; 
  const Real _hardening_modulus; 
  const Real _hardening_exponent;
 };

} // end namespace