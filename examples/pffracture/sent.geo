//+
Point(1) = {0, 0, 0, 1.0};
//+
Point(2) = {2.56, 0, 0, 1.0};
//+
Point(3) = {5, 0, 0, 1.0};
//+
Point(4) = {5, 25, 0, 1.0};
//+
Point(5) = {0, 25, 0, 1.0};
//+
Line(1) = {1, 2};
//+
Line(2) = {2, 3};
//+
Line(3) = {3, 4};
//+
Line(4) = {4, 5};
//+
Line(5) = {5, 1};
//+
Curve Loop(1) = {1, 2, 3, 4, 5};
//+
Plane Surface(1) = {1};
//+
Physical Surface("Material") = {1};
//+
Physical Curve("Left") = {5};
//+
Physical Curve("Top") = {4};
//+
Physical Curve("Crack") = {1};
Physical Curve("Bottom") = {2};

Physical Point("CrackTip") = {2};

// We could also use a `Box' field to impose a step change in element sizes
// inside a box
Field[1] = Box;
Field[1].VIn = 0.1;
Field[1].VOut = 1.0;
Field[1].XMin = 0.0;
Field[1].XMax = 5.0;
Field[1].YMin = 0.0;
Field[1].YMax = 1.0;

Background Field = 1;

Mesh.ElementOrder = 1;
Mesh.SecondOrderIncomplete = 2;


Mesh.RecombinationAlgorithm = 1;
Mesh.Algorithm = 5;  // 5 or 8:
//Recombine Surface {:};