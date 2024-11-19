#include <algorithm>
#include "ADComputeElasticPFFStress.h"
#include "RankTwoTensor.h"
#include "RankFourTensor.h"


registerMooseObject("SolidMechanicsApp", ADComputeElasticPFFStress);

InputParameters
ADComputeElasticPFFStress::validParams()
{
  InputParameters params = ADComputeLinearElasticStress::validParams();
  params.addClassDescription("Compute phase-field fracture stress using elasticity for small strains");
  
  // Consumed property names
  params.addParam<MaterialPropertyName>("degradation_name", "degradation",
                                        "The name of the material degradation property.");

  // Declared property names
  params.addParam<MaterialPropertyName>("strain_energy_name", "strain_energy",
                                        "The name of the material degradation property.");
  return params;
}

ADComputeElasticPFFStress::ADComputeElasticPFFStress(
    const InputParameters & parameters
)
  : ADComputeLinearElasticStress(parameters),

  _degradation(getOptionalADMaterialProperty<Real>("degradation_name")),

  _strain_energy(declareADProperty<Real>("strain_energy_name")),

  _strain_energy_old(getMaterialPropertyOld<Real>("strain_energy_name"))
{
}

void
ADComputeElasticPFFStress::initialSetup()
{
  ADComputeLinearElasticStress::initialSetup();
}

void
ADComputeElasticPFFStress::computeQpStress()
{
  // Obtain degradation functions
  ADReal degradation = _degradation[_qp] ? _degradation[_qp] : 1.0;

  // Assign value for elastic strain, which is equal to the mechanical strain
  _elastic_strain[_qp] = _mechanical_strain[_qp];

  // stress = C * e
  _stress[_qp] = degradation * _elasticity_tensor[_qp] * _elastic_strain[_qp];

  // Elastic strain energy
  _strain_energy[_qp] = 0.5 * _stress[_qp].doubleContraction(_elastic_strain[_qp])
    / (_degradation[_qp]+ 1e-7);

  _strain_energy[_qp] = std::max(_strain_energy[_qp], _strain_energy_old[_qp]);
}
