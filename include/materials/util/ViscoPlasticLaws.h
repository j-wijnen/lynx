#pragma once

#include "Moose.h"

/**
 * Implements multiple visco-plastic laws.
 */

class ViscoPlasticLaw
{
public:
  ViscoPlasticLaw(Real viscosity);

  virtual ~ViscoPlasticLaw() = default;

  virtual Real getStress(Real dplastic_mult,
                         Real dt,
                         Real yield_stress);

  virtual Real getDerivative(Real dplastic_mult,
                             Real dt,
                             Real yield_stress,
                             Real yield_stress_derivative);
protected:
  const Real _viscosity;
};

class PerzynaViscoPlasticLaw : public ViscoPlasticLaw
{
public:
  PerzynaViscoPlasticLaw(Real viscosity,
                         Real rate_sensitivity);

  virtual Real getStress(Real dplastic_mult,
                         Real dt,
                         Real yield_stress) override;

  virtual Real getDerivative(Real dplastic_mult,
                             Real dt,
                             Real yield_stress,
                             Real yield_stress_derivative) override;

protected:
  const Real _rate_sensitivity;
};

class PericViscoPlasticLaw : public ViscoPlasticLaw
{
public:
  PericViscoPlasticLaw(Real viscosity,
                       Real rate_sensitivity);

  virtual Real getStress(Real dplastic_mult,
                         Real dt,
                         Real yield_stress) override;

  virtual Real getDerivative(Real dplastic_mult,
                             Real dt,
                             Real yield_stress,
                             Real yield_stress_derivative) override;

protected:
  const Real _rate_sensitivity;
};