#pragma once

#include "ADMaterial.h"

/**
 * ADComputeLinearElasticStress computes the stress following linear elasticity theory (small
 * strains)
 */
class ADComputePFFDrivingForce : public ADMaterial
{
public:
  static InputParameters validParams();

  ADComputePFFDrivingForce(const InputParameters & parameters);


protected:

  virtual void computeQpProperties() override;

  // Parameters
  const Real _gc;

  // Consumed properties
  const ADMaterialProperty<Real> & _d_degradation,
                                 & _strain_energy;

  // Declared properteis
  ADMaterialProperty<Real> & _crack_driving_force;
};
