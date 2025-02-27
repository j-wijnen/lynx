[GlobalParams]
  displacements = 'disp_x disp_y'
[]

[Mesh]
  [generated]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 10
    ny = 5
    xmax = 2
    ymax = 1
  []
[]

[Physics/SolidMechanics/QuasiStatic]
  [all]
    strain = finite
    new_system = true
    add_variables = true
    generate_output = 'pk1_stress_xx pk1_stress_yy pk1_stress_xy 
      vonmises_pk1_stress 
      strain_xx strain_yy strain_xy'
    use_automatic_differentiation = false
    formulation = total
  []
[]


[Materials]
  [stress]
    type = IADNeoHookeanStress
    shear_modulus = 80e3
    bulk_modulus = 200e3
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
    [rightx]
        type = DirichletBC
        variable = disp_x
        boundary = right
        value = 0.5
    []
[]

[Preconditioning]
  [smp]
    type = SMP 
    full = true 
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
  nl_abs_tol = 1e-6
  nl_rel_tol = 1e-6
  nl_max_its = 5000
[]

[Outputs]
  [out]
    type = Exodus
    time_step_interval = 1
  []
  print_linear_residuals = false
[]