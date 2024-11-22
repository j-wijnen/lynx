[GlobalParams]
  displacements = 'disp_x disp_y'
  end_time = 1.0
  dt = 0.04
[]

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

[Physics/SolidMechanics/QuasiStatic]
  [all]
    strain = small
    add_variables = true
    generate_output = 'stress_xx strain_xx'
  []
[]

[AuxVariables]
  [degradation]
    family = monomial
    order = first
    initial_condition = 0.0
  []
  [strain_energy]
   family = monomial
   order = first
  []
[]

[AuxKernels]
  [strain_energy_coupling]
    type = MaterialRealAux
    property = strain_energy
    variable = strain_energy
    execute_on = TIMESTEP_END
  []
[]

[Materials]
  [strain_energy]
    type = MaterialFromVariable
    property = degradation
    variable = degradation
  []
  [elasticity]
    type = ComputeIsotropicElasticityTensor
    youngs_modulus = 210e3
    poissons_ratio = 0.3
  []
  [stress]
    type = ComputePFFStress
  []
[]

[BCs]
    [right]
        type = LinearRampDirichletBC
        variable = disp_x
        boundary = right
        value = 0.01
    []
    [left]
        type = DirichletBC
        variable = disp_x
        boundary = left
        value = 0.
    []
    [bottomleft]
        type = DirichletBC
        variable = disp_y
        boundary = leftbottom
        value = 0.
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
  fixed_point_max_its = 1000
  fixed_point_min_its = 1
[]


[Outputs]
  exodus = true
  print_linear_residuals=false
[]

[MultiApps]
  [damage]
    type = TransientMultiApp
    input_files = 'test_picard_sub.i'
    execute_on = timestep_begin
  []
[]

[Transfers]
  [push_crack_driving_force]
    type = MultiAppCopyTransfer
    to_multi_app = damage
    source_variable = 'strain_energy'
    variable = 'strain_energy'
  []
  [pull_phi]
    type = MultiAppCopyTransfer
    from_multi_app = damage
    source_variable = 'degradation'
    variable = 'degradation'
  []
[]