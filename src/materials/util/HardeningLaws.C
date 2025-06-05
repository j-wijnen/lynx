//* This file is part of Lynx, 
//* an open-source application for the simulation  
//* of mechanics and multi-physics problems
//* https://github.com/j-wijnen/lynx
//*
//* Lynx is powered by the MOOSE Framework
//* https://www.mooseframework.org
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "HardeningLaws.h"

HardeningLaw::HardeningLaw()
{
}

Real
HardeningLaw::getValue(Real plastic_multiplier)
{
  mooseError("getValue of HardeningLaw should be overridden");
  return 0.0;
}

Real 
HardeningLaw::getDerivative(Real plastic_multiplier)
{
  mooseError("getValue of HardeningLaw should be overridden");
  return 0.0;
}

// ====================

LinearHardening::LinearHardening(Real initial_yield_stress, 
                                 Real hardening_modulus
)
  : HardeningLaw(),
  _initial_yield_stress(initial_yield_stress),
  _hardening_modulus(hardening_modulus)
{
}

Real
LinearHardening::getValue(Real plastic_multiplier)
{
  return _initial_yield_stress + _hardening_modulus * plastic_multiplier;
}

Real 
LinearHardening::getDerivative(Real plastic_multiplier)
{
  return _hardening_modulus;
}

// ====================

PowerLawHardening::PowerLawHardening(Real initial_yield_stress, 
                                     Real hardening_modulus, 
                                     Real hardening_exponent
)
  : HardeningLaw(),
  _initial_yield_stress(initial_yield_stress),
  _hardening_modulus(hardening_modulus),
  _hardening_exponent(hardening_exponent)
{
}

Real
PowerLawHardening::getValue(Real plastic_multiplier)
{
  return _initial_yield_stress * std::pow(
    1. + _hardening_modulus / _initial_yield_stress * plastic_multiplier, 
    _hardening_exponent);
}

Real 
PowerLawHardening::getDerivative(Real plastic_multiplier)
{
  return _hardening_modulus * _hardening_exponent * std::pow(
    1. + _hardening_modulus / _initial_yield_stress * plastic_multiplier,
     _hardening_exponent - 1.);
}

// ====================

SwiftHardening::SwiftHardening(Real initial_yield_stress, 
                               Real hardening_modulus, 
                               Real hardening_exponent
)
  : HardeningLaw(),
  _initial_yield_stress(initial_yield_stress),
  _modulus(hardening_modulus),
  _exponent(hardening_exponent)
{
}

Real
SwiftHardening::getValue(Real plastic_multiplier)
{
  return _modulus * std::pow(
    std::pow(_initial_yield_stress/_modulus, 1./_exponent)
    + plastic_multiplier, _exponent);
}

Real 
SwiftHardening::getDerivative(Real plastic_multiplier)
{
  return _exponent * _modulus * std::pow(
    std::pow(_initial_yield_stress/_modulus, 1.0/_exponent)
    + plastic_multiplier, _exponent - 1.0);
}

// ====================

VariablePowerLawHardening::VariablePowerLawHardening(
  const OptionalMaterialProperty<Real> & initial_yield_stress,
  const OptionalMaterialProperty<Real> & hardening_modulus,
  const OptionalMaterialProperty<Real> & hardening_exponent,
  const unsigned int & qp
)
  : HardeningLaw(),
  _initial_yield_stress(initial_yield_stress),
  _hardening_modulus(hardening_modulus),
  _hardening_exponent(hardening_exponent),
  _qp(qp),

  _check_coupled_properties(true)
{
}

Real
VariablePowerLawHardening::getValue(Real plastic_multiplier)
{
  if (_check_coupled_properties)
    checkCoupledProperties();

  return _initial_yield_stress[_qp] * std::pow(
    1. + _hardening_modulus[_qp] / _initial_yield_stress[_qp] * plastic_multiplier, 
    _hardening_exponent[_qp]);
}

Real 
VariablePowerLawHardening::getDerivative(Real plastic_multiplier)
{
  return _hardening_modulus[_qp] * _hardening_exponent[_qp] * std::pow(
    1. + _hardening_modulus[_qp] / _initial_yield_stress[_qp] * plastic_multiplier,
     _hardening_exponent[_qp] - 1.);
}

void 
VariablePowerLawHardening::checkCoupledProperties()
{
  if (!_initial_yield_stress)
    mooseError("`initial_yield_stress` property is not coupled");
  if (!_hardening_modulus)
    mooseError("`hardening_modulus` property is not coupled");
  if (!_hardening_exponent)
    mooseError("`hardening_exponent` property is not coupled");
  
  _check_coupled_properties = false;
}