#pragma once

#include "AuxKernel.h"

class FractureEnergyAux : public AuxKernel
{
public:
  static InputParameters validParams();

  FractureEnergyAux(const InputParameters & parameters);
  virtual ~FractureEnergyAux() {}

protected:
  virtual Real computeValue();

  // Coupled damage variable
  const VariableValue & _damage;
  const VariableGradient & _grad_damage;

  /// Fracture toughenss property
  const MaterialProperty<Real> & _fracture_toughness;
  const MaterialProperty<Real> & _length_scale;
};
