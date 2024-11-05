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

private:

  // Phase fractions
  const MaterialProperty<Real> & _frac_f,
                               & _frac_p,
                               & _frac_b,
                               & _frac_m,
                               & _frac_a;

  /// A helper object for performaing linear interpolations on tabulated data for calculating the
  /// diffusivity property.
  //LinearInterpolation _piecewise_func;

};