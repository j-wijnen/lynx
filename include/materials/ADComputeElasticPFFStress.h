#pragma once

#include "ADComputeLinearElasticStress.h"

/**
 * ADComputeLinearElasticStress computes the stress following linear elasticity theory (small
 * strains)
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
};
