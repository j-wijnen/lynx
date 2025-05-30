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
  Real hardening_modulus, 
  Real hardening_exponent
)
  : HardeningLaw(yield_stress),
   _hardening_modulus(hardening_modulus),
  _hardening_exponent(hardening_exponent)
{
}

Real
PowerLawHardening::getValue(Real hardening_parameter)
{
  return _yield_stress0 * std::pow(
    1. + _hardening_modulus / _yield_stress0 * hardening_parameter, 
    _hardening_exponent);
}

Real 
PowerLawHardening::getDerivative(Real hardening_parameter)
{
  return _hardening_modulus * _hardening_exponent * std::pow(
    1. + _hardening_modulus / _yield_stress0 * hardening_parameter,
     _hardening_exponent - 1.);
}

SwiftHardening::SwiftHardening(
  Real yield_stress, 
  Real hardening_modulus, 
  Real hardening_exponent
)
  : HardeningLaw(yield_stress),
   _modulus(hardening_modulus),
   _exponent(hardening_exponent)
{
}

Real
SwiftHardening::getValue(Real hardening_parameter)
{
  return _modulus * std::pow(
    std::pow(_yield_stress0/_modulus, 1./_exponent)
    + hardening_parameter, _exponent);
}

Real 
SwiftHardening::getDerivative(Real hardening_parameter)
{
  return _exponent * _modulus * std::pow(
    std::pow(_yield_stress0/_modulus, 1.0/_exponent)
    + hardening_parameter, _exponent - 1.0);
}