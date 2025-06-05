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

#include <array>
#include "ComputeEigenstrainBase.h"

namespace lynx
{

/**
 * Calculates the volumetric expansion of the material
 * based on changes in density
 */

class VolumetricExpansionFromDensity : public ComputeEigenstrainBase
{
public:
  static InputParameters validParams();

  VolumetricExpansionFromDensity(const InputParameters& params);

protected:
  virtual void computeQpEigenstrain() override;

  const MaterialProperty<Real> & _density;

  const Real _reference_density;
};

}