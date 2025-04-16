#pragma once

/*
* Provides some additional tensor definitions and functions
*/

#include "RankTwoTensor.h"
#include "RankFourTensor.h"

// Identity tensors
RankTwoTensor Identity = RankTwoTensor::Identity();
RankFourTensor IdentityFour = RankFourTensor::IdentityFour();
RankFourTensor IdentityFourSym = 0.5*(IdentityFour + IdentityFour.transposeKl());
RankFourTensor IdentityTwoTwo = RankTwoTensor::Identity().outerProduct(RankTwoTensor::Identity());
RankFourTensor IdentityFourDev = IdentityFourSym - IdentityTwoTwo / 3.;