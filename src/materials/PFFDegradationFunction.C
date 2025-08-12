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

#include "PFFDegradationFunction.h"

namespace lynx
{

registerMooseObject("LynxApp", PFFDegradationFunction);
registerMooseObject("LynxApp", ADPFFDegradationFunction);

template <bool is_ad>
InputParameters
PFFDegradationFunctionTempl<is_ad>::validParams()
{
  InputParameters params = DerivativeMaterialInterface<Material>::validParams();
  params.addClassDescription("Compute phase-field fracture degradation functions");

  // Coupled variable
  params.addRequiredCoupledVar("variable", "Name of the damage variable.");

  // Declared property names
  params.addParam<MaterialPropertyName>("pff_degradation_name", "pff_degradation",
                                        "The name of the material degradation property.");

  // Parameters
  params.addParam<Real>("epsilon", 1e-7, "Constant for numerical stability.");

  return params;
}

template <bool is_ad>
PFFDegradationFunctionTempl<is_ad>::PFFDegradationFunctionTempl(
    const InputParameters & params
)
  : DerivativeMaterialInterface<Material>(params),

  // Coupled damage variable
  _u_name(coupledName("variable")),
  _u(coupledGenericValue<is_ad>("variable")),

  // Degradation function
  _g(declareGenericProperty<Real, is_ad>("pff_degradation_name")),
  _dg_du(declarePropertyDerivative<Real, is_ad>("pff_degradation", _u_name)),
  _d2g_du2(declarePropertyDerivative<Real, is_ad>("pff_degradation", _u_name, _u_name)),
  
  // Parameters
  _epsilon(getParam<Real>("epsilon"))
{
}

template <bool is_ad>
void
PFFDegradationFunctionTempl<is_ad>::computeQpProperties()
{
  GenericReal<is_ad> u_term = (1.0 - _u[_qp]);
  Real e_term = (1.0 - _epsilon);

  _g[_qp] = u_term * u_term * e_term + _epsilon;
  _dg_du[_qp] = -2.0 * u_term * e_term;
  _d2g_du2[_qp] =  2.0 * e_term;
}

} // close namespace