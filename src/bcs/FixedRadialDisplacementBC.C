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

#include "FixedRadialDisplacementBC.h"

namespace lynx
{

registerMooseObject("LynxApp", FixedRadialDisplacementBC);

InputParameters
FixedRadialDisplacementBC::validParams()
{
  InputParameters params = DirichletBCBase::validParams();

  params.addClassDescription("Applies a linearly increasing radial displacement");

  MooseEnum component("x y");
  params.addRequiredParam<MooseEnum>("component", component, 
    "Component of displacement");
  params.addRequiredParam<Real>("value",
    "Radial displacement at `t=end_time`");
  params.addParam<std::vector<Real>>("origin", {0.0, 0.0},
    "Origin for radial calculations");
  params.addParam<Real>("start_time", 0.0, 
    "Start time of increase. Before this time the value is kept at 0.");
  params.addParam<Real>("end_time", 0.0, 
    "Time at which the condition should be satisfied. After this time the value is kept constant.");

  return params;
}

FixedRadialDisplacementBC::FixedRadialDisplacementBC(const InputParameters & params)
  : DirichletBCBase(params),
    _component(getParam<MooseEnum>("component")),
    _value(getParam<Real>("value")),
    _origin(getParam<std::vector<Real>>("origin")),
    _start_time(getParam<Real>("start_time")),
    _end_time(getParam<Real>("end_time"))
{
  if (_origin.size() != 2)
    mooseError("`origin` parameter needs to be length 2");
}

Real
FixedRadialDisplacementBC::computeQpValue()
{
  // Linear ramp value between `start_time` and `end_time`
  Real progress = 1.0;
  if (_end_time > 0.0)
    progress = std::min(std::max((_t - _start_time) / (_end_time - _start_time), 0.0), 1.0);

  Real x = (*_current_node)(0) - _origin[0];
  Real y = (*_current_node)(1) - _origin[1];
  Real theta = std::atan2(y, x);

  Real projection;
  switch (_component)
  {
    case 0:
      projection = std::cos(theta);
      break;
    case 1:
      projection = std::sin(theta);
      break;
  }

  return projection * progress * _value;
}

} // end namespace