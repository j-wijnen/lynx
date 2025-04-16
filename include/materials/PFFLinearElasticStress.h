#pragma once

#include "ComputeStressBase.h"
#include "GuaranteeConsumer.h"
#include "RankTwoTensor.h"
#include "RankFourTensor.h"

/**
 * PFFLinearElasticStress computes the degraded stress following linear elasticity (small
 * strains) and corresponding strain energy for phase field fracture modelling
 */
class PFFLinearElasticStress : public ComputeStressBase,
                               public GuaranteeConsumer
{
public:
  static InputParameters validParams();

  PFFLinearElasticStress(const InputParameters & parameters);

protected:
  virtual void initialSetup() override;

  virtual void initQpStatefulProperties() override;

  virtual void computeQpStress() override;

  // Consumed properties
  const MaterialProperty<RankFourTensor> & _elasticity_tensor;
  const OptionalMaterialProperty<Real> & _g;

  // Declared properteis
  MaterialProperty<Real> & _pff_energy;

  // Stateful properties
  const MaterialProperty<Real> & _pff_energy_old;

  // Parameters 
  const bool _consistent_split;
};
