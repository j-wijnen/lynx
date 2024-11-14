#pragma once

#include "Material.h"

#include "LinearInterpolation.h"


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

  const std::vector<std::string> _prop_names,
                                 _prop_files;

  const GenericVariableValue<is_ad> & _temperature;

  // Material properties to be defined
  std::vector<GenericMaterialProperty<Real, is_ad> *> _properties;

  // Phase fractions
  const MaterialProperty<Real> & _xf,
                               & _xp,
                               & _xb,
                               & _xm,
                               & _xa;

  // Save phase/temperature dependent properties
  std::vector<std::vector<Real>> _ferrite,
                                 _pearlite,
                                 _bainite,
                                 _martensite,
                                 _austenite;

  // Create interpolation type based on is_ad
  typedef typename std::conditional<is_ad,ADLinearInterpolation,LinearInterpolation>::type 
    GenericLinearInterpolation;

  std::vector<GenericLinearInterpolation> _piecewise_funcs;

};


typedef PhaseFractionMaterialFromCSVTempl<false> PhaseFractionMaterialFromCSV;
typedef PhaseFractionMaterialFromCSVTempl<true> ADPhaseFractionMaterialFromCSV;