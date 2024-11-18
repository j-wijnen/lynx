#pragma once

#include "ADMaterial.h"

/**
 * ADComputeLinearElasticStress computes the stress following linear elasticity theory (small
 * strains)
 */
class ADComputeDegradationFunction : public ADMaterial
{
public:
  static InputParameters validParams();

  ADComputeDegradationFunction(const InputParameters & parameters);


protected:

  virtual void computeQpProperties() override;

  const ADVariableValue & _phi;

  // Declared properteis
  ADMaterialProperty<Real> & _degradation,
                           & _d_degradation;

  // Parameters
  Real _epsilon;
};
