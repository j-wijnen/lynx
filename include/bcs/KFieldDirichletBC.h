#pragma once

#include "DirichletBCBase.h"

/*  Applies a linearly increasing K-field to the boundary
 */

class KFieldDirichletBC : public DirichletBCBase
{
public:
  static InputParameters validParams();

  KFieldDirichletBC(const InputParameters & params);

protected:
  virtual Real computeQpValue() override;

  MooseEnum _direction;
  Real _K_value;
  Real _J_value;
  Real _youngs_modulus;
  Real _poissons_ratio;
  Real _start_time;
  Real _end_time;
  bool _J_prescribed;
};