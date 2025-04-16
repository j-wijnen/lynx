#include "HardeningLaws.h"

HardeningLaw::HardeningLaw(Real yield_stress)
  : _yield_stress0(yield_stress)
{
}

Real 
HardeningLaw::getValue(Real hardening_parameter)
{
  return _yield_stress0;
}

Real 
HardeningLaw::getDerivative(Real hardening_parameter)
{
  return 0.0;
}

LinearHardening::LinearHardening(
  Real yield_stress, 
  Real hardening_modulus
)
  : HardeningLaw(yield_stress),
    _hardening_modulus(hardening_modulus)
{
}

Real
LinearHardening::getValue(Real hardening_parameter)
{
  return _yield_stress0 + _hardening_modulus * hardening_parameter;
}

Real 
LinearHardening::getDerivative(Real hardening_parameter)
{
  return _hardening_modulus;
}

PowerLawHardening::PowerLawHardening(
  Real yield_stress, 
  Real youngs_modulus, 
  Real hardening_exponent
)
  : HardeningLaw(yield_stress),
   _youngs_modulus(youngs_modulus),
  _hardening_exponent(hardening_exponent)
{
}

Real
PowerLawHardening::getValue(Real hardening_parameter)
{
  return _yield_stress0 * std::pow(
    1. + _youngs_modulus / _yield_stress0 * hardening_parameter, 
    _hardening_exponent);
}

Real 
PowerLawHardening::getDerivative(Real hardening_parameter)
{
  return _youngs_modulus * _hardening_exponent * std::pow(
    1. + _youngs_modulus / _yield_stress0 * hardening_parameter,
     _hardening_exponent - 1.);
}