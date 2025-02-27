#pragma once

#include "ComputeLagrangianStressPK1.h"

/**
 * NeoHookeanStress related to strain energy density function:
 * W = G/2 * (I1b - 3) + K * U
 * U = 0.25*(J^2 - 1) - 0.5*ln(J) 
 */

class IADNeoHookeanStress : public ComputeLagrangianStressPK1
{
public:

  static InputParameters validParams();

  IADNeoHookeanStress(const InputParameters & parameters);

protected:

  virtual void computeQpPK1Stress() override;

  // Elastic parameters
  const Real _shearmod, _bulkmod;
};