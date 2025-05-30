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

#include "SSPTSteel.h"

namespace lynx
{

registerMooseObject("LynxApp", SSPTSteel);

InputParameters
SSPTSteel::validParams()
{
  InputParameters params = Material::validParams();

  params.addRequiredCoupledVar("variable", "Temperature variable");

  params.addParam<Real>("fraction_austenite", 0.0, "Initial austenite fraction");
  params.addParam<Real>("fraction_ferrite", 0.0, "Initial ferrite fraction");
  params.addParam<Real>("fraction_pearite", 0.0, "Initial pearlite fraction");
  params.addParam<Real>("fraction_bainite", 0.0, "Initial bainite fraction");
  params.addParam<Real>("fraction_martensite", 0.0, "Initial martensite fraction");

  params.addParam<Real>("comp_C", 0.0, "Carbon content");
  params.addParam<Real>("comp_Mn", 0.0, "Manganese content");
  params.addParam<Real>("comp_Si", 0.0, "Silicon content");
  params.addParam<Real>("comp_Ni", 0.0, "Nickel content");
  params.addParam<Real>("comp_Cr", 0.0, "Chromium content");
  params.addParam<Real>("comp_Mo", 0.0, "Molybdenum content");
  params.addParam<Real>("comp_Co", 0.0, "Cobalt content");
  params.addParam<Real>("comp_V", 0.0, "Vanadium content");
  params.addParam<Real>("comp_W", 0.0, "Tungsten content");
  params.addParam<Real>("comp_As", 0.0, "Arsenic content");
  params.addParam<Real>("comp_Al", 0.0, "Aluminum content");
  params.addParam<Real>("comp_P", 0.0, "Phosphorus content");
  params.addParam<Real>("comp_Ti", 0.0, "Titanium content");
  params.addParam<Real>("comp_Cu", 0.0, "Copper content");

  params.addParam<Real>("temperature_Ae3", 0.0, "Ferrite transformation temperature");
  params.addParam<Real>("temperature_Ae1", 0.0, "Pearlite transformation temperature");
  params.addParam<Real>("temperature_Bs", 0.0, "Bainite transformation temperature");
  params.addParam<Real>("temperature_Ms", 0.0, "Martensite transformation temperature");

  params.addParam<Real>("grain_size_init", 30.0, "Initial austenite grain size");
  params.addParam<Real>("grain_size_min", 30.0, "Minimum austenite grain size");
  params.addParam<Real>("grain_size_max", 30.0, "Maximum austenite grain size");

  return params;
}

SSPTSteel::SSPTSteel(const InputParameters & parameters)
  : Material(parameters),

  // Couple temperature
  _temperature(coupledValue("variable")),
  _temperature_old(coupledValueOld("variable")),

  // Declare material properties
  _fractions{&declareProperty<Real>("fraction_ferrite"),
             &declareProperty<Real>("fraction_pearlite"),
             &declareProperty<Real>("fraction_bainite"),
             &declareProperty<Real>("fraction_martensite"),
             &declareProperty<Real>("fraction_austenite")},
  _nucleation{&declareProperty<Real>("nucleation_ferrite"),
              &declareProperty<Real>("nucleation_pearlite"),
              &declareProperty<Real>("nucleation_bainite")},
  _grain_size(declareProperty<Real>("grain_size")),
  _fractions_old{&getMaterialPropertyOld<Real>("fraction_ferrite"), 
                 &getMaterialPropertyOld<Real>("fraction_pearlite"),
                 &getMaterialPropertyOld<Real>("fraction_bainite"),
                 &getMaterialPropertyOld<Real>("fraction_martensite"),
                 &getMaterialPropertyOld<Real>("fraction_austenite")},
  _nucleation_old{&getMaterialPropertyOld<Real>("nucleation_ferrite"),
                  &getMaterialPropertyOld<Real>("nucleation_pearlite"),
                  &getMaterialPropertyOld<Real>("nucleation_bainite")},
  _grain_size_old(getMaterialPropertyOld<Real>("grain_size")),

  // Initial fractions
  _fractions_init{getParam<Real>("fraction_ferrite"),
                  getParam<Real>("fraction_pearlite"),
                  getParam<Real>("fraction_bainite"),
                  getParam<Real>("fraction_martensite"),
                  getParam<Real>("fraction_austenite")},

  // Composition
  _comp_C(getParam<Real>("comp_C")),
  _comp_Mn(getParam<Real>("comp_Mn")),
  _comp_Si(getParam<Real>("comp_Si")),
  _comp_Ni(getParam<Real>("comp_Ni")),
  _comp_Cr(getParam<Real>("comp_Cr")),
  _comp_Mo(getParam<Real>("comp_Mo")),
  _comp_Co(getParam<Real>("comp_Co")),
  _comp_V(getParam<Real>("comp_V")),
  _comp_W(getParam<Real>("comp_W")),
  _comp_As(getParam<Real>("comp_As")),
  _comp_Al(getParam<Real>("comp_Al")),
  _comp_P(getParam<Real>("comp_P")),
  _comp_Ti(getParam<Real>("comp_Ti")),
  _comp_Cu(getParam<Real>("comp_Cu")),

  // Transformation temperatures
  _temperature_Ae3(getParam<Real>("temp_Ae3")),
  _temperature_Ae1(getParam<Real>("temp_Ae1")),
  _temperature_Bs(getParam<Real>("temp_Bs")),
  _temperature_Ms(getParam<Real>("temp_Ms")),

  // Austenite grain sizes
  _grain_size_init(getParam<Real>("grain_size_init")),
  _grain_size_min(getParam<Real>("grain_size_min")),
  _grain_size_max(getParam<Real>("grain_size_max"))
{
  // Bound check on fractions
  Real fracsum = 0.0;

  for( const auto phase : phases )
  {
    if( _fractions_init[phase] < 0.0 || _fractions_init[phase] > 1.0 )
      mooseError("Initial fractions should be between 0 and 1");

    fracsum += _fractions_init[phase];
  }

  if( fracsum < 1.0 )
    _fractions_init[austenite] = 1.0 - fracsum;
  else
    for( const auto phase : phases )
      _fractions_init[phase] /= fracsum;

  // Temperature formulas Grange
  if( _temperature_Ae3 == 0.0 )
    _temperature_Ae3 = 5./9.*(1570. - 323.*_comp_C - 25.*_comp_Mn + 80.*_comp_Si 
      - 32.*_comp_Ni - 3.*_comp_Cr - 30.);
  if( _temperature_Ae1 == 0.0 )
    _temperature_Ae1 = 5./9.*(1333. - 25.*_comp_Mn + 40.*_comp_Si - 26.*_comp_Ni 
      + 42.*_comp_Cr - 32.);
  if( _temperature_Bs == 0.0 )
    _temperature_Bs = 637. - 58.*_comp_C - 35.*_comp_Mn - 15.*_comp_Ni 
      - 34.*_comp_Cr - 41.*_comp_Mo;
  if( _temperature_Ms == 0.0 )
    _temperature_Ms = 539. - 423.*_comp_C - 30.4*_comp_Mn - 17.7*_comp_Ni 
      - 12.1*_comp_Cr - 7.5*_comp_Mo + 10.*_comp_Co - 7.5*_comp_Si;

  // Set upper and lower temperatures
  _temperature_lower[ferrite] = _temperature_Bs;
  _temperature_upper[ferrite] = _temperature_Ae3;

  _temperature_lower[pearlite] = _temperature_Bs;
  _temperature_upper[pearlite] = _temperature_Ae1;

  _temperature_lower[bainite] = _temperature_Ms;
  _temperature_upper[bainite] = _temperature_Bs;

  _temperature_lower[martensite] = -273.15;
  _temperature_upper[martensite] = _temperature_Ms;

  _temperature_lower[austenite] = _temperature_Ae1;
  _temperature_upper[austenite] = 1e4;

  // Composition formulas Li
  _fcomp[ferrite] = std::exp(1.0 + 6.31*_comp_C + 1.78*_comp_Mn + 0.31*_comp_Si 
    + 1.12*_comp_Ni + 2.7*_comp_Cr + 4.06*_comp_Mo);
  _fcomp[pearlite] = std::exp(-4.25 + 4.12*_comp_C + 4.36*_comp_Mn + 0.44*_comp_Si 
    + 1.71*_comp_Ni + 3.33*_comp_Ni + 5.19*std::sqrt(_comp_Mo));
  _fcomp[bainite] = std::exp(-10.23 + 10.18*_comp_C + 0.85*_comp_Mn 
    + 0.55*_comp_Ni + 0.90*_comp_Cr + 0.36*_comp_Mo);
}

void
SSPTSteel::initQpStatefulProperties()
{
  for (auto phase : phases)
  {
    (*_fractions[phase])[_qp] = _fractions_init[phase];

    if (phase < 3)
      (*_nucleation[phase])[_qp] = _fractions_init[phase] < _tolerance ? 0.0 : 1.0;
  }

  _grain_size[_qp] = _grain_size_init;
}

void
SSPTSteel::computeQpProperties()
{
  if( _dt == 0.0 )
    return;

  Real dtemperature = _temperature[_qp] - _temperature_old[_qp];
  std::array<Real,5> dfractions;
  std::array<Real,3> dnucleation = {0.0, 0.0, 0.0};;
  Real  dgrain_size = 0.0;

  // Heating stage, austenite formation/grain growth
  if( dtemperature > 0.0 && checkTemperatureRange(austenite) )
  {
    if( (*_fractions[austenite])[_qp] < 1.0 - _tolerance )
    {
      dfractions[austenite] = austeniteTransformation();
      
      for (auto phase : phases)
        if (phase != austenite)
          dfractions[phase] = -dfractions[austenite] * (*_fractions[phase])[_qp]/(1.0 - (*_fractions[austenite])[_qp]);
    }

    // Reset initial grain size if first austenitization inc
    if( _temperature_old[_qp] <= _temperature_lower[austenite] )
      dgrain_size = ((*_fractions_old[austenite])[_qp] - 1.0) * _grain_size_old[_qp] 
        + (1.0 - (*_fractions_old[austenite])[_qp])*_grain_size_min;
    
    dgrain_size += grainGrowth();
  }

  // Cooling stage, austenite decomposition
  else if( dtemperature <= 0.0 && (*_fractions[austenite])[_qp] > _tolerance )
  {
    dfractions[austenite] = 0.0;
    for (auto phase : phases)
    {
      // Ferrite, pearlite, bainite
      if (phase < 3 && checkTemperatureRange(phase))
        std::tie(dfractions[phase], dnucleation[phase]) = diffusiveTransformation(phase, 
          (*_fractions_old[phase])[_qp], (*_nucleation_old[phase])[_qp]);

      // Martensite
      if (phase == martensite && checkTemperatureRange(phase))
        dfractions[phase] = martensiteTransformation();
    
      // Remove from austenite fraction
      if (phase != austenite)
          dfractions[austenite] -= dfractions[phase];
    }
  }

  // Correct for over/under shooting
  Real correct = 1.0;
  if( (*_fractions_old[austenite])[_qp] + dfractions[austenite] > 1.0 )
    correct = (1.0 - (*_fractions_old[austenite])[_qp]) / dfractions[austenite];
  else if( (*_fractions_old[austenite])[_qp] + dfractions[austenite] < 0.0 )
    correct = -(*_fractions_old[austenite])[_qp] / dfractions[austenite];

  // Update phase fractions
  for (auto phase : phases)
  {
    (*_fractions[phase])[_qp] = (*_fractions_old[phase])[_qp] + correct * dfractions[phase];
    (*_fractions[phase])[_qp] = std::min(std::max((*_fractions[phase])[_qp], 0.0), 1.0);

    // Update nucleation variables
    if (phase < 3)
    {
      (*_nucleation[phase])[_qp] = (*_nucleation_old[phase])[_qp] + dnucleation[phase];

      // Phases have to re-nucleate if almost zero
      if( (*_fractions[phase])[_qp] < _tolerance && (*_nucleation[phase])[_qp] >= 1.0 )
        (*_nucleation[phase])[_qp] = 0.0;
    }
  }

  _grain_size[_qp] = _grain_size_old[_qp] + dgrain_size;
}

Real  
SSPTSteel::austeniteTransformation()
{
  // Split increment if only partially in temperature range
  Real temperature, dt;
  std::tie(temperature, dt) = splitIncrementCooldown(austenite);

  Real fraction_eq, tau;
  if( temperature <= _temperature_Ae3 )
  {
    fraction_eq = (temperature - _temperature_Ae1) / (_temperature_Ae3 - _temperature_Ae1);
    tau = 1.0 - 0.8*x_eq;
  }
  else 
  {
    fraction_eq = 1.0;
    tau = 0.05;
  }

  return _dt * (fraction_eq - (*_fractions_old[austenite])[_qp]) / (tau + dt);
}

Real 
SSPTSteel::grainGrowth()
{
  // Split increment if only partially in temperature range
  Real temperature, dt;
  std::tie(temperature, dt) = splitIncrementCooldown(austenite);

  return dt * 10.0e8 * exp(-22853.0/(temperature+273.15)) 
    * (1.0/_grain_size_old[_qp] - 1.0/_grain_size_max);
}

std::tuple<Real,Real>
SSPTSteel::diffusiveTransformation(Phase phase,
                                   Real fraction,
                                   Real nucleation)
{
  Real temperature, dt;
  std::tie(temperature, dt) = splitIncrementHeating(phase);

  Real fun_tc = funTc(phase, temperature);
  Real dnucleation = 0.0;
  Real dfraction = 0.0;
  Real dfraction_dt;

  // Nucleation phase
  if( nucleation < 1.0 - _tolerance )
  {
    dfraction_dt = fun_tc / 0.10434035495809084;
    dnucleation = dfraction_dt * dt;

    if( nucleation + dnucleation >= 1.0 )
    {
      dfraction = 0.01;
      dt = (nucleation + dnucleation - 1.0) / dfraction_dt;
    }
  }

  // Transformation phase: solve for fraction
  if( nucleation + dnucleation >= 1.0 )
  {
    Real fraction0 = fraction + dfraction;
    Real fraction1 = fraction0;
    Real fraction2 = 0.5*(fraction0 + 1.0);
    Real fraction3 = 1.0;

    Real r1 = diffusiveTransformationResidual(fraction0, fraction1, dt, fun_tc);
    Real r2 = diffusiveTransformationResidual(fraction0, fraction2, dt, fun_tc);
    Real r3 = diffusiveTransformationResidual(fraction0, fraction3, dt, fun_tc);

    Real cr, cs, ct, cp, cq, r;

    // Brentq root finding algorithm
    unsigned int iter = 0;
    while( true )
    {
      ++iter;

      // New guess
      cr = r2 / r3;
      cs = r2 / r1;
      ct = r1 / r3;
      cp = cs*(ct*(cr-ct)*(fraction3-fraction2) - (1.0-cr)*(fraction2-fraction1));
      cq = (ct-1.0)*(cr-1.0)*(cs-1.0);

      fraction = fraction2 + cp / cq;

      // Bisection method if outside bounds
      if (!((fraction > fraction1 && fraction < fraction2 && r1*r2 < 0.) 
          || (fraction > fraction2 && fraction < fraction3 && r2*r3 < 0.)))
        if( (r1 < 0. && r2 > 0.) || (r1 > 0. && r2 < 0.) )
          fraction = 0.5 * (fraction1 + fraction2);
        else
          fraction = 0.5 * (fraction2 + fraction3);
  
      // Check convergence
      r = diffusiveTransformationResidual(fraction0, fraction, dt, fun_tc);

      if( std::abs(r) < _tolerance )
        break;
      else if( iter > 100 )
        mooseError("Diffuse transformation did not converge");

      // Update points
      if( fraction > fraction1 && fraction < fraction2)
      {
        fraction3 = fraction2;
        r3 = r2;
      }
      else
      {
        fraction1 = fraction2;
        r1 = r2;
      }

      fraction2 = fraction;
      r2 = r;
    }

    dfraction = fraction - fraction0;
  }

  return {dfraction, dnucleation};
}

Real
SSPTSteel::diffusiveTransformationResidual(Real fraction0,
                                           Real fraction,
                                           Real dt,
                                           Real fun_tc)
{
  return fraction - fraction0 - dt * fun_tc * std::pow(fraction, 0.4*(1.0-fraction)) * std::pow(1.0 - fraction, 0.4*fraction);
}

Real 
SSPTSteel::funTc(Phase phase, Real temperature)
{
  Real Gsize_astm = 2.88539 * std::log(254.0/_grain_size[_qp]) + 1.0;

  Gsize_astm = 7.163588007638449;

  return std::pow(_temperature_upper[phase]-temperature, _ucool_exponent[phase]) 
    * std::exp(-1.384e4/(temperature+273.15)) 
    * std::pow(2.0, _grain_size_factor[phase]*Gsize_astm) / _fcomp[phase];
}

Real
SSPTSteel::martensiteTransformation()
{
  return ((*_fractions_old[austenite])[_qp] + (*_fractions_old[martensite])[_qp]) * 
    (1.0 - exp(-1.1e-2*(_temperature_upper[martensite] - _temperature[_qp]))) - (*_fractions_old[martensite])[_qp];
}

bool
SSPTSteel::checkTemperatureRange(
  Phase phase
)
{
  return (_temperature[_qp] > _temperature_lower[phase] && _temperature[_qp] <= _temperature_upper[phase]) 
    || (_temperature_old[_qp] > _temperature_lower[phase] && _temperature_old[_qp] <= _temperature_upper[phase]);
}

std::tuple<Real,Real> 
SSPTSteel::splitIncrementHeating(Phase phase)
{
  Real temperature, dt;

  // Lower bound
  if( _temperature_old[_qp] < _temperature_lower[phase] )
  {
    temperature = _temperature[_qp];
    dt = (_temperature[_qp] - _temperature_lower[phase]) / (_temperature[_qp] - _temperature_old[_qp]) * _dt;
  }
  // Upper bound
  else if( _temperature[_qp] > _temperature_upper[phase] )
  {
    temperature = _temperature_upper[phase];
    dt = (_temperature_upper[phase] - _temperature_old[_qp]) / (_temperature[_qp] - _temperature_old[_qp]) * _dt;
  }
  else
  {
    temperature = _temperature[_qp];
    dt = _dt;
  }

  return {temperature, dt};
}

std::tuple<Real,Real> 
SSPTSteel::splitIncrementCooldown(Phase phase)
{
  Real temperature, dt;

  // Upper bound
  if( _temperature_old[_qp] > _temperature_upper[phase] )
  {
    temperature = _temperature[_qp];
    dt = (_temperature_upper[phase] - _temperature[_qp]) / (_temperature_old[_qp] - _temperature[_qp]) * _dt;
  }
  // Lower bound
  else if( _temperature[_qp] < _temperature_lower[phase] )
  {
    temperature = _temperature_lower[phase];
    dt = (_temperature_old[_qp] - _temperature_lower[phase]) / (_temperature_old[_qp] - _temperature[_qp]) * _dt;
  }
  else
  {
    temperature = _temperature[_qp];
    dt = _dt;
  }

  return {temperature, dt};
}

}