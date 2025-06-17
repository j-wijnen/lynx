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

#include "RadialDisplacementBC.h"
#include "Coupleable.h"

namespace lynx
{

registerMooseObject("LynxApp", ADRadialDisplacementBC);
registerMooseObject("LynxApp", RadialDisplacementBC);

template<bool is_ad>
InputParameters
RadialDisplacementBCTempl<is_ad>::validParams()
{
  InputParameters params = RadialDisplacementBCBase<is_ad>::validParams();

  params.addClassDescription("Applies a linearly increasing radial displacement");

  // Variables
  params.addRequiredCoupledVar("displacements",
    "The string of displacements suitable for the problem statement");

  // Parameters
  MooseEnum component("x y");
  params.addRequiredParam<MooseEnum>("component", component, 
    "Component of displacement");
  params.addRequiredParam<Real>("value",
    "Radial displacement at `t=end_time`");
  params.addRequiredParam<Real>("penalty",
    "Penalty to weakly enforce boundary condition");
  params.addParam<std::vector<Real>>("origin", {0.0, 0.0},
    "Origin for radial calculations");
  params.addParam<Real>("start_time", 0.0, 
    "Start time of increase. Before this time the value is kept at 0.");
  params.addParam<Real>("end_time", 0.0, 
    "Time at which the condition should be satisfied. After this time the value is kept constant.");

  return params;
}

template<bool is_ad>
RadialDisplacementBCTempl<is_ad>::RadialDisplacementBCTempl(const InputParameters & params)
  : RadialDisplacementBCBase<is_ad>(params),

    // Variables
    _disp(this->template coupledGenericValues<is_ad>("displacements")),
    _disp_var(this->coupledIndices("displacements")),

    // Parameters
    _component(this->template getParam<MooseEnum>("component")),
    _value(this->template getParam<Real>("value")),
    _penalty(this->template getParam<Real>("penalty")),
    _origin(this->template getParam<std::vector<Real>>("origin")),
    _start_time(this->template getParam<Real>("start_time")),
    _end_time(this->template getParam<Real>("end_time"))
{
  if (_origin.size() != 2)
    mooseError("`origin` parameter needs to be length 2");
  if (_disp.size() < 2)
    mooseError("At least two displacement variables have to be specified");
}

template <bool is_ad>
GenericReal<is_ad>
RadialDisplacementBCTempl<is_ad>::computeQpResidual()
{
  // Linear ramp radial disp between `start_time` and `end_time`
  Real progress = 1.0;
  if (_end_time > 0.0)
    progress = std::min(std::max((_t - _start_time) / (_end_time - _start_time), 0.0), 1.0);
  Real ur = progress * _value;

  // Fill coordinate vectors
  std::array<Real,2> x0 = {_q_point[_qp](0) - _origin[0],
                           _q_point[_qp](1) - _origin[1]};
  std::array<GenericReal<is_ad>,2> x = {x0[0] + (*_disp[0])[_qp],
                                        x0[1] + (*_disp[1])[_qp]};

  // Radius
  Real r0 = std::sqrt(x0[0]*x0[0] + x0[1]*x0[1]);
  GenericReal<is_ad> r = std::sqrt(x[0]*x[0] + x[1]*x[1]);

  // Distance and radial vec component
  GenericReal<is_ad> d = r - r0 - ur;
  GenericReal<is_ad> n = x[_component] / r;

  return _penalty * _test[_i][_qp] * d * n;
}

Real
RadialDisplacementBC::computeQpJacobian()
{
  // Linear ramp radial disp between `start_time` and `end_time`
  Real progress = 1.0;
  if (_end_time > 0.0)
    progress = std::min(std::max((_t - _start_time) / (_end_time - _start_time), 0.0), 1.0);
  Real ur = progress * _value;

  // Fill coordinate vectors
  std::array<Real,2> x0 = {_q_point[_qp](0) - _origin[0],
                           _q_point[_qp](1) - _origin[1]};
  std::array<Real,2> x = {x0[0] + (*_disp[0])[_qp],
                          x0[1] + (*_disp[1])[_qp]};

  // Radius
  Real r0 = std::sqrt(x0[0]*x0[0] + x0[1]*x0[1]);
  Real r = std::sqrt(x[0]*x[0] + x[1]*x[1]);

  // Distance and component of radial vector
  Real d = r - r0 - ur;
  Real n = x[_component] / r;
  Real dd_du = n;
  Real dn_du = 1. / r - x[_component]*x[_component] / (r*r*r);

  return _penalty * _test[_i][_qp] * (dd_du*n + d*dn_du) * _phi[_j][_qp];
}

Real
RadialDisplacementBC::computeQpOffDiagJacobian(unsigned int jvar)
{
  for (unsigned int coupled_component = 0; coupled_component < 2; ++coupled_component)
    if (jvar == _disp_var[coupled_component])
    {
      // Linear ramp radial disp between `start_time` and `end_time`
      Real progress = 1.0;
      if (_end_time > 0.0)
        progress = std::min(std::max((_t - _start_time) / (_end_time - _start_time), 0.0), 1.0);
      Real ur = progress * _value;

      // Fill coordinate vectors
      std::array<Real,2> x0 = {_q_point[_qp](0) - _origin[0],
                              _q_point[_qp](1) - _origin[1]};
      std::array<Real,2> x = {x0[0] + (*_disp[0])[_qp],
                              x0[1] + (*_disp[1])[_qp]};

      // Radius
      Real r0 = std::sqrt(x0[0]*x0[0] + x0[1]*x0[1]);
      Real r = std::sqrt(x[0]*x[0] + x[1]*x[1]);

      // Distance and component of radial vector
      Real d = r - r0 - ur;
      Real n = x[_component] / r;
      Real dd_du = x[(_component + 1) % 2] / r;
      Real dn_du = - x[0]*x[1] / (r*r*r);
  
      return _penalty * _test[_i][_qp] * (dd_du*n + d*dn_du) * _phi[_j][_qp];
    }

  return 0.0;
}

} // end namespace