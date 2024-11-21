#include <algorithm>
#include "ComputePFFStress.h"
#include "RankTwoTensor.h"
#include "RankFourTensor.h"


registerMooseObject("SolidMechanicsApp", ComputePFFStress);

InputParameters
ComputePFFStress::validParams()
{
  InputParameters params = ComputeStressBase::validParams();
  params.addClassDescription("Compute phase-field fracture stress using elasticity for small strains");
  
  // Consumed property names
  params.addParam<MaterialPropertyName>("degradation_name", "degradation",
                                        "The name of the material degradation property.");

  // Declared property names
  params.addParam<MaterialPropertyName>("strain_energy_name", "strain_energy",
                                        "The name of the material degradation property.");
  return params;
}

ComputePFFStress::ComputePFFStress(
    const InputParameters & parameters
)
  : ComputeStressBase(parameters),

  // Consumed properties
  _elasticity_tensor(getMaterialProperty<RankFourTensor>(_base_name + "elasticity_tensor")),
  _degradation(getOptionalMaterialProperty<Real>("degradation_name")),

  // Declared properties
  _strain_energy(declareProperty<Real>("strain_energy_name")),

  // Stateful properties
  _strain_energy_old(getMaterialPropertyOld<Real>("strain_energy_name"))
{
}


void
ComputePFFStress::initQpStatefulProperties()
{
  _strain_energy[_qp] = 0.0;
}


void
ComputePFFStress::computeQpStress()
{
  // Obtain degradation functions
  Real degradation = _degradation[_qp] ? _degradation[_qp] : 1.0;

  // Assign value for elastic strain, which is equal to the mechanical strain
  _elastic_strain[_qp] = _mechanical_strain[_qp];

  // stress and jacobian
  _stress[_qp] = degradation * _elasticity_tensor[_qp] * _mechanical_strain[_qp];

  _Jacobian_mult[_qp] = _elasticity_tensor[_qp];

  // strain energy
  _strain_energy[_qp] = 0.5 * (_elasticity_tensor[_qp] * _mechanical_strain[_qp]).doubleContraction(_mechanical_strain[_qp]); 
  _strain_energy[_qp] = std::max(_strain_energy[_qp], _strain_energy_old[_qp]);
}
