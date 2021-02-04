//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "SweepUserObject.h"

registerMooseObject("MooseApp", SweepUserObject);

defineLegacyParams(SweepUserObject);

InputParameters
SweepUserObject::validParams()
{
  InputParameters params = GeneralUserObject::validParams();
  params.addClassDescription("Performs a parallel sweep");

  params += SweepStudy::validParams();

  return params;
}

SweepUserObject::SweepUserObject(const InputParameters & parameters)
    : GeneralUserObject(parameters),
      _sweep_study(this->comm(), parameters, _subproblem.mesh())
{
}

void
SweepUserObject::execute()
{
  _sweep_study.preExecute();
  _sweep_study.generateWork();
  _sweep_study.execute();
}
