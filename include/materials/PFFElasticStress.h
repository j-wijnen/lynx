#pragma once

#include "ComputeStressBase.h"
#include "RankTwoTensor.h"
#include "RankFourTensor.h"

/**
 * PFFElasticStress computes the degraded stress following linear elasticity (small
 * strains) and corresponding strain energy for phase field fracture modelling
 */
class PFFElasticStress : public ComputeStressBase
{
public:

  static InputParameters validParams();

  PFFElasticStress(const InputParameters & parameters);

protected:

  virtual void initQpStatefulProperties() override;

  virtual void computeQpStress() override;

  // Consumed properties
  const MaterialProperty<RankFourTensor> & _elasticity_tensor;
  const OptionalMaterialProperty<Real> & _g;

  // Declared properteis
  MaterialProperty<Real> & _strain_energy;

  // Stateful properties
  const MaterialProperty<Real> & _strain_energy_old;
};
