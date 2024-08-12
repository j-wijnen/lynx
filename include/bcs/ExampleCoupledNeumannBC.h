#pragma once

#include "IntegratedBC.h"

class ExampleCoupledNeumannBC : public IntegratedBC
{
public:
  static InputParameters validParams();

  ExampleCoupledNeumannBC(const InputParameters & params);

protected:
  virtual Real computeQpResidual() override;

private:
  // Multiplier on the boundary.
  Real _alpha;

  /// reference to a user-specifiable coupled (independent) variable
  const VariableValue & _coupled_var;
};