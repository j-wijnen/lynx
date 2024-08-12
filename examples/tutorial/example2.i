# Example 2

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

[Variables]
  [diffused]
    order = FIRST
    family = LAGRANGE
  []
[]

[Kernels]
  [diffusion]
    type = Diffusion
    variable = diffused
  []
  [convection]
    type = ExampleConstantConvection
    variable = diffused
    velocity = '0.0 0.0 1.0'
  []
[]

[BCs]
  [left]
    type = DirichletBC
    variable = diffused
    boundary = "left"
    value = 1
  []
  [right]
    type = DirichletBC
    variable = diffused
    boundary = "right"
    value = 0
  []
[]

[Executioner]
  type = Steady
  solve_type = "PJFNK"
  petsc_options_iname = '-pc_type -sub_pc_type'
  petsc_options_value = 'asm      ilu'
  l_tol = 1.0e-4
  nl_rel_tol = 1.0e-8
  nl_abs_tol = 1.0e-10
[]

[Outputs]
  execute_on = 'timestep_end'
  exodus = true
[]