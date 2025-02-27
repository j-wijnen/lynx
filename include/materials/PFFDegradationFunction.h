#pragma once

#include "DerivativeMaterialInterface.h"
#include "Material.h"

/**
 * Compute material degradation functions based on the damage parameter
 */
class PFFDegradationFunction : public DerivativeMaterialInterface<Material>
{
public:

  static InputParameters validParams();

  PFFDegradationFunction(const InputParameters & parameters);

protected:

  virtual void computeQpProperties() override;

  // Coupled damage variable
  const std::string _u_name;
  const VariableValue & _u;

  // Declared properteis
  MaterialProperty<Real> & _g,
                         & _dg_du,
                         & _d2g_du2;

  // Parameters
  Real _epsilon;
};
