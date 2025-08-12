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

#include "GenericComputeStressBase.h"

#include "ComputeStressBase.h"
#include "ADComputeStressBase.h"
#include "RankFourTensor.h"

namespace lynx
{
  
template <bool is_ad>
GenericComputeStressBase<is_ad>::GenericComputeStressBase(const InputParameters & params)
  : std::conditional<is_ad, ADComputeStressBase, ComputeStressBase>::type(params)
{
}

template <bool is_ad>
void
GenericComputeStressBase<is_ad>::setJacobian(const GenericRankFourTensor<is_ad> & jacobian)
{
}

template <>
void
GenericComputeStressBase<false>::setJacobian(const RankFourTensor & jacobian)
{
  _Jacobian_mult[_qp] = jacobian;
}

}