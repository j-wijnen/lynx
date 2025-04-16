#include "ViscoPlasticLaws.h"

ViscoPlasticLaw::ViscoPlasticLaw(Real viscosity)
  : _viscosity(viscosity)
{
}

Real 
ViscoPlasticLaw::getStress(Real dplastic_mult, 
                           Real dt, 
                           Real yield_stress)                                       
{
  return yield_stress * (_viscosity * dplastic_mult / dt + 1.0);
}

Real 
ViscoPlasticLaw::getDerivative(Real dplastic_mult, 
                               Real dt, 
                               Real yield_stress,
                               Real yield_stress_derivative) 
{
  return yield_stress * _viscosity / dt 
    + yield_stress_derivative * (_viscosity * dplastic_mult / dt + 1.0);
}

PerzynaViscoPlasticLaw::PerzynaViscoPlasticLaw(Real viscosity,
                                               Real rate_sensitivity)
  : ViscoPlasticLaw(viscosity),
  _rate_sensitivity(rate_sensitivity)
{
}

Real 
PerzynaViscoPlasticLaw::getStress(Real dplastic_mult, 
                           Real dt, 
                           Real yield_stress)                                       
{
  return yield_stress * (std::pow(_viscosity * dplastic_mult / dt, _rate_sensitivity) + 1.0);
}

Real 
PerzynaViscoPlasticLaw::getDerivative(Real dplastic_mult, 
                               Real dt, 
                               Real yield_stress,
                               Real yield_stress_derivative) 
{
  Real dsigma = yield_stress_derivative * (std::pow(_viscosity * dplastic_mult / dt, _rate_sensitivity) + 1.);
  if (dplastic_mult > 1e-10)
    dsigma += yield_stress * _viscosity * _rate_sensitivity / dt
      * std::pow(_viscosity * dplastic_mult / dt, _rate_sensitivity - 1.);
  return dsigma;
}

PericViscoPlasticLaw::PericViscoPlasticLaw(Real viscosity,
                                           Real rate_sensitivity)
  : ViscoPlasticLaw(viscosity),
  _rate_sensitivity(rate_sensitivity)
{
}

Real 
PericViscoPlasticLaw::getStress(Real dplastic_mult, 
                                  Real dt, 
                                  Real yield_stress)                                       
{
  return yield_stress * std::pow(_viscosity * dplastic_mult / dt + 1.0, _rate_sensitivity);
}

Real 
PericViscoPlasticLaw::getDerivative(Real dplastic_mult, 
                                      Real dt, 
                                      Real yield_stress,
                                      Real yield_stress_derivative) 
{
  return yield_stress * _viscosity * _rate_sensitivity / dt 
    * std::pow(_viscosity * dplastic_mult / dt + 1.0, _rate_sensitivity - 1.)
    + yield_stress_derivative * std::pow(_viscosity * dplastic_mult / dt + 1., _rate_sensitivity);
}
