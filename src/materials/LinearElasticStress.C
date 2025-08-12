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

#include "LinearElasticStress.h"
#include "RankFourTensor.h"
#include "RankTwoTensor.h"

namespace lynx 
{

registerMooseObject("LynxApp", LinearElasticStress);
registerMooseObject("LynxApp", ADLinearElasticStress);

template <bool is_ad>
InputParameters
LinearElasticStressTempl<is_ad>::validParams()
{
  InputParameters params = GenericComputeStressBase<is_ad>::validParams();
  params.addClassDescription("This material computes the Cauchy stress for small strain elasticity.");
  return params;
}

template <bool is_ad>
LinearElasticStressTempl<is_ad>::LinearElasticStressTempl(
    const InputParameters & parameters
)
  : GenericComputeStressBase<is_ad>(parameters),

  // Consumed properties
  _elasticity_tensor(this->template getGenericMaterialProperty<RankFourTensor, is_ad>(_base_name + "elasticity_tensor"))
{
}

template <bool is_ad>
void LinearElasticStressTempl<is_ad>::computeQpStress()
{
  _elastic_strain[_qp] = _mechanical_strain[_qp];
  _stress[_qp] = _elasticity_tensor[_qp] * _elastic_strain[_qp];
  setJacobian(_elasticity_tensor[_qp]);
}

};