#include "ExampleDiffusion.h"

registerMooseObject("LynxApp", ExampleDiffusion);


InputParameters
ExampleDiffusion::validParams()
{
  InputParameters params = Diffusion::validParams();
  return params;
}


ExampleDiffusion::ExampleDiffusion(
  const InputParameters & params
)
  : Diffusion(params),
    _diffusivity(getMaterialProperty<Real>("diffusivity"))
{

}


Real
ExampleDiffusion::computeQpResidual()
{
  return _diffusivity[_qp] * Diffusion::computeQpResidual();
}


Real
ExampleDiffusion::computeQpJacobian()
{
  return _diffusivity[_qp] * Diffusion::computeQpJacobian();
}