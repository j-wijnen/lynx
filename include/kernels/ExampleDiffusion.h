#pragma once

#include "Diffusion.h"

class ExampleDiffusion : public Diffusion 
{
public:
  static InputParameters validParams();

  ExampleDiffusion(const InputParameters & params);

protected:
  virtual Real computeQpResidual() override;

  virtual Real computeQpJacobian() override;

private:
  const MaterialProperty<Real> & _diffusivity;
};