#pragma once

#include "ADKernel.h"

class ADPhaseFieldFracture : public ADKernel 
{
public:
  static InputParameters validParams();

  ADPhaseFieldFracture(const InputParameters & params);

protected:

  virtual ADReal computeQpResidual() override;

  const Real _length_scale;

  const ADMaterialProperty<Real> & _crack_driving_force;
};