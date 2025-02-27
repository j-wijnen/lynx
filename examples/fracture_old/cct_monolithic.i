[GlobalParams]
  displacements = 'disp_x disp_y'
  end_time = 0.2
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

[Variables]
  [phi]
    initial_condition = 0.0
  []
[]

[Kernels]
  [phinonlocal]
    type = PhaseFieldFracture
    variable = phi
    length_scale = 0.3
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
  [driving_force]
    type = ComputePFFDrivingForce
    variable = phi
    Gc = 1.28
  []
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
    [crack]
      type = DirichletBC
      variable = phi
      boundary = Crack
      value = 1.
    []
[]

[Executioner]
  type = Transient
  #dt = 0.0004
  num_steps = 500
  solve_type = "NEWTON"
  petsc_options = ''
  petsc_options_iname = '-pc_type -pc_factor_mat_solver_type -snes_linesearch_minlambda'
  petsc_options_value = 'lu mumps 0.1'
  #petsc_options_iname = '-pc_type -pc_hypre_type'
  #petsc_options_value = 'hypre boomeramg'
  l_tol = 1e-10
  nl_abs_tol = 1e-8
  nl_rel_tol = 1e-4
  nl_abs_div_tol = 1e+50
  nl_div_tol = 1e+50
  line_search = basic
  nl_max_its = 50000
  automatic_scaling = true 
  compute_scaling_once = false
[]

[Outputs]
  [out]
    type = Exodus
    time_step_interval = 20
  []
  print_linear_residuals = false
[]