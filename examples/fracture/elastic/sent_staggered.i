[GlobalParams]
  displacements = 'disp_x disp_y'
  end_time = 1.0
  dt = 0.01
  order = second
[]

[Mesh]
  file = '../mesh/sent.msh'
[]

[Physics/SolidMechanics/QuasiStatic]
  [all]
    strain = small
    add_variables = true
    generate_output = 'stress_yy strain_yy vonmises_stress'
    save_in = 'force_x force_y'
  []
[]

[AuxVariables]
  [damage]
    family = lagrange
  []
  [pff_energy]
   family = monomial
  []
  [force_x]
    family = lagrange 
  []
  [force_y]
    family = lagrange 
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
    type = PFFElasticStress
  []
  [pff_degradation]
    type = PFFDegradationFunction
    variable = damage
  []
[]

[BCs]
    [top]
        type = LinearRampDirichletBC
        variable = disp_y
        boundary = Top
        value = 0.03
    []
    [bottom]
        type = DirichletBC
        variable = disp_y
        boundary = Bottom
        value = 0.
    []
    [left]
        type = DirichletBC
        variable = disp_x
        boundary = Left
        value = 0.
    []
[]

[Postprocessors]
  [cracktip]
    type = CrackTipLocation
    variable = damage
    direction = x 
    outputs = data
  []
  [force]
    type = NodalSum
    boundary = Top 
    variable = force_y 
    outputs = data
  []
  [disp]
    type = AverageNodalVariableValue
    variable = disp_y 
    boundary = Top 
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
  petsc_options_iname = '-pc_type'
  petsc_options_value = 'lu'
  nl_abs_tol = 1e-6
  nl_rel_tol = 1e-3
  fixed_point_max_its = 1000
  fixed_point_min_its = 1
  fixed_point_abs_tol = 1e-6
  fixed_point_rel_tol = 1e-3
[]

[MultiApps]
  [damage]
    type = TransientMultiApp
    input_files = 'sent_staggered_sub.i'
    execute_on = timestep_end
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
    min_simulation_time_interval = 0.1
  []
  [data]
    type = CSV
  []
[]