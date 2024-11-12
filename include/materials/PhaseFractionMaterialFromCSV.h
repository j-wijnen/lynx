#pragma once

#include "Material.h"

#include "LinearInterpolation.h"


class PhaseFractionMaterialFromCSV : public Material 
{

public:

  static InputParameters validParams();

  PhaseFractionMaterialFromCSV(
    const InputParameters& params 
  );

protected:

  virtual void computeQpProperties() override;

  const std::vector<std::string> _prop_names,
                                 _prop_files;

  const VariableValue & _temperature;

  // Material properties to be defined
  std::vector<MaterialProperty<Real> *> _properties;

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

  std::vector<LinearInterpolation> _piecewise_funcs;

};