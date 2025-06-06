//* This file is part of Lynx, 
//* an open-source application for the simulation  
//* of mechanics and multi-physics problems
//* https://github.com/j-wijnen/lynx
//*
//* Lynx is powered by the MOOSE Framework
//* https://www.mooseframework.org
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "PhaseFractionMaterialFromCSV.h"
#include "DelimitedFileReader.h"
#include "general_static_cast.h"

namespace lynx
{

registerMooseObject("LynxApp", PhaseFractionMaterialFromCSV);
registerMooseObject("LynxApp", ADPhaseFractionMaterialFromCSV);

template<bool is_ad>
InputParameters
PhaseFractionMaterialFromCSVTempl<is_ad>::validParams()
{
  InputParameters params = Material::validParams();

  params.addRequiredParam<std::vector<std::string>>("prop_names", "Properties to be defined.");
  params.addRequiredParam<std::vector<std::string>>("prop_files", "CSV files with phase properties.");
  params.addParam<bool>("inverse_average", false, "Compute phase averages with Ruess bounds");

  params.addRequiredCoupledVar("coupled_variable", "The temperature variable that will be used for interpolation.");

  return params;
}

template<bool is_ad>
PhaseFractionMaterialFromCSVTempl<is_ad>::PhaseFractionMaterialFromCSVTempl(
  const InputParameters & parameters
)
  : Material(parameters),

  // Parameters
  _prop_names(getParam<std::vector<std::string>>("prop_names")),
  _prop_files(getParam<std::vector<std::string>>("prop_files")),
  _inverse_average(getParam<bool>("inverse_average")),

  // Coupled temperature variable
  _temperature(coupledGenericValue<is_ad>("coupled_variable")),

  // Required phase fractions
  _fraction_f(getMaterialProperty<Real>("fraction_ferrite")),
  _fraction_p(getMaterialProperty<Real>("fraction_pearlite")),
  _fraction_b(getMaterialProperty<Real>("fraction_bainite")),
  _fraction_m(getMaterialProperty<Real>("fraction_martensite")),
  _fraction_a(getMaterialProperty<Real>("fraction_austenite"))

{
  auto nprops =  _prop_names.size();
  auto nfiles = _prop_files.size();

  mooseAssert(nfiles == 1 || nfiles == nprops, "Number of files must be 1 or equal to the number of properties");

  // Allocate memory for each property
  _properties.reserve(nprops);
  _ferrite.reserve(nprops);
  _pearlite.reserve(nprops);
  _bainite.reserve(nprops);
  _martensite.reserve(nprops);
  _austenite.reserve(nprops);
  _piecewise_funcs.reserve(nfiles);

  // Read CSV input files
  if(nprops == nfiles)
    readPropertiesMultipleFiles();
  else
    readPropertiesSingleFile();
}

template<bool is_ad>
void
PhaseFractionMaterialFromCSVTempl<is_ad>::readPropertiesMultipleFiles()
{
  for (int iprop = 0; iprop < _prop_names.size(); ++iprop)
  {
    // Declare property
    _properties.push_back(&declareGenericProperty<Real, is_ad>(_prop_names[iprop]));

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
    for (int i = 0; i < ndata; ++i)
      interp_values[i] = (Real) i;

    _piecewise_funcs.push_back(GenericLinearInterpolation(temperature, interp_values));  
  }
}

template<bool is_ad>
void
PhaseFractionMaterialFromCSVTempl<is_ad>::readPropertiesSingleFile()
{
  // Read data from single csv file
  MooseUtils::DelimitedFileReader csv_reader(_prop_files[0]);
  csv_reader.read();

  const auto & temperature = csv_reader.getData("temperature");
  int ndata = temperature.size();

  // Create one interpolation array so that we dont have to interpolate for every phase
  std::vector<Real> interp_values(ndata);
  for( int i = 0; i < ndata; ++i )
    interp_values[i] = (Real) i;

  _piecewise_funcs.push_back(GenericLinearInterpolation(temperature, interp_values)); 

  for (int iprop = 0; iprop < _prop_names.size(); ++iprop)
  {
    // Declare property
    auto prop_name = _prop_names[iprop];
    _properties.push_back(&declareGenericProperty<Real, is_ad>(prop_name));

    _ferrite.push_back(csv_reader.getData(prop_name + "_ferrite"));
    _pearlite.push_back(csv_reader.getData(prop_name + "_pearlite"));
    _bainite.push_back(csv_reader.getData(prop_name + "_bainite"));
    _martensite.push_back(csv_reader.getData(prop_name + "_martensite"));
    _austenite.push_back(csv_reader.getData(prop_name + "_austenite")); 
  }
}

template<bool is_ad>
void
PhaseFractionMaterialFromCSVTempl<is_ad>::computeQpProperties()
{
  if( _prop_names.size() == _prop_files.size() )
    computeQpPropertiesMultipleFiles();
  else
    computeQpPropertiesSingleFile();
}

template<bool is_ad>
void
PhaseFractionMaterialFromCSVTempl<is_ad>::computeQpPropertiesMultipleFiles()
{
  for (int iprop = 0; iprop < _properties.size(); ++iprop)
  {
    auto interp_value = _piecewise_funcs[iprop].sample(_temperature[_qp]);

    // Calculate indices and interpolation weight of phase arrays
    int ii = general_static_cast<int>(interp_value);
    int jj = ii +1;
    auto w = interp_value - static_cast<Real>(ii);

    if (_inverse_average)
    {
      (*_properties[iprop])[_qp] = 1.0 / (
        _fraction_f[_qp] / ((1.0-w)*_ferrite[iprop][ii] + w*_ferrite[iprop][jj]) 
        + _fraction_p[_qp] / ((1.0-w)*_pearlite[iprop][ii] + w*_pearlite[iprop][jj])
        + _fraction_b[_qp] / ((1.0-w)*_bainite[iprop][ii] + w*_bainite[iprop][jj])
        + _fraction_m[_qp] / ((1.0-w)*_martensite[iprop][ii] + w*_martensite[iprop][jj])
        + _fraction_a[_qp] / ((1.0-w)*_austenite[iprop][ii] + w*_austenite[iprop][jj]));
    }
    else
    {
      (*_properties[iprop])[_qp] = 
        ((1.0-w)*_ferrite[iprop][ii] + w*_ferrite[iprop][jj]) * _fraction_f[_qp]
        + ((1.0-w)*_pearlite[iprop][ii] + w*_pearlite[iprop][jj]) * _fraction_p[_qp]
        + ((1.0-w)*_bainite[iprop][ii] + w*_bainite[iprop][jj]) * _fraction_b[_qp]
        + ((1.0-w)*_martensite[iprop][ii] + w*_martensite[iprop][jj]) * _fraction_m[_qp]
        + ((1.0-w)*_austenite[iprop][ii] + w*_austenite[iprop][jj]) * _fraction_a[_qp];
    }
  }
}

template<bool is_ad>
void
PhaseFractionMaterialFromCSVTempl<is_ad>::computeQpPropertiesSingleFile()
{
  auto interp_value = _piecewise_funcs[0].sample(_temperature[_qp]);

  // Calculate indices and interpolation weight of phase arrays
  int ii = general_static_cast<int>(interp_value);
  int jj = ii +1;
  auto w = interp_value - static_cast<Real>(ii);

  for (int iprop = 0; iprop < _properties.size(); ++iprop)
  {
    if (_inverse_average)
    {
      (*_properties[iprop])[_qp] = 1.0 / (
        _fraction_f[_qp] / ((1.0-w)*_ferrite[iprop][ii] + w*_ferrite[iprop][jj]) 
        + _fraction_p[_qp] / ((1.0-w)*_pearlite[iprop][ii] + w*_pearlite[iprop][jj])
        + _fraction_b[_qp] / ((1.0-w)*_bainite[iprop][ii] + w*_bainite[iprop][jj])
        + _fraction_m[_qp] / ((1.0-w)*_martensite[iprop][ii] + w*_martensite[iprop][jj])
        + _fraction_a[_qp] / ((1.0-w)*_austenite[iprop][ii] + w*_austenite[iprop][jj]));
    }
    else
    {
      (*_properties[iprop])[_qp] = 
        ((1.0-w)*_ferrite[iprop][ii] + w*_ferrite[iprop][jj]) * _fraction_f[_qp]
        + ((1.0-w)*_pearlite[iprop][ii] + w*_pearlite[iprop][jj]) * _fraction_p[_qp]
        + ((1.0-w)*_bainite[iprop][ii] + w*_bainite[iprop][jj]) * _fraction_b[_qp]
        + ((1.0-w)*_martensite[iprop][ii] + w*_martensite[iprop][jj]) * _fraction_m[_qp]
        + ((1.0-w)*_austenite[iprop][ii] + w*_austenite[iprop][jj]) * _fraction_a[_qp];
    }
  }
}

} // end namespace