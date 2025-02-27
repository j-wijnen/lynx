#pragma once

#include "ComputeStressBase.h"
#include "RankFourTensor.h"

/**
 * LinearElasticStress computes the linear elastic stress
 */
class LinearElasticStress : public ComputeStressBase
{
public:

  static InputParameters validParams();

  LinearElasticStress(const InputParameters & parameters);

protected:

  virtual void computeQpStress() override;

  // Consumed properties
  const MaterialProperty<RankFourTensor> & _elasticity_tensor;
};