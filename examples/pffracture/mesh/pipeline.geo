// Gmsh .geo file for a 2D hollow circle (annulus)

//------------------------------------------------------------------
// Parameters
//------------------------------------------------------------------

// Define the radii for the inner and outer circles
R_inner = 500.0;
R_outer = 510.0;

// Define a characteristic length for the mesh elements.
// A smaller value will result in a finer mesh.
h_max = 5.0;
h_min = 0.1;
W_box = 10;

//------------------------------------------------------------------
// Geometry Definition
//------------------------------------------------------------------

// 1. Define the center point for both circles
Point(1) = {0, 0, 0, h_max};

// 2. Define the points for the outer circle
//    Points are defined as: Point(tag) = {x, y, z, characteristic_length};
Point(2) = {R_outer, 0, 0, h_max};
Point(3) = {-R_outer, 0, 0, h_max};

// 3. Define the points for the inner circle
Point(4) = {R_inner, 0, 0, h_max};
Point(5) = {-R_inner, 0, 0, h_max};

// 4. Define the curves (arcs) for the circles
//    Curves (Circles) are defined as: Circle(tag) = {start_point, center_point, end_point};
//    We create two semi-circles for each full circle.

// Outer circle arcs
Circle(1) = {2, 1, 3}; // Top semi-circle
Circle(2) = {3, 1, 2}; // Bottom semi-circle

// Inner circle arcs
Circle(3) = {4, 1, 5}; // Top semi-circle
Circle(4) = {5, 1, 4}; // Bottom semi-circle

// 5. Define the curve loops
//    A curve loop is a closed boundary of curves.
//    The order of curves determines the direction (clockwise/counter-clockwise).

Curve Loop(1) = {1, 2};    // Outer boundary
Curve Loop(2) = {3, 4};    // Inner boundary (the hole)

// 6. Define the surface
//    A plane surface is defined by one or more curve loops.
//    The first loop is the exterior boundary. Subsequent loops define holes.
Plane Surface(1) = {1, 2};

//------------------------------------------------------------------
// Physical Groups (Optional but recommended)
//------------------------------------------------------------------
// This helps in identifying boundaries and surfaces in the solver.

Physical Curve("inner_boundary", 1) = {3, 4};
Physical Curve("outer_boundary", 2) = {1, 2};
Physical Surface("domain", 3) = {1};
Physical Point("inner_left") = {5};
Physical Point("inner_right") = {4};
Physical Point("outer_left") = {3};
Physical Point("outer_right") = {2};

//------------------------------------------------------------------
// Mesh size
//------------------------------------------------------------------

Mesh.MeshSizeFromPoints = 0;
Mesh.MeshSizeFromCurvature = 0;
Mesh.MeshSizeExtendFromBoundary = 0;

Field[1] = Box;
Field[1].VIn = h_min;
Field[1].VOut = h_max;
Field[1].XMin = -0.5*W_box;
Field[1].XMax = 0.5*W_box;
Field[1].YMin = 0.0;
Field[1].YMax = R_outer;
Field[1].Thickness = 2.0*W_box;

Background Field = 1;

Mesh.Algorithm = 8;  // 5 (delaunay) or 8 (frontal delaunay for quads):
Mesh.RecombineAll = 1;
Mesh.RecombinationAlgorithm = 2; // 0: simple, 1: blossom, 2: simple full-quad, 3: blossom full-quad
Mesh.ElementOrder = 1;
Mesh.SecondOrderIncomplete = 2;