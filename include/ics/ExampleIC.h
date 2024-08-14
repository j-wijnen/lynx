#pragma once

#include "InitialCondition.h"

/* Implements an initial condition
 * u = 2 * C * x
 */

class ExampleIC : public InitialCondition
{
public:

  static InputParameters validParams();

  ExampleIC(
    const InputParameters& params
  );

protected:

  virtual Real value(
    const Point& p 
  );

private: 

  const Real _coefficient;
};
