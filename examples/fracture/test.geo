Point(1) = {0, 0, 0, 10.0};
//+
Point(2) = {1, 0, 0, 10.0};
//+
Point(3) = {1, 1, 0, 10.0};
//+
Point(4) = {0, 1, 0, 10.0};
// +
Line(1) = {1, 2};
//+
Line(2) = {2, 3};
//+
Line(3) = {3, 4};
//+
Line(4) = {4, 1};
//+
Curve Loop(1) = {1, 2, 3, 4};
//+
Plane Surface(1) = {1};
//+
Physical Surface("Material") = {1};
//+
Physical Curve("Left") = {4};
//+
Physical Curve("Right") = {2};
//+
Physical Point("LeftBottom") = {1};

Transfinite Line {1, 2, 3, 4} = 2;
Transfinite Surface{1};

Recombine Surface{1};

Mesh.ElementOrder = 2;
Mesh.SecondOrderIncomplete = 1;
