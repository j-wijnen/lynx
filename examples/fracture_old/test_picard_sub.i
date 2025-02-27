[Mesh]
  [generated]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 1
    ny = 1
    xmax = 1
    ymax = 1
  []
  [leftbottom]
    type = BoundingBoxNodeSetGenerator
    new_boundary = leftbottom
    bottom_left = '-0.5 -0.5 0.0'
    top_right = '0.5 0.5 0'
    input = generated
  []
[]

[Variables]
  [phi]
    initial_condition = 0.0
  []
[]

[AuxVariables]
  [bounds_dummy]
  []
  [strain_energy]
    family = monomial
    order = first
  []
  [degradation]
    family = monomial
    order = first
  []
[]

[Kernels]
  [phinonlocal]
    type = PhaseFieldFracture
    variable = phi
    length_scale = 1.5
  []
[]

[AuxKernels]
  [couple_degradation]
    type = MaterialRealAux
    variable = degradation
    property = degradation
    execute_on = TIMESTEP_END
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
    Gc = 2.5
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
  petsc_options_iname = '-pc_type'
  petsc_options_value = 'lu'
  l_tol = 1e-8
  nl_abs_tol = 1e-6
  nl_rel_tol = 1e-4
  line_search = none
[]

[Outputs]
  print_linear_residuals=false
[]