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

#include "IsotropicPlasticStress.h"

namespace lynx {

/**
 * Isotropic plasticity including TRIP deformation based on
 * phase fractions
 */

class IsotropicPlasticTRIPStress : public IsotropicPlasticStress
{
public:
  static InputParameters validParams();

  IsotropicPlasticTRIPStress(const InputParameters & parameters);

protected:
  virtual void initQpStatefulProperties() override;

  virtual void computeQpStress() override;

  // Consumed properties
  const MaterialProperty<Real> & _fraction_f;
  const MaterialProperty<Real> & _fraction_p;
  const MaterialProperty<Real> & _fraction_b;
  const MaterialProperty<Real> & _fraction_m;

  // Stateful properties
  const MaterialProperty<Real> & _fraction_f_old;
  const MaterialProperty<Real> & _fraction_p_old;
  const MaterialProperty<Real> & _fraction_b_old;
  const MaterialProperty<Real> & _fraction_m_old;

  // Parameters
  const Real _trip_parameter_f;
  const Real _trip_parameter_p;
  const Real _trip_parameter_b;
  const Real _trip_parameter_m;
};

} // End namespace
