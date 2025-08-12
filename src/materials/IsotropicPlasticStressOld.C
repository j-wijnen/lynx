#include "IsotropicPlasticStressOld.h"
#include "ElasticityTensorTools.h"
#include "MooseEnum.h"
#include "MooseTensorUtils.h"
#include "HardeningBase.h"
#include "NoHardening.h"

using ElasticityTensorTools::getIsotropicShearModulus;

namespace lynx
{

registerMooseObject("LynxApp", IsotropicPlasticStressOld);

InputParameters
IsotropicPlasticStressOld::validParams()
{
  InputParameters params = ComputeStressBase::validParams();
  params.addClassDescription("This material computes the stress for small strain"
    "isotropic von Mises plasticity.");
  
  params.addParam<MaterialName>("hardening_material", "Name of the material providing the hardening law");
  params.addParam<Real>("tolerance", 1e-8, "Tolerance of the return map algorithm");
  params.addParam<Real>("yield_stress", "Yield stress if no hardening material is provided");

  return params;
}

IsotropicPlasticStressOld::IsotropicPlasticStressOld(
    const InputParameters & params
)
  : ComputeStressBase(params),
    GuaranteeConsumer(this),

  // Consumed material properties
  _elasticity_tensor(getMaterialProperty<RankFourTensor>(_base_name + "elasticity_tensor")),

  // Declared material properties
  _plastic_strain(declareProperty<RankTwoTensor>(_base_name + "plastic_strain")),
  _plastic_multiplier(declareProperty<Real>(_base_name + "plastic_multiplier")),
  _yield_stress(declareProperty<Real>(_base_name + "yield_stress")),

  // Stateful properties
  _plastic_strain_old(getMaterialPropertyOld<RankTwoTensor>(_base_name + "plastic_strain")),
  _plastic_multiplier_old(getMaterialPropertyOld<Real>(_base_name + "plastic_multiplier")),

  // // Yield parameters
  _tolerance(getParam<Real>("tolerance"))
{
  // No hardening material provided > create NoHardening object
  if (!params.isParamSetByUser("hardening_material"))
  {
    _no_hardening = true;

    if (!params.isParamSetByUser("yield_stress"))
      mooseError("`yield_stress` is not set and no hardening material is provided");

    _hardening_law = dynamic_cast<HardeningBase<false> *>(new NoHardening(params));
  }
  // Hardening material is provided
  else
  {
    _no_hardening = false;

    if (params.isParamSetByUser("yield_stress"))
      mooseError("`yield_stress` should be set in hardening material");
  }
}

IsotropicPlasticStressOld::~IsotropicPlasticStressOld()
{
  if (_no_hardening)
    delete _hardening_law;
}

void IsotropicPlasticStressOld::initialSetup()
{
  ComputeStressBase::initialSetup();

  // Check if the material is isotropic
  if(!hasGuaranteedMaterialProperty(_base_name + "elasticity_tensor", Guarantee::ISOTROPIC))
    mooseError("The elasticity tensor must be isotropic.");

  // Obtain hardening material if provided
  if (!_no_hardening)
  {
    const MaterialName & hardening_material = getParam<MaterialName>("hardening_material");
    _hardening_law = dynamic_cast<HardeningBase<false> *>(&getMaterialByName(hardening_material));
    _hardening_law->coupleQp(&_qp);
  }
 }

void
IsotropicPlasticStressOld::initQpStatefulProperties()
{
  ComputeStressBase::initQpStatefulProperties();

  _plastic_strain[_qp].zero();
  _plastic_multiplier[_qp] = 0.0;
}

void 
IsotropicPlasticStressOld::computeQpStress()
{
  Real shear_modulus = getIsotropicShearModulus(_elasticity_tensor[_qp]);

  // Elastic trial state
  _elastic_strain[_qp] = _mechanical_strain[_qp] - _plastic_strain_old[_qp];
  _stress[_qp] = _elasticity_tensor[_qp] * _elastic_strain[_qp];
  _Jacobian_mult[_qp] = _elasticity_tensor[_qp];

  RankTwoTensor stress_dev = _stress[_qp].deviatoric();
  Real stress_eq = _sqrt32 * std::sqrt(stress_dev.doubleContraction(stress_dev));

  // Check for yielding
  _yield_stress[_qp] = _hardening_law->getYieldStress(_plastic_multiplier[_qp]);
  if(stress_eq > _yield_stress[_qp])
  {
    // Return map
    Real dplastic_mult = computeReturnMap(stress_eq);

    // Update strains, stress, jacobian
    RankTwoTensor N = _sqrt32 * stress_dev / stress_eq;

    _plastic_strain[_qp] = _plastic_strain_old[_qp] + _sqrt32 * dplastic_mult * N;
    _plastic_multiplier[_qp] = _plastic_multiplier_old[_qp] + dplastic_mult;
    _elastic_strain[_qp] = _mechanical_strain[_qp] - _plastic_strain[_qp];

    _stress[_qp] = _elasticity_tensor[_qp] * _elastic_strain[_qp];

    _Jacobian_mult[_qp] += 6. * shear_modulus*shear_modulus * (
      - dplastic_mult / stress_eq * IdentityFourDev
      + (dplastic_mult / stress_eq + 1. / computeReturnDerivative(dplastic_mult)) 
      * N.outerProduct(N));
  }
}

Real
IsotropicPlasticStressOld::computeReturnMap(Real trial_stress)
{
  Real dplastic_mult = 0.0;
  Real dplastic_mult_prev;
  Real r;
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

Real
IsotropicPlasticStressOld::computeReturnResidual(Real trial_stress, 
                                              Real dplastic_mult)
{
  return trial_stress - 3. * getIsotropicShearModulus(_elasticity_tensor[_qp]) * dplastic_mult
    - _yield_stress[_qp];
}

Real
IsotropicPlasticStressOld::computeReturnDerivative(Real dplastic_mult)
{
  return - 3. * getIsotropicShearModulus(_elasticity_tensor[_qp])
    - _hardening_law->getDerivative(_plastic_multiplier_old[_qp] + dplastic_mult);
}

} // end namespace