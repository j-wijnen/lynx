#pragma once

#include "GenericComputeStressBase.h"
#include "GuaranteeConsumer.h"
#include "RankTwoTensor.h"
#include "HardeningBase.h"

namespace lynx 
{

/**
 * IsotropicPlasticStress implements J2 plasticity
 */
template <bool is_ad>
class IsotropicPlasticStressTempl : public GenericComputeStressBase<is_ad>,
                                    public GuaranteeConsumer
{
public:
  static InputParameters validParams();

  IsotropicPlasticStressTempl(const InputParameters & params);

  ~IsotropicPlasticStressTempl();

  virtual void initialSetup() override;

protected:
  virtual void initQpStatefulProperties() override;

  virtual void computeQpStress() override;

  // Implements single equation return map
  virtual GenericReal<is_ad> computeReturnMap(GenericReal<is_ad> trial_stress);

  // Residual and derivative called in the return map algorithm 
  // residual = trial_stress - dsigma_correct - sigma 
  virtual GenericReal<is_ad> computeReturnResidual(GenericReal<is_ad> trial_stress, 
                                                   GenericReal<is_ad> dplastic_mult);

  virtual GenericReal<is_ad> computeReturnDerivative(GenericReal<is_ad> dplastic_mult);

  // Consumed material properties
  const GenericMaterialProperty<RankFourTensor, is_ad> & _elasticity_tensor;

  // Declared material properties
  GenericMaterialProperty<RankTwoTensor, is_ad> & _plastic_strain;
  GenericMaterialProperty<Real, is_ad> & _plastic_multiplier;
  GenericMaterialProperty<Real, is_ad> & _yield_stress;

  // Stateful material properties
  const MaterialProperty<RankTwoTensor> & _plastic_strain_old;
  const MaterialProperty<Real> & _plastic_multiplier_old;

  // Yield parameters
  const Real _tolerance;

  // Hardening law
  HardeningBase<is_ad> * _hardening_law;
  bool _no_hardening;

  // constants
  const Real _sqrt32 = std::sqrt(3.0 / 2.0);
  const Real _sqrt23 = std::sqrt(2.0 / 3.0);

  // Members from base template
  using GenericComputeStressBase<is_ad>::_base_name;
  using GenericComputeStressBase<is_ad>::_mechanical_strain;
  using GenericComputeStressBase<is_ad>::_elastic_strain;
  using GenericComputeStressBase<is_ad>::_stress;
  using GenericComputeStressBase<is_ad>::_qp;
  using GenericComputeStressBase<is_ad>::setJacobian;
};

typedef IsotropicPlasticStressTempl<false> IsotropicPlasticStress;
typedef IsotropicPlasticStressTempl<true> ADIsotropicPlasticStress;

} // end namespace
