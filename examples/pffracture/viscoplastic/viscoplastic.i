csv_file = 'viscoplastic_001s'

[GlobalParams]
  displacements = 'disp_x disp_y'
  end_time = 0.01
  num_steps = 200
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

[Kernels]
  [pff]
    type = PhaseFieldFractureAT2
    variable = damage
  []
[]

[Materials]
  [elasticity]
    type = ComputeIsotropicElasticityTensor
    youngs_modulus = 210e3
    poissons_ratio = 0.3
  []
  [properties]
    type = GenericConstantMaterial
    prop_names = 'fracture_toughness length_scale'
    prop_values = '20 1.0'
  []
  [degradation]
    type = PFFDegradationFunction
    variable = damage
  []
  [stress]
    type = PFFIsotropicViscoPlasticStress
    hardening_law = powerlaw
    yield_stress = 200
    hardening_modulus = 210e3
    hardening_exponent = 0.1
    viscosity = 1e-1
    beta = 0.1
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
      value = 0.2
  []
  [left]
      type = DirichletBC
      variable = disp_x
      boundary = left
      value = 0.
  []
  [bottom]
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
    rank_two_tensor = mechanical_strain 
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
  [elastic_energy]
    type = ElementAverageMaterialProperty
    mat_prop = pff_elastic_energy
    outputs = data
  []
  [plastic_energy]
    type = ElementAverageMaterialProperty
    mat_prop = pff_plastic_energy
    outputs = data
  []
[]

[Executioner]
  type = Transient
  solve_type = "NEWTON"
  petsc_options_iname = '-pc_type'
  petsc_options_value = 'lu'
  nl_abs_tol = 1e-4
  line_search = none
  nl_max_its = 500
  n_max_nonlinear_pingpong = 500
[]

[Outputs]
  print_linear_residuals = false
  [data]
    type = CSV
    file_base = ${csv_file}
  []
[]