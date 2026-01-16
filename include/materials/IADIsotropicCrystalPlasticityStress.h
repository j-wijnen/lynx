
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

#include "ComputeLagrangianStressPK1.h"

namespace lynx
{

/**
 * Finite isotropic plasticity with similar implementation
 * as crystal plasticity
 */

class IADIsotropicCrystalPlasticityStress : public ComputeLagrangianStressPK1
{
public:
  static InputParameters validParams();

  IADIsotropicCrystalPlasticityStress(const InputParameters & parameters);

protected:
  virtual void computeQpPK1Stress() override;

  virtual void initQpStatefulProperties() override;

  // Declared properties
  MaterialProperty<RankTwoTensor> & _Fp;
  MaterialProperty<Real> & _lamtot;

  // Stateful properties
  const MaterialProperty<RankTwoTensor> & _Fp_old;
  const MaterialProperty<Real> & _lamtot_old;

  // Consumed properties
  const MaterialProperty<RankFourTensor> & _elasticity_tensor;

  // Parameters
  const Real _lamdot0;
  const Real _yieldstress0;
  const Real _rate_sensitivity;

  const Real _rel_tol;
  const Real _abs_tol;

  const size_t _lamdot_idx;


};

} // end namespace