#pragma once

#include "TimeDerivative.h"

/* Implements a constant-scalable time-dependent kernel
 * (test, coeff*du/dt)
 */

class ExampleTimeDerivative : public TimeDerivative
{
public:

  static InputParameters validParams();

  ExampleTimeDerivative(
    const InputParameters& params
  );

protected:

  virtual Real computeQpResidual() override;

  virtual Real computeQpJacobian() override;

private:
  
  const Real _time_coeff;

};