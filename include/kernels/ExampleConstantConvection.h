#pragma once

#include "Kernel.h"

/* Implements the kernel
 * (v . grad(u), test)
 * where v is a constant
 */

class ExampleConstantConvection : public Kernel
{
public:
  static InputParameters validParams();

  ExampleConstantConvection(const InputParameters & params);

protected:
  virtual Real computeQpResidual() override;

  virtual Real computeQpJacobian() override;

private:
  RealVectorValue _v;
};