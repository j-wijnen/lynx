[GlobalParams]
  order = first
[]

[Mesh]
  file = cct.msh
[]

[Variables]
  [phi]
    initial_condition = 0.0
  []
[]

[AuxVariables]
  [strain_energy]
    family = monomial
  []
[]

[Kernels]
  [phinonlocal]
    type = PhaseFieldFracture
    variable = phi
    length_scale = 0.3
  []
[]

[Materials]
  [strain_energy]
    type = MaterialFromVariable
    property = strain_energy
    variable = strain_energy
  []
  [degradation]
    type = ComputeDegradationFunction
    variable = phi
  []
  [driving_force]
    type = ComputePFFDrivingForce
    variable = phi
    Gc = 1.28
  []
[]

[BCs]
    [crack]
      type = DirichletBC
      variable = phi
      boundary = Crack
      value = 1.
    []
[]

[Executioner]
  type = Transient
  solve_type = "NEWTON"
  petsc_options_iname = '-pc_type -pc_factor_mat_solver_type'
  petsc_options_value = 'lu mumps'
  #petsc_options_iname = '-pc_type -pc_hypre_type'
  #petsc_options_value = 'hypre boomeramg'
  l_tol = 1e-10
  nl_abs_tol = 1e-8
  nl_rel_tol = 1e-4
  line_search = none
  nl_max_its = 5000
[]

[Outputs]
  print_linear_residuals = false
[]