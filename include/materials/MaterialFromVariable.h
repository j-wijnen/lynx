#pragma once

#include "Material.h"

/**
 * Transfers a variable to a material property
 */
class MaterialFromVariable : public Material
{
public:
  static InputParameters validParams();

  MaterialFromVariable(const InputParameters & parameters);

protected:
  virtual void computeQpProperties() override;

  // Coupled variables
  const VariableValue & _var;

  // Declared properties
  MaterialProperty<Real> & _prop;
};

