[GlobalParams]
  displacements = 'disp_x disp_y'
  end_time = 1.0
  dt = 0.02
[]

[Mesh]
  [generated]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 1
    ny = 1
    xmax = 1
    ymax = 1
  []
[]

[Physics/SolidMechanics/QuasiStatic]
  [all]
    strain = small
    generate_output = 'stress_xx strain_xx'
    add_variables = true
  []
[]

[Variables]
  [damage]
      family = lagrange
  []
[]

[AuxVariables]
  [force_x]
    family = lagrange
  []
  [force_y]
    family = lagrange
  []
[]

[Kernels]
  [pff]
    type = PhaseFieldFractureAT2
    variable = damage
  []
[]

[Materials]
  [properties]
    type = GenericConstantMaterial
    prop_names = 'fracture_toughness length_scale'
    prop_values = '2.5 1.0'
  []
  [elasticity]
    type = ComputeIsotropicElasticityTensor
    youngs_modulus = 210e3
    poissons_ratio = 0.3
  []
  [stress]
    type = PFFLinearElasticStress
  []
  [degradation]
    type = PFFDegradationFunction
    variable = damage
  []
  [driving_force]
    type = PFFDrivingForce
    variable = damage
  []
[]

[BCs]
  [right]
      type = LinearRampDirichletBC
      variable = disp_x
      boundary = right
      value = 0.02
  []
  [left]
      type = DirichletBC
      variable = disp_x
      boundary = left
      value = 0.
  []
  [bottomleft]
      type = DirichletBC
      variable = disp_y
      boundary = bottom
      value = 0.
  []
[]

[Executioner]
  type = Transient
  solve_type = "NEWTON"
  petsc_options_iname = '-pc_type'
  petsc_options_value = 'lu'
  line_search = none
  nl_abs_tol = 1e-3
  nl_max_its = 500
  n_max_nonlinear_pingpong = 500
[]

[Outputs]
  exodus = true
[]