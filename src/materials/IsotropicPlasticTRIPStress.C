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

#include "IsotropicPlasticTRIPStress.h"
#include "ElasticityTensorTools.h"
#include "MooseTensorUtils.h"

using ElasticityTensorTools::getIsotropicShearModulus;
using ElasticityTensorTools::getIsotropicBulkModulus;

namespace lynx {

registerMooseObject("LynxApp", IsotropicPlasticTRIPStress);

InputParameters
IsotropicPlasticTRIPStress::validParams()
{
  InputParameters params = IsotropicPlasticStress::validParams();
  params.addClassDescription("Isotropic plasticity including TRIP deformation" 
    "based on phase fractions");

  params.addParam<Real>("trip_parameter_ferrite", 0.0, "TRIP parameter for ferrite");
  params.addParam<Real>("trip_parameter_pearlite", 0.0, "TRIP parameter for pearlite");
  params.addParam<Real>("trip_parameter_bainite", 0.0, "TRIP parameter for bainite");
  params.addParam<Real>("trip_parameter_martensite", 0.0, "TRIP parameter for martensite");

  return params;
}

IsotropicPlasticTRIPStress::IsotropicPlasticTRIPStress(const InputParameters & parameters)
  : IsotropicPlasticStress(parameters),

  // Consumed properties
  _fraction_f(getMaterialProperty<Real>("fraction_ferrite")),
  _fraction_p(getMaterialProperty<Real>("fraction_pearlite")),
  _fraction_b(getMaterialProperty<Real>("fraction_bainite")),
  _fraction_m(getMaterialProperty<Real>("fraction_martensite")),

  // Stateful properties
  _fraction_f_old(getMaterialPropertyOld<Real>("fraction_ferrite")),
  _fraction_p_old(getMaterialPropertyOld<Real>("fraction_pearlite")),
  _fraction_b_old(getMaterialPropertyOld<Real>("fraction_bainite")),
  _fraction_m_old(getMaterialPropertyOld<Real>("fraction_martensite")),

  // Parameters
  _trip_parameter_f(getParam<Real>("trip_parameter_ferrite")),
  _trip_parameter_p(getParam<Real>("trip_parameter_pearlite")),
  _trip_parameter_b(getParam<Real>("trip_parameter_bainite")),
  _trip_parameter_m(getParam<Real>("trip_parameter_martensite"))
{
}

void
IsotropicPlasticTRIPStress::initQpStatefulProperties()
{
  IsotropicPlasticStress::initQpStatefulProperties();
}

void 
IsotropicPlasticTRIPStress::computeQpStress()
{
  Real shear_modulus = getIsotropicShearModulus(_elasticity_tensor[_qp]);

  // TRIP contribution part independent on stress
  Real trip_factor = 3. * _trip_parameter_f * (1. - _fraction_f[_qp]) * (_fraction_f[_qp] - _fraction_f_old[_qp])
    + 3. * _trip_parameter_p * (1. - _fraction_p[_qp]) * (_fraction_p[_qp] - _fraction_p_old[_qp])
    + 3. * _trip_parameter_b * (1. - _fraction_b[_qp]) * (_fraction_b[_qp] - _fraction_b_old[_qp])
    + 3. * _trip_parameter_m * (1. - _fraction_m[_qp]) * (_fraction_m[_qp] - _fraction_m_old[_qp]);

  // Elastic trial state
  _elastic_strain[_qp] = _mechanical_strain[_qp] - _plastic_strain_old[_qp];
  _stress[_qp] = _elasticity_tensor[_qp] * _elastic_strain[_qp];
  _Jacobian_mult[_qp] = _elasticity_tensor[_qp];

  // Calculate phase fractions
  RankTwoTensor stress_dev = _stress[_qp].deviatoric();
  Real stress_tr = _sqrt32 * std::sqrt(stress_dev.doubleContraction(stress_dev));
  RankTwoTensor N = _sqrt32 * stress_dev / stress_tr;

  // Calculate TRIP contribution
  Real dplastic_mult = 0.0;
  Real stress_eq = stress_tr;
  if (trip_factor > 0.0)
  {
    stress_eq = stress_tr / (1.0 + 2.0 * shear_modulus * trip_factor);
    dplastic_mult = 2./3. * trip_factor * stress_eq;
  }

  // Check for conventional yielding
  // Classical return map (reverts TRIP contribution)
  _yield_stress[_qp] = _hardening->getValue(_effective_plastic_strain_old[_qp] + dplastic_mult);
  if(stress_eq > _yield_stress[_qp])
  {
    dplastic_mult = computeReturnMap(stress_tr);

    _Jacobian_mult[_qp] += 6. * shear_modulus*shear_modulus * (
      - dplastic_mult / stress_tr * IdentityFourDev
      + (dplastic_mult / stress_tr + 1. / computeReturnDerivative(dplastic_mult)) 
      * N.outerProduct(N));
  }
  // Jacobian update for TRIP effect
  else if (dplastic_mult > 0.0) // Jacobian update for TRIP
  {
    _Jacobian_mult[_qp] -= 4. * shear_modulus*shear_modulus * trip_factor
      / (1.0 + 2.0 * shear_modulus * trip_factor) * IdentityFourDev;
  }

  // Update strains and stress
  if (dplastic_mult > 0.0)
  {
    _plastic_strain[_qp] = _plastic_strain_old[_qp] + _sqrt32 * dplastic_mult * N;
    _effective_plastic_strain[_qp] = _effective_plastic_strain_old[_qp] + dplastic_mult;
    _elastic_strain[_qp] = _mechanical_strain[_qp] - _plastic_strain[_qp];

    _stress[_qp] = _elasticity_tensor[_qp] * _elastic_strain[_qp];
  }
}

} // end namespace