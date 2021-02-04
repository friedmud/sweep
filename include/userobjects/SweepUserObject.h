//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "SweepStudy.h"

#include "GeneralUserObject.h"

// Forward Declarations
class SweepUserObject;

template <>
InputParameters validParams<SweepUserObject>();

class SweepUserObject : public GeneralUserObject
{
public:
  static InputParameters validParams();

  SweepUserObject(const InputParameters & parameters);

  virtual void initialize() override {}
  virtual void execute() override;
  virtual void finalize() override {}

protected:

  SweepStudy _sweep_study;
};
