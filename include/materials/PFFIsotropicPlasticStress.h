#pragma once

#include "IsotropicPlasticStress.h"

/**
 * LinearElasticStress computes the linear elastic stress
 */

class PFFIsotropicPlasticStress : public IsotropicPlasticStress
{
public:
  static InputParameters validParams();

  PFFIsotropicPlasticStress(const InputParameters & parameters);

protected:
  virtual void initQpStatefulProperties() override;

  virtual void computeQpStress() override;

  virtual Real computeReturnResidual(Real trial_stress,
    Real dplastic_mult) override;

  virtual Real computeReturnDerivative(Real dplastic_mult) override;

  // Consumed properties
  const OptionalMaterialProperty<Real> & _g;

  // Declared properties
  MaterialProperty<Real> & _pff_energy;
  MaterialProperty<Real> & _elastic_energy;
  MaterialProperty<Real> & _plastic_energy;

  // Stateful properties
  const MaterialProperty<Real> & _elastic_energy_old;
  const MaterialProperty<Real> & _plastic_energy_old; 

  // Parameters
  const Real _beta;
  const bool _energy_split;
  const bool _consistent_split;
};
