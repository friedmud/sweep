//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html
#include "SweepTestApp.h"
#include "SweepApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "MooseSyntax.h"
#include "ModulesApp.h"

InputParameters
SweepTestApp::validParams()
{
  InputParameters params = SweepApp::validParams();
  return params;
}

SweepTestApp::SweepTestApp(InputParameters parameters) : MooseApp(parameters)
{
  SweepTestApp::registerAll(
      _factory, _action_factory, _syntax, getParam<bool>("allow_test_objects"));
}

SweepTestApp::~SweepTestApp() {}

void
SweepTestApp::registerAll(Factory & f, ActionFactory & af, Syntax & s, bool use_test_objs)
{
  SweepApp::registerAll(f, af, s);
  if (use_test_objs)
  {
    Registry::registerObjectsTo(f, {"SweepTestApp"});
    Registry::registerActionsTo(af, {"SweepTestApp"});
  }
}

void
SweepTestApp::registerApps()
{
  registerApp(SweepApp);
  registerApp(SweepTestApp);
}

/***************************************************************************************************
 *********************** Dynamic Library Entry Points - DO NOT MODIFY ******************************
 **************************************************************************************************/
// External entry point for dynamic application loading
extern "C" void
SweepTestApp__registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  SweepTestApp::registerAll(f, af, s);
}
extern "C" void
SweepTestApp__registerApps()
{
  SweepTestApp::registerApps();
}
