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

#include "LynxApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ActionFactory.h"
#include "ModulesApp.h"
#include "MooseSyntax.h"

InputParameters
LynxApp::validParams()
{
  InputParameters params = MooseApp::validParams();
  params.set<bool>("use_legacy_material_output") = false;
  params.set<bool>("use_legacy_initial_residual_evaluation_behavior") = false;
  return params;
}

LynxApp::LynxApp(InputParameters parameters) : MooseApp(parameters)
{
  LynxApp::registerAll(_factory, _action_factory, _syntax);
}

LynxApp::~LynxApp() {}

void
LynxApp::registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  ModulesApp::registerAllObjects<LynxApp>(f, af, s);
  Registry::registerObjectsTo(f, {"LynxApp"});
  Registry::registerActionsTo(af, {"LynxApp"});

  /* register custom execute flags, action syntax, etc. here */
  //registerSyntax("RadialDisplacementBC", "Actions/RadialDisplacementBCAction/*");
}

void
LynxApp::registerApps()
{
  registerApp(LynxApp);
}

/***************************************************************************************************
 *********************** Dynamic Library Entry Points - DO NOT MODIFY ******************************
 **************************************************************************************************/
extern "C" void
LynxApp__registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  LynxApp::registerAll(f, af, s);
}
extern "C" void
LynxApp__registerApps()
{
  LynxApp::registerApps();
}
