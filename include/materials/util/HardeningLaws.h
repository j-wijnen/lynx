#pragma once

#include "Moose.h"

/**
 * Implements multiple hardening laws.
 */

class HardeningLaw
{
public:
  HardeningLaw(Real yield_stress);

  virtual ~HardeningLaw() = default;

  virtual Real getValue(Real hardening_parameter);

  virtual Real getDerivative(Real hardening_parameter);

protected:
  const Real _yield_stress0;
 };

/**
 * Linear hardening: sy = h * eps
 */

class LinearHardening : public HardeningLaw
{
public:
  LinearHardening(Real yield_stress, 
                  Real hardening_modulus);

  virtual Real getValue(Real hardening_parameter) override;

  virtual Real getDerivative(Real hardening_parameter) override;

protected:
  const Real _hardening_modulus;
};

/**
 * Power-law hardening: sy = sy0 * (1 + h * eps / sy0 )^n
 */

class PowerLawHardening : public HardeningLaw
{
public:
  PowerLawHardening(Real yield_stress, 
                    Real hardening_modulus, 
                    Real hardening_exponent);

  virtual Real getValue(Real hardening_parameter) override;

  virtual Real getDerivative(Real hardening_parameter) override;

protected:
  const Real _hardening_modulus;
  const Real _hardening_exponent;

};

/**
 * Swift hardening:
 * (rewritten to use sy0 and initial h) 
 * sy = h * ((sy0/h)^(1/n) + eps)^n
 */

class SwiftHardening : public HardeningLaw
{
public:
  SwiftHardening(Real yield_stress, 
                    Real hardening_modulus, 
                    Real hardening_exponent);

  virtual Real getValue(Real hardening_parameter) override;

  virtual Real getDerivative(Real hardening_parameter) override;

protected:
  const Real _modulus;
  const Real _exponent;

};