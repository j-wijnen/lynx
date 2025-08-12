#pragma once

#include "ComputeStressBase.h"
#include "GuaranteeConsumer.h"
#include "RankTwoTensor.h"
#include "HardeningBase.h"

namespace lynx 
{

/**
 * IsotropicPlasticStressOld implements J2 plasticity
 */

class IsotropicPlasticStressOld : public ComputeStressBase,
                               public GuaranteeConsumer
{
public:
  static InputParameters validParams();

  IsotropicPlasticStressOld(const InputParameters & parameters);

  ~IsotropicPlasticStressOld();

  virtual void initialSetup() override;

protected:
  virtual void initQpStatefulProperties() override;

  virtual void computeQpStress() override;

  // Implements single equation return map
  virtual Real computeReturnMap(Real trial_stress);

  // Residual and derivative called in the return map algorithm 
  // residual = trial_stress - dsigma_correct - sigma 
  virtual Real computeReturnResidual(Real trial_stress, 
                                     Real dplastic_mult);

  virtual Real computeReturnDerivative(Real dplastic_mult);

  // Consumed material properties
  const MaterialProperty<RankFourTensor> & _elasticity_tensor;

  // Declared material properties
  MaterialProperty<RankTwoTensor> & _plastic_strain;
  MaterialProperty<Real> & _plastic_multiplier;
  MaterialProperty<Real> & _yield_stress;

  // Stateful material properties
  const MaterialProperty<RankTwoTensor> & _plastic_strain_old;
  const MaterialProperty<Real> & _plastic_multiplier_old;

  // Yield parameters
  const Real _tolerance;

  // Hardening law
  HardeningBase<false> * _hardening_law;
  bool _no_hardening;

  // constants
  const Real _sqrt32 = std::sqrt(3.0 / 2.0);
  const Real _sqrt23 = std::sqrt(2.0 / 3.0);
};

} // end namespace
