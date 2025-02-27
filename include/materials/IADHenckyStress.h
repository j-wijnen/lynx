#pragma once

#include "ComputeLagrangianStressPK1.h"

/**
 */

class IADHenckyStress : public ComputeLagrangianStressPK1
{
public:

  static InputParameters validParams();

  IADHenckyStress(const InputParameters & parameters);

protected:

  virtual void computeQpPK1Stress() override;

  // Elastic parameters
  const MaterialProperty<RankFourTensor> & _elasticity_tensor;
};