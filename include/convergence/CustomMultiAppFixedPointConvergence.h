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

#include "DefaultMultiAppFixedPointConvergence.h"

namespace lynx {

/**
 * A custom convergence class inheriting from DefaultMultiAppFixedPointConvergence.
 *
 * This class can be used to extend or modify the default behavior for
 * MultiApp fixed point convergence checks.
 */
class CustomMultiAppFixedPointConvergence : public DefaultMultiAppFixedPointConvergence
{
public:
  static InputParameters validParams();

  CustomMultiAppFixedPointConvergence(const InputParameters & params);

  /**
   * Implement custom convergence checking logic.
   */
  virtual MooseConvergenceStatus checkConvergence(unsigned int iter) override;
};

} // end namespace