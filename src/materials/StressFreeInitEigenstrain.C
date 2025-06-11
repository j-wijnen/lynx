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

#include "StressFreeInitEigenstrain.h"
#include "RankTwoTensor.h"

namespace lynx
{

registerMooseObject("LynxApp", StressFreeInitEigenstrain);

InputParameters
StressFreeInitEigenstrain::validParams()
{
  InputParameters params = ComputeEigenstrainBase::validParams();

  params.addRequiredCoupledVar(
      "displacements",
      "The displacements appropriate for the simulation geometry and coordinate system");

  return params;
}

StressFreeInitEigenstrain::StressFreeInitEigenstrain(const InputParameters & parameters)
  : ComputeEigenstrainBase(parameters),
  _ndisp(coupledComponents("displacements")),
  _disp(coupledValues("displacements")),
  _grad_disp(coupledGradients("displacements")),
  _eigenstrain_old(getMaterialPropertyOld<RankTwoTensor>(_eigenstrain_name))  
{
  // set unused dimensions to zero
  _disp.resize(3, &_zero);
  _grad_disp.resize(3, &_grad_zero);
}

// StressFreeInitEigenstrain::initStatefulProperties
// {
//   for (_qp = 0; _qp < n_points; ++_qp)
//     initQpStatefulProperties();
 
//    // checking for statefulness of properties via this loop is necessary
//    // because owned props might have been promoted to stateful by calls to
//    // getMaterialProperty[Old/Older] from other objects.  In these cases, this
//    // object won't otherwise know that it owns stateful properties.
//   for (const auto id : _supplied_prop_ids)
//     if (materialData().getMaterialPropertyStorage().getPropRecord(id).stateful() &&
//         !_overrides_init_stateful_props)
//       mooseWarning(std::string("Material \"") + name() +
//                   "\" provides one or more stateful "
//                   "properties but initQpStatefulProperties() "
//                   "was not overridden in the derived class.");
//  }

void 
StressFreeInitEigenstrain::initQpStatefulProperties()
{
  // Initialize eigenstrain as current strain
  const auto grad_tensor = RankTwoTensor ::initializeFromRows(
    (*_grad_disp[0])[_qp], (*_grad_disp[1])[_qp], (*_grad_disp[2])[_qp]);

  _eigenstrain[_qp] = 0.5 * (grad_tensor + grad_tensor.transpose());
}

void
StressFreeInitEigenstrain::computeQpEigenstrain()
{
  _eigenstrain[_qp] = _eigenstrain_old[_qp];
}

} // end namespace