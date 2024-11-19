#pragma once

#include "ADComputeLinearElasticStress.h"

/**
 * ADComputeElasticPFFStress computes the degraded stress following linear elasticity theory (small
 * strains) and corresponding strain energy for phase field fracture modelling
 */
class ADComputeElasticPFFStress : public ADComputeLinearElasticStress
{
public:
  static InputParameters validParams();

  ADComputeElasticPFFStress(const InputParameters & parameters);

  virtual void initialSetup() override;

protected:

  virtual void computeQpStress() override;

  // Consumed properties
  const OptionalADMaterialProperty<Real> & _degradation;

  // Declared properteis
  ADMaterialProperty<Real> & _strain_energy;

  // Stateful properties
  const MaterialProperty<Real> & _strain_energy_old;
};
