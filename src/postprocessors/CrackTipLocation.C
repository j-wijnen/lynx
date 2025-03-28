#include "CrackTipLocation.h"
#include <algorithm>

registerMooseObject("LynxApp", CrackTipLocation);

InputParameters
CrackTipLocation::validParams()
{
  InputParameters params = NodalVariablePostprocessor::validParams();
  params.addClassDescription(
      "Computes the location of the crack tip in specified direction");
 
  MooseEnum direction("-x -y -z x y z");
  params.addRequiredParam<MooseEnum>("direction", direction, 
    "Direction of crack tip coordinate.");

  params.addParam<Real>("threshold", 0.95, 
    "Threshold to determine crack tip based on variable.");
  params.addParam<Real>("initial_value", 0.0,
    "Initial coordinate of the crack tip");

  return params;
}

CrackTipLocation::CrackTipLocation(const InputParameters & parameters)
  : NodalVariablePostprocessor(parameters), 
  _crack_tip_coord(getParam<Real>("initial_value")),
  _threshold(getParam<Real>("threshold"))
{
  MooseEnum direction = getParam<MooseEnum>("direction");

  if (direction == "-x")
  {
    _coord_idx = 0;
    _negative_direction= true;
  }
  else if (direction == "-y")
  {
    _coord_idx = 1;
    _negative_direction= true;
  }
  else if (direction == "-z")
  {
    _coord_idx = 2;
    _negative_direction= true;
  }
  else if (direction == "x")
  {
    _coord_idx = 0;
    _negative_direction= false;
  }
  else if (direction == "y")
  {
    _coord_idx = 1;
    _negative_direction= false;
  }
  else if (direction == "z")
  {
    _coord_idx = 2;
    _negative_direction= false;
  }
}

void 
CrackTipLocation::initialize()
{
}

void
CrackTipLocation::execute()
{
  // DOF not on node
  if(mooseVariable()->dofIndices().size() == 0)
    return;

  if(_u[_qp] < _threshold)
    return;

  Real coord = (*_current_node)(_coord_idx);

  if (_negative_direction)
    _crack_tip_coord = std::min(coord, _crack_tip_coord);
  else
    _crack_tip_coord = std::max(coord, _crack_tip_coord);     
}

Real
CrackTipLocation::getValue() const
{
  return _crack_tip_coord;
}

void
CrackTipLocation::threadJoin(const UserObject & y)
{
  const auto & pps = static_cast<const CrackTipLocation &>(y);

  if (_negative_direction)
    _crack_tip_coord = std::min(_crack_tip_coord, pps._crack_tip_coord);
  else
    _crack_tip_coord = std::max(_crack_tip_coord, pps._crack_tip_coord);
}

void
CrackTipLocation::finalize()
{
  if (_negative_direction)
    gatherMin(_crack_tip_coord);
  else
    gatherMax(_crack_tip_coord);
}