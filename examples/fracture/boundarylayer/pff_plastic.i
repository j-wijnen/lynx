end_time = 100
num_steps = 300
J = 100

[GlobalParams]
  end_time = ${end_time}
  num_steps = ${num_steps}
  displacements = 'disp_x disp_y'
  order = second
  J = ${J}
  youngs_modulus = 210e3
  poissons_ratio = 0.3
[]

[Mesh]
  file = ../mesh/boundarylayer_quads.msh
[]

[Physics/SolidMechanics/QuasiStatic]
  [all]
    strain = small
    add_variables = true
    generate_output = 'stress_yy vonmises_stress strain_xy effective_plastic_strain'
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
  [properties]
    type = GenericConstantMaterial
    prop_names = 'fracture_toughness length_scale'
    prop_values = '10.0 0.4'
  []
  [stress]
    type = PFFIsotropicPlasticStress
    block = Material
    hardening_law = powerlaw
    yield_stress = 400
    hardening_exponent = 0.1
    beta = 0.1
  []
  [elasticlayer]
    type = PFFIsotropicPlasticStress
    block = Elasticlayer
    yield_stress = 1e20
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
  [Jx]
    type = KFieldDirichletBC
    variable = disp_x
    direction = x
    boundary = Outer
  []
  [Jy]
    type = KFieldDirichletBC
    variable = disp_y
    direction = y
    boundary = Outer
  []
  [FixY]
    type = DirichletBC
    variable = disp_y
    boundary = FixY
    value = 0.
  []
  [crack]
    type = DirichletBC
    variable = damage 
    boundary = InitialPF
    value = 1.0
    preset = true
  []
[]

[Functions]
  [Jvalue]
    type = ParsedFunction
    expression = 't / ${end_time} * ${J}'
  []
[]

[Postprocessors]
  [Jvalue]
    type = FunctionValuePostprocessor
    function = Jvalue
    outputs = data
  []
  [cracktip]
    type = CrackTipLocation
    variable = damage
    direction = x 
    outputs = data
  []
[]

[Executioner]
  type = Transient
  solve_type = "NEWTON"
  petsc_options_iname = '-pc_type -pc_factor_mat_solver_type'
  petsc_options_value = 'lu mumps'
  l_tol = 1e-8
  nl_abs_tol = 1e-4
  nl_rel_tol = 1e-4
  line_search = none
  nl_max_its = 2000
  n_max_nonlinear_pingpong = 2000
[]

[Outputs]
  print_linear_residuals = false
  [out]
    type = Exodus
    time_step_interval = 10
  []
  [data]
    type = CSV
  []
[]