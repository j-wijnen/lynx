[GlobalParams]
  displacements = 'disp_x disp_y'
  end_time = 0.2
  order = first
[]

[Mesh]
  file = cct.msh
[]

[Physics/SolidMechanics/QuasiStatic]
  [all]
    strain = small
    add_variables = true
    incremental = false
    generate_output = 'stress_xx stress_yy stress_xy vonmises_stress strain_xx strain_yy strain_xy'
    use_automatic_differentiation = false
  []
[]

[AuxVariables]
  [phi]
    family = lagrange
    initial_condition = 0.0
  []
  [strain_energy]
   family = monomial
  []
[]

[AuxKernels]
  [strain_energy_coupling]
    type = MaterialRealAux
    property = strain_energy
    variable = strain_energy
    execute_on = TIMESTEP_END
  []
[]

[Materials]
  [elasticity]
    type = ComputeIsotropicElasticityTensor
    youngs_modulus = 210e3
    poissons_ratio = 0.3
  []
  [stress]
    type = ComputePFFStress
  []
  [degradation]
    type = ComputeDegradationFunction
    variable = phi
  []
  #[degradation]
  #  type = MaterialFromVariable
  #  property = degradation
  #  variable = degradation
  #[]
[]


[BCs]
    [top]
        type = LinearRampDirichletBC
        variable = disp_y
        boundary = Top
        value = 0.04
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
[]

[Executioner]
  type = Transient
  dt = 0.0004
  solve_type = "NEWTON"
  petsc_options_iname = '-pc_type -pc_factor_mat_solver_type'
  petsc_options_value = 'lu mumps'
  #petsc_options_iname = '-pc_type -pc_hypre_type'
  #petsc_options_value = 'hypre boomeramg'
  l_tol = 1e-10
  nl_abs_tol = 1e-8
  nl_rel_tol = 1e-4
  fixed_point_abs_tol = 1e-8
  fixed_point_rel_tol = 1e-4
  line_search = none
  nl_max_its = 5000
  fixed_point_max_its = 1000
  fixed_point_min_its = 1
[]

[MultiApps]
  [damage]
    type = TransientMultiApp
    input_files = 'cct_picard_sub.i'
    execute_on = timestep_begin
  []
[]

[Transfers]
  [push_crack_driving_force]
    type = MultiAppCopyTransfer
    to_multi_app = damage
    source_variable = 'strain_energy'
    variable = 'strain_energy'
  []
  [pull_phi]
    type = MultiAppCopyTransfer
    from_multi_app = damage
    source_variable = 'phi'
    variable = 'phi'
  []
[]

[Outputs]
  [out]
    type = Exodus
    time_step_interval = 20
  []
  print_linear_residuals = false
[]