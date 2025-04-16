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

class PowerLawHardening : public HardeningLaw
{
public:
  PowerLawHardening(Real yield_stress, 
                    Real youngs_modulus, 
                    Real hardening_exponent);

  virtual Real getValue(Real hardening_parameter) override;

  virtual Real getDerivative(Real hardening_parameter) override;

protected:
  const Real _youngs_modulus;
  const Real _hardening_exponent;

};