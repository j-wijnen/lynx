#pragma once

#include "NodalBC.h"

/*  Linearly increases from the current value to
 *  the prescribed value at the given time
 */

class LinearRampDirichletBC : public NodalBC
{
public:
  static InputParameters validParams();

  LinearRampDirichletBC(const InputParameters & params);

protected:
  virtual Real computeQpResidual() override;

private:

  // value and time at which value should be satisfied
  Real _value, _end_time;

  // Variable from previous timestep
  const VariableValue & _u_old;
};