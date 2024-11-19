#include "ADComputePFFDrivingForce.h"


registerMooseObject("SolidMechanicsApp", ADComputePFFDrivingForce);

InputParameters
ADComputePFFDrivingForce::validParams()
{
  InputParameters params = ADMaterial::validParams();
  params.addClassDescription("Compute phase-field fracture stress using elasticity for small strains");
  
  // Parameters
  params.addRequiredParam<Real>("Gc", "Critical energy release rate.");

  // Consumed property names
  params.addParam<MaterialPropertyName>("degradation_name", "degradation",
                                        "The name of the material degradation property.");
  params.addParam<MaterialPropertyName>("ddegradationdphi_name", "ddegradationdphi",
                                        "The name of the derivative of the material degradation property.");
  params.addParam<MaterialPropertyName>("strain_energy_name", "strain_energy",
                                        "The name of the material degradation property.");

  // Declared property names
  params.addParam<MaterialPropertyName>("crack_driving_force_name", "crack_driving_force",
                                        "The name of the crack driving force property.");
  return params;
}

ADComputePFFDrivingForce::ADComputePFFDrivingForce(
    const InputParameters & parameters
)
  : ADMaterial(parameters),

  _gc(getParam<Real>("Gc")),

  _ddegradationdphi(getADMaterialProperty<Real>("ddegradationdphi_name")),
  _strain_energy(getADMaterialProperty<Real>("strain_energy_name")),

  _crack_driving_force(declareADProperty<Real>("crack_driving_force_name"))
{
}

void
ADComputePFFDrivingForce::computeQpProperties()
{
  _crack_driving_force[_qp] = _ddegradationdphi[_qp] * _strain_energy[_qp] / _gc;
}
