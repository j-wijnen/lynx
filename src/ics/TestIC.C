#include "TestIC.h"

registerMooseObject("LynxApp", TestIC);


InputParameters
TestIC::validParams()
{
  InputParameters params = InitialCondition::validParams();
  params.addParam<Real>("value", "The value of the initial condition");
  return params;
}


TestIC::TestIC(
  const InputParameters& params 
)
  : InitialCondition(params),
  _coefficient(getParam<Real>("value"))
{

}


Real 
TestIC::value(
  const Point& p
)
{
  if( ( p(0) > 39.9 && p(1) > 9.9 ) || ( p(0) < 0.1 && p(1) < 0.1 ) )
    std::cout << " Set IC point [" << p(0) << ", " << p(1) << "]" << std::endl;
  return _coefficient;
}