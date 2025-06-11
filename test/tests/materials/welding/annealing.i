[GlobalParams]
  displacements = 'disp_x disp_y'
[]

[Mesh]
  [generated]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 2
    ny = 2
    xmax = 1
    ymax = 1
  []
  [leftbottom]
    type = BoundingBoxNodeSetGenerator
    new_boundary = leftbottom
    bottom_left = '-0.01 -0.01 0.0'
    top_right = '0.01 0.01 0'
    input = generated
  []
[]

[AuxVariables]
  [temperature]
    family = lagrange 
    order = first 
  []
[]

[AuxKernels]
  [temperature_kernel]
    type = FunctionAux
    variable = temperature
    function = temperature_func
    execute_on = timestep_begin
  []
[]

[Physics/SolidMechanics/QuasiStatic]
  [all]
    strain = small
    incremental = false
    add_variables = true
    generate_output = 'effective_plastic_strain'
  []
[]

[Materials]
  [generic]
    type = GenericConstantMaterial
    prop_names = 'fraction_ferrite fraction_pearlite fraction_bainite fraction_martensite'
    prop_values = '0.0 0.0 0.0 0.0'
  []
  [elasticity]
    type = ComputeIsotropicElasticityTensor
    youngs_modulus = 200e3
    poissons_ratio = 0.3
  []
  [stress]
    type = WeldingPlasticStress
    initial_yield_stress = 200.0
    hardening_law = linear 
    hardening_modulus = 10e3
    temperature_variable = temperature 
    annealing_temperature = 1400
    outputs = all
    output_properties = 'plastic_multiplier yield_stress'
  []
[]

[Functions]
  [temperature_func]
    type = ParsedFunction
    expression = 't/0.5 * 1400'
  []
[]

[BCs]
  [fix_x]
    type = DirichletBC
    variable = disp_x
    boundary = left
    value = 0
  []
  [fix_y]
    type = DirichletBC
    variable = disp_y
    boundary = leftbottom
    value = 0
  []
  [load_x]
    type = LinearRampDirichletBC
    variable = disp_x 
    boundary = right 
    value = 0.1
    end_time = 1.0
  []
[]

[Problem]
  type = FEProblem
[]

[Executioner]
  type = Transient
  solve_type = NEWTON
  end_time = 1
  dt = 0.2
  petsc_options_iname = '-pc_type'
  petsc_options_value = 'lu'
  nl_abs_tol = 1.0e-8
[]

[Outputs]
  [out]
    type = Exodus 
    time_step_interval = 1
  []

  print_linear_residuals = false
[]
