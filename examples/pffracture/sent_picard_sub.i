[GlobalParams]
  order = first
[]

[Mesh]
  file = sent.msh
[]

[Variables]
  [damage]
    initial_condition = 0.0
  []
[]

[AuxVariables]
  [strain_energy]
    family = monomial
    order = first
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
    prop_values = '2.5 1.0'
  []
  [strain_energy]
    type = MaterialFromVariable
    property = strain_energy
    variable = strain_energy
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
  petsc_options_iname = '-pc_type'
  petsc_options_value = 'lu'
  nl_abs_tol = 1e-4
  nl_rel_tol = 1e-3
[]

[Outputs]
  print_linear_residuals=false
[]