#include "NeoHookeanStress.h"
#include "RankFourTensor.h"
#include "RankTwoTensor.h"
#include "FastorTensorUtils.h"
#include "MooseTensorUtils.h"

using Fastor::trace;
using Fastor::det;
using Fastor::inv;
using Fastor::outer;
using Fastor::trans;
using lynx::FastorTensorUtils::FTensor2;
using lynx::FastorTensorUtils::FTensor4;
using lynx::FastorTensorUtils::I2;
using lynx::FastorTensorUtils::I2I2;
using lynx::FastorTensorUtils::I4;
using lynx::FastorTensorUtils::I4RT;
using lynx::FastorTensorUtils::I4S;
using lynx::FastorTensorUtils::convertRankTwoTensor;
using lynx::FastorTensorUtils::convertRankFourTensor;
using lynx::FastorTensorUtils::dAinvTdA;

namespace lynx 
{

registerMooseObject("LynxApp", NeoHookeanStress);

InputParameters
NeoHookeanStress::validParams()
{
  InputParameters params = ComputeLagrangianStressPK1::validParams();
  params.addClassDescription("This material computes the PK1 stress for NeoHookean elasticity.");

  params.addRequiredParam<Real>(
    "shear_modulus", "The shear modulus (G) of the material");
  params.addRequiredParam<Real>(
    "bulk_modulus", "The shear modulus (K) of the material");
  return params;
}

NeoHookeanStress::NeoHookeanStress(
    const InputParameters & parameters
)
  : ComputeLagrangianStressPK1(parameters),

  // parameters
  _shearmod(getParam<Real>("shear_modulus")),
  _bulkmod(getParam<Real>("bulk_modulus"))
{
  mooseAssert(_large_kinematics, "large_kinematics needs to be set to true");
}

void NeoHookeanStress::computeQpPK1Stress()
{
  // Invariants
  Real J = _F[_qp].det()
  Real Jinv23 = 1.0 / std::pow(J, 2.0/3.0);
  Real I1 = trace(trans(F) % F);
  Real I1b = Jinv23 * I1;

  // Def grads
  FTensor2 Fb = Jinv23 * F;
  FTensor2 Finv = inv(F);
  FTensor2 FinvT = trans(Finv);

  // 1st order derivatives of invariants
  FTensor2 dJ = J * FinvT;
  FTensor2 dJinv23 = -2./3. * Jinv23 * FinvT;
  FTensor2 dI1 = 2.0 * F;
  FTensor2 dI1b = 2.0 * Fb - 2./3. * I1b * FinvT;

  // Derivatives of def grads
  FTensor4 dFinvT = dAinvTdA(Finv);
  FTensor4 dFb = Jinv23 * I4RT - 2./3. * outer(Fb, FinvT);

  // 2nd order derivatives of invariants
  FTensor4 d2J = outer(FinvT, dJ) + J * dFinvT;
  FTensor4 d2I1b = 2.0 * dFb - 2./3. * (outer(FinvT, dI1b) + I1b * dFinvT);

  // Stress and jacobian
  FTensor2 P = 0.5 * _shearmod * dI1b + 0.5 * _bulkmod * (J - 1.0/J) * dJ;
  FTensor4 J4 = 0.5 * _shearmod * d2I1b + 0.5 * _bulkmod * (
     (1.0 + 1.0/(J*J))*outer(dJ, dJ) + (J - 1.0/J) * d2J);

  _pk1_stress[_qp] = convertRankTwoTensor<RankTwoTensor,FTensor2>(P);
  _pk1_jacobian[_qp] = convertRankFourTensor<RankFourTensor,FTensor4>(J4);
}

} // end namespace