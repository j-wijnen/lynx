# Example 4

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
  [convected]
    order = FIRST
    family = LAGRANGE
  []
[]

[Kernels]
  [diffusion_u]
    type = Diffusion
    variable = diffused
  []
  [convection]
    type = ExampleCoupledConvection
    variable = diffused
    coupled_variable = convected
  []
  [diffusion_v]
    type = Diffusion
    variable = convected
  []
[]

[BCs]
  [left_u]
    type = DirichletBC
    variable = diffused
    boundary = "left"
    value = 0
  []
  [right_u_coupled]
    type = ExampleCoupledDirichletBC
    variable = diffused
    coupled_variable = convected
    boundary = "right"
    alpha = 2
  []
  [left_v]
    type = DirichletBC
    variable = convected
    boundary = "left"
    value = 0
  []
  [right_v]
    type = DirichletBC
    variable = convected
    boundary = "right"
    value = 2
  []
[]

[Executioner]
  type = Steady
  solve_type = "NEWTON"
  petsc_options_iname = '-pc_type'
  petsc_options_value = 'hypre'
  l_tol = 1.0e-6
  nl_rel_tol = 1.0e-4
  nl_abs_tol = 1.0e-4
[]

[Outputs]
  exodus = true
[]
