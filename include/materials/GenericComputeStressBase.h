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

#include "ComputeStressBase.h"
#include "ADComputeStressBase.h"
#include "RankFourTensor.h"

namespace lynx {

/**
 * Template for generic ComputeStressBase models
 */
template <bool is_ad>
class GenericComputeStressBase : public std::conditional<is_ad, ADComputeStressBase, ComputeStressBase>::type
{
public:
  GenericComputeStressBase(const InputParameters & params);

protected:
  void setJacobian(const GenericRankFourTensor<is_ad> & jacobian);
};

} // end namespace
