[GlobalParams]
  displacements = 'disp_x disp_y'
  end_time = 1.0
  dt = 0.02
[]

[Mesh]
  [generated]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 2
    ny = 2
    xmax = 1
    ymax = 1
  []
  [leftbottom]
    type = BoundingBoxNodeSetGenerator
    new_boundary = leftbottom
    bottom_left = '-0.1 -0.1 0.0'
    top_right = '0.1 0.1 0'
    input = generated
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
    type = PFFElasticStress
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
      boundary = leftbottom
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

[Executioner]
  type = Transient
  solve_type = "NEWTON"
  petsc_options_iname = '-pc_type'
  petsc_options_value = 'lu'
  l_tol = 1e-8
  nl_abs_tol = 1e-6
  nl_rel_tol = 1e-4
  line_search = none
[]

[Outputs]
  [data]
    type = CSV
  []
  print_linear_residuals = false
[]