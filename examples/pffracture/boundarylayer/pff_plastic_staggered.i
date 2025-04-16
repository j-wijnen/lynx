end_time = 1
num_steps = 50
J = 600

[GlobalParams]
  end_time = ${end_time}
  num_steps = ${num_steps}
  displacements = 'disp_x disp_y'
  J = ${J}
  youngs_modulus = 210e3
  poissons_ratio = 0.3
[]

[Mesh]
  file = ../mesh/boundarylayer.msh
[]

[Physics/SolidMechanics/QuasiStatic]
  [all]
    strain = small
    add_variables = true
    generate_output = 'effective_plastic_strain'
  []
[]

[AuxVariables]
  [damage]
    family = lagrange
    order = first
  []
  [pff_energy]
   family = monomial
   order = second
  []
[]

[AuxKernels]
  [strain_energy_coupling]
    type = MaterialRealAux
    property = pff_energy
    variable = pff_energy
    execute_on = TIMESTEP_END
  []
[]

[Materials]
  [elasticity]
    type = ComputeIsotropicElasticityTensor
  []
  [stress]
    type = PFFIsotropicPlasticStress
    block = Material
    hardening_law = powerlaw
    yield_stress = 498
    hardening_modulus = 210e3
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
  [iterations]
    type = NumFixedPointIterations
    outputs = data 
  []
[]

[UserObjects]
  [terminate]
    type = Terminator
    expression = 'cracktip > 1.1'
    fail_mode = HARD
    execute_on = TIMESTEP_END
  []
[]

[Executioner]
  type = Transient
  solve_type = "NEWTON"
  petsc_options_iname = '-pc_type -pc_factor_mat_solver_type'
  petsc_options_value = 'lu mumps'
  line_search = none
  nl_abs_tol = 1e-4
  nl_max_its = 50
  fixed_point_abs_tol = 1e-2
  fixed_point_min_its = 1
  fixed_point_max_its = 200
  accept_on_max_fixed_point_iteration = true  
  residual_and_jacobian_together = true
[]

[MultiApps]
  [damage]
    type = TransientMultiApp
    input_files = 'pff_plastic_staggered_sub.i'
    execute_on = timestep_end
  []
[]

[Transfers]
  [push_crack_driving_force]
    type = MultiAppCopyTransfer
    to_multi_app = damage
    source_variable = 'pff_energy'
    variable = 'pff_energy'
  []
  [pull_damage]
    type = MultiAppCopyTransfer
    from_multi_app = damage
    source_variable = 'damage'
    variable = 'damage'
  []
[]

[Outputs]
  print_linear_residuals = false
  checkpoint = false
  [out]
    type = Exodus
    time_step_interval = 1
  []
  [data]
    type = CSV
  []
[]