#pragma once

#include "NodalBC.h"

class ExampleCoupledDirichletBC : public NodalBC
{
public:
  static InputParameters validParams();

  ExampleCoupledDirichletBC(const InputParameters & params);

protected:
  virtual Real computeQpResidual() override;

private:
  // Multiplier on the boundary.
  Real _alpha;

  /// reference to a user-specifiable coupled (independent) variable
  const VariableValue & _coupled_var;
};