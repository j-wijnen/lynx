#include "PhaseFractionMaterialFromCSV.h"
#include "DelimitedFileReader.h"

registerMooseObject("LynxApp", PhaseFractionMaterialFromCSV);


InputParameters
PhaseFractionMaterialFromCSV::validParams()
{
  InputParameters params = Material::validParams();

  params.addRequiredParam<std::vector<std::string>>("prop_names", "Properties to be defined.");
  params.addRequiredParam<std::vector<std::string>>("prop_files", "CSV files with phase properties.");

  params.addRequiredCoupledVar("variable", "The temperature variable that will be used for interpolation.");

  return params;
}


PhaseFractionMaterialFromCSV::PhaseFractionMaterialFromCSV(
  const InputParameters & parameters
)
  : Material(parameters),

  // Parameters
  _prop_names(getParam<std::vector<std::string>>("prop_names")),
  _prop_files(getParam<std::vector<std::string>>("prop_files")),

  // Coupled temperature variable
  _temperature(coupledValue("variable")),

  // Required phase fractions
  _xf(getMaterialProperty<Real>("frac_f")),
  _xp(getMaterialProperty<Real>("frac_p")),
  _xb(getMaterialProperty<Real>("frac_b")),
  _xm(getMaterialProperty<Real>("frac_m")),
  _xa(getMaterialProperty<Real>("frac_a"))

{
  // Allocate space for each property
  auto nprops = _prop_names.size();
  _properties.reserve(nprops);
  _ferrite.reserve(nprops);
  _pearlite.reserve(nprops);
  _bainite.reserve(nprops);
  _martensite.reserve(nprops);
  _austenite.reserve(nprops);
  _piecewise_funcs.reserve(nprops);

  for(unsigned int iprop = 0; iprop < nprops; ++iprop)
  {
    // Declare property
    _properties.push_back(&declareProperty<Real>(_prop_names[iprop]));

    // Read data from csv files
    MooseUtils::DelimitedFileReader csv_reader(_prop_files[iprop]);
    csv_reader.read();

    const auto & temperature = csv_reader.getData("temperature");
    int ndata = temperature.size();

    _ferrite.push_back(csv_reader.getData("ferrite"));
    _pearlite.push_back(csv_reader.getData("pearlite"));
    _bainite.push_back(csv_reader.getData("bainite"));
    _martensite.push_back(csv_reader.getData("martensite"));
    _austenite.push_back(csv_reader.getData("austenite"));

    // Create one interpolation array so that we dont have to interpolate for every phase
    std::vector<Real> interp_values(ndata);
    for( int i = 0; i < ndata; ++i )
      interp_values[i] = (Real) i;

    _piecewise_funcs.push_back(LinearInterpolation(temperature, interp_values));  
  }
}


void
PhaseFractionMaterialFromCSV::computeQpProperties()
{
  for(unsigned int iprop = 0; iprop < _properties.size(); ++iprop)
  {
    Real interp_value = _piecewise_funcs[iprop].sample(_temperature[_qp]);

    // Calculate indices and interpolation weight of phase arrays
    unsigned int ii = static_cast<unsigned int>(interp_value);
    unsigned int jj = ii +1;
    Real w = interp_value - static_cast<Real>(ii);

    // Set actual property
    (*_properties[iprop])[_qp] = ((1.0-w)*_ferrite[iprop][ii] + w*_ferrite[iprop][jj]) * _xf[_qp]
      + ((1.0-w)*_pearlite[iprop][ii] + w*_pearlite[iprop][jj]) * _xp[_qp]
      + ((1.0-w)*_bainite[iprop][ii] + w*_bainite[iprop][jj]) * _xb[_qp]
      + ((1.0-w)*_martensite[iprop][ii] + w*_martensite[iprop][jj]) * _xm[_qp]
      + ((1.0-w)*_austenite[iprop][ii] + w*_austenite[iprop][jj]) * _xa[_qp];
  }
}