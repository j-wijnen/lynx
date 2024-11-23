[GlobalParams]
  displacements = 'disp_x disp_y'
  end_time = 7.42
  family = lagrange
  order = second
[]

[Mesh]
  file = cct.msh
[]

[AuxVariables]
  [resid_x]
  []
  [resid_y]
  []
[]

[Physics/SolidMechanics/QuasiStatic]
  [all]
    strain = small
    planar_formulation = plane_strain
    add_variables = true
    incremental = false
    generate_output = 'stress_xx stress_yy stress_xy vonmises_stress strain_xx strain_yy strain_xy'
    save_in = 'resid_x resid_y'
  []
[]

[Materials]
  [elasticity]
    type = ComputeIsotropicElasticityTensor
    youngs_modulus = 210e3
    poissons_ratio = 0.3
  []
  [stress]
    type = ComputeLinearElasticStress
  []
[]

[BCs]
    [top]
        type = LinearRampDirichletBC
        variable = disp_y
        boundary = Top
        value = 0.742
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

[DomainIntegral]
  integrals = JIntegral
  2d = true
  boundary = CrackTip
  crack_direction_method = CrackDirectionVector
  crack_direction_vector = '1 0 0'
  radius_inner = '0.5'
  radius_outer = '1.0'
  output_q = false
  incremental = false
[]

[Postprocessors]
  [react_y]
    type = SidesetReaction
    direction = '0 1 0'
    stress_tensor = stress
    boundary = Top
  []
  [react_y2]
    type = NodalSum
    boundary = Top
    variable = resid_y
  []
[]

[Executioner]
  type = Transient
  dt = 0.742
  solve_type = "NEWTON"
  petsc_options_iname = '-pc_type -pc_factor_mat_solver_type'
  petsc_options_value = 'lu mumps'
  #petsc_options_iname = '-pc_type -pc_hypre_type'
  #petsc_options_value = 'hypre boomeramg'
  l_tol = 1e-5
  nl_abs_tol = 1e-8
  nl_rel_tol = 1e-4
[]

[Outputs]
  exodus = true
  csv = true
[]