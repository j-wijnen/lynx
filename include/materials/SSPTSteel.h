#pragma once

#include "Material.h"

// Use enum to index/loop phases
enum Phase {
  ferrite,
  pearlite,
  bainite,
  martensite,
  austenite
};

const std::array<Phase,5> phases = {
  ferrite, 
  pearlite, 
  bainite, 
  martensite, 
  austenite
};

// SSPTSteel model definition
class SSPTSteel : public Material 
{
public:

  static InputParameters validParams();

  SSPTSteel(
    const InputParameters& params 
  );

protected:

  // Compute quadrature point properties
  virtual void initQpStatefulProperties() override;

  virtual void computeQpProperties() override;

  // Austenite transformation using LeBlond equation
  Real austeniteTransformation();

  Real grainGrowth();

  // Diffusive transformation using Kirkaldy equations
  std::tuple<Real,Real> diffusiveTransformation(
    Phase phase,
    Real x,
    Real nuc
  );

  Real diffusiveTransformationResidual(
    Real x0,
    Real x,
    Real dt,
    Real fun_tc
  );

  Real funTc(
    Phase phase,
    Real temp
  );

  // Calculates the incremenet in martensite upon heating using the
  // Koistenen-Marburger equation
  Real martensiteTransformation();

  // Checks whether temperature is in transformation range
  bool checkTemperatureRange(Phase phase);

  // Reduces the time increment to be between lower and upper bounds
  // returns (T_split, dt_split)
  std::tuple<Real,Real> splitIncrementHeating(Phase phase);

  std::tuple<Real,Real> splitIncrementCooldown(Phase phase);

  // Coupled variables
  const VariableValue & _temp, 
                      & _temp_old;

  // Stateful material properties
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
                               & _nucb_old,
                               & _Gsize_old;

  // Initial phase fractions (input)
  std::array<Real,5> _x_init;

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
  Real _temp_Ae3,
       _temp_Ae1,
       _temp_Bs,
       _temp_Ms;

  std::array<Real,5>  _temp_lower,
                      _temp_upper;

  // Grain sizes (PAG)
  Real _Gsize_init,
       _Gsize_min,
       _Gsize_max;

  // Compositional functions
  std::array<Real,3> _fcomp;

  // Phase dependent constants
  const std::array<Real,3> _Gsize_factor = {0.41, 0.32, 0.29};
  const std::array<int,3> _ucool_exponent = {3, 3, 2};

  // Misc. constants
  const Real _tolerance = 1e-4;
};

