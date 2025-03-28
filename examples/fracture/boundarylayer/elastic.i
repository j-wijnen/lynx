[GlobalParams]
  end_time = 1.0
  dt = 0.5
  displacements = 'disp_x disp_y'
  youngs_modulus = 210e3
  poissons_ratio = 0.3
  K = 100
  order = second
[]

[Mesh]
  file = ../mesh/boundarylayer_quads.msh
[]

[Physics/SolidMechanics/QuasiStatic]
  [all]
    strain = small
    add_variables = true
    generate_output = 'stress_yy vonmises_stress strain_xy'
  []
[]

[AuxVariables]
  [analytic_x]
    family = lagrange
  []
  [analytic_y]
    family = lagrange 
  []
[]

[Functions]
  [theta]
    type = ParsedFunction
    expression = 'atan2(y, x)'
  []
  [radius]
    type = ParsedFunction
    expression = 'sqrt(x*x + y*y)'
  []
  [disp_x_fn]
    type = ParsedFunction
    expression = 't*K / (2*G) * sqrt(r/(pi*2)) * cos(theta/2) * (2 - 4*nu + 2 * sin(theta/2)^2)'
    symbol_names = 'K r theta nu G'
    symbol_values = '100 radius theta 0.3 80769.23076923077'
  []
  [disp_y_fn]
    type = ParsedFunction
    expression = 't*K / (2*G) * sqrt(r/(pi*2)) * sin(theta/2) * (4 - 4*nu - 2 * cos(theta/2)^2)'
    symbol_names = 'K r theta nu G'
    symbol_values = '100 radius theta 0.3 80769.23076923077'
  []
[]

[AuxKernels]
  [disp_x_ak]
    type = FunctionAux
    variable = analytic_x
    function = 'disp_x_fn'
  []
  [disp_y_ak]
    type = FunctionAux
    variable = analytic_y
    function = 'disp_y_fn'
  []
[]

[Materials]
  [elasticity]
    type = ComputeIsotropicElasticityTensor
  []
  [stress]
    type = LinearElasticStress
  []
[]

[BCs]
    [FixY]
        type = DirichletBC
        variable = disp_y
        boundary = FixY
        value = 0.
    []
    [Jy]
      type = KFieldDirichletBC
      variable = disp_y
      direction = y
      boundary = Outer
    []
    [Jx]
      type = KFieldDirichletBC
      variable = disp_x
      direction = x
      boundary = Outer
  []
[]

[DomainIntegral]
  integrals = Jintegral
  2d = true
  crack_direction_method = CrackDirectionVector
  crack_direction_vector = '1 0 0'
  boundary = CrackTip
  incremental = false
  radius_inner = 1.0
  radius_outer = 2.0
  symmetry_plane = 0
[]

[Executioner]
  type = Transient
  solve_type = "NEWTON"
  petsc_options_iname = '-pc_type -pc_factor_mat_solver_type'
  petsc_options_value = 'lu mumps'
  l_tol = 1e-8
  nl_abs_tol = 1e-8
  nl_rel_tol = 1e-4
[]

[Outputs]
  [out]
    type = Exodus
  []
  print_linear_residuals = false
[]