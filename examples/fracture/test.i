[GlobalParams]
  displacements = 'disp_x disp_y'
  end_time = 1.0
  dt = 0.5
  family = lagrange
  order = second
[]

[Mesh]
  file = test.msh
[]

[Variables]
  [phi]
    initial_condition = 0.0
  []
[]

[Physics/SolidMechanics/QuasiStatic]
  [all]
    strain = small
    use_automatic_differentiation = true
    add_variables = true
    incremental = false
    generate_output = 'stress_xx strain_xx'
  []
[]

[Kernels]
  [phinonlocal]
    type = ADPhaseFieldFracture
    variable = phi
    length_scale = 0.4
  []
[]

[Materials]
  active = 'elasticity stress driving_force degradation'
  #active = 'elasticity stress degradationparsed drivingforceparsed'
  [elasticity]
    type = ADComputeIsotropicElasticityTensor
    youngs_modulus = 210e3
    poissons_ratio = 0.3
  []
  [degradation]
    type = ADComputeDegradationFunction
    variable = phi
  []
  [stress]
    type = ADComputeElasticPFFStress
  []
  [driving_force]
    type = ADComputePFFDrivingForce
    Gc = 1.0
  []
[]

[BCs]
    [right]
        type = LinearRampDirichletBC
        variable = disp_x
        boundary = Right
        value = 0.1
    []
    [left]
        type = DirichletBC
        variable = disp_x
        boundary = Left
        value = 0.
    []
    [bottomleft]
        type = DirichletBC
        variable = disp_y
        boundary = LeftBottom
        value = 0.
    []
[]

[Problem]
  type = FEProblem
[]

[Preconditioning]
[smp]
  type = SMP
[]
[]

[Executioner]
  type = Transient
  solve_type = "NEWTON"
  petsc_options_iname = '-pc_type -pc_factor_mat_solver_type'
  petsc_options_value = 'lu mumps'
  l_tol = 1e-8
  nl_abs_tol = 1e-6
  nl_rel_tol = 1e-2
[]

[Outputs]
  exodus = true
  print_linear_residuals = true
[]

[Debug]
  show_var_residual_norms = true
[]