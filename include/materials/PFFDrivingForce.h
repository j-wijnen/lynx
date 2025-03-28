#pragma once

#include "DerivativeMaterialInterface.h"
#include "Material.h"

/**
 * ComputeLinearElasticStress computes the stress following linear elasticity theory (small
 * strains)
 */
class PFFDrivingForce : public DerivativeMaterialInterface<Material>
{
public:
  static InputParameters validParams();

  PFFDrivingForce(const InputParameters & parameters);


protected:

  virtual void computeQpProperties() override;

  // Coupled variables
  const std::string _u_name;

  // Consumed properties
  const std::string _degradation_name;
  const MaterialProperty<Real> & _dg_du,
                               & _d2g_du2,
                               & _pff_energy;

  // Declared properties
  const std::string _pff_driving_force_name;
  MaterialProperty<Real> & _pff_driving_force,
                         & _dpff_driving_force_du;
};

