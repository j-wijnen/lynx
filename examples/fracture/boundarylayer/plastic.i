[GlobalParams]
  end_time = 1.0
  num_steps = 20
  displacements = 'disp_x disp_y'
  youngs_modulus = 210e3
  poissons_ratio = 0.3
  J = 300
[]

[Mesh]
  file = ../mesh/boundarylayer_quads.msh
[]

[Physics/SolidMechanics/QuasiStatic]
  [material]
    strain = small
    add_variables = true
    generate_output = 'stress_yy vonmises_stress strain_xy effective_plastic_strain'
  []
[]

[Materials]
  [elasticlayer]
    type = IsotropicPlasticStress
    block = Elasticlayer
    yield_stress = 1e99
  []
  [stress]
    type = IsotropicPlasticStress
    block = Material
    hardening_law = powerlaw
    yield_stress = 200
    hardening_exponent = 0.1
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

[Executioner]
  type = Transient
  solve_type = "NEWTON"
  petsc_options_iname = '-pc_type -pc_factor_mat_solver_type'
  petsc_options_value = 'lu mumps'
  nl_abs_tol = 1e-8
  nl_rel_tol = 1e-6
[]

[Outputs]
  [out]
    type = Exodus
  []
  print_linear_residuals = false
[]