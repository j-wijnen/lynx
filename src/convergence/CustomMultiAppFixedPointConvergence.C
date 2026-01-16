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
#include "FEProblemBase.h"
#include "PetscSupport.h"
#include "Executioner.h"
#include "NonlinearSystemBase.h"
#include "TaggingInterface.h"
#include "AuxiliarySystem.h"
#include "MooseVariableScalar.h"
#include "NonlinearSystem.h"

namespace lynx 
{

registerMooseObject("LynxApp", CustomMultiAppFixedPointConvergence);

InputParameters
CustomMultiAppFixedPointConvergence::validParams()
{
  InputParameters params = DefaultMultiAppFixedPointConvergence::validParams();
  params.addClassDescription("A custom convergence checker for MultiApp fixed point solves that "
                             "inherits from DefaultMultiAppFixedPointConvergence.");

  params.addParam<std::string>("filename", "File to write convergence information to");
  
  params.addParam<TagName>("reference_vector", "Reference vector for convergence checking");

  return params;
}

CustomMultiAppFixedPointConvergence::CustomMultiAppFixedPointConvergence(const InputParameters & params)
  : DefaultMultiAppFixedPointConvergence(params),
   _has_reference_vector(false),
   _reference_vector(nullptr)
{
  if (isParamValid("filename"))
  {
    std::string filename = getParam<std::string>("filename");
    _file.open(filename.c_str());
    if (!_file.is_open())
      mooseError("Could not open file: ", filename);

    // Set float format
    _file << std::scientific << std::setprecision(7);
  }

  if (isParamValid("reference_vector"))
  {
    _has_reference_vector = true;
    _reference_vector_tag_id = _fe_problem.getVectorTagID(getParam<TagName>("reference_vector"));
    _reference_vector = &_fe_problem.getNonlinearSystemBase(0).getVector(_reference_vector_tag_id);
  }
}

CustomMultiAppFixedPointConvergence::~CustomMultiAppFixedPointConvergence()
{
  if (_file.is_open())
    _file.close();
}

Convergence::MooseConvergenceStatus
CustomMultiAppFixedPointConvergence::checkConvergence(unsigned int iter)
{
  MooseConvergenceStatus status;
  if (!_has_reference_vector)
    status = DefaultMultiAppFixedPointConvergence::checkConvergence(iter);
  else
  {

  }

  printFixedPointConvergenceToFile(iter,
                                   _fixed_point_initial_norm,
                                   _fixed_point_timestep_begin_norm,
                                   _fixed_point_timestep_end_norm);

  // Exit if default class is not iterating
  if (status != MooseConvergenceStatus::ITERATING)
    return status;

  return status;
}

void
CustomMultiAppFixedPointConvergence::printFixedPointConvergenceToFile(unsigned int iter,
                                                                      Real initial_norm,
                                                                      const std::vector<Real> & timestep_begin_norms,
                                                                      const std::vector<Real> & timestep_end_norms)
{
  // Do nothing if file is not open
  if (!_file.is_open())
    return;

  Real max_norm = std::max(timestep_begin_norms[iter], timestep_end_norms[iter]);
  Real max_norm_prev;
  
  // New timestep
  if (iter == 0)
  {
    if (_t_step > 1)
      _file << '\n';

    _file << "Time Step " << _t_step << '\n'
          << " 0 Picard |R| = "
          << std::min(std::numeric_limits<Real>::max(), initial_norm) << '\n';

    max_norm_prev = initial_norm;
  }
  else
    max_norm_prev = std::max(timestep_begin_norms[iter-1], timestep_end_norms[iter-1]);

  // Print residual norms
  _file << std::setw(2) << iter + 1
        << " Picard |R| = " << max_norm 
        << ", rate = " << (max_norm - max_norm_prev) / max_norm_prev << '\n';

  _file << std::flush;
}

} // end namespace

