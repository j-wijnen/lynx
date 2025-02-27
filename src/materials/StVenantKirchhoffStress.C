#include "StVenantKirchhoffStress.h"
#include "RankFourTensor.h"
#include "RankTwoTensor.h"
#include "FastorTensor.h"

using Fastor::trans;
using Fastor::einsum;
using Fastor::Index;

registerMooseObject("LynxApp", StVenantKirchhoffStress);

InputParameters
StVenantKirchhoffStress::validParams()
{
  InputParameters params = ComputeLagrangianStressPK1::validParams();
  params.addClassDescription("This material computes the Cauchy stress for small strain elasticity.");
  return params;
}

StVenantKirchhoffStress::StVenantKirchhoffStress(
    const InputParameters & parameters
)
  : ComputeLagrangianStressPK1(parameters),

  // Consumed properties
  _elasticity_tensor(getMaterialProperty<RankFourTensor>(_base_name + "elasticity_tensor"))
{
  mooseAssert(_large_kinematics, "large_kinematics needs to be set to true");
}

void StVenantKirchhoffStress::computeQpPK1Stress()
{
  FTensor2 F = convertRankTwoTensor<FTensor2,RankTwoTensor>(_F[_qp]);
  FTensor4 C4 = convertRankFourTensor<FTensor4,RankFourTensor>(_elasticity_tensor[_qp]);

  // Compute the stress
  FTensor2 E = 0.5 * (trans(F) % F - I2);
  FTensor2 S = einsum<Index<1,2,3,4>,Index<4,3>>(C4, E);

  // Push forward to PK1 stress
  FTensor2 P = F % S;

  // Push forward Jacobian
  // J = I4RT : (S . I4) +  F . C4 :(F^T . I4RT)
  FTensor4 J4 = einsum<Index<1,2,3,4>,Index<4,5>,Index<5,3,6,7>>(
      I4RT, S, I4)
    + einsum<Index<1,2>,Index<2,3,4,5>,Index<6,5>,Index<6,4,7,8>>(
      F, C4, F, I4RT);

  _pk1_stress[_qp] = convertRankTwoTensor<RankTwoTensor,FTensor2>(P);
  _pk1_jacobian[_qp] = convertRankFourTensor<RankFourTensor,FTensor4>(J4);
}