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

#include <array>
#include "Material.h"

namespace lynx
{

/**
 * Enum used to index/loop phases 
 */ 

enum Phase {
  ferrite,
  pearlite,
  bainite,
  martensite,
  austenite
};

const std::array<Phase,5> phases = {
  ferrite, 
  pearlite, 
  bainite, 
  martensite, 
  austenite
};

/**
 * SSPTSteel simulates solid-state phase transformations
 * and calculates underlying phase fractions of ferrite,
 * pearlite, bainite, martensite, and austenite
 */

class SSPTSteel : public Material 
{
public:
  static InputParameters validParams();

  SSPTSteel(const InputParameters& params);

protected:
  virtual void initQpStatefulProperties() override;

  virtual void computeQpProperties() override;

  // Austenite transformation using LeBlond equation
  Real austeniteTransformation();

  Real grainGrowth();

  // Diffusive transformation using Kirkaldy equations
  std::tuple<Real,Real> diffusiveTransformation(Phase phase,
                                                Real x,
                                                Real nuc);

  Real diffusiveTransformationResidual(Real x0,
                                       Real x,
                                       Real dt,
                                       Real fun_tc);

  Real funTc(Phase phase, Real temp);

  // Calculates the incremenet in martensite upon heating using the
  // Koistenen-Marburger equation
  Real martensiteTransformation();

  // Checks whether temperature is in transformation range
  bool checkTemperatureRange(Phase phase);

  // Reduces the time increment to be between lower and upper bounds
  // returns (T_split, dt_split)
  std::tuple<Real,Real> splitIncrementHeating(Phase phase);
  std::tuple<Real,Real> splitIncrementCooldown(Phase phase);

  // Coupled variables
  const VariableValue & _temperature; 
  const VariableValue & _temperature_old;

  // Stateful material properties
  const std::array<MaterialProperty<Real> *,5> _fractions;
  const std::array<MaterialProperty<Real> *,3> _nucleation;
  MaterialProperty<Real> & _grain_size;

  const std::array<const MaterialProperty<Real> *,5> _fractions_old;
  const std::array<const MaterialProperty<Real> *,3> _nucleation_old;
  const MaterialProperty<Real> & _grain_size_old;

  // Initial phase fractions (input)
  std::array<Real,5> _fractions_init;

  // Composition (input)
  Real  _comp_C,
        _comp_Mn,
        _comp_Si,
        _comp_Ni,
        _comp_Cr,
        _comp_Mo,
        _comp_Co,
        _comp_V,
        _comp_W,
        _comp_As,
        _comp_Al,
        _comp_P,
        _comp_Ti,
        _comp_Cu;

  // Transformation temperatures
  Real _temperature_Ae3,
       _temperature_Ae1,
       _temperature_Bs,
       _temperature_Ms;

  std::array<Real,5>  _temperature_lower,
                      _temperature_upper;

  // Grain sizes (PAG)
  Real _grain_size_init,
       _grain_size_min,
       _grain_size_max;

  // Compositional functions
  std::array<Real,3> _fcomp;

  // Phase dependent constants
  const std::array<Real,3> _grain_size_factor = {0.41, 0.32, 0.29};
  const std::array<int,3> _ucool_exponent = {3, 3, 2};

  // Misc. constants
  const Real _tolerance = 1e-4;
};

}