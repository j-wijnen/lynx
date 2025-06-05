#pragma once

/*
* Provides some additional tensor definitions and functions
*/

#include "ADReal.h"
#include "RankTwoTensor.h"
#include "RankFourTensor.h"

// Identity tensors
RankTwoTensor Identity = RankTwoTensor::Identity();
RankFourTensor IdentityFour = RankFourTensor::IdentityFour();
RankFourTensor IdentityFourSym = 0.5*(IdentityFour + IdentityFour.transposeKl());
RankFourTensor IdentityTwoTwo = RankTwoTensor::Identity().outerProduct(RankTwoTensor::Identity());
RankFourTensor IdentityFourDev = IdentityFourSym - IdentityTwoTwo / 3.;

// Automatic differentiation utils
inline void initRankTwoTensorDerivatives(ADRankTwoTensor& A)
{
  for(auto i : make_range(3))
    for(auto j : make_range(3))
      Moose::derivInsert(A(i,j).derivatives(), 3*i + j, 1.0);
}

inline RankFourTensor getRankTwoRankTwoDerivative(const ADRankTwoTensor& A)
{
  RankFourTensor dA;

  for(auto i : make_range(3))
    for(auto j : make_range(3))
      for(auto k : make_range(3))
        for(auto l : make_range(3))
          dA(i,j,k,l) = A(i,j).derivatives()[k*3 + l];

  return dA;
}

inline RankTwoTensor getADRankTwoTensorValues(const ADRankTwoTensor& A)
{
  RankTwoTensor B;

  for(auto i : make_range(3))
    for(auto j : make_range(3))
      B(i,j) = A(i,j).value();

  return B;
}