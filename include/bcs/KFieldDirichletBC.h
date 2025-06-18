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

/**  
 * Applies a linearly increasing K-field to the boundary
 */
class KFieldDirichletBC : public DirichletBCBase
{
public:
  static InputParameters validParams();

  KFieldDirichletBC(const InputParameters & params);

protected:
  virtual Real computeQpValue() override;

  MooseEnum _component;
  Real _K_value;
  Real _J_value;
  Real _youngs_modulus;
  Real _poissons_ratio;
  Real _start_time;
  Real _end_time;
  bool _J_prescribed;
};

} // end namespace