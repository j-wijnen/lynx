#pragma once

#include "ComputeLagrangianStressPK1.h"
#include "RankTwoTensor.h"

/**
 * LinearElasticStress computes the linear elastic stress
 */

class IsotropicPlasticStressNew : public ComputeLagrangianStressPK1
{
public:

  static InputParameters validParams();

  IsotropicPlasticStressNew(const InputParameters & parameters);

protected:

  virtual void initQpStatefulProperties() override;

  virtual void computeQpPK1Stress() override;

  virtual void computeLameConstants();

  virtual FunctionValue getYieldStress(Real plastic_strain_eq);

  // Declared material properties
  MaterialProperty<RankTwoTensor> & _elastic_strain;
  MaterialProperty<RankTwoTensor> & _plastic_strain;
  MaterialProperty<Real> & _plastic_strain_eq;

  // Consumed material properties
  const MaterialProperty<RankTwoTensor> & _mechanical_strain;

  // Stateful material properties
  const MaterialProperty<RankTwoTensor> & _plastic_strain_old;
  const MaterialProperty<Real> & _plastic_strain_eq_old;

  // Lame constants
  Real _youngs_modulus;
  Real _poissons_ratio;
  Real _bulk_modulus;
  Real _shear_modulus;
  const bool _isset_youngs_modulus;
  const bool _isset_poissons_ratio;
  const bool _isset_bulk_modulus;
  const bool _isset_shear_modulus;

  // Yield parameters
  const MooseEnum _hardening_law;
  const Real _yield_stress0;
  const Real _hardening_modulus;
  const Real _hardening_exponent;
  const Real _tolerance;
};
