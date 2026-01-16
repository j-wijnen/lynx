[GlobalParams]
  displacements = 'disp_x disp_y'
  end_time = 1.0
  num_steps = 20
[]

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

[Physics/SolidMechanics/QuasiStatic]
  [all]
    strain = small
    add_variables = true
    generate_output = 'vonmises_stress strain_xx'
  []
[]

[AuxVariables]
  [damage]
    family = lagrange
  []
  [pff_energy]
   family = monomial
  []
[]

[AuxKernels]
  [pff_energy_coupling]
    type = MaterialRealAux
    property = pff_energy
    variable = pff_energy
    execute_on = TIMESTEP_END
  []
[]

[Materials]
  [elasticity]
    type = ComputeIsotropicElasticityTensor
    youngs_modulus = 210e3
    poissons_ratio = 0.3
  []
  [stress]
    type = PFFLinearElasticStress
  []
  [pff_degradation]
    type = PFFDegradationFunction
    variable = damage
  []
[]

[BCs]
    [disp_x]
        type = RadialDisplacementBC
        variable = disp_x
        component = x
        boundary = inner_boundary
        value = 0.4
        penalty = 1e6
    []
    [disp_y]
        type = RadialDisplacementBC
        variable = disp_y
        component = y
        boundary = inner_boundary
        value = 0.4
        penalty = 1e6
    []
    [fix_y]
        type = DirichletBC
        variable = disp_y
        boundary = inner_left 
        value = 0.0
    []
[]

[Postprocessors]
  [cracktip]
    type = CrackTipLocation
    variable = damage
    direction = y 
    outputs = data
  []
  [iterations]
    type = NumFixedPointIterations
    outputs = data
  []
[]

[Executioner]
  type = Transient
  solve_type = "NEWTON"
  petsc_options_iname = '-pc_type -pc_factor_mat_solver_type'
  petsc_options_value = ' lu       mumps'
  nl_abs_tol = 1e-4
  multiapp_fixed_point_convergence = conv
[]

[Convergence]
  [conv]
    type = CustomMultiAppFixedPointConvergence
    fixed_point_max_its = 200
    fixed_point_min_its = 1
    accept_on_max_fixed_point_iteration = true
    fixed_point_abs_tol = 1e-4 # 1e-2
    fixed_point_rel_tol = 1e-20
    filename = convergence.txt
  []
[]

[MultiApps]
  [damage]
    type = TransientMultiApp
    input_files = 'pipeline_sub.i'
    execute_on = TIMESTEP_END

  []
[]

[Transfers]
  [push_crack_driving_force]
    type = MultiAppCopyTransfer
    to_multi_app = damage
    source_variable = 'pff_energy'
    variable = 'pff_energy'
  []
  [pull_damage]
    type = MultiAppCopyTransfer
    from_multi_app = damage
    source_variable = 'damage'
    variable = 'damage'
  []
[]

[Outputs]
  print_linear_residuals = false
  [out]
    type = Exodus
  []
  [data]
    type = CSV
  []
[]