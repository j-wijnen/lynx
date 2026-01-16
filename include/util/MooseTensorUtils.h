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

#include "ADReal.h"
#include "RankTwoTensor.h"
#include "RankFourTensor.h"

/**
* Provides additional tensor definitions and functions
* for the MOOSE tensor classes
*/

namespace lynx 
{
namespace MooseTensorUtils
{

// ================
// Identity tensors
// ================

const RankTwoTensor Identity(RankTwoTensor::initIdentity);
const RankFourTensor IdentityFour(RankFourTensor::initIdentityFour);
const RankFourTensor IdentityFourSym(RankFourTensor::initIdentitySymmetricFour);
const RankFourTensor IdentityTwoTwo = RankTwoTensor::Identity().outerProduct(RankTwoTensor::Identity());
const RankFourTensor IdentityFourDev = IdentityFour - IdentityTwoTwo / 3.;
const RankFourTensor IdentityFourSymDev = IdentityFourSym - IdentityTwoTwo / 3.;

// ============
// Dot products
// ============

// dot

template<typename T>
inline RankTwoTensorTempl<T> dot(const RankTwoTensorTempl<T> & A, const RankTwoTensorTempl<T> & B)
{
  RankTwoTensorTempl<T> result;
  for (size_t i = 0; i < 3; ++i)
    for (size_t j = 0; j < 3; ++j)
      for (size_t k = 0; k < 3; ++k)
        result(i,j) += A(i,k) * B(k,j);
  return result;
}

template<typename T>
inline RankFourTensorTempl<T> dot(const RankFourTensorTempl<T> & A, const RankTwoTensorTempl<T> & B)
{
  RankFourTensorTempl<T> result;
  for (size_t i = 0; i < 3; ++i)
    for (size_t j = 0; j < 3; ++j)
      for (size_t k = 0; k < 3; ++k)
        for (size_t l = 0; l < 3; ++l)
          for (size_t m = 0; m < 3; ++m)
            result(i,j,k,l) += A(i,j,k,m) * B(m,l);
  return result;
}

template<typename T>
inline RankFourTensorTempl<T> dot(const RankTwoTensorTempl<T> & A, const RankFourTensorTempl<T> & B)
{
  RankFourTensorTempl<T> result;
  for (size_t i = 0; i < 3; ++i)
    for (size_t j = 0; j < 3; ++j)
      for (size_t k = 0; k < 3; ++k)
        for (size_t l = 0; l < 3; ++l)
          for (size_t m = 0; m < 3; ++m)
            result(i,j,k,l) += A(i,m) * B(m,j,k,l);
  return result;
}

// ==============
// Outer products
// ==============

// outer

inline RankFourTensorTempl<T> outer(const RankTwoTensorTempl<T> & A, const RankTwoTensorTempl<T> & B)
{
  RanktFourTensorTempl<T> result(RankFourTensorTempl<T>::initNone);
  for (size_t i = 0; i < 3; ++i)
    for (size_t j = 0; j < 3; ++j)
      for (size_t k = 0; k < 3; ++k)
        for (size_t l = 0; l < 3; ++l)
          result(i,j,k,l) = A(i,k) * B(j,l);
  return result;
}

template<size_t i, size_t j, size_t k, size_t l, typename T>
inline RankFourTensorTempl<T> outer(const RankTwoTensorTempl<T> & A, const RankTwoTensorTempl<T> & B)
{
  RankFourTensorTempl<T> result(RankFourTensorTempl<T>::initNone);
  size_t idx[4];
  for (idx[0] = 0; idx[0] < 3; ++idx[0])
    for (idx[1] = 0; idx[1] < 3; ++idx[1])
      for (idx[2] = 0; idx[2] < 3; ++idx[2])
        for (idx[3] = 0; idx[3] < 3; ++idx[3])
          result(idx[0],idx[1],idx[2],idx[3]) = A(idx[i],idx[j]) * B(idx[k],idx[l]);
  return result;
}

// outerIdentity

template<size_t i, size_t j, size_t k, size_t l, typename T>
inline RankFourTensorTempl<T> outerIdentity(const RankTwoTensorTempl<T> & A)
{
  RankFourTensorTempl<T> result(RankFourTensorTempl<T>::initNone);
  size_t idx[4];
  for (idx[0] = 0; idx[0] < 3; ++idx[0])
    for (idx[1] = 0; idx[1] < 3; ++idx[1])
      for (idx[2] = 0; idx[2] < 3; ++idx[2])
        for (idx[3] = 0; idx[3] < 3; ++idx[3])
          if (idx[k] == idx[l])
            result(idx[0],idx[1],idx[2],idx[3]) = A(idx[i],idx[j]);
          else 
            result(idx[0],idx[1],idx[2],idx[3]) = 0.0;
  return result;
}

// ==========================
// Invariants and derviatives
// ==========================

// det

inline T det(const RankTwoTensorTempl<T> & A)
{
  return A(0,0)*A(1,1)*A(2,2) + A(0,1)*A(1,2)*A(2,0) + A(0,2)*A(1,0)*A(2,1)
       - A(0,2)*A(1,1)*A(2,0) - A(0,1)*A(1,0)*A(2,2) - A(0,0)*A(1,2)*A(2,1);
}

// ===============================
// Automatic differentiation utils
// ===============================

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
  RankTwoTensor B(RankTwoTensor::initNone);

  for(auto i : make_range(3))
    for(auto j : make_range(3))
      B(i,j) = A(i,j).value();

  return B;
}

// Function calculating dA^-1/dA
template<typename T>
inline RankFourTensorTempl<T> dinverse(const RankTwoTensorTempl<T>& Ai)
{
  RankFourTensorTempl<T> result(RankFourTensorTempl<T>::initNone);

  for (size_t i : make_range(3))
    for (size_t j : make_range(3))
      for (size_t k : make_range(3))
        for (size_t l : make_range(3))
          result(i,j,k,l) = - Ai(i,k) * Ai(l,j);

  return result;
}

}} // end namespace