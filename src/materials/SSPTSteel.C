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

  // Parameters
  _xa_init(getParam<Real>("frac_a")),
  _xf_init(getParam<Real>("frac_f")),
  _xp_init(getParam<Real>("frac_p")),
  _xb_init(getParam<Real>("frac_b")),
  _xm_init(getParam<Real>("frac_m")),

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
  _temp_Ms(getParam<Real>("temp_Ms")),

  _tolerance(1e-4)
{
  // Bound check on fractions
  if( _xa_init < 0.0 || _xa_init > 1.0 )
    mooseError("frac_a should be between 0 and 1");
  if( _xf_init < 0.0 || _xf_init > 1.0 )
    mooseError("frac_f should be between 0 and 1");

  // Ensure all fractions add to 1
  Real fracsum = _xa_init + _xf_init + _xp_init + _xb_init + _xm_init;

  if( fracsum < 1.0 )
  {
    _xa_init = 1.0 - fracsum;
  }
  else
  {
    _xa_init /= fracsum;
    _xf_init /= fracsum;
    _xp_init /= fracsum;
    _xb_init /= fracsum;
    _xm_init /= fracsum;
  }

  // Temperature formulas Grange
  if( _temp_Ae3 == 0.0 )
    _temp_Ae3 = 5./9.*(1570. - 323.*_comp_C - 25.*_comp_Mn + 80.*_comp_Si - 32.*_comp_Ni - 3.*_comp_Cr - 30.);
  if( _temp_Ae1 == 0.0 )
    _temp_Ae3 = 5./9.*(1333. - 25.*_comp_Mn + 40.*_comp_Si - 26.*_comp_Ni + 42.*_comp_Cr - 32.);
  if( _temp_Bs == 0.0 )
    _temp_Bs = 637. - 58.*_comp_C - 35.*_comp_Mn - 15.*_comp_Ni - 34.*_comp_Cr - 41.*_comp_Mo;
  if( _temp_Bs == 0.0 )
    _temp_Ms = 539. - 423.*_comp_C - 30.4*_comp_Mn - 17.7*_comp_Ni - 12.1*_comp_Cr - 7.5*_comp_Mo + 10.*_comp_Co - 7.5*_comp_Si;

  // Composition formulas Li
  _fcomp_f = std::exp(1.0 + 6.31*_comp_C + 1.78*_comp_Mn + 0.31*_comp_Si + 1.12*_comp_Ni + 2.7*_comp_Cr + 4.06*_comp_Mo);
  _fcomp_p = std::exp(-4.25 + 4.12*_comp_C + 4.36*_comp_Mn + 0.44*_comp_Si + 1.71*_comp_Ni + 3.33*_comp_Ni + 5.19*std::sqrt(_comp_Mo));
  _fcomp_b = std::exp(-10.23 + 10.18*_comp_C + 0.85*_comp_Mn + 0.55*_comp_Ni + 0.90*_comp_Cr + 0.36*_comp_Mo);
}


void
SSPTSteel::initQpStatefulProperties()
{
  _xa[_qp] = _xa_init;
  _xf[_qp] = _xf_init;
  _xp[_qp] = _xp_init;
  _xb[_qp] = _xb_init;
  _xm[_qp] = _xm_init;

  _nucf[_qp] = _xf_init < _tolerance ? 0.0 : 1.0;
  _nucp[_qp] = _xp_init < _tolerance ? 0.0 : 1.0;
  _nucb[_qp] = _xb_init < _tolerance ? 0.0 : 1.0;

  _Gsize[_qp] = 7.0;
}


void
SSPTSteel::computeQpProperties()
{
  Real dxa, dxf, dxp, dxb, dxm, dnucf, dnucp, dnucb, fun_tc, 
    temp_split, dt_split;

  dxa = 0.0;
  dxf = 0.0;
  dxp = 0.0;
  dxb = 0.0;
  dxm = 0.0;
  dnucf = 0.0;
  dnucp = 0.0;
  dnucb = 0.0;

  // Heating stage
  if( _temp[_qp] > _temp_old[_qp] && _xa[_qp] < 1.0 - _tolerance )
  {
    if( _temp[_qp] > _temp_Ae3 || _temp_old[_qp] > _temp_Ae3 )
    {
      std::tie(temp_split, dt_split) = _split_increment(_temp_Ae3, 1e99);
      dxa = _austenite_transformation(temp_split, dt_split);
      
      dxf = -dxa * _xf[_qp]/(1.0 - _xa[_qp]);
      dxp = -dxa * _xp[_qp]/(1.0 - _xa[_qp]);
      dxb = -dxa * _xb[_qp]/(1.0 - _xa[_qp]);
      dxm = -dxa * _xm[_qp]/(1.0 - _xa[_qp]);
    }
  }

  // Cooling stage
  else if( _xa[_qp] > _tolerance )
  {
    // Ferrite transformation
    if( (_temp[_qp] > _temp_Bs && _temp[_qp] <= _temp_Ae3) 
      || (_temp_old[_qp] > _temp_Bs && _temp_old[_qp] <= _temp_Ae3) )
    {
      std::tie(temp_split, dt_split) = _split_increment(_temp_Bs, _temp_Ae3);
      fun_tc = _fun_tc(_temp_Ae3-temp_split, 3, 0.41, _fcomp_f);
      std::tie(dxf, dnucf) = _diffusive_transformation_linear(_nucf_old[_qp], _xf_old[_qp], 
        fun_tc, temp_split, dt_split);
    }

    // Pearlite transformation
    if( (_temp[_qp] > _temp_Bs && _temp[_qp] <= _temp_Ae1)
      || (_temp_old[_qp] > _temp_Bs && _temp_old[_qp] <= _temp_Ae1) )
    {
      std::tie(temp_split, dt_split) = _split_increment(_temp_Bs, _temp_Ae1);
      fun_tc = _fun_tc(_temp_Ae1-temp_split, 3, 0.32, _fcomp_p);
      std::tie(dxp, dnucp) = _diffusive_transformation_linear(_nucp_old[_qp], _xp_old[_qp], 
        fun_tc, temp_split, dt_split);
    }

    // Bainite transformation
    if( (_temp[_qp] > _temp_Ms && _temp[_qp] <= _temp_Bs) 
      || (_temp_old[_qp] > _temp_Ms && _temp_old[_qp] <= _temp_Bs) )
    {
      std::tie(temp_split, dt_split) = _split_increment(_temp_Ms, _temp_Bs);
      fun_tc = _fun_tc(_temp_Bs-temp_split, 2, 0.29, _fcomp_b);
      std::tie(dxb, dnucb) = _diffusive_transformation_linear(_nucb_old[_qp], _xb_old[_qp], 
        fun_tc, temp_split, dt_split);
    }

    // Martensite transformation
    if( _temp[_qp] < _temp_Ms )
      dxm = _martensite_transformation();

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

}


Real  
SSPTSteel::_austenite_transformation(
  Real temp,
  Real dt
)
{
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
SSPTSteel::_diffusive_transformation_linear(
  Real nuc,
  Real x,
  Real fun_tc,
  Real temp,
  Real dt
)
{
  Real dnuc, dx, dxdt;

  dnuc = 0.0;
  dx = 0.0;

  // Nucleation phase
  if( nuc < 1.0 - _tolerance )
  {
    dxdt = fun_tc / 0.10434035495809084;
    dnuc = dxdt * dt;

    if( nuc + dnuc >= 1.0 )
    {
      dx = 0.01;
      dt = (nuc - 1.0) / dxdt;
    }
  }

  // Growth phase
  if( nuc + dnuc >= 1.0 - _tolerance )
  {
    dx += 0.5134989515253945 * dt * fun_tc;

    if( x + dx > 1.0 )
      dx = 1.0 - x;
  }

  return {dx, dnuc};
}


Real
SSPTSteel::_martensite_transformation()
{
  return (_xa[_qp] + _xm[_qp]) * (1.0 - exp(-1.1e-2*(_temp_Ms - _temp[_qp]))) - _xm[_qp];
}

Real 
SSPTSteel::_fun_tc(
  Real temp_ucool,
  int temp_power,
  Real Gsize_factor,
  Real fcomp
)
{
  return std::pow(temp_ucool, temp_power) * std::exp(-1.384e4/(_temp[_qp]+273.15)) 
        * std::pow(2.0, Gsize_factor*_Gsize[_qp]) / fcomp;
}


std::tuple<Real,Real> 
SSPTSteel::_split_increment(
  Real temp_lower,
  Real temp_upper
)
{
  Real temp, dt;

  // Temperature increase
  if( _temp[_qp] > _temp_old[_qp] )
  {
    // Lower bound
    if( _temp_old[_qp] < temp_lower )
    {
      temp = _temp[_qp];
      dt = (_temp[_qp] - temp_lower) / (_temp[_qp] - _temp_old[_qp]) * _dt;
    }
    // Upper bound
    else if( _temp[_qp] > temp_upper )
    {
      temp = temp_upper;
      dt = (temp_upper - _temp_old[_qp]) / (_temp[_qp] - _temp_old[_qp]) * _dt;
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
    if( _temp_old[_qp] > temp_upper )
    {
      temp = _temp[_qp];
      dt = (temp_upper - _temp[_qp]) / (_temp_old[_qp] - _temp[_qp]) * _dt;
    }
    // Lower bound
    else if( _temp[_qp] < temp_lower )
    {
      temp = temp_lower;
      dt = (_temp_old[_qp] - temp_lower) / (_temp_old[_qp] - _temp[_qp]) * _dt;
    }
    else
    {
      temp = _temp[_qp];
      dt = _dt;
    }
  }

  return {temp, dt};
}