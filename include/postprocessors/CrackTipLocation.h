#pragma once

#include "NodalVariablePostprocessor.h"

/**
 * Computes the location of the crack tip. This is the maximum nodal
 * coordinate in the specified direction where the damage variable
 * is greater than a threshold (0.95 by default).
 */
class CrackTipLocation : public NodalVariablePostprocessor
{
public:
  static InputParameters validParams();

  CrackTipLocation(const InputParameters & parameters);

  virtual void initialize() override;

  virtual void execute() override;
  
  virtual Real getValue() const override;

  virtual void threadJoin(const UserObject & y) override;

  virtual void finalize() override;

protected:
  Real _crack_tip_coord;

  const Real _threshold;

  int _coord_idx;
  bool _negative_direction;
};