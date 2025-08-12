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

#include "CustomMultiAppFixedPointConvergence.h"

namespace lynx {

registerMooseObject("LynxApp", CustomMultiAppFixedPointConvergence);

InputParameters
CustomMultiAppFixedPointConvergence::validParams()
{
  InputParameters params = DefaultMultiAppFixedPointConvergence::validParams();
  params.addClassDescription("A custom convergence checker for MultiApp fixed point solves that "
                             "inherits from DefaultMultiAppFixedPointConvergence.");
  
  return params;
}

CustomMultiAppFixedPointConvergence::CustomMultiAppFixedPointConvergence(const InputParameters & params)
  : DefaultMultiAppFixedPointConvergence(params)
{
}


Convergence::MooseConvergenceStatus
CustomMultiAppFixedPointConvergence::checkConvergence(unsigned int iter)
{
  // You can call the base class to get its status first
  MooseConvergenceStatus status = DefaultMultiAppFixedPointConvergence::checkConvergence(iter);

  _console << "Im here!!!!!!!!!!!!!!!!!!" << std::endl;
  if (iter > 1)
  {
    Real difference = std::abs(_fixed_point_timestep_end_norm[iter] - _fixed_point_timestep_end_norm[iter-1]);
    _console << "Difference: " << difference << std::endl;
  }

  return status;
}

} // end namespace

