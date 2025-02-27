#include "PFFElasticStress.h"
#include "RankTwoTensor.h"
#include "RankFourTensor.h"


registerMooseObject("LynxApp", PFFElasticStress);

InputParameters
PFFElasticStress::validParams()
{
  InputParameters params = ComputeStressBase::validParams();
  params.addClassDescription("Compute the phase-field fracture stress using elasticity for small strains");
  
  // Consumed property names
  params.addParam<MaterialPropertyName>("pff_degradation_name", "pff_degradation",
                                        "The name of the material degradation property.");

  // Declared property names
  params.addParam<MaterialPropertyName>("strain_energy_name", "strain_energy",
                                        "The name of the material degradation property.");
  return params;
}

PFFElasticStress::PFFElasticStress(
    const InputParameters & parameters
)
  : ComputeStressBase(parameters),

  // Consumed properties
  _elasticity_tensor(getMaterialProperty<RankFourTensor>(_base_name + "elasticity_tensor")),
  _g(getOptionalMaterialProperty<Real>("pff_degradation_name")),

  // Declared properties
  _strain_energy(declareProperty<Real>("strain_energy_name")),

  // Stateful properties
  _strain_energy_old(getMaterialPropertyOld<Real>("strain_energy_name"))
{
}

void
PFFElasticStress::initQpStatefulProperties()
{
  _strain_energy[_qp] = 0.0;
}

void
PFFElasticStress::computeQpStress()
{
  // Obtain degradation function
  Real g = _g ? _g[_qp] : 1.0;

  // stress and jacobian
  _elastic_strain[_qp] = _mechanical_strain[_qp];
  _stress[_qp] = g * _elasticity_tensor[_qp] * _elastic_strain[_qp];
  _Jacobian_mult[_qp] = g * _elasticity_tensor[_qp];

  // strain energy
  _strain_energy[_qp] = std::max(
    0.5 * (_elasticity_tensor[_qp] * _mechanical_strain[_qp]).doubleContraction(_mechanical_strain[_qp]),
    _strain_energy_old[_qp]
  ); 
}
