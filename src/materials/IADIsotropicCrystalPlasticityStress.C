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

#include "IADIsotropicCrystalPlasticityStress.h"
#include "RankFourTensor.h"
#include "RankTwoTensor.h"
#include "MooseTensorUtils.h"

using lynx::MooseTensorUtils::Identity;
using lynx::MooseTensorUtils::IdentityFourDev;
using lynx::MooseTensorUtils::initRankTwoTensorDerivatives;
using lynx::MooseTensorUtils::getADRankTwoTensorValues;
using lynx::MooseTensorUtils::getRankTwoRankTwoDerivative;

namespace lynx
{

registerMooseObject("LynxApp", IADIsotropicCrystalPlasticityStress);

InputParameters
IADIsotropicCrystalPlasticityStress::validParams()
{
  InputParameters params = ComputeLagrangianStressPK1::validParams();
  params.addClassDescription("This material computes isotropic plasticity"
    "with similar implementation as crystal plasticity");

  params.addRequiredParam<Real>("lamdot0", "Reference plastic strain rate");
  params.addRequiredParam<Real>("rate_sensitivity", "The visco-plastic rate sensitivity");
  params.addParam<Real>("rel_tol", 1e-4, "Relative tolerance for convergence");
  params.addParam<Real>("abs_tol", 1e-8, "Absolute tolerance for convergence");

  return params;
}

IADIsotropicCrystalPlasticityStress::IADIsotropicCrystalPlasticityStress(
    const InputParameters & parameters
)
  : ComputeLagrangianStressPK1(parameters),

  // Declared properties
  _Fp(declareProperty<RankTwoTensor>(_base_name + "Fp")),
  _lamtot(declareProperty<Real>(_base_name + "lamtot")),

  // Stateful properties
  _Fp_old(getMaterialPropertyOld<RankTwoTensor>(_base_name + "Fp")),
  _lamtot_old(getMaterialPropertyOld<Real>(_base_name + "lamtot")),

  // Consumed properties
  _elasticity_tensor(getMaterialProperty<RankFourTensor>(_base_name + "elasticity_tensor")),

  _lamdot0(getParam<Real>("lamdot0")),
  _yieldstress0(200.0),
  _rate_sensitivity(getParam<Real>("rate_sensitivity")),
  _rel_tol(1e-3),
  _abs_tol(1e-8),
  _lamdot_idx(9)
{
  if (!_large_kinematics)
    mooseError("large_kinematics needs to be set to true");
}

void IADIsotropicCrystalPlasticityStress::initQpStatefulProperties()
{
  ComputeLagrangianStressPK1::initQpStatefulProperties();

  _Fp[_qp].setToIdentity();
  _lamtot[_qp] = 0.0;
}

void IADIsotropicCrystalPlasticityStress::computeQpPK1Stress()
{
  // For auto differentiation of jacobian
  ADRankTwoTensor F = _F[_qp];
  initRankTwoTensorDerivatives(F);

  // For auto differentiation of internal loop
  ADReal lamdot = 0.0;
  Moose::derivInsert(lamdot.derivatives(), _lamdot_idx, _rate_sensitivity);

  // Temporarily set do_derivatives to true
  const bool do_derivatives_old = ADReal::do_derivatives;
  ADReal::do_derivatives = true;

  // Trial stress for flow direction
  ADRankTwoTensor Fe = F * _Fp_old[_qp].inverse();
  ADRankTwoTensor Ce = Fe.transpose() * Fe;
  ADRankTwoTensor Ee = 0.5 * (Ce - Identity);
  ADRankTwoTensor Se = _elasticity_tensor[_qp] * Ee;
  ADRankTwoTensor M = Ce * Se;
  ADRankTwoTensor Md = IdentityFourDev * M;
  ADRankTwoTensor N = Md / std::sqrt(Md.doubleContraction(Md));

  // Other variables used in NR loop
  ADRankTwoTensor Dp, Fp;
  ADReal Meq, r;
  size_t iter = 0;

  // Solve for lamdot
  while (true)
  {
    // Plastic increment
    Dp = std::sqrt(3./2.) * lamdot * N;
    Fp = (Identity + _dt * Dp) * _Fp_old[_qp]; // 1st order taylor of exp
    //Fp = (Identity - 0.5 * _dt * Dp).inverse() * (Identity + 0.5 * _dt * Dp) * _Fp_old[_qp]; // 1st order pade of exp

    // Intermediate 2nd PK stress
    Fe = F * Fp.inverse();
    Ce = Fe.transpose() * Fe;
    Ee = 0.5 * (Ce - Identity);
    Se = _elasticity_tensor[_qp] * Ee;

    // Mandel stress
    M = Ce * Se;
    Md = IdentityFourDev * M;
    Meq = std::sqrt(3./2. * Md.doubleContraction(Md));

    // Convergence check
    r = lamdot - _lamdot0 * std::pow(Meq / _yieldstress0, 1./_rate_sensitivity);

    if ((lamdot > 0.0 && std::abs(r) < _rel_tol * std::abs(lamdot)) || std::abs(r) < _abs_tol)
    {
      break;
    }
    else if (iter == 100)
    {
      mooseError();
    }

    // Update lamdot
    lamdot -= r / r.derivatives()[_lamdot_idx];;
    lamdot.derivatives()[_lamdot_idx] = 1.0;
    iter++;
  }

  // Reset do_derivatives
  ADReal::do_derivatives = do_derivatives_old;

  // Compute PK1 stress
  ADRankTwoTensor pk1 = Fe * Se * _Fp_old[_qp].inverse().transpose();

  _pk1_stress[_qp] = getADRankTwoTensorValues(pk1);
  _pk1_jacobian[_qp] = getRankTwoRankTwoDerivative(pk1);

  // Save state
  _Fp[_qp] = getADRankTwoTensorValues(Fp);
  _lamtot[_qp] = _lamtot_old[_qp] + _dt * lamdot.value();
}

} // end namespace