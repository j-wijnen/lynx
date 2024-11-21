#pragma once

#include "DerivativeMaterialInterface.h"
#include "Kernel.h"

/**
 * Kernel computing the micoforce-balance for phase field fracture mechanics
 */
class PhaseFieldFracture : public DerivativeMaterialInterface<Kernel>
{
public:

  static InputParameters validParams();

  PhaseFieldFracture(const InputParameters & params);

protected:

  virtual Real computeQpResidual() override;

  virtual Real computeQpJacobian() override;

  // Parameters
  const Real _length_scale;

  // Consumed properties
  const MaterialProperty<Real> & _crack_driving_force,
                               & _dcrack_driving_force_dphi;
};