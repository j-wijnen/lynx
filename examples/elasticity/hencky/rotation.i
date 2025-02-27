[GlobalParams]
  end_time = 1.0
  dt = 0.1
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
    type = NeoHookeanStress
    shear_modulus = 80e3
    bulk_modulus = 200e3
  []
[]

[BCs]
    [rotatex]
        type = FunctionDirichletBC
        variable = disp_x
        boundary = 'left right'
        function = rotatefunc_x
    []
    [rotatey]
        type = FunctionDirichletBC
        variable = disp_y
        boundary = 'left right'
        function = rotatefunc_y
    []
[]

[Functions]
  [rotatefunc_x]
    type = ParsedFunction
    expression = 'cos(0.5*t*pi)*x - sin(0.5*t*pi)*y - x'
  []
  [rotatefunc_y]
    type = ParsedFunction
    expression = 'sin(0.5*t*pi)*x + cos(0.5*t*pi)*y - y'
  []
[]

[Preconditioning]
  [smp]
    type = SMP 
    full = true 
  []
[]

[Executioner]
  type = Transient
  solve_type = "NEWTON"
  petsc_options_iname = '-pc_type -pc_factor_mat_solver_type'
  petsc_options_value = 'lu mumps'
  #petsc_options_iname = '-pc_type -pc_hypre_type'
  #petsc_options_value = 'hypre boomeramg'
  #l_tol = 1e-8
  nl_abs_tol = 1e-6
  nl_rel_tol = 1e-10
  nl_max_its = 5000
[]

[Outputs]
  [out]
    type = Exodus
    time_step_interval = 1
  []
  print_linear_residuals = false
[]