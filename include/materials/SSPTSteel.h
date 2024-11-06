#pragma once

#include "Material.h"

#include "LinearInterpolation.h"


enum Phase {
  ferrite,
  pearlite,
  bainite,
  martensite,
  austenite
};


class SSPTSteel : public Material 
{

public:

  static InputParameters validParams();

  SSPTSteel(
    const InputParameters& params 
  );

protected:

  virtual void initQpStatefulProperties() override;

  virtual void computeQpProperties() override;

private:

  // Austenite transformation using LeBlond equation
  Real _austenite_transformation(
    Real temp,
    Real dt
  );

  // Diffusive transformation using Kirkaldy equations
  // returns (dx, dnuc)
  std::tuple<Real,Real> _diffusive_transformation_linear(
    Phase phase
  );

  std::tuple<Real,Real> _diffusive_transformation_old(
    Real nuc,
    Real x,
    Real temp_lower,
    Real temp_upper,
    Real fun_tc,
    Real Gsize_factor,
    int ucooltemp_power
  );

  // Calculates the incremenet in martensite upon heating using the
  // Koistenen-Marburger equation
  Real _martensite_transformation();

  Real _fun_tc(
    Real fcomp,
    Real Gsize_factor,
    Real ucool_temp,
    int ucool_power
  );

  // Reduces the time increment to be between lower and upper bounds
  // returns (T_split, dt_split)
  std::tuple<Real,Real> _split_increment(
    Real temp_lower,
    Real temp_upper
  );

  // Coupled variables
  const VariableValue & _temp, 
                      & _temp_old;

  // Statefull material properties
  MaterialProperty<Real> & _xa,
                         & _xf,
                         & _xp,
                         & _xb,
                         & _xm,
                         & _nucf,
                         & _nucp,
                         & _nucb,
                         & _Gsize;

  const MaterialProperty<Real> & _xa_old,
                               & _xf_old,
                               & _xp_old,
                               & _xb_old,
                               & _xm_old,
                               & _nucf_old,
                               & _nucp_old,
                               & _nucb_old;

  // Initial phase fractions (input)
  Real  _xa_init,
        _xf_init,
        _xp_init,
        _xb_init,
        _xm_init;

  // Composition (input)
  Real  _comp_C,
        _comp_Mn,
        _comp_Si,
        _comp_Ni,
        _comp_Cr,
        _comp_Mo,
        _comp_Co,
        _comp_V,
        _comp_W,
        _comp_As,
        _comp_Al,
        _comp_P,
        _comp_Ti,
        _comp_Cu;

  // Transformation temperatures
  Real  _temp_Ae3, 
        _temp_Ae1,
        _temp_Bs,
        _temp_Ms;

  // Compositional functions
  Real  _fcomp_f,
        _fcomp_p,
        _fcomp_b;

  // Misc. constants
  const Real _tolerance;
};

