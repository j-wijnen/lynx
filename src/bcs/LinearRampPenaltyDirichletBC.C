

#include "LinearRampPenaltyDirichletBC.h"

registerMooseObject("LynxApp", LinearRampPenaltyDirichletBC);

InputParameters
LinearRampPenaltyDirichletBC::validParams()
{
  InputParameters params = IntegratedBC::validParams();

  params.addClassDescription("Enforces a Linear Ramped Dirichlet boundary condition "
    "in a weak sense by penalizing differences between the current "
    "solution and the Dirichlet data.");

  params.addRequiredParam<Real>("penalty", "Penalty scalar");
  params.addRequiredParam<Real>("value", "Value to be described");
  params.addParam<Real>("start_time", 0.0, "Start time of increase. Before this time the value is kept constant.");
  params.addParam<Real>("end_time", 0.0, "Time at which the condition should be satisfied");
 
  return params;
}

LinearRampPenaltyDirichletBC::LinearRampPenaltyDirichletBC(const InputParameters & parameters)
  : IntegratedBC(parameters), 
  _penalty(getParam<Real>("penalty")), 
  _value(getParam<Real>("value")),
  _start_time(getParam<Real>("start_time")),
  _end_time(getParam<Real>("end_time")),
  _u_old(valueOld())
{
}

Real
LinearRampPenaltyDirichletBC::computeQpResidual()
{
  Real v;

  // Apply constant value before/after start/end time
  if(_t <= _start_time)
    v = _u_old[_qp];
  else if(_t >= _end_time)
    v = _value;

  // Linear ramp to value
  v = (_t - _t_old) / (_end_time - _t_old) * (_value - _u_old[_qp]) + _u_old[_qp];

  return _penalty * _test[_i][_qp] * (-v + _u[_qp]);
}

Real
LinearRampPenaltyDirichletBC::computeQpJacobian()
{
  return _penalty * _phi[_j][_qp] * _test[_i][_qp];
}
