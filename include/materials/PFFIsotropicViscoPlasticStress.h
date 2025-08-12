#pragma once

#include "PFFIsotropicPlasticStress.h"

/**
 * Computes the stress following the Perzyna/Peric viscoplastic model
 * Default:   deps_p = dt/eta * (sigma/sigma_y)
 * Perzyna: deps_p = dt/eta * ((sigma - sigma_y)/sigma_y)^n
 * Peric:   deps_p = dt/eta * ((sigma/sigma_y)^n - 1)^n
 */

 namespace lynx
 {

class PFFIsotropicViscoPlasticStress : public PFFIsotropicPlasticStress
{
public:
  static InputParameters validParams();

  PFFIsotropicViscoPlasticStress(const InputParameters & parameters);

protected:
  virtual Real computeReturnResidual(Real trial_stress,
                                     Real dplastic_mult) override;

  virtual Real computeReturnDerivative(Real dplastic_mult) override;

  Real _viscosity;
  Real _rate_sensitivity;
  MooseEnum _viscosity_type;

  std::unique_ptr<ViscoPlasticLaw> _viscosity_law;
};

} // end namespace