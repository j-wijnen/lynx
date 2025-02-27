#pragma once

#include "ComputeLagrangianStressPK1.h"
#include "RankTwoTensor.h"
#include "RankFourTensor.h"

/**
 * StVenantKirchhoffStress computes the linear elastic stress
 */
class StVenantKirchhoffStress : public ComputeLagrangianStressPK1
{
public:

  static InputParameters validParams();

  StVenantKirchhoffStress(const InputParameters & parameters);

protected:

  virtual void computeQpPK1Stress() override;

  // Consumed properties
  const MaterialProperty<RankFourTensor> & _elasticity_tensor;
};