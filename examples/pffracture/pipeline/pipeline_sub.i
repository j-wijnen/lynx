[Mesh]
  [pipe]
    type = FileMeshGenerator
    file = pipeline_linear.msh
  []
  [pipe_crack]
    type = LineNodeSetGenerator
    input = pipe 
    new_boundary = crack 
    point1 = '0 0 0'
    point2 = '0 505 0'
    width = 0.25
  []
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
    prop_values = '2.5 0.5'
  []
  [strain_energy]
    type = MaterialFromVariable
    property = pff_energy
    coupled_variable = pff_energy
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
    boundary = crack 
    value = 1.0
  []
[]

[Executioner]
  type = Transient
  solve_type = "NEWTON"
  petsc_options_iname = '-pc_type -pc_factor_mat_solver_type'
  petsc_options_value = ' lu       mumps'
  line_search = none
  nl_abs_tol = 1e-6
[]

[Outputs]
  print_linear_residuals=false
[]