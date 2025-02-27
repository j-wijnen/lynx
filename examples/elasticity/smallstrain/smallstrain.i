[GlobalParams]
  displacements = 'disp_x disp_y'
[]

[Mesh]
  [generated]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 20
    ny = 10
    xmax = 2
    ymax = 1
  []
[]

[Physics/SolidMechanics/QuasiStatic]
  [all]
    strain = small
    add_variables = true
    generate_output = 'stress_xx stress_yy stress_xy 
      vonmises_stress strain_yy strain_xy'
  []
[]


[Materials]
  [elasticity]
    type = ComputeIsotropicElasticityTensor
    youngs_modulus = 210e3
    poissons_ratio = 0.3
  []
  [stress]
    type = LinearElasticStress
  []
[]

[BCs]
    [leftx]
        type = DirichletBC
        variable = disp_x
        boundary = left
        value = 0.
    []
    [lefty]
        type = DirichletBC
        variable = disp_y
        boundary = left
        value = 0.
    []
    [right]
        type = DirichletBC
        variable = disp_x
        boundary = right
        value = 0.5
    []
[]

[Executioner]
  type = Steady
  solve_type = "NEWTON"
  petsc_options_iname = '-pc_type -pc_factor_mat_solver_type'
  petsc_options_value = 'lu mumps'
  #petsc_options_iname = '-pc_type -pc_hypre_type'
  #petsc_options_value = 'hypre boomeramg'
  l_tol = 1e-8
  nl_abs_tol = 1e-8
  nl_rel_tol = 1e-4
  line_search = none
  nl_max_its = 5000
[]

[Outputs]
  [out]
    type = Exodus
    time_step_interval = 1
  []
  print_linear_residuals = false
[]