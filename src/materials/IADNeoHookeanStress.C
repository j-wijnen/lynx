#include "IADNeoHookeanStress.h"
#include "RankFourTensor.h"
#include "RankTwoTensor.h"
#include "FastorTensorUtils.h"
#include "MooseTensorUtils.h"

using Fastor::trace;
using Fastor::det;
using Fastor::inv;
using Fastor::outer;
using Fastor::trans;

registerMooseObject("LynxApp", IADNeoHookeanStress);

InputParameters
IADNeoHookeanStress::validParams()
{
  InputParameters params = ComputeLagrangianStressPK1::validParams();
  params.addClassDescription("This material computes the PK1 stress for NeoHookean elasticity.");

  params.addRequiredParam<Real>(
    "shear_modulus", "The shear modulus (G) of the material");
  params.addRequiredParam<Real>(
    "bulk_modulus", "The shear modulus (K) of the material");
  return params;
}

IADNeoHookeanStress::IADNeoHookeanStress(
    const InputParameters & parameters
)
  : ComputeLagrangianStressPK1(parameters),

  // parameters
  _shearmod(getParam<Real>("shear_modulus")),
  _bulkmod(getParam<Real>("bulk_modulus"))
{
  if (!_large_kinematics)
    mooseError("large_kinematics needs to be set to true");
}

void IADNeoHookeanStress::computeQpPK1Stress()
{
  ADRankTwoTensor F = _F[_qp];
  initRankTwoTensorDerivatives(F);

  // Right Cauchy-green tensor
  ADRankTwoTensor C = F.transpose() * F;

  // Invariants
  ADReal J = F.det();
  ADReal Jinv23 = 1.0 / std::pow(J, 2.0/3.0);
  ADReal I1 = C.trace();
  ADReal I1b = Jinv23 * I1;
 
  // Def grads
  ADRankTwoTensor Fb = Jinv23 * F;
  ADRankTwoTensor FinvT = F.inverse().transpose();
 
  // 1st order derivatives of invariants
  ADRankTwoTensor dJ = F.ddet();
  ADRankTwoTensor dI1b = 2.0 * Fb - 2./3. * I1b * FinvT;

  // PK1 Stress
  ADRankTwoTensor P = 0.5 * _shearmod * dI1b + 0.5 * _bulkmod * (J - 1.0/J) * dJ;

  _pk1_stress[_qp] = getADRankTwoTensorValues(P);
  _pk1_jacobian[_qp] = getRankTwoRankTwoDerivative(P);
}