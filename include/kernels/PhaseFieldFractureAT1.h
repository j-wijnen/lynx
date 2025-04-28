#pragma once

#include "DerivativeMaterialInterface.h"
#include "Kernel.h"

/**
 * Kernel computing the micoforce-balance for AT1 phase field fracture mechanics
 * 3/4 * Gc/ell (0.5 - ell^2 nabla^2 phi) = driving_force
 */
class PhaseFieldFractureAT1 : public DerivativeMaterialInterface<Kernel>
{
public:

  static InputParameters validParams();

  PhaseFieldFractureAT1(const InputParameters & params);

protected:

  virtual Real computeQpResidual() override;

  virtual Real computeQpJacobian() override;

  // Consumed properties
  const MaterialProperty<Real> & _length_scale,
                               & _fracture_toughness,
                               & _pff_driving_force,
                               & _dpff_driving_force_du;
};