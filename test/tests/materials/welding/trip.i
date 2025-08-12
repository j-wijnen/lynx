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

[Variables]
  [strain_zz]
  []
[]

[AuxVariables]
  [fraction_ferrite]
    family = lagrange
    order = first 
    initial_condition = 0.0
  []
[]

[AuxKernels]
  [fraction_ferrite]
    type = FunctionAux
    variable = fraction_ferrite
    function = fraction_ferrite
    execute_on = timestep_begin
  []
[]

[Physics/SolidMechanics/QuasiStatic]
  [all]
    strain = small
    incremental = false
    add_variables = true
    generate_output = 'plastic_strain_xx'
    planar_formulation = weak_plane_stress 
    out_of_plane_strain = strain_zz
  []
[]

[Materials]
  [fraction_ferrite]
    type = MaterialFromVariable 
    property = fraction_ferrite
    coupled_variable = fraction_ferrite 
  []
  [generic]
    type = GenericConstantMaterial
    prop_names = 'fraction_pearlite fraction_bainite fraction_martensite'
    prop_values = '0.0 0.0 0.0'
  []
  [elasticity]
    type = ComputeIsotropicElasticityTensor
    youngs_modulus = 200e3
    poissons_ratio = 0.3
  []
  [stress]
    type = WeldingPlasticStress
    yield_stress = 1e20
    outputs = all
    output_properties = plastic_multiplier
    trip_parameter_ferrite = 1.0
  []
[]

[Functions]
  [fraction_ferrite]
    type = ParsedFunction
    expression = 't'
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
    type = NeumannBC
    variable = disp_x 
    boundary = right 
    value = 1.0
  []
[]

[Problem]
  type = FEProblem
[]

[Executioner]
  type = Transient
  solve_type = Newton
  end_time = 1
  dt = 0.02
  petsc_options_iname = '-pc_type'
  petsc_options_value = 'lu'
  nl_abs_tol = 1.0e-8
[]

[Outputs]
  [out]
    type = Exodus 
    time_step_interval = 50
  []

  print_linear_residuals = false
[]
