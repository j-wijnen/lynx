#pragma once

#include "IntegratedBC.h"

/**
 * Weakly enforce a Dirichlet BC using a penalty term. This class is
 * an alternative to the DirichletBC that maintains the symmetry (if
 * any) present in the original problem, and does not involve
 * explicitly zeroing matrix rows for its implementation. The main
 * drawback of this approach is that the penalty parameter must tend
 * to infinity in order for the constraint to be satisfied in the
 * limit as h->0, and this causes the Jacobian matrix to be
 * ill-conditioned.
 *
 * The weak form contribution for this term is:
 *
 * \f$ (p (u - g), \psi)_{\Gamma} \f$,
 *
 * where:
 * p = penalty parameter (user-selectable)
 * u = the unknown
 * g = Dirichlet data (given)
 * Gamma = the part of the boundary where the penalty BC is applied.
 */
class LinearRampPenaltyDirichletBC : public IntegratedBC
{
public:
  static InputParameters validParams();

  LinearRampPenaltyDirichletBC(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;

private:
  Real _penalty;
  Real _value;
  Real _start_time;
  Real _end_time;
  
  // Variable from previous timestep
  const VariableValue & _u_old;
};
