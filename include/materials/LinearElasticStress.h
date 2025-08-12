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

#include "GenericComputeStressBase.h"
#include "RankFourTensor.h"

namespace lynx {

/**
 * LinearElasticStress computes the linear elastic stress
 */
template <bool is_ad>
class LinearElasticStressTempl : public GenericComputeStressBase<is_ad>
{
public:

  static InputParameters validParams();

  LinearElasticStressTempl(const InputParameters & params);

protected:

  virtual void computeQpStress() override;

  // Consumed properties
  const GenericMaterialProperty<RankFourTensor, is_ad> & _elasticity_tensor;

  using GenericComputeStressBase<is_ad>::_base_name;
  using GenericComputeStressBase<is_ad>::_mechanical_strain;
  using GenericComputeStressBase<is_ad>::_elastic_strain;
  using GenericComputeStressBase<is_ad>::_stress;
  using GenericComputeStressBase<is_ad>::_qp;
  using GenericComputeStressBase<is_ad>::setJacobian;
};

typedef LinearElasticStressTempl<false> LinearElasticStress;
typedef LinearElasticStressTempl<true> ADLinearElasticStress;

};