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

#include "HardeningBase.h"

namespace lynx
{

template <bool is_ad>
InputParameters
HardeningBase<is_ad>::validParams()
{
  InputParameters params = Material::validParams();
  params.set<bool>("compute") = false;
  return params;
}

template <bool is_ad>
HardeningBase<is_ad>::HardeningBase(const InputParameters & params)
  : Material(params)
{
}

template <bool is_ad>
void
HardeningBase<is_ad>::coupleQp(unsigned int * qp)
{
  _qp = qp;
}

template <bool is_ad>
void
HardeningBase<is_ad>::computeProperties()
{
  // Material does not have any properties
}

template <bool is_ad>
void
HardeningBase<is_ad>::resetProperties()
{
  // Material does not have any properties
}

template <bool is_ad>
GenericReal<is_ad>
HardeningBase<is_ad>::getYieldStress(GenericReal<is_ad> plastic_multiplier)
{
  mooseError("getYieldStress should be overriden");
  return 0.0;
}

template <bool is_ad>
GenericReal<is_ad> 
HardeningBase<is_ad>::getDerivative(GenericReal<is_ad> plastic_multiplier)
{
  mooseError("getDerivative should be overriden");
  return 0.0;
}

} // end namespace