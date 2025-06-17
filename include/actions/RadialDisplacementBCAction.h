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

#include "Action.h"

namespace lynx
{

class RadialDisplacementBCAction : public Action
{
public:
  static InputParameters validParams();

  RadialDisplacementBCAction(const InputParameters & params);

  virtual void act() override;

protected:
  /// Flag to use automatic differentiation
  const bool _use_ad;

  /// displacement variables
  std::vector<VariableName> _displacements;

  /// number of displacement variables
  unsigned int _ndisp;

  /// auxvariables to save residuals
  std::vector<AuxVariableName> _save_in;
};

} // end namespace