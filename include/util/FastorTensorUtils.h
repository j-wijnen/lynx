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

#include <Fastor/Fastor.h>
#include "RankTwoTensor.h"

/**
* Provides additional tensor definitions and functions
* for the MOOSE tensor classes
*/

namespace lynx 
{
namespace FastorTensorUtils
{

// Aliases
using FTensor2 = Fastor::Tensor<Real,3,3>;
using FTensor4 = Fastor::Tensor<Real,3,3,3,3>;
using Fastor::einsum;
template<size_t... indices> using In = Fastor::Index<indices...>;
template<size_t... indices> using Out = Fastor::OIndex<indices...>;
using Fastor::inner;
using Fastor::trans;
using Fastor::inv;
using Fastor::trace;
using Fastor::evaluate;

// Conversions between tensor implementations
template <class Tout, class Tin>
inline Tout convertRankTwoTensor(const Tin& tensor)
{
  Tout result;

  for (size_t i : make_range(3))
    for (size_t j : make_range(3))
      result(i,j) = tensor(i,j);
    
  return result;
}

template <class Tout, class Tin>
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

/**
 * Unit tensors
 * (Higher order unit tensors are defined for double
 *  inner product a = A_ij B_ij)
 */

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
      result(i,j,i,j) = 1.0;

  return result;
}();

const static FTensor4 I4RT = Fastor::trans(I4);

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

const static FTensor4 I4d = I4 - I2I2 / 3.0;
const static FTensor4 I4Sd = I4S - I2I2 / 3.0;

/**
 * Linear algebra functions
 */ 

// double contraction
// inline auto ddot(const FTensor2& A, const FTensor2 & B)
// {
//   std::cout << "function 1 is used\n";
//   return einsum<In<0,1>,In<0,1>>(A, B);
// }

// template<typename Derived0, typename Derived1>
// inline auto ddot(const Fastor::AbstractTensor<Derived0,2> & A, const Fastor::AbstractTensor<Derived1,2> & B)
// {
//   std::cout << "function 2 is used\n";
//   return einsum<In<0,1>,In<0,1>>(A, B);
// }

// inline auto ddot(const FTensor4 & A, const FTensor2 & B)
// {
//   return einsum<Fastor::Index<0,1,2,3>,Fastor::Index<2,3>>(A, B);
// }

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

}} // end