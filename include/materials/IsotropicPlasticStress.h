#pragma once

#include "ComputeStressBase.h"
#include "GuaranteeConsumer.h"
#include "RankTwoTensor.h"
#include "HardeningLaws.h"

/**
 * LinearElasticStress computes the linear elastic stress
 */

class IsotropicPlasticStress : public ComputeStressBase,
                               public GuaranteeConsumer
{
public:
  static InputParameters validParams();

  IsotropicPlasticStress(const InputParameters & parameters);

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

  // Assign the hardening law
  virtual void setHardeningLaw(const InputParameters & parameters);

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
  const MooseEnum _hardening_law;
  const Real _initial_yield_stress;
  const Real _hardening_modulus;
  const Real _hardening_exponent;
  const Real _tolerance;

  // Yield parameters for variable properties
  const OptionalMaterialProperty<Real> & _initial_yield_stress_prop;
  const OptionalMaterialProperty<Real> & _hardening_modulus_prop;
  const OptionalMaterialProperty<Real> & _hardening_exponent_prop;

  std::unique_ptr<HardeningLaw> _hardening;

  // constants
  const Real _sqrt32 = std::sqrt(3.0 / 2.0);
  const Real _sqrt23 = std::sqrt(2.0 / 3.0);
};
