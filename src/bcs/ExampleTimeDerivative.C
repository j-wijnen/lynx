#include "ExampleTimeDerivative.h"

#include "Material.h"

registerMooseObject("LynxApp", ExampleTimeDerivative);


InputParameters
ExampleTimeDerivative::validParams()
{
  InputParameters params = TimeDerivative::validParams();
  params.addParam<Real>("time_coefficient", 1.0, "Time coefficient");
  return params;
}


ExampleTimeDerivative::ExampleTimeDerivative(
  const InputParameters& params 
)
  : TimeDerivative(params),
  _time_coeff(getParam<Real>("time_coefficient"))
{

}


Real 
ExampleTimeDerivative::computeQpResidual()
{
  return _time_coeff * TimeDerivative::computeQpResidual();
}


Real 
ExampleTimeDerivative::computeQpJacobian()
{
  return _time_coeff * TimeDerivative::computeQpJacobian();
}