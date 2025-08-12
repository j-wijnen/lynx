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

#include "DerivativeMaterialInterface.h"
#include "Material.h"

namespace lynx
{

/**
 * Compute material degradation functions based on the damage parameter
 */
template <bool is_ad>
class PFFDegradationFunctionTempl : public DerivativeMaterialInterface<Material>
{
public:

  static InputParameters validParams();

  PFFDegradationFunctionTempl(const InputParameters & params);

protected:

  virtual void computeQpProperties() override;

  // Coupled damage variable
  const std::string _u_name;
  const GenericVariableValue<is_ad> & _u;

  // Declared properteis
  GenericMaterialProperty<Real, is_ad> & _g;
  GenericMaterialProperty<Real, is_ad> & _dg_du;
  GenericMaterialProperty<Real, is_ad> & _d2g_du2;

  // Parameters
  Real _epsilon;
};

typedef PFFDegradationFunctionTempl<false> PFFDegradationFunction;
typedef PFFDegradationFunctionTempl<true> ADPFFDegradationFunction;

} // end namespace