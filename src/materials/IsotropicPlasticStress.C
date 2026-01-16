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

#include "IsotropicPlasticStress.h"
#include "ElasticityTensorTools.h"
#include "MooseEnum.h"
#include "MooseTensorUtils.h"
#include "HardeningBase.h"
#include "NoHardening.h"

using ElasticityTensorTools::getIsotropicShearModulus;
using MetaPhysicL::raw_value;
using lynx::MooseTensorUtils::IdentityFourSymDev;
using lynx::MooseTensorUtils::IdentityTwoTwo;

namespace lynx
{

registerMooseObject("LynxApp", IsotropicPlasticStress);
registerMooseObject("LynxApp", ADIsotropicPlasticStress);

template <bool is_ad>
InputParameters
IsotropicPlasticStressTempl<is_ad>::validParams()
{
  InputParameters params = GenericComputeStressBase<is_ad>::validParams();
  params.addClassDescription("This material computes the stress for small strain"
    "isotropic von Mises plasticity.");
  
  params.addParam<MaterialName>("hardening_material", "Name of the material providing the hardening law");
  params.addParam<Real>("tolerance", 1e-8, "Tolerance of the return map algorithm");
  params.addParam<Real>("yield_stress", "Yield stress if no hardening material is provided");

  return params;
}

template <bool is_ad>
IsotropicPlasticStressTempl<is_ad>::IsotropicPlasticStressTempl(const InputParameters & params)
  : GenericComputeStressBase<is_ad>(params),
    GuaranteeConsumer(this),

  // Consumed material properties
  _elasticity_tensor(this->template getGenericMaterialProperty<RankFourTensor, is_ad>(_base_name + "elasticity_tensor")),

  // Declared material properties
  _plastic_strain(this->template declareGenericProperty<RankTwoTensor, is_ad>(_base_name + "plastic_strain")),
  _plastic_multiplier(this->template declareGenericProperty<Real, is_ad>(_base_name + "plastic_multiplier")),
  _yield_stress(this->template declareGenericProperty<Real, is_ad>(_base_name + "yield_stress")),

  // Stateful properties
  _plastic_strain_old(this->template getMaterialPropertyOld<RankTwoTensor>(_base_name + "plastic_strain")),
  _plastic_multiplier_old(this->template getMaterialPropertyOld<Real>(_base_name + "plastic_multiplier")),

  // // Yield parameters
  _tolerance(this->template getParam<Real>("tolerance"))
{
  // No hardening material provided > create NoHardening object
  if (!params.isParamSetByUser("hardening_material"))
  {
    _no_hardening = true;

    if (!params.isParamSetByUser("yield_stress"))
      mooseError("`yield_stress` is not set and no hardening material is provided");

    _hardening_law = dynamic_cast<HardeningBase<is_ad> *>(new NoHardeningTempl<is_ad>(params));
  }
  // Hardening material is provided
  else
  {
    _no_hardening = false;

    if (params.isParamSetByUser("yield_stress"))
      mooseError("`yield_stress` should be set in hardening material");
  }
}

template <bool is_ad>
IsotropicPlasticStressTempl<is_ad>::~IsotropicPlasticStressTempl()
{
  if (_no_hardening)
    delete _hardening_law;
}

template <bool is_ad>
void IsotropicPlasticStressTempl<is_ad>::initialSetup()
{
  GenericComputeStressBase<is_ad>::initialSetup();

  // Check if the material is isotropic
  if(!hasGuaranteedMaterialProperty(_base_name + "elasticity_tensor", Guarantee::ISOTROPIC))
    mooseError("The elasticity tensor must be isotropic.");

  // Obtain hardening material if provided
  if (!_no_hardening)
  {
    const MaterialName & hardening_material = this->template getParam<MaterialName>("hardening_material");
    _hardening_law = dynamic_cast<HardeningBase<is_ad> *>(&this->template getMaterialByName(hardening_material));
    _hardening_law->coupleQp(&_qp);
  }
 }

template <bool is_ad>
void
IsotropicPlasticStressTempl<is_ad>::initQpStatefulProperties()
{
  GenericComputeStressBase<is_ad>::initQpStatefulProperties();

  _plastic_strain[_qp].zero();
  _plastic_multiplier[_qp] = 0.0;
}

template <bool is_ad>
void 
IsotropicPlasticStressTempl<is_ad>::computeQpStress()
{
  GenericReal<is_ad> shear_modulus = getIsotropicShearModulus(_elasticity_tensor[_qp]);

  // Elastic trial state
  _elastic_strain[_qp] = _mechanical_strain[_qp] - _plastic_strain_old[_qp];
  _stress[_qp] = _elasticity_tensor[_qp] * _elastic_strain[_qp];
  GenericRankFourTensor<is_ad> jacobian = _elasticity_tensor[_qp];

  GenericRankTwoTensor<is_ad> stress_dev = _stress[_qp].deviatoric();
  GenericReal<is_ad> stress_eq = _sqrt32 * std::sqrt(stress_dev.doubleContraction(stress_dev));

  // Check for yielding
  _yield_stress[_qp] = _hardening_law->getYieldStress(_plastic_multiplier[_qp]);
  if(stress_eq > _yield_stress[_qp])
  {
    // Return map
    GenericReal<is_ad> dplastic_mult = computeReturnMap(stress_eq);

    // Update strains, stress, jacobian
    GenericRankTwoTensor<is_ad> N = _sqrt32 * stress_dev / stress_eq;

    _plastic_strain[_qp] = _plastic_strain_old[_qp] + _sqrt32 * dplastic_mult * N;
    _plastic_multiplier[_qp] = _plastic_multiplier_old[_qp] + dplastic_mult;
    _elastic_strain[_qp] = _mechanical_strain[_qp] - _plastic_strain[_qp];

    _stress[_qp] = _elasticity_tensor[_qp] * _elastic_strain[_qp];

    jacobian += 6. * shear_modulus*shear_modulus * (
      - dplastic_mult / stress_eq * IdentityFourSymDev
      + (dplastic_mult / stress_eq + 1. / computeReturnDerivative(dplastic_mult)) 
      * N.outerProduct(N));
  }

  setJacobian(jacobian);
}

template <bool is_ad>
GenericReal<is_ad>
IsotropicPlasticStressTempl<is_ad>::computeReturnMap(GenericReal<is_ad> trial_stress)
{
  GenericReal<is_ad> dplastic_mult;
  dplastic_mult = 0.0;
  GenericReal<is_ad> dplastic_mult_prev;
  GenericReal<is_ad> r;
  int iter = 0;

  while(true)
  {
    _yield_stress[_qp] = _hardening_law->getYieldStress(_plastic_multiplier_old[_qp] + dplastic_mult);
    r = computeReturnResidual(trial_stress, dplastic_mult);

    if (abs(r) < _tolerance * _yield_stress[_qp])
      return dplastic_mult;
    else if (iter > 30)
      mooseException("Plastic return map not converging");

    dplastic_mult_prev = dplastic_mult;
    dplastic_mult -= r / computeReturnDerivative(dplastic_mult); 

    // Prevent negative plastic multiplier
    if (dplastic_mult < 0.0)
      dplastic_mult = 0.5 * dplastic_mult_prev;

    ++iter;
  }
}

template <bool is_ad>
GenericReal<is_ad>
IsotropicPlasticStressTempl<is_ad>::computeReturnResidual(GenericReal<is_ad> trial_stress, 
                                                          GenericReal<is_ad> dplastic_mult)
{
  return trial_stress - 3. * getIsotropicShearModulus(_elasticity_tensor[_qp]) * dplastic_mult
    - _yield_stress[_qp];
}

template <bool is_ad>
GenericReal<is_ad>
IsotropicPlasticStressTempl<is_ad>::computeReturnDerivative(GenericReal<is_ad> dplastic_mult)
{
  return - 3. * getIsotropicShearModulus(_elasticity_tensor[_qp])
    - _hardening_law->getDerivative(_plastic_multiplier_old[_qp] + dplastic_mult);
}

} // end namespace