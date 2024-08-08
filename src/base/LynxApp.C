#include "LynxApp.h"
#include "Moose.h"
#include "AppFactory.h"
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
