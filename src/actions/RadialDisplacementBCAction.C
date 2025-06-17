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

#include "RadialDisplacementBCAction.h"
#include "Factory.h"
#include "FEProblem.h"
#include "Conversion.h"

namespace lynx
{


registerMooseAction("LynxApp", RadialDisplacementBCAction, "add_bc");

InputParameters
RadialDisplacementBCAction::validParams()
{
  InputParameters params = Action::validParams();
  params.addClassDescription("Set up radial displacement boundary conditions");

  params.addRequiredParam<std::vector<BoundaryName>>("boundary",
    "The list of boundary IDs from the mesh where the pressure will be applied");

  params.addRequiredParam<std::vector<VariableName>>("displacements",
    "The displacements appropriate for the simulation");
  params.addParam<std::vector<AuxVariableName>>("save_in", {}, "The displacement residuals");

  params.addParam<bool>("fixed", false, 
    "Flag to constrain tangential displacement");
  params.addParam<Real>("penalty", 0.0,
    "Penalty parameter");
  params.addParam<bool>("use_automatic_differentiation", false,
    "Flag to use automatic differentiation (AD) objects when possible");
  return params;
}

RadialDisplacementBCAction::RadialDisplacementBCAction(const InputParameters & params)
  : Action(params),
    _use_ad(getParam<bool>("use_automatic_differentiation")),
    _displacements(getParam<std::vector<VariableName>>("displacements")),
    _ndisp(_displacements.size()),
    _save_in(getParam<std::vector<AuxVariableName>>("save_in"))
{
  if (_ndisp == 1)
    mooseError("RadialDisplacementBC is specific to 2D and 3D models.");

  if (_save_in.size() != 0 && _save_in.size() != _ndisp)
    mooseError("Number of save_in variables should equal to the number of displacement variables ",
               _displacements.size());

}

void
RadialDisplacementBCAction::act()
{
  std::string ad_prepend = "";
  if (_use_ad)
    ad_prepend = "AD";
  const std::string kernel_name = ad_prepend + "RadialDisplacementBC";

  // Create radial BC Kernels
  for (unsigned int i = 0; i < 2; ++i)
  {
    // Create unique kernel name for each of the components
    std::string unique_kernel_name = kernel_name + "_" + _name + "_" + Moose::stringify(i);

    InputParameters params = _factory.getValidParams(kernel_name);
    params.applyParameters(parameters());
    params.set<bool>("use_displaced_mesh") = false;
    params.set<unsigned int>("component") = i;
    params.set<NonlinearVariableName>("variable") = _displacements[i];

    if (_save_in.size() == 2)
      params.set<std::vector<AuxVariableName>>("save_in") = {_save_in[i]};

    _problem->addBoundaryCondition(kernel_name, unique_kernel_name, params);
  }
}

} // end namespace