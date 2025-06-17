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

#include "DirichletBCBase.h"

namespace lynx
{

/*  Applies a linearly increasing radial displacement
 */

class FixedRadialDisplacementBC : public DirichletBCBase
{
public:
  static InputParameters validParams();

  FixedRadialDisplacementBC(const InputParameters & params);

protected:
  virtual Real computeQpValue() override;

  // Parameters
  const MooseEnum _component;
  const Real _value;
  const std::vector<Real> _origin;
  const Real _start_time;
  const Real _end_time;
};

} // end namespace