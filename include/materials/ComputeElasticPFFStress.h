#pragma once

#include "ComputeLinearElasticStress.h"

/**
 * ComputeElasticPFFStress computes the degraded stress following linear elasticity theory (small
 * strains) and corresponding strain energy for phase field fracture modelling
 */
class ComputeElasticPFFStress : public ComputeLinearElasticStress
{
public:
  static InputParameters validParams();

  ComputeElasticPFFStress(const InputParameters & parameters);

  virtual void initialSetup() override;

protected:

  virtual void initQpStatefulProperties() override;

  virtual void computeQpStress() override;

  // Consumed properties
  //const OptionalMaterialProperty<Real> & _degradation;

  // Declared properteis
  MaterialProperty<Real> & _strain_energy;

  // Stateful properties
  const MaterialProperty<Real> & _strain_energy_old;
};
