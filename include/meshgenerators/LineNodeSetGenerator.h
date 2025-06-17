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

#include "MeshGenerator.h"

namespace lynx
{

/**
 * Selects a set of nodes and assigns a nodeset name to them based on
 * a line defined between two points and a width. In 3D, the line is 
 * extruded along a specified axis. 
 */
class LineNodeSetGenerator : public MeshGenerator
{
public:
  static InputParameters validParams();

  LineNodeSetGenerator(const InputParameters & parameters);

  std::unique_ptr<MeshBase> generate() override;

protected:
  std::unique_ptr<MeshBase> & _input;

  // Line definition
  // const std::vector<Real> _x1;
  // const std::vector<Real> _x2;
  const RealVectorValue _x1;
  const RealVectorValue _x2;
  const Real _width;
};

} // end namespace