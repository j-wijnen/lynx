[GlobalParams]
  order = second
[]

[Mesh]
  file = ../mesh/sent.msh
[]

[Variables]
  [damage]
    family = lagrange
  []
[]

[AuxVariables]
  [pff_energy]
    family = monomial
  []
[]

[Kernels]
  [pff]
    type = PhaseFieldFractureAT2
    variable = damage
  []
[]

[Materials]
  [properties]
    type = GenericConstantMaterial
    prop_names = 'fracture_toughness length_scale'
    prop_values = '20 0.5'
  []
  [strain_energy]
    type = MaterialFromVariable
    property = pff_energy
    variable = pff_energy
  []
  [degradation]
    type = PFFDegradationFunction
    variable = damage
  []
  [driving_force]
    type = PFFDrivingForce
    variable = damage
  []
[]

[BCs]
  [crack]
    type = DirichletBC
    variable = damage
    boundary = Crack 
    value = 1.0
  []
[]

[Executioner]
  type = Transient
  solve_type = "NEWTON"
  petsc_options_iname = '-pc_type -pc_factor_mat_solver_type'
  petsc_options_value = 'lu mumps'
  nl_abs_tol = 1e-6
  nl_rel_tol = 1e-3
[]

[Outputs]
  print_linear_residuals=false
[]