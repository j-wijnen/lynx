//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "IncreasingDT.h"

// Moose includes

registerMooseObject("LynxApp", IncreasingDT);

InputParameters
IncreasingDT::validParams()
{
  InputParameters params = TimeStepper::validParams();
  params.addParam<Real>("dt", 1., "The initial time step size.");
  params.addParam<Real>("ratio", 2., "The ratio used to calculate the next timestep");
  params.addParam<Real>("max_dt", 10., "Largest timestep allowed");
  params.addParam<unsigned int>("ninc", 4, "Number of increments between timestep change");
  return params;
}

IncreasingDT::IncreasingDT(
  const InputParameters & parameters
)
  : TimeStepper(parameters), 
  _dt(getParam<Real>("dt")),
  _ratio(getParam<Real>("ratio")), 
  _max_dt(getParam<Real>("max_dt")),
  _ninc(getParam<unsigned int>("ninc"))
{
  _first_inc = true;
}

Real
IncreasingDT::computeInitialDT()
{
  return _dt;
}

Real
IncreasingDT::computeDT()
{
  // Initial dt
  if( _first_inc )
  {
    _first_inc = false;
    _iinc = 1;
    return _dt;
  }

  // Count increments since last change
  ++_iinc;

  // Adapt if conditions satisfied
  if( _iinc > _ninc && getCurrentDT() < _max_dt )
  {
    _iinc = 1;
    return std::min(_ratio * getCurrentDT(), _max_dt);
  }
  else
    return getCurrentDT();
}