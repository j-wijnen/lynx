#pragma once

#include "Kernel.h"

/* Implements the kernel
 * (v . grad(u), test)
 * where v is another variable
 */

class ExampleMaterialCoupledConvection : public Kernel
{
public:
  static InputParameters validParams();

  ExampleMaterialCoupledConvection(const InputParameters & params);

protected:
  virtual Real computeQpResidual() override;

  virtual Real computeQpJacobian() override;

private:
  const MaterialProperty<RealGradient> & _grad_v;
};