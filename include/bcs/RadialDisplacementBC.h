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

#pragma once

#include "ADIntegratedBC.h"
#include "IntegratedBC.h"

namespace lynx
{

/*  Applies a linearly increasing radial displacement
 */

template<bool is_ad>
using RadialDisplacementBCBase = typename std::conditional<is_ad, ADIntegratedBC, IntegratedBC>::type;

template<bool is_ad>
class RadialDisplacementBCTempl : public RadialDisplacementBCBase<is_ad>
{
public:
  static InputParameters validParams();

  RadialDisplacementBCTempl(const InputParameters & params);

protected:
  virtual GenericReal<is_ad> computeQpResidual() override;

  // Coupled displacements
  const std::vector<const GenericVariableValue<is_ad> *> _disp;
  const std::vector<unsigned int> _disp_var;

  // Parameters
  const MooseEnum _component;
  const Real _value;
  const Real _penalty;
  const std::vector<Real> _origin;
  const Real _start_time;
  const Real _end_time;

  std::vector<Real> _dd_du;

  using RadialDisplacementBCBase<is_ad>::_t;
  using RadialDisplacementBCBase<is_ad>::_i;
  using RadialDisplacementBCBase<is_ad>::_qp;
  using RadialDisplacementBCBase<is_ad>::_q_point;
  using RadialDisplacementBCBase<is_ad>::_test;
};

class RadialDisplacementBC : public RadialDisplacementBCTempl<false>
{
public:
  using RadialDisplacementBCTempl<false>::RadialDisplacementBCTempl;

protected:
  virtual Real computeQpJacobian() override;
  virtual Real computeQpOffDiagJacobian(const unsigned int jvar_num) override;
};

typedef RadialDisplacementBCTempl<true> ADRadialDisplacementBC;

} // end namespace