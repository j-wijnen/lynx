#pragma once

#include "DirichletBCBase.h"

/*  Linearly increases from the current value to
 *  the prescribed value at the given time
 */

class LinearRampDirichletBC : public DirichletBCBase
{
public:
  static InputParameters validParams();

  LinearRampDirichletBC(const InputParameters & params);

protected:
  virtual Real computeQpValue() override;

private:

  // value and time at which value should be satisfied
  Real _value, _start_time, _end_time;

  // Variable from previous timestep
  const VariableValue & _u_old;
};