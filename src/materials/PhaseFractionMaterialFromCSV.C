#include "PhaseFractionMaterialFromCSV.h"
#include "DelimitedFileReader.h"

registerMooseObject("LynxApp", PhaseFractionMaterialFromCSV);


InputParameters
PhaseFractionMaterialFromCSV::validParams()
{
  InputParameters params = Material::validParams();

  //params.addRequiredParam<std::vector<std::string>>("prop_names");

  return params;
}



PhaseFractionMaterialFromCSV::PhaseFractionMaterialFromCSV(
  const InputParameters & parameters
)
  : Material(parameters),
  _frac_f(getMaterialProperty<Real>("frac_f")),
  _frac_p(getMaterialProperty<Real>("frac_p")),
  _frac_b(getMaterialProperty<Real>("frac_b")),
  _frac_m(getMaterialProperty<Real>("frac_m")),
  _frac_a(getMaterialProperty<Real>("frac_a"))

  // // Initialize our piecewise function helper with the user-specified interpolation points.
  // _piecewise_func(getParam<std::vector<Real>>("independent_vals"),
  //                   getParam<std::vector<Real>>("dependent_vals"))
{
  MooseUtils::DelimitedFileReader csv_reader("filename");

  
}


void
PhaseFractionMaterialFromCSV::computeQpProperties()
{
  // // Diffusivity is the value of the interpolated piece-wise function described by the user
  // _diffusivity[_qp] = _piecewise_func.sample(_q_point[_qp](2));

  // // Convection velocity is set equal to the gradient of the variable set by the user.
  // _convection_velocity[_qp] = _diffusion_gradient[_qp];
}