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
 * volumetric locking correction is currently not accounted for
 */

class StressFreeInitEigenstrain : public ComputeEigenstrainBase
{
public:
  static InputParameters validParams();

  StressFreeInitEigenstrain(const InputParameters& params);

protected:
  virtual void initQpStatefulProperties() override;

  virtual void computeQpEigenstrain() override;

  /// Coupled displacement variables
  unsigned int _ndisp;
  std::vector<const VariableValue *> _disp;
  std::vector<const VariableGradient *> _grad_disp;

  // Stateful property, current property is inherited
  const MaterialProperty<RankTwoTensor> & _eigenstrain_old;

};

}