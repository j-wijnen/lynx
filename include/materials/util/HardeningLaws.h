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

#pragma once

#include "Moose.h"
#include "MaterialProperty.h"

/**
 * Implements multiple hardening laws.
 * These hardening laws are used by IsotropicPlasticStress
 * and child classes.
 */

class HardeningLaw
{
public:
  HardeningLaw();

  virtual ~HardeningLaw() = default;

  virtual Real getValue(Real plastic_multiplier);

  virtual Real getDerivative(Real plastic_multiplier);

protected:
  
 };

/**
 * Linear hardening: sy = h * eps
 */

class LinearHardening : public HardeningLaw
{
public:
  LinearHardening(Real initial_yield_stress, 
                  Real hardening_modulus);

  virtual Real getValue(Real plastic_multiplier) override;

  virtual Real getDerivative(Real plastic_multiplier) override;

protected:
  const Real _initial_yield_stress;
  const Real _hardening_modulus;
};

/**
 * Power-law hardening: sy = sy0 * (1 + h * eps / sy0 )^n
 */

class PowerLawHardening : public HardeningLaw
{
public:
  PowerLawHardening(Real initial_yield_stress, 
                    Real hardening_modulus, 
                    Real hardening_exponent);

  virtual Real getValue(Real plastic_multiplier) override;

  virtual Real getDerivative(Real plastic_multiplier) override;

protected:
  const Real _initial_yield_stress;
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
  SwiftHardening(Real initial_yield_stress, 
                 Real hardening_modulus, 
                 Real hardening_exponent);

  virtual Real getValue(Real plastic_multiplier) override;

  virtual Real getDerivative(Real plastic_multiplier) override;

protected:
  const Real _initial_yield_stress;
  const Real _modulus;
  const Real _exponent;

};

/**
 * Variable power-law hardening: sy = sy0 * (1 + h * eps / sy0 )^n
 */

class VariablePowerLawHardening : public HardeningLaw
{
public:
  VariablePowerLawHardening(const OptionalMaterialProperty<Real> & initial_yield_stress,
                            const OptionalMaterialProperty<Real> & hardening_modulus,
                            const OptionalMaterialProperty<Real> & hardening_exponent,
                            const unsigned int & qp);

  virtual Real getValue(Real plastic_multiplier) override;

  virtual Real getDerivative(Real plastic_multiplier) override;

protected:
  const OptionalMaterialProperty<Real> & _initial_yield_stress;
  const OptionalMaterialProperty<Real> & _hardening_modulus;
  const OptionalMaterialProperty<Real> & _hardening_exponent;
  const unsigned int & _qp;

  virtual void checkCoupledProperties();
  bool _check_coupled_properties;
};

/**
 * Variable swifthardening: 
 * (rewritten to use sy0 and initial h) 
 * sy = h * ((sy0/h)^(1/n) + eps)^n
 */

class VariableSwiftHardening : public HardeningLaw
{
public:
  VariableSwiftHardening(const OptionalMaterialProperty<Real> & initial_yield_stress,
                            const OptionalMaterialProperty<Real> & hardening_modulus,
                            const OptionalMaterialProperty<Real> & hardening_exponent,
                            const unsigned int & qp);

  virtual Real getValue(Real plastic_multiplier) override;

  virtual Real getDerivative(Real plastic_multiplier) override;

protected:
  const OptionalMaterialProperty<Real> & _initial_yield_stress;
  const OptionalMaterialProperty<Real> & _hardening_modulus;
  const OptionalMaterialProperty<Real> & _hardening_exponent;
  const unsigned int & _qp;

  virtual void checkCoupledProperties();
  bool _check_coupled_properties;
};

