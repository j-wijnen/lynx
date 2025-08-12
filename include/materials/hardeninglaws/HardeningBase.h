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

#include "Material.h"

namespace lynx
{

/**
 * Base class for hardening laws
 */
template <bool is_ad>
class HardeningBase : public Material
{
public:
  static InputParameters validParams();

  HardeningBase(const InputParameters & params);

  virtual void coupleQp(unsigned int * qp);

  virtual void computeProperties() override;

  virtual void resetProperties() override;

  virtual GenericReal<is_ad> getYieldStress(GenericReal<is_ad> plastic_multiplier);

  virtual GenericReal<is_ad> getDerivative(GenericReal<is_ad> plastic_multiplier);

protected:
  unsigned int * _qp;
};

} // end namespace