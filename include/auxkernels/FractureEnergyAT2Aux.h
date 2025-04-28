#pragma once

#include "AuxKernel.h"

class FractureEnergyAT2Aux : public AuxKernel
{
public:
  static InputParameters validParams();

  FractureEnergyAT2Aux(const InputParameters & parameters);
  virtual ~FractureEnergyAT2Aux() {}

protected:
  virtual Real computeValue();

  // Coupled damage variable
  const VariableValue & _damage;
  const VariableGradient & _grad_damage;

  /// Fracture toughenss property
  const MaterialProperty<Real> & _fracture_toughness;
  const MaterialProperty<Real> & _length_scale;
};
