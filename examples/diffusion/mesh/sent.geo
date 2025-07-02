//+
Point(1) = {0, 0, 0, 1.0};
//+
Point(2) = {2.56, 0, 0, 1.0};
//+
Point(3) = {5, 0, 0, 1.0};
//+
Point(4) = {5, 24, 0, 1.0};
//+
Point(5) = {0, 24, 0, 1.0};
//+
Point(6) = {5, 25, 0, 1.0};
//+
Point(7) = {0, 25, 0, 1.0};
//+
Line(1) = {1, 2};
//+
Line(2) = {2, 3};
//+
Line(3) = {3, 4};
//+
Line(4) = {4, 6};
//+
Line(5) = {6, 7};
//+
Line(6) = {7, 5};
//+
Line(7) = {5, 1};
//+
Line(8) = {5, 4};
//+
Curve Loop(1) = {7, 1, 2, 3, -8};
//+
Plane Surface(1) = {1};
//+
Curve Loop(2) = {5, 6, 8, 4};
//+
Plane Surface(2) = {2};
//+
Physical Surface("Material", 9) = {1};
//+
Physical Surface("ElasticLayer", 10) = {2};
//+
Physical Curve("Left", 11) = {7, 6};
//+
Physical Curve("Top", 12) = {5};
//+
Physical Curve("Bottom", 13) = {2};
//+
Physical Curve("Crack", 14) = {1};
//+
Physical Point("CrackTip", 15) = {2};
//+
Physical Curve("Right", 16) = {3, 4};


// We could also use a `Box' field to impose a step change in element sizes
// inside a box
Field[1] = Box;
Field[1].VIn = 0.5;
Field[1].VOut = 1.0;
Field[1].XMin = 0.0;
Field[1].XMax = 5.0;
Field[1].YMin = 0.0;
Field[1].YMax = 5.0;

Background Field = 1;