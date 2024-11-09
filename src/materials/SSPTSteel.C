#include "SSPTSteel.h"

registerMooseObject("LynxApp", SSPTSteel);


InputParameters
SSPTSteel::validParams()
{
  InputParameters params = Material::validParams();

  params.addRequiredCoupledVar("variable", "Temperature variable");

  params.addParam<Real>("frac_a", 0.0, "Initial austenite fraction");
  params.addParam<Real>("frac_f", 0.0, "Initial ferrite fraction");
  params.addParam<Real>("frac_p", 0.0, "Initial pearlite fraction");
  params.addParam<Real>("frac_b", 0.0, "Initial bainite fraction");
  params.addParam<Real>("frac_m", 0.0, "Initial martensite fraction");

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

  params.addParam<Real>("temp_Ae3", 0.0, "Ferrite transformation temperature");
  params.addParam<Real>("temp_Ae1", 0.0, "Pearlite transformation temperature");
  params.addParam<Real>("temp_Bs", 0.0, "Bainite transformation temperature");
  params.addParam<Real>("temp_Ms", 0.0, "Martensite transformation temperature");

  return params;
}


SSPTSteel::SSPTSteel(
  const InputParameters & parameters
)
  : Material(parameters),

  // Couple temperature
  _temp(coupledValue("variable")),
  _temp_old(coupledValueOld("variable")),

  // Declare material properties
  _xa(declareProperty<Real>("frac_a")),
  _xf(declareProperty<Real>("frac_f")),
  _xp(declareProperty<Real>("frac_p")),
  _xb(declareProperty<Real>("frac_b")),
  _xm(declareProperty<Real>("frac_m")),
  _nucf(declareProperty<Real>("nuc_f")),
  _nucp(declareProperty<Real>("nuc_p")),
  _nucb(declareProperty<Real>("nuc_b")),
  _Gsize(declareProperty<Real>("grain_size")),
  _xa_old(getMaterialPropertyOld<Real>("frac_a")),
  _xf_old(getMaterialPropertyOld<Real>("frac_f")),
  _xp_old(getMaterialPropertyOld<Real>("frac_p")),
  _xb_old(getMaterialPropertyOld<Real>("frac_b")),
  _xm_old(getMaterialPropertyOld<Real>("frac_m")),
  _nucf_old(getMaterialPropertyOld<Real>("nuc_f")),
  _nucp_old(getMaterialPropertyOld<Real>("nuc_p")),
  _nucb_old(getMaterialPropertyOld<Real>("nuc_b")),

  // Initial fractions
  _x_init{getParam<Real>("frac_f"),
          getParam<Real>("frac_p"),
          getParam<Real>("frac_b"),
          getParam<Real>("frac_m"),
          getParam<Real>("frac_a")},

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
  _temp_Ae3(getParam<Real>("temp_Ae3")),
  _temp_Ae1(getParam<Real>("temp_Ae1")),
  _temp_Bs(getParam<Real>("temp_Bs")),
  _temp_Ms(getParam<Real>("temp_Ms"))
{
  // Bound check on fractions
  Real fracsum = 0.0;

  for( const auto phase : phases )
  {
    if( _x_init[phase] < 0.0 || _x_init[phase] > 1.0 )
      mooseError("Initial fractions should be between 0 and 1");

    fracsum += _x_init[phase];
  }

  if( fracsum < 1.0 )
    _x_init[austenite] = 1.0 - fracsum;
  else
    for( const auto phase : phases )
      _x_init[phase] /= fracsum;

  // Temperature formulas Grange
  if( _temp_Ae3 == 0.0 )
    _temp_Ae3 = 5./9.*(1570. - 323.*_comp_C - 25.*_comp_Mn + 80.*_comp_Si 
      - 32.*_comp_Ni - 3.*_comp_Cr - 30.);
  if( _temp_Ae1 == 0.0 )
    _temp_Ae1 = 5./9.*(1333. - 25.*_comp_Mn + 40.*_comp_Si - 26.*_comp_Ni 
      + 42.*_comp_Cr - 32.);
  if( _temp_Bs == 0.0 )
    _temp_Bs = 637. - 58.*_comp_C - 35.*_comp_Mn - 15.*_comp_Ni 
      - 34.*_comp_Cr - 41.*_comp_Mo;
  if( _temp_Ms == 0.0 )
    _temp_Ms = 539. - 423.*_comp_C - 30.4*_comp_Mn - 17.7*_comp_Ni 
      - 12.1*_comp_Cr - 7.5*_comp_Mo + 10.*_comp_Co - 7.5*_comp_Si;

  // Set upper and lower temperatures
  _temp_lower[ferrite] = _temp_Bs;
  _temp_upper[ferrite] = _temp_Ae3;

  _temp_lower[pearlite] = _temp_Bs;
  _temp_upper[pearlite] = _temp_Ae1;

  _temp_lower[bainite] = _temp_Ms;
  _temp_upper[bainite] = _temp_Bs;

  _temp_lower[martensite] = -273.15;
  _temp_upper[martensite] = _temp_Ms;

  _temp_lower[austenite] = _temp_Ae1;
  _temp_upper[austenite] = 1e4;

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
  _xf[_qp] = _x_init[ferrite];
  _xp[_qp] = _x_init[pearlite];
  _xb[_qp] = _x_init[bainite];
  _xm[_qp] = _x_init[martensite];
  _xa[_qp] = _x_init[austenite];

  _nucf[_qp] = _x_init[ferrite] < _tolerance ? 0.0 : 1.0;
  _nucp[_qp] = _x_init[pearlite] < _tolerance ? 0.0 : 1.0;
  _nucb[_qp] = _x_init[bainite] < _tolerance ? 0.0 : 1.0;

  _Gsize[_qp] = 7.0;
}


void
SSPTSteel::computeQpProperties()
{
  if( _dt == 0.0 )
    return;

  Real dtemp = _temp[_qp] - _temp_old[_qp],
       dxa = 0.0,
       dxf = 0.0,
       dxp = 0.0,
       dxb = 0.0,
       dxm = 0.0,
       dnucf = 0.0,
       dnucp = 0.0,
       dnucb = 0.0;

  // Heating stage, austenite formation
  if( dtemp > 0.0 && _xa[_qp] < 1.0 - _tolerance && checkTemperatureRange(austenite) )
  {
    dxa = austeniteTransformation();
    
    dxf = -dxa * _xf[_qp]/(1.0 - _xa[_qp]);
    dxp = -dxa * _xp[_qp]/(1.0 - _xa[_qp]);
    dxb = -dxa * _xb[_qp]/(1.0 - _xa[_qp]);
    dxm = -dxa * _xm[_qp]/(1.0 - _xa[_qp]);
  }

  // Cooling stage, austenite decomposition
  else if( dtemp <= 0.0 && _xa[_qp] > _tolerance )
  {
    if( checkTemperatureRange(ferrite) )
      std::tie(dxf, dnucf) = diffusiveTransformation(ferrite, _xf[_qp], _nucf[_qp]);

    if( checkTemperatureRange(pearlite) )
      std::tie(dxp, dnucp) = diffusiveTransformation(pearlite, _xp[_qp], _nucp[_qp]);

    if( checkTemperatureRange(bainite) )
      std::tie(dxb, dnucb) = diffusiveTransformation(bainite, _xb[_qp], _nucb[_qp]);

    if( checkTemperatureRange(martensite) )
      dxm = martensiteTransformation();

    dxa = -(dxf + dxp + dxb + dxm);
  }

  // Correct for over/under shooting
  Real corr = 1.0;
  if( _xa_old[_qp] + dxa > 1.0 )
    corr = (1.0 - _xa_old[_qp]) / dxa;
  else if( _xa_old[_qp] + dxa < 0.0 )
    corr = -_xa_old[_qp] / dxa;

  // Update material properties
  _xa[_qp] = _xa_old[_qp] + corr * dxa;
  _xf[_qp] = _xf_old[_qp] + corr * dxf;
  _xp[_qp] = _xp_old[_qp] + corr * dxp;
  _xb[_qp] = _xb_old[_qp] + corr * dxb;
  _xm[_qp] = _xm_old[_qp] + corr * dxm;

  _nucf[_qp] = _nucf_old[_qp] + dnucf;
  _nucp[_qp] = _nucp_old[_qp] + dnucp;
  _nucb[_qp] = _nucb_old[_qp] + dnucb;

  // Phases have to re-nucleate if almost zero
  if( _xf[_qp] < _tolerance && _nucf[_qp] >= 1.0 )
    _nucf[_qp] = 0.0;
  if( _xp[_qp] < _tolerance && _nucp[_qp] >= 1.0 )
    _nucp[_qp] = 0.0;
  if( _xb[_qp] < _tolerance && _nucb[_qp] >= 1.0 )
    _nucb[_qp] = 0.0;

   // Need to enforce bounds due to numerical inaccuracies
  _xa[_qp] = std::min(std::max(_xa[_qp], 0.0), 1.0);
  _xf[_qp] = std::min(std::max(_xf[_qp], 0.0), 1.0);
  _xp[_qp] = std::min(std::max(_xp[_qp], 0.0), 1.0);
  _xb[_qp] = std::min(std::max(_xb[_qp], 0.0), 1.0);
  _xm[_qp] = std::min(std::max(_xm[_qp], 0.0), 1.0);
}


Real  
SSPTSteel::austeniteTransformation()
{
  // Split increment if only partially in temperature range
  Real temp, dt;
  std::tie(temp, dt) = splitIncrement(austenite);

  Real x_eq, tau;
  if( temp <= _temp_Ae3 )
  {
    x_eq = (temp - _temp_Ae1) / (_temp_Ae3 - _temp_Ae1);
    tau = 1.0 - 0.8*x_eq;
  }
  else 
  {
    x_eq = 1.0;
    tau = 0.05;
  }

  return _dt * (x_eq - _xa[_qp]) / (tau + dt);
}


std::tuple<Real,Real>
SSPTSteel::diffusiveTransformation(
  Phase phase,
  Real x,
  Real nuc
)
{
  Real temp, dt;
  std::tie(temp, dt) = splitIncrement(phase);

  Real fun_tc = funTc(phase, temp),
       dnuc = 0.0,
       dx = 0.0,
       dxdt;

  // Nucleation phase
  if( nuc < 1.0 - _tolerance )
  {
    dxdt = fun_tc / 0.10434035495809084;
    dnuc = dxdt * dt;

    if( nuc + dnuc >= 1.0 )
    {
      dx = 0.01;
      dt = (nuc + dnuc - 1.0) / dxdt;
    }
  }

  // Transformation phase
  if( nuc + dnuc >= 1.0 - _tolerance )
  {
    Real x0 = x + dx,
         x1 = x0,
         x2 = 0.5*(x0 + 1.0),
         x3 = 1.0;

    Real r1 = diffusiveTransformationResidual(x0, x1, dt, fun_tc),
         r2 = diffusiveTransformationResidual(x0, x2, dt, fun_tc),
         r3 = diffusiveTransformationResidual(x0, x3, dt, fun_tc);

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
      cp = cs*(ct*(cr-ct)*(x3-x2) - (1.0-cr)*(x2-x1));
      cq = (ct-1.0)*(cr-1.0)*(cs-1.0);

      x = x2 + cp / cq;

      // std::cout << "\n" << "x " << x << " r " << r << " dt " << dt << " " << _dt << std::endl;
      // std::cout << "x1 " << x1 << " r1 " << r1 << std::endl;
      // std::cout << "x2 " << x2 << " r2 " << r2 << std::endl;
      // std::cout << "x3 " << x3 << " r3 " << r3 << std::endl;

      // Bisection method if outside bounds
      if( !((x > x1 && x < x2 && r1*r2 < 0.) || (x > x2 && x < x3 && r2*r3 < 0.)) )
        if( (r1 < 0. && r2 > 0.) || (r1 > 0. && r2 < 0.) )
          x = 0.5 * (x1 + x2);
        else
          x = 0.5 * (x2 + x3);
  
      // Check convergence
      r = diffusiveTransformationResidual(x0, x, dt, fun_tc);

      if( std::abs(r) < _tolerance )
        break;
      else if( iter > 100 )
        mooseError("Diffuse transformation did not converge");

      // Update points
      if( x > x1 && x < x2)
      {
        x3 = x2;
        r3 = r2;
      }
      else
      {
        x1 = x2;
        r1 = r2;
      }

      x2 = x;
      r2 = r;
    }

    dx = x - x0;
  }

  return {dx, dnuc};
}


Real
SSPTSteel::diffusiveTransformationResidual(
  Real x0,
  Real x,
  Real dt,
  Real fun_tc
)
{
  return x - x0 - dt * fun_tc * std::pow(x, 0.4*(1.0-x)) * std::pow(1.0 - x, 0.4*x);
}


Real 
SSPTSteel::funTc(
  Phase phase,
  Real temp
)
{
  return std::pow(_temp_upper[phase]-temp, _ucool_exponent[phase]) 
    * std::exp(-1.384e4/(temp+273.15)) 
    * std::pow(2.0, _Gsize_factor[phase]*_Gsize[_qp]) / _fcomp[phase];
}


Real
SSPTSteel::martensiteTransformation()
{
  return (_xa[_qp] + _xm[_qp]) * 
    (1.0 - exp(-1.1e-2*(_temp_upper[martensite] - _temp[_qp]))) - _xm[_qp];
}


bool
SSPTSteel::checkTemperatureRange(
  Phase phase
)
{
  return (_temp[_qp] > _temp_lower[phase] && _temp[_qp] <= _temp_upper[phase]) 
    || (_temp_old[_qp] > _temp_lower[phase] && _temp_old[_qp] <= _temp_upper[phase]);
}


std::tuple<Real,Real> 
SSPTSteel::splitIncrement(
  Phase phase
)
{
  Real temp, dt;

  // Temperature increase
  if( _temp[_qp] > _temp_old[_qp] )
  {
    // Lower bound
    if( _temp_old[_qp] < _temp_lower[phase] )
    {
      temp = _temp[_qp];
      dt = (_temp[_qp] - _temp_lower[phase]) / (_temp[_qp] - _temp_old[_qp]) * _dt;
    }
    // Upper bound
    else if( _temp[_qp] > _temp_upper[phase] )
    {
      temp = _temp_upper[phase];
      dt = (_temp_upper[phase] - _temp_old[_qp]) / (_temp[_qp] - _temp_old[_qp]) * _dt;
    }
    else
    {
      temp = _temp[_qp];
      dt = _dt;
    }
  }

  // Temperature decrase
  else
  {
    // Upper bound
    if( _temp_old[_qp] > _temp_upper[phase] )
    {
      temp = _temp[_qp];
      dt = (_temp_upper[phase] - _temp[_qp]) / (_temp_old[_qp] - _temp[_qp]) * _dt;
    }
    // Lower bound
    else if( _temp[_qp] < _temp_lower[phase] )
    {
      temp = _temp_lower[phase];
      dt = (_temp_old[_qp] - _temp_lower[phase]) / (_temp_old[_qp] - _temp[_qp]) * _dt;
    }
    else
    {
      temp = _temp[_qp];
      dt = _dt;
    }
  }

  return {temp, dt};
}