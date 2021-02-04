#include "SweepApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"
#include "MooseSyntax.h"

InputParameters
SweepApp::validParams()
{
  InputParameters params = MooseApp::validParams();

  // Do not use legacy DirichletBC, that is, set DirichletBC default for preset = true
  params.set<bool>("use_legacy_dirichlet_bc") = false;

  return params;
}

SweepApp::SweepApp(InputParameters parameters) : MooseApp(parameters)
{
  SweepApp::registerAll(_factory, _action_factory, _syntax);
}

SweepApp::~SweepApp() {}

void
SweepApp::registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  ModulesApp::registerAll(f, af, s);
  Registry::registerObjectsTo(f, {"SweepApp"});
  Registry::registerActionsTo(af, {"SweepApp"});

  /* register custom execute flags, action syntax, etc. here */
}

void
SweepApp::registerApps()
{
  registerApp(SweepApp);
}

/***************************************************************************************************
 *********************** Dynamic Library Entry Points - DO NOT MODIFY ******************************
 **************************************************************************************************/
extern "C" void
SweepApp__registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  SweepApp::registerAll(f, af, s);
}
extern "C" void
SweepApp__registerApps()
{
  SweepApp::registerApps();
}
