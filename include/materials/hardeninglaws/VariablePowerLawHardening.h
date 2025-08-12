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
 * Implements power law hardening
 * sy = sy0 * (1 + E/sy * lambda)^n
 */

class VariablePowerLawHardening : public HardeningBase<false>
{
public:
  static InputParameters validParams();

  VariablePowerLawHardening(const InputParameters & params);

  virtual Real getYieldStress(Real plastic_multiplier) override;

  virtual Real getDerivative(Real plastic_multiplier) override;

protected:
  // Consumed properties
  const MaterialProperty<Real> & _initial_yield_stress;
  const MaterialProperty<Real> & _youngs_modulus;
  const MaterialProperty<Real> & _hardening_exponent;
 };

} // end namespace