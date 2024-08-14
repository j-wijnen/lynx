[Mesh]
  [generated]
    type = GeneratedMeshGenerator
    dim = 3
    nx = 10
    ny = 5
    nz = 5
    xmax = 5
    ymax = 2
    zmax = 2
  []
[]

[Variables]
  [./diffused]
    order = FIRST
    family = LAGRANGE
  [../]
[]

[Kernels]
  [./diff]
    type = Diffusion
    variable = diffused
  [../]

  # Include our time derivative here
  [./euler]
    type = ExampleTimeDerivative
    variable = diffused
    time_coefficient = 20.0
  [../]
[]

[BCs]
  [./left_diffused]
    type = DirichletBC
    variable = diffused
    boundary = 'left'
    value = 0
  [../]

  [./right_diffused]
    type = DirichletBC
    variable = diffused
    boundary = 'right'
    value = 1
  [../]
[]

# Transient (time-dependent) details for simulations go here:
[Executioner]
  type = Transient   # Here we use the Transient Executioner (instead of steady)
  solve_type = 'PJFNK'
  num_steps = 75 # Run for 75 time steps, solving the system each step.
  dt = 1 # each time step will have duration "1"
  nl_abs_tol = 1e-6
  l_tol = 1e-8
[]

[Outputs]
  execute_on = 'timestep_end'
  exodus = true
[]