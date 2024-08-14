#include "ExampleIC.h"

registerMooseObject("LynxApp", ExampleIC);


InputParameters
ExampleIC::validParams()
{
  InputParameters params = InitialCondition::validParams();
  params.addParam<Real>("coefficient", "The value of the initial condition");
  return params;
}


ExampleIC::ExampleIC(
  const InputParameters& params 
)
  : InitialCondition(params),
  _coefficient(getParam<Real>("coefficient"))
{

}


Real 
ExampleIC::value(
  const Point& p
)
{
  return 2. * _coefficient * std::abs(p(0));
}