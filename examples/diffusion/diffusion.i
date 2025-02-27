hydrogen_outer = 0.0

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

[Variables]
  [hydrogen]
    order = FIRST
    family = LAGRANGE
    initial_condition = 0.2
  []
[]

[Kernels]
    [diffusion]
        type = MatDiffusion
        variable = hydrogen
        diffusivity = 1e-6
    []
    [timederivative]
        type = TimeDerivative
        variable = hydrogen
    []
[]

[BCs]
    [left]
        type = DirichletBC
        variable = hydrogen
        boundary = left
        value = ${hydrogen_outer}
    []
    [right]
        type = DirichletBC
        variable = hydrogen
        boundary = right
        value = ${hydrogen_outer}
    []
    [top]
        type = DirichletBC
        variable = hydrogen
        boundary = top
        value = ${hydrogen_outer}
    []
    [bottom]
        type = DirichletBC
        variable = hydrogen
        boundary = bottom
        value = ${hydrogen_outer}
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

  l_tol = 1e-8
  nl_abs_tol = 1e-6
  nl_rel_tol = 1e-6

  end_time = 86400
  dt = 1800
[]

[Outputs]
  [out]
    type = Exodus
    time_step_interval = 1
  []
  print_linear_residuals = false
[]