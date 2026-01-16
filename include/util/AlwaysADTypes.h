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

/**
* Provides additional tensor definitions and functions
* for the MOOSE tensor classes
*/

#include "ADReal.h"
#include "RankTwoTensorForward.h"

namespace lynx 
{

typedef DualNumber<Real, DNDerivativeType, /*allow_skipping_derivatives=*/false> AADReal;

typedef RankTwoTensorTempl<AADReal> AADRankTwoTensor;

} // end namespace