#pragma once

#include "InitialCondition.h"

/* Implements an initial condition
 * u = C
 */

class TestIC : public InitialCondition
{
public:

  static InputParameters validParams();

  TestIC(
    const InputParameters& params
  );

protected:

  virtual Real value(
    const Point& p 
  );

private: 

  const Real _coefficient;
};
