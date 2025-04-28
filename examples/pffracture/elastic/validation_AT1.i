[GlobalParams]
  displacements = 'disp_x disp_y'
  end_time = 1.0
  dt = 0.01
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
  [bounds_dummy]
    family = lagrange 
  []
[]

[Kernels]
  [pff]
    type = PhaseFieldFractureAT1
    variable = damage
  []
[]

[Materials]
  [properties]
    type = GenericConstantMaterial
    prop_names = 'fracture_toughness length_scale'
    prop_values = '2.5 3.555555'
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
      value = 0.005
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

[Postprocessors]
  [damage]
    type = AverageNodalVariableValue
    variable = damage
    outputs = data
  []
  [strain]
    type = MaterialTensorAverage
    rank_two_tensor = elastic_strain 
    index_i = 0
    index_j = 0
    outputs = data
  []
  [stress]
    type = MaterialTensorAverage
    rank_two_tensor = stress 
    index_i = 0
    index_j = 0
    outputs = data
  []
[]

[Bounds]
  [damage_upper_bound]
    type = ConstantBounds
    variable = bounds_dummy
    bounded_variable = damage
    bound_type = upper
    bound_value = 1
  []
  [damage_lower_bound]
    type = VariableOldValueBounds
    variable = bounds_dummy
    bounded_variable = damage
    bound_type = lower
  []
[]

[Executioner]
  type = Transient
  solve_type = "NEWTON"
  petsc_options_iname = '-snes_type   -pc_type'
  petsc_options_value = 'vinewtonrsls  lu'
  line_search = none
  nl_abs_tol = 1e-3
  nl_max_its = 500
  n_max_nonlinear_pingpong = 500
[]

[Outputs]
  [data]
    type = CSV
  []
  print_linear_residuals = false
[]