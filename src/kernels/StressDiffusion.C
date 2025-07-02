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

#include "StressDiffusion.h"

namespace lynx
{

registerMooseObject("LynxApp", StressDiffusion);

InputParameters
StressDiffusion::validParams()
{
  InputParameters params = Kernel::validParams();

  // Coupled hydrostatic stress variable
  params.addRequiredCoupledVar(
      "hydrostatic_stress", "The hydrostatic stress variable.");

  // Option to set names of consumed properties
  params.addParam<MaterialPropertyName>(
      "diffusivity", "D", "The diffusivity value or material property");

  return params;
}

StressDiffusion::StressDiffusion(const InputParameters & params)
  : Kernel(params),

  // Coupled variable
  _hydrostatic_stress(coupledValue("hydrostatic_stress")),
  _grad_hydrostatic_stress(coupledGradient("hydrostatic_stress")),

  // Consumed properties
  _diffusivity(getMaterialProperty<Real>("diffusivity")),

  // Parameters 
  _Vh(2000.),
  _R(8314.5),
  _T(293.15)
 {
}

Real
StressDiffusion::computeQpResidual()
{
  return -_grad_test[_i][_qp] * _Vh * _diffusivity[_qp] / (_R * _T)
    * _u[_qp] * _grad_hydrostatic_stress[_qp];
}

Real
StressDiffusion::computeQpJacobian()
{
  return -_grad_test[_i][_qp] * _Vh * _diffusivity[_qp] / (_R * _T)
    * _phi[_j][_qp] * _grad_hydrostatic_stress[_qp];
}

} // end namespace