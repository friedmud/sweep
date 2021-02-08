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

  params.addRequiredParam<VariableName>("variable", "The name of the Auxiliary Variable to fill");
  params.addRequiredParam<BoundaryName>("boundary", "The starting boundary to sweep from");

  params.addRelationshipManager(
    "ElementSideNeighborLayers",
    Moose::RelationshipManagerType::GEOMETRIC,

    [](const InputParameters & /*obj_params*/, InputParameters & rm_params) {
      rm_params.set<unsigned short>("layers") = 1;
    }
  );

  return params;
}

SweepUserObject::SweepUserObject(const InputParameters & parameters)
    : GeneralUserObject(parameters),
      _sweep_study(this->comm(), parameters, _subproblem.mesh(), _fe_problem.getAuxiliarySystem())
{
}

void
SweepUserObject::execute()
{
  _sweep_study.preExecute();
  _sweep_study.generateWork();
  _sweep_study.execute();
  _sweep_study.postExecute();
}
