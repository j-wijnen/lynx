#include "IADHenckyStress.h"
#include "RankFourTensor.h"
#include "RankTwoTensor.h"
#include "ADTensorUtils.h"

registerMooseObject("LynxApp", IADHenckyStress);

InputParameters
IADHenckyStress::validParams()
{
  InputParameters params = ComputeLagrangianStressPK1::validParams();
  params.addClassDescription("This material computes the PK1 stress for Hencky elasticity.");

  return params;
}

IADHenckyStress::IADHenckyStress(
    const InputParameters & parameters
)
  : ComputeLagrangianStressPK1(parameters),

  // Consumed properties
  _elasticity_tensor(getMaterialProperty<RankFourTensor>(_base_name + "elasticity_tensor"))
{
  if (!_large_kinematics)
    mooseError("large_kinematics needs to be set to true");
}

void IADHenckyStress::computeQpPK1Stress()
{
  ADRankTwoTensor F = _F[_qp];
  initRankTwoTensorDerivatives(F);

  // Left Cauchy-green tensor
  ADRankTwoTensor B = F * F.transpose();

  // Eigenvalues/vectors
  std::vector<ADReal> eigvals;
  ADRankTwoTensor eigvecs;
  B.symmetricEigenvaluesEigenvectors(eigvals, eigvecs);

  // Assemble hencky strain
  ADRankTwoTensor strain;
  for(auto i : make_range(3))
    for(auto j : make_range(3))
      strain(i,j) = 0.5 * std::log(eigvals[0]) * eigvecs(i,0) * eigvecs(j,0) 
        + 0.5 * std::log(eigvals[1]) * eigvecs(i,1) * eigvecs(j,1)
        + 0.5 * std::log(eigvals[2]) * eigvecs(i,2) * eigvecs(j,2);

  // Stress measures
  ADRankTwoTensor kirchhoff = _elasticity_tensor[_qp] * strain;
  ADRankTwoTensor pk1 = kirchhoff * F.transpose().inverse();

  _pk1_stress[_qp] = getADRankTwoTensorValues(pk1);
  _pk1_jacobian[_qp] = getRankTwoRankTwoDerivative(pk1);
}