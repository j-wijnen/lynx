[GlobalParams]
  displacements = 'disp_x disp_y'
  end_time = 1.0
  dt = 0.02
  order = second
[]

[Mesh]
  file = '../mesh/sent.msh'
[]

[Physics/SolidMechanics/QuasiStatic]
  [all]
    strain = small
    add_variables = true
    generate_output = 'stress_yy strain_yy vonmises_stress'
    save_in = 'force_x force_y'
  []
[]

[Variables]
  [damage]
    family = lagrange
    order = first
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
    prop_values = '2.5 0.5'
  []
  [elasticity]
    type = ComputeIsotropicElasticityTensor
    youngs_modulus = 210e3
    poissons_ratio = 0.3
  []
  [degradation]
    type = PFFDegradationFunction
    variable = damage
  []
  [stress]
    type = PFFLinearElasticStress
  []
  [driving_force]
    type = PFFDrivingForce
    variable = damage
  []
[]

[BCs]
    [top]
        type = LinearRampDirichletBC
        variable = disp_y
        boundary = Top
        value = 0.03
    []
    [bottom]
        type = DirichletBC
        variable = disp_y
        boundary = Bottom
        value = 0.
    []
    [left]
        type = DirichletBC
        variable = disp_x
        boundary = Left
        value = 0.
    []
    [crack]
      type = DirichletBC
      variable = damage
      boundary = Crack 
      value = 1.0
    []
[]

[Postprocessors]
  [cracktip]
    type = CrackTipLocation
    variable = damage
    direction = x 
    outputs = data
  []
  [force]
    type = NodalSum
    boundary = Top 
    variable = force_y 
    outputs = data
  []
  [disp]
    type = AverageNodalVariableValue
    variable = disp_y 
    boundary = Top 
    outputs = data 
  []
  [iterations]
    type = NumNonlinearIterations
    outputs = data
  []
[]

[Executioner]
  type = Transient
  solve_type = "NEWTON"
  petsc_options_iname = '-pc_type -pc_factor_mat_solver_type'
  petsc_options_value = ' lu       mumps'
  line_search = none
  nl_abs_tol = 1e-2
  nl_max_its = 1000
  n_max_nonlinear_pingpong = 1000
  l_abs_tol = 1e-4

  [TimeSteppers]
    [timestepper]
        type = IncreasingDT
    []
  []
[]

[Outputs]
  print_linear_residuals = false
  [out]
    type = Exodus
    min_simulation_time_interval = 0.1
  []
  [data]
    type = CSV
  []
[]