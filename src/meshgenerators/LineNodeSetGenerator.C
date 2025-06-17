//* This file is part of Lynx, 
//* an open-source application for the simulation  
//* of mechanics and multi-physics problems
//* https://github.com/j-wijnen/lynx
//*
//* Lynx is powered by the MOOSE Framework
//* https://www.mooseframework.org
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "LineNodeSetGenerator.h"
#include "MooseMeshUtils.h"
#include "CastUniquePointer.h"
#include "MooseUtils.h"

#include "libmesh/node.h"

namespace lynx
{

registerMooseObject("LynxApp", LineNodeSetGenerator);

InputParameters
LineNodeSetGenerator::validParams()
{
  InputParameters params = MeshGenerator::validParams();
  params.addClassDescription(
      "Assigns all of the nodes on a line with specified width to a new nodeset.");

  params.addRequiredParam<MeshGeneratorName>("input", 
    "The mesh we want to modify");
  params.addRequiredParam<std::vector<BoundaryName>>("new_boundary",
    "The name of the nodeset to create");
  params.addRequiredParam<RealVectorValue>("point1",
    "The first point (in x,y,z with spaces in-between) of the line to select the nodes");
  params.addRequiredParam<RealVectorValue>("point2",
    "The second point (in x,y,z with spaces in-between) of the line to select the nodes");
  params.addRequiredParam<Real>("width",
    "Width (diameter) of the line");

  return params;
}

LineNodeSetGenerator::LineNodeSetGenerator(const InputParameters & parameters)
  : MeshGenerator(parameters),
    _input(getMesh("input")),
    _x1(getParam<RealVectorValue>("point1")),
    _x2(getParam<RealVectorValue>("point2")),
    _width(getParam<Real>("width"))
{

}

std::unique_ptr<MeshBase>
LineNodeSetGenerator::generate()
{
  std::unique_ptr<MeshBase> mesh = std::move(_input);

  // Get the BoundaryIDs from the mesh
  std::vector<BoundaryName> boundary_names = getParam<std::vector<BoundaryName>>("new_boundary");
  std::vector<BoundaryID> boundary_ids =
      MooseMeshUtils::getBoundaryIDs(*mesh, boundary_names, true);
  if (boundary_ids.size() != 1)
    mooseError("Only one boundary ID can be assigned to a nodeset using a bounding box!");

  // Get a reference to our BoundaryInfo object
  BoundaryInfo & boundary_info = mesh->get_boundary_info();

  bool found_node = false;
  RealVectorValue line12 = _x2 - _x1;
  Real length12 = std::sqrt(line12 * line12);
  Real d, distance;
  RealVectorValue p;

  // Loop over the nodes and assign node set id to nodes close to the line
  for (auto & node : as_range(mesh->active_nodes_begin(), mesh->active_nodes_end()))
  {
    // Calculate projection onto line
    d = (*node - _x1) * line12 / (length12*length12);

    // Needs to fall in between x1-x2
    if (d < 0.0 || d > 1.0)
      continue;

    p = _x1 + d * line12;
    distance = (p - *node).norm();

    if (distance <= 0.5 * _width)
    {
      boundary_info.add_node(node, boundary_ids[0]);
      found_node = true;
    }
  }

  // Unless at least one processor found a node in the bounding box,
  // the user probably specified it incorrectly.
  this->comm().max(found_node);

  if (!found_node)
    mooseError("No nodes found within the bounding box");

  boundary_info.nodeset_name(boundary_ids[0]) = boundary_names[0];

  mesh->set_isnt_prepared();
  return dynamic_pointer_cast<MeshBase>(mesh);
}

} // end namespace