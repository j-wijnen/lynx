#include "LinearElasticStress.h"
#include "RankFourTensor.h"
#include "RankTwoTensor.h"

registerMooseObject("LynxApp", LinearElasticStress);

InputParameters
LinearElasticStress::validParams()
{
  InputParameters params = ComputeStressBase::validParams();
  params.addClassDescription("This material computes the Cauchy stress for small strain elasticity.");
  return params;
}

LinearElasticStress::LinearElasticStress(
    const InputParameters & parameters
)
  : ComputeStressBase(parameters),

  // Consumed properties
  _elasticity_tensor(getMaterialProperty<RankFourTensor>(_base_name + "elasticity_tensor"))
{
}

void LinearElasticStress::computeQpStress()
{
  _elastic_strain[_qp] = _mechanical_strain[_qp];
  _stress[_qp] = _elasticity_tensor[_qp] * _elastic_strain[_qp];
  _Jacobian_mult[_qp] = _elasticity_tensor[_qp];
}