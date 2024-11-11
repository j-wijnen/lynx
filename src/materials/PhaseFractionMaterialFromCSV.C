#include "PhaseFractionMaterialFromCSV.h"
#include "DelimitedFileReader.h"

registerMooseObject("LynxApp", PhaseFractionMaterialFromCSV);


InputParameters
PhaseFractionMaterialFromCSV::validParams()
{
  InputParameters params = Material::validParams();

  params.addRequiredParam<std::string>("prop_names", "Properties to be defined.");
  params.addRequiredParam<std::string>("prop_files", "CSV files with phase properties.");

  params.addRequiredCoupledVar("variable", "The temperature variable that will be used for interpolation.");

  return params;
}



PhaseFractionMaterialFromCSV::PhaseFractionMaterialFromCSV(
  const InputParameters & parameters
)
  : Material(parameters),

  // Parameters
  _prop_names(getParam<std::string>("prop_names")),
  _prop_files(getParam<std::string>("prop_files")),

  // Coupled temperature variable
  _temperature(coupledValue("variable")),

  // Required phase fractions
  _xf(getMaterialProperty<Real>("frac_f")),
  _xp(getMaterialProperty<Real>("frac_p")),
  _xb(getMaterialProperty<Real>("frac_b")),
  _xm(getMaterialProperty<Real>("frac_m")),
  _xa(getMaterialProperty<Real>("frac_a"))

{
  // Declare properties to be set
  _properties = & declareProperty<Real>(_prop_names);

  // Read data from csv files
  MooseUtils::DelimitedFileReader csv_reader(_prop_files);
  
  csv_reader.read();
  const auto & temperature = csv_reader.getData("temperature");
  int ndata = temperature.size();

  _ferrite = csv_reader.getData("ferrite");
  _pearlite = csv_reader.getData("pearlite");
  _bainite = csv_reader.getData("bainite");
  _martensite = csv_reader.getData("martensite");
  _austenite = csv_reader.getData("austenite");

  // Create one interpolation array so that we dont have to interpolate for every phase
  std::vector<Real> interp_values(ndata);
  for( int i = 0; i < ndata; ++i )
    interp_values[i] = (Real) i;

  _piecewise_func.setData(temperature, interp_values);  
}


void
PhaseFractionMaterialFromCSV::computeQpProperties()
{
  double interp_value = _piecewise_func.sample(_temperature[_qp]);

  // Calculate indices and interpolation weight of phase arrays
  unsigned int ii = static_cast<unsigned int>(interp_value),
               jj = ii +1;
  Real w = interp_value - static_cast<Real>(ii);

  // Set actual property
  (*_properties)[_qp] = ((1.0-w)*_ferrite[ii] + w*_ferrite[jj]) * _xf[_qp]
    + ((1.0-w)*_pearlite[ii] + w*_pearlite[jj]) * _xp[_qp]
    + ((1.0-w)*_bainite[ii] + w*_bainite[jj]) * _xb[_qp]
    + ((1.0-w)*_martensite[ii] + w*_martensite[jj]) * _xm[_qp]
    + ((1.0-w)*_austenite[ii] + w*_austenite[jj]) * _xa[_qp];
}