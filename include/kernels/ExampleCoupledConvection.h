#pragma once

#include "Kernel.h"

/* Implements the kernel
 * (v . grad(u), test)
 * where v is another variable
 */

class ExampleCoupledConvection : public Kernel
{
public:
  static InputParameters validParams();

  ExampleCoupledConvection(const InputParameters & params);

protected:
  virtual Real computeQpResidual() override;

  virtual Real computeQpJacobian() override;

private:
  const VariableGradient & _grad_v;
};