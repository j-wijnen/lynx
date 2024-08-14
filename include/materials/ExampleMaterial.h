#pragma once

#include "Material.h"

#include "LinearInterpolation.h"


class ExampleMaterial : public Material 
{

public:

  static InputParameters validParams();

  ExampleMaterial(
    const InputParameters& params 
  );

protected:

  virtual void computeQpProperties() override;

private:

  // member variable to hold the computed diffusivity coefficient
  MaterialProperty<Real>& _diffusivity;
  // member variable to hold the computed convection velocity gradient term
  MaterialProperty<RealGradient>& _convection_velocity;

  /// A place to store the coupled variable gradient for calculating the convection velocity
  /// property.
  const VariableGradient& _diffusion_gradient;

  /// A helper object for performaing linear interpolations on tabulated data for calculating the
  /// diffusivity property.
  LinearInterpolation _piecewise_func;

};