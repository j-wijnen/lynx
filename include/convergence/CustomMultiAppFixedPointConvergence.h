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

#include "DefaultMultiAppFixedPointConvergence.h"

// Libmesh/PETSc includes
#include "libmesh/enum_norm_type.h"
#include <petsc.h>
#include <petscmat.h>

namespace lynx {

/**
 * A custom convergence class inheriting from DefaultMultiAppFixedPointConvergence.
 *
 * This class can be used to extend or modify the default behavior for
 * MultiApp fixed point convergence checks.
 */
class CustomMultiAppFixedPointConvergence : public DefaultMultiAppFixedPointConvergence
{
public:
  static InputParameters validParams();

  CustomMultiAppFixedPointConvergence(const InputParameters & params);

  ~CustomMultiAppFixedPointConvergence();


  /**
   * Implement custom convergence checking logic.
   */
  virtual MooseConvergenceStatus checkConvergence(unsigned int iter) override;

protected:

  /**
   * Print the residual of the current fixed-point iteration to file
   */
  virtual void printFixedPointConvergenceToFile(unsigned int iter,
                                                Real initial_norm,
                                                const std::vector<Real> & timestep_begin_norms,
                                                const std::vector<Real> & timestep_end_norms);

  std::ofstream _file;

  /**
   * Reference vector implementation
   */
  bool _has_reference_vector;
  TagID _reference_vector_tag_id;
  const NumericVector<Number> * _reference_vector;
  libMesh::FEMNormType _norm_type;
};

} // end namespace