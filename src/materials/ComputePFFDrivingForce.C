#include "ComputePFFDrivingForce.h"


registerMooseObject("SolidMechanicsApp", ComputePFFDrivingForce);

InputParameters
ComputePFFDrivingForce::validParams()
{
  InputParameters params = DerivativeMaterialInterface<Material>::validParams();
  params.addClassDescription("Compute phase-field fracture stress using elasticity for small strains");
  
  // Parameters
  params.addRequiredParam<Real>("Gc", "Critical energy release rate.");

  // Coupled variables
  params.addRequiredCoupledVar("variable", "Name of the damage variable.");

  // Consumed property names
  params.addParam<MaterialPropertyName>("degradation_name", "degradation",
                                        "The name of the material degradation property.");
  params.addParam<MaterialPropertyName>("strain_energy_name", "strain_energy",
                                        "The name of the material degradation property.");

  // Declared property names
  params.addParam<MaterialPropertyName>("crack_driving_force_name", "crack_driving_force",
                                        "The name of the crack driving force property.");
  return params;
}


ComputePFFDrivingForce::ComputePFFDrivingForce(
    const InputParameters & parameters
)
  : DerivativeMaterialInterface<Material>(parameters),

  // Coupled damage variable
  _phi_name(coupledName("variable")),

  // Consumed properties
  _degradation_name(getParam<MaterialPropertyName>("degradation_name")),
  _ddegradationdphi(getMaterialPropertyDerivative<Real>(_degradation_name, _phi_name)),
  _d2degradationd2phi(getMaterialPropertyDerivative<Real>(_degradation_name, _phi_name, _phi_name)),
  
  _strain_energy(getMaterialProperty<Real>("strain_energy_name")),

  // Declared properties
  _crack_driving_force_name(getParam<MaterialPropertyName>("crack_driving_force_name")),
  _crack_driving_force(declareProperty<Real>("crack_driving_force_name")),
  _dcrack_driving_force_dphi(declarePropertyDerivative<Real>(_crack_driving_force_name, _phi_name)),

  // Parameters
  _gc(getParam<Real>("Gc"))
{
}


void
ComputePFFDrivingForce::computeQpProperties()
{
  _crack_driving_force[_qp] = _ddegradationdphi[_qp] * _strain_energy[_qp] / _gc;
  _dcrack_driving_force_dphi[_qp] = _d2degradationd2phi[_qp] * _strain_energy[_qp] / _gc;
}
