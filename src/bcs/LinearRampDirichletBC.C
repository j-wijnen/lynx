#include "LinearRampDirichletBC.h"

registerMooseObject("LynxApp", LinearRampDirichletBC);

InputParameters
LinearRampDirichletBC::validParams()
{
  InputParameters params = NodalBC::validParams();

  params.addClassDescription("Linearly increases from the current value to"
                             "the prescribed value at the given time");

  params.addRequiredParam<Real>("value", "Value to be described");
  params.addRequiredParam<Real>("time", "Time at which the condition should be satisfied");

  return params;
}

LinearRampDirichletBC::LinearRampDirichletBC(const InputParameters & params)
  : NodalBC(params),
    _value(getParam<Real>("value")),
    _time(getParam<Real>("time")),
    _u_old(valueOld())
{
}

Real
LinearRampDirichletBC::computeQpResidual()
{
  Real up = (_t - _t_old) / (_time - _t_old) * (_value - _u_old[_qp]) + _u_old[_qp];
  return _u[_qp] - up;
}