#include "LinearRampDirichletBC.h"

registerMooseObject("LynxApp", LinearRampDirichletBC);


InputParameters
LinearRampDirichletBC::validParams()
{
  InputParameters params = NodalBC::validParams();

  params.addClassDescription("Linearly increases from the current value to"
                             "the prescribed value at the given time");

  params.addRequiredParam<Real>("value", "Value to be described");
  params.addRequiredParam<Real>("end_time", "Time at which the condition should be satisfied");

  return params;
}


LinearRampDirichletBC::LinearRampDirichletBC(
  const InputParameters & params
)
  : NodalBC(params),
    _value(getParam<Real>("value")),
    _end_time(getParam<Real>("end_time")),
    _u_old(valueOld())
{
}


Real
LinearRampDirichletBC::computeQpResidual()
{
  // Apply constant value after end time is reached
  if( _t >= _end_time )
    return _u[_qp] - _value;

  // Linear ramp to value
  Real up = (_t - _t_old) / (_end_time - _t_old) * (_value - _u_old[_qp]) + _u_old[_qp];
  return _u[_qp] - up;
}