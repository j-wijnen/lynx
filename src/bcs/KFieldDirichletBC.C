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

#include "KFieldDirichletBC.h"

namespace lynx
{

registerMooseObject("LynxApp", KFieldDirichletBC);

InputParameters
KFieldDirichletBC::validParams()
{
  InputParameters params = DirichletBCBase::validParams();

  params.addClassDescription("Applies a linearly increasing K-field to the boundary");

  MooseEnum direction("x y");
  params.addRequiredParam<MooseEnum>("direction", direction, "Direction of displacement component");
  params.addRequiredParam<Real>("youngs_modulus", "Young's modulus");
  params.addRequiredParam<Real>("poissons_ratio", "Poissons ratio");
  params.addParam<Real>("K", 0.0, "K-value to be described");
  params.addParam<Real>("J", 0.0, "J-value to be described");
  params.addParam<Real>("start_time", 0.0, 
    "Start time of increase. Before this time the value is kept at 0.");
  params.addParam<Real>("end_time", 0.0, 
    "Time at which the condition should be satisfied. After this time the value is kept constant.");

  return params;
}


KFieldDirichletBC::KFieldDirichletBC(const InputParameters & params)
  : DirichletBCBase(params),
    _direction(getParam<MooseEnum>("direction")),
    _K_value(getParam<Real>("K")),
    _J_value(getParam<Real>("J")),
    _youngs_modulus(getParam<Real>("youngs_modulus")),
    _poissons_ratio(getParam<Real>("poissons_ratio")),
    _start_time(getParam<Real>("start_time")),
    _end_time(getParam<Real>("end_time"))
{
  _J_prescribed = params.isParamSetByUser("J");

  if(_J_prescribed == params.isParamSetByUser("K"))
    mooseError("`K` OR `J` have to be defined.");
}


Real
KFieldDirichletBC::computeQpValue()
{
  // Apply constant value before/after start/end time
  Real progress = (_t - _start_time) / (_end_time - _start_time);
  progress = std::min(std::max(progress, 0.0), 1.0);

  Real K;
  if( _J_prescribed )
    K = std::sqrt(progress * _J_value * _youngs_modulus 
      / (1. - _poissons_ratio*_poissons_ratio));
  else 
    K = progress * _K_value;

  Real x = (*_current_node)(0);
  Real y = (*_current_node)(1); 
  Real r = std::sqrt(x*x + y*y);
  Real theta = std::atan2(y, x);
  
  const Real pi = 3.14159265358979323846;
  const Real c05 = std::cos(.5*theta);
  const Real s05 = std::sin(.5*theta);

  Real f;
  switch(_direction)
  {
    case 0:
      f = c05 * (2. - 4.*_poissons_ratio + 2.*s05*s05);
      break;
    case 1:
      f = s05 * (4. - 4.*_poissons_ratio - 2.*c05*c05);
      break;
  }

  return K * (1. + _poissons_ratio) / _youngs_modulus  * std::sqrt(.5*r/pi) * f;
}

} // end