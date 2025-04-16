h_min = 0.04;
h_max = 10.0;
h_med = 1.0;

Rb = 100;
box_left = 0.5;
box_right = 1.5;
box_height = 1.0;

Point(1) = {Rb, 0, 0};
Point(2) = {0, 0, 0};
Point(3) = {-Rb, 0, 0};
Point(4) = {-box_left, 0, 0};
Point(5) = {0.9*Rb, 0, 0};
Point(6) = {-0.9*Rb, 0, 0};

Circle(1) = {1, 2, 3};
Circle(2) = {5, 2, 6};
Line(3) = {2, 5};
Line(4) = {5, 1};
Line(5) = {2, 4};
Line(6) = {3, 6};
Line(7) = {6, 4};

Curve Loop(1) = {7, -5, 3, 2};
Plane Surface(1) = {1};

Curve Loop(2) = {2, -6, -1, -4};
Plane Surface(2) = {2};

Physical Surface("Material") = {1};
Physical Surface("Elasticlayer") = {2};
Physical Point("CrackTip") = {2};
Physical Curve("InitialPF") = {5};
Physical Curve("FixY") = {3, 4};
Physical Curve("Outer") = {1};

Field[1] = Box;
Field[1].VIn = h_min;
Field[1].VOut = h_max;
Field[1].XMin = -box_left;
Field[1].XMax = box_right;
Field[1].YMin = 0.0;
Field[1].YMax = box_height;

Field[2] = MathEval;
Field[2].F = Sprintf("%g  + (x*x + y*y)/(3*%g)^4 * (%g-%g)", h_min, box_height, h_med, h_min);

Field[3] = Min;
Field[3].FieldsList = {1, 2};

Background Field = 3;

Mesh.ElementOrder = 2;

// Quads only
Mesh.Algorithm = 8;
Mesh.RecombineAll = 1;
Mesh.RecombinationAlgorithm = 2; // or 3
Mesh.SecondOrderIncomplete = 1;


