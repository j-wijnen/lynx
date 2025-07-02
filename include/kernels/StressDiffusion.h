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

#include "Kernel.h"

namespace lynx
{

/**
 * Kernel defining hydrostatic stress-dependent diffusion
 */
class StressDiffusion : public Kernel
{
public:

  static InputParameters validParams();

  StressDiffusion(const InputParameters & params);

protected:

  virtual Real computeQpResidual() override;

  virtual Real computeQpJacobian() override;

  // Coupled variables
  const VariableValue & _hydrostatic_stress;
  const VariableGradient & _grad_hydrostatic_stress;

  // Consumed properties
  const MaterialProperty<Real> & _diffusivity;

  // Parameters
  const Real _Vh;
  const Real _R;
  const Real _T;
};

} // end namespace