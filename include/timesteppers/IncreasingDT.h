//* This file is part of the LYNX framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "TimeStepper.h"

/**
 * This class doubles the timestep at every n iteration
 * until it reaches a user-specified minimum value.
 */
class IncreasingDT : public TimeStepper
{
public:

  static InputParameters validParams();

  IncreasingDT(const InputParameters & parameters);

protected:

  virtual Real computeInitialDT() override;

  virtual Real computeDT() override;

private:

  const Real _dt; 
  const Real _dt0;
  const Real _dt_max;
  const Real _ratio;
  
  const int _ninc;

  bool _first_inc;
  bool _second_inc;
  int _iinc;
};