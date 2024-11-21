#pragma once

#include "DerivativeMaterialInterface.h"
#include "Material.h"

/**
 * ComputeLinearElasticStress computes the stress following linear elasticity theory (small
 * strains)
 */
class ComputePFFDrivingForce : public DerivativeMaterialInterface<Material>
{
public:
  static InputParameters validParams();

  ComputePFFDrivingForce(const InputParameters & parameters);


protected:

  virtual void computeQpProperties() override;

  // Coupled variables
  const std::string _phi_name;

  // Consumed properties
  const std::string _degradation_name;
  const MaterialProperty<Real> & _ddegradationdphi,
                               & _d2degradationd2phi,
                               & _strain_energy;

  // Declared properties
  const std::string _crack_driving_force_name;
  MaterialProperty<Real> & _crack_driving_force,
                         & _dcrack_driving_force_dphi;

  // Parameters
  const Real _gc;
};

