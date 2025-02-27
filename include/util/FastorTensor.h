#pragma once

#include <Fastor/Fastor.h>
#include "RankTwoTensor.h"

// Aliases
using FTensor2 = Fastor::Tensor<Real,3,3>;
using FTensor4 = Fastor::Tensor<Real,3,3,3,3>;

// Conversions between tensor implementations
template<class Tout, class Tin>
inline Tout convertRankTwoTensor(const Tin& tensor)
{
  Tout result;

  for (size_t i : make_range(3))
    for (size_t j : make_range(3))
      result(i,j) = tensor(i,j);
    
  return result;
}

template<class Tout, class Tin>
inline Tout convertRankFourTensor(const Tin& tensor)
{
  Tout result;

  for (size_t i : make_range(3))
    for (size_t j : make_range(3))
      for (size_t k : make_range(3))
        for (size_t l : make_range(3))
          result(i,j,k,l) = tensor(i,j,k,l);
    
  return result;
}

// Unit tensors
const static FTensor2 I2 = []
{
  Fastor::Tensor<double,3,3> result =
    {{1.0, 0.0, 0.0},
     {0.0, 1.0, 0.0},
     {0.0, 0.0, 1.0}};

  return result;
}();

const static FTensor4 I4 = []
{
  FTensor4 result;
  result.zeros();

  for (size_t i : make_range(3))
    for (size_t j : make_range(3))
      result(i,j,j,i) = 1.0;

  return result;
}();

const static FTensor4 I4RT = Fastor::transpose(I4);

const static FTensor4 I4S = 0.5 * ( I4 + I4RT );

const static FTensor4 I2I2 = []
{
  FTensor4 result;
  result.zeros();

  for (size_t i : make_range(3))
    for (size_t j : make_range(3))
      result(i,i,j,j) = 1.0;

  return result;
} ();


// Function calculating dA^-1/dA
inline FTensor4 dAinvdA(const FTensor2& Ainv)
{
  FTensor4 result;
  result.zeros();

  for (size_t i : make_range(3))
    for (size_t j : make_range(3))
      for (size_t k : make_range(3))
        for (size_t l : make_range(3))
          result(i,j,k,l) = - Ainv(i,k) * Ainv(l,j);

  return result;
}

// Function calculating dA^-T/dA
inline FTensor4 dAinvTdA(const FTensor2& Ainv)
{
  FTensor4 result;
  result.zeros();

  for (size_t i : make_range(3))
    for (size_t j : make_range(3))
      for (size_t k : make_range(3))
        for (size_t l : make_range(3))
          result(i,j,k,l) = - Ainv(l,i) * Ainv(j,k);

  return result;
}

// Function calculating dA^-1/dA for symmetric A
FTensor4 dAinvdAS(const FTensor2& Ainv)
{
  FTensor4 result;
  result.zeros();

  for (size_t i : make_range(3))
    for (size_t j : make_range(3))
      for (size_t k : make_range(3))
        for (size_t l : make_range(3))
          result(i,j,k,l) = -0.5 * (Ainv(i,k) * Ainv(j,l) + Ainv(i,l) * Ainv(j,k));

  return result;
}