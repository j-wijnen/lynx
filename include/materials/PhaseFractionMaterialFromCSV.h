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

#include "Material.h"
#include "LinearInterpolation.h"

namespace lynx 
{

/** 
 * Phase and temperature dependent properties
 * Reads from a single or multiple CSV files
 */ 

template<bool is_ad>
class PhaseFractionMaterialFromCSVTempl : public Material 
{
public:
  static InputParameters validParams();

  PhaseFractionMaterialFromCSVTempl(
    const InputParameters& params 
  );

protected:
  virtual void readPropertiesMultipleFiles();

  virtual void readPropertiesSingleFile();

  virtual void computeQpProperties() override;

  virtual void computeQpPropertiesMultipleFiles();

  virtual void computeQpPropertiesSingleFile();

  const std::vector<std::string> _prop_names;
  const std::vector<std::string> _prop_files;

  const GenericVariableValue<is_ad> & _temperature;

  // Material properties to be defined
  std::vector<GenericMaterialProperty<Real, is_ad> *> _properties;

  // Phase fractions
  const MaterialProperty<Real> & _fraction_f;
  const MaterialProperty<Real> & _fraction_p;
  const MaterialProperty<Real> & _fraction_b;
  const MaterialProperty<Real> & _fraction_m;
  const MaterialProperty<Real> & _fraction_a;

  // Save phase/temperature dependent properties
  std::vector<std::vector<Real>> _ferrite;
  std::vector<std::vector<Real>> _pearlite;
  std::vector<std::vector<Real>> _bainite;
  std::vector<std::vector<Real>> _martensite;
  std::vector<std::vector<Real>> _austenite;

  // Create interpolation type based on is_ad
  typedef typename std::conditional<is_ad,ADLinearInterpolation,LinearInterpolation>::type 
    GenericLinearInterpolation;

  std::vector<GenericLinearInterpolation> _piecewise_funcs;

};

typedef PhaseFractionMaterialFromCSVTempl<false> PhaseFractionMaterialFromCSV;
typedef PhaseFractionMaterialFromCSVTempl<true> ADPhaseFractionMaterialFromCSV;

}