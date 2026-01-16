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

#include "IsotropicCrystalPlasticityStress.h"
#include "RankFourTensor.h"
#include "RankTwoTensor.h"
#include "MooseTensorUtils.h"

using namespace lynx::MooseTensorUtils;

namespace lynx
{

registerMooseObject("LynxApp", IsotropicCrystalPlasticityStress);

InputParameters
IsotropicCrystalPlasticityStress::validParams()
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

IsotropicCrystalPlasticityStress::IsotropicCrystalPlasticityStress(
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

void IsotropicCrystalPlasticityStress::initQpStatefulProperties()
{
  ComputeLagrangianStressPK1::initQpStatefulProperties();

  _Fp[_qp].setToIdentity();
  _lamtot[_qp] = 0.0;
}

void IsotropicCrystalPlasticityStress::computeQpPK1Stress()
{

  // Temporarily set do_derivatives to true
  const bool do_derivatives_old = ADReal::do_derivatives;
  ADReal::do_derivatives = true;

  enum {i,j,k,l,m};
  
  // Trial stress for Flow direction
  RankTwoTensor F;
  F(0,0) = 1.01;
  F(1,1) = 0.9950371902099892;
  F(2,2) = 0.9950371902099892;
  RankTwoTensor Fpi = _Fp_old[_qp].inverse();
  RankTwoTensor Fe = F * Fpi;
  RankTwoTensor Ce = Fe.transpose() * Fe;
  RankTwoTensor Ee = 0.5 * (Ce - Identity);
  RankTwoTensor Se = _elasticity_tensor[_qp] * Ee;
  RankTwoTensor M = Ce * Se;
  RankTwoTensor Md = IdentityFourDev * M;
  RankTwoTensor N = Md / std::sqrt(Md.doubleContraction(Md));

  // Other tensors used in NR loop
  Real Meq, dMeq_dlamdot;
  RankTwoTensor Dp, Fp, dDp_dlamdot, dMeq_dMd;
  RankFourTensor dFp_dDp, dFpi_dFp, dFe_dFp, dCe_dFe, dM_dCe;

  // Iteration variables
  Real lamdot = 0.0;
  Real r, dr_dlamdot;
  size_t iter = 0;

  // Solve for lamdot
  while (true)
  {
    // Plastic increment
    Dp = std::sqrt(3./2.) * lamdot * N;
    Fp = (Identity + _dt * Dp) * _Fp_old[_qp];
    Fpi = Fp.inverse();

    // Intermediate 2nd PK stress
    Fe = F * Fpi;
    Ce = Fe.transpose() * Fe;
    Ee = 0.5 * (Ce - Identity);
    Se = _elasticity_tensor[_qp] * Ee;

    // Mandel stress
    M = Ce * Se;
    Md = M - M.trace() / 3.0 * Identity;
    Meq = std::sqrt(3.0/2.0 * Md.doubleContraction(Md));

    // Residual
    r = lamdot - _lamdot0 * std::pow(Meq / _yieldstress0, 1./_rate_sensitivity);

    if ((lamdot > 0.0 && std::abs(r) < _rel_tol * std::abs(lamdot)) || std::abs(r) < _abs_tol)
    {
      std::cout << "Converged in " << iter << std::endl;
      break;
    }
    else if (iter == 100)
    {
      mooseError();
    }

    // Derivatives plastic increment
    dDp_dlamdot = std::sqrt(3./2.) * N;
    dFp_dDp = _dt * _Fp_old[_qp].times<l,j,i,k>(Identity);

    // Derivatives 2nd PK stress
    dFpi_dFp = - outer<i,k,l,j>(Fpi, Fpi);
    dFe_dFp = _F[_qp].times<i,m,m,j,k,l>(dFpi_dFp);
    dCe_dFe = outerIdentity<k,i,j,l>(Fe) + outerIdentity<k,j,i,l>(Fe);

    // Derivative Mandel stress
    dM_dCe = outerIdentity<l,j,i,k>(Se) + 0.5 * outerIdentity<i,k,j,l>(Ce) * _elasticity_tensor[_qp];
    dMeq_dMd = 3.0 * Md / (2.0 * Meq);

    // Link derivatives
    dMeq_dlamdot = dMeq_dMd.doubleContraction(IdentityFourDev * dM_dCe * dCe_dFe * dFe_dFp * dFp_dDp * dDp_dlamdot);

    // Update lamdot
    dr_dlamdot = 1.0 - _lamdot0 / (_yieldstress0 * _rate_sensitivity) 
      * std::pow(Meq / _yieldstress0, 1./_rate_sensitivity - 1.0) * dMeq_dlamdot;
    lamdot -= r / dr_dlamdot;
    
    ++iter;

  }

  // // Reset do_derivatives
  // ADReal::do_derivatives = do_derivatives_old;

  // // Compute PK1 stress
  // ADRankTwoTensor pk1 = Fe * Se * _Fp_old[_qp].inverse().transpose();

  // _pk1_stress[_qp] = getADRankTwoTensorValues(pk1);
  // _pk1_jacobian[_qp] = getRankTwoRankTwoDerivative(pk1);

  // // Save state
  // _Fp[_qp] = getADRankTwoTensorValues(Fp);
  // _lamtot[_qp] = _lamtot_old[_qp] + _dt * lamdot.value();
}

} // end namespace
