#include "PFFDrivingForce.h"


registerMooseObject("LynxApp", PFFDrivingForce);

InputParameters
PFFDrivingForce::validParams()
{
  InputParameters params = DerivativeMaterialInterface<Material>::validParams();
  params.addClassDescription("Compute phase-field fracture stress using elasticity for small strains");
  
  // Coupled variables
  params.addRequiredCoupledVar("variable", "Name of the damage variable.");

  // Consumed property names
  params.addParam<MaterialPropertyName>("pff_degradation_name", "pff_degradation",
                                        "The name of the material degradation property.");
  params.addParam<MaterialPropertyName>("strain_energy_name", "strain_energy",
                                        "The name of the elastic strain energy density.");

  // Declared property names
  params.addParam<MaterialPropertyName>("pff_driving_force_name", "pff_driving_force",
                                        "The name of the crack driving force property.");
  return params;
}

PFFDrivingForce::PFFDrivingForce(
    const InputParameters & parameters
)
  : DerivativeMaterialInterface<Material>(parameters),

  // Coupled damage variable
  _u_name(coupledName("variable")),

  // Consumed properties
  _degradation_name(getParam<MaterialPropertyName>("pff_degradation_name")),
  _dg_du(getMaterialPropertyDerivative<Real>(_degradation_name, _u_name)),
  _d2g_du2(getMaterialPropertyDerivative<Real>(_degradation_name, _u_name, _u_name)),
  
  _strain_energy(getMaterialProperty<Real>("strain_energy_name")),

  // Declared properties
  _pff_driving_force_name(getParam<MaterialPropertyName>("pff_driving_force_name")),
  _pff_driving_force(declareProperty<Real>("pff_driving_force_name")),
  _dpff_driving_force_du(declarePropertyDerivative<Real>(_pff_driving_force_name, _u_name))
{
}

void
PFFDrivingForce::computeQpProperties()
{
  _pff_driving_force[_qp] = -1.0 *  _dg_du[_qp] * _strain_energy[_qp];
  _dpff_driving_force_du[_qp] = -1.0 * _d2g_du2[_qp] * _strain_energy[_qp];
}
