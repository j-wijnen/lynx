#pragma once

#include "DerivativeMaterialInterface.h"
#include "Material.h"

/**
 * Compute material degradation functions based on the damage parameter
 */
class ComputeDegradationFunction : public DerivativeMaterialInterface<Material>
{
public:

  static InputParameters validParams();

  ComputeDegradationFunction(const InputParameters & parameters);

protected:

  virtual void computeQpProperties() override;

  // Coupled damage variable
  const std::string _phi_name;
  const VariableValue & _phi;

  // Declared properteis
  MaterialProperty<Real> & _degradation,
                         & _ddegradationdphi,
                         & _d2degradationd2phi;

  // Parameters
  Real _epsilon;
};
