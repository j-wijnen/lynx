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

#include "VolumetricExpansionFromDensity.h"
#include "RankTwoTensor.h"

namespace lynx
{

registerMooseObject("LynxApp", VolumetricExpansionFromDensity);

InputParameters
VolumetricExpansionFromDensity::validParams()
{
  InputParameters params = ComputeEigenstrainBase::validParams();

  params.addParam<MaterialPropertyName>("density_name", "density", "Name of the density property");
  params.addRequiredParam<Real>("reference_density", "Initial density of the material");

  return params;
}

VolumetricExpansionFromDensity::VolumetricExpansionFromDensity(const InputParameters & parameters)
  : ComputeEigenstrainBase(parameters),
  _density(getMaterialProperty<Real>("density_name")),
  _reference_density(getParam<Real>("reference_density"))
  
{
}

void
VolumetricExpansionFromDensity::computeQpEigenstrain()
{
  _eigenstrain[_qp] = (std::pow(_reference_density / _density[_qp], 1./3.) - 1.0)
    * RankTwoTensor::Identity(); 
}

}