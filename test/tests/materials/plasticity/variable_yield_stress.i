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
  [initial_yield_stress]
    family = lagrange
    order = first 
    initial_condition = 0.0
  []
  [yield_stress]
    family = monomial
    order = first 
  []
[]

[AuxKernels]
  [initial_yield_stress]
    type = FunctionAux
    variable = initial_yield_stress
    function = initial_yield_stress
    execute_on = timestep_begin
  []
  [yield_stress_output]
    type = MaterialRealAux
    variable = yield_stress
    property = yield_stress
  []
[]

[Physics/SolidMechanics/QuasiStatic]
  [all]
    strain = small
    incremental = false
    add_variables = true
    generate_output = 'plastic_strain_xx'
  []
[]

[Materials]
  [initial_yield_stress]
    type = MaterialFromVariable 
    property = initial_yield_stress
    coupled_variable = initial_yield_stress
  []
  [generic]
    type = GenericConstantMaterial
    prop_names = 'youngs_modulus hardening_exponent'
    prop_values = '200e3 0.0'
  []
  [elasticity]
    type = ComputeIsotropicElasticityTensor
    youngs_modulus = 200e3
    poissons_ratio = 0.3
  []
  [stress]
    type = IsotropicPlasticStress
    hardening_material = variable_powerlaw
  []
  [variable_powerlaw]
    type = VariablePowerLawHardening
  []
[]

[Functions]
  [initial_yield_stress]
    type = ParsedFunction
    expression = '200 + 200*x'
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
[]

[Problem]
  type = FEProblem
  solve = false
[]

[Executioner]
  type = Steady
  solve_type = Newton
  petsc_options_iname = '-pc_type'
  petsc_options_value = 'lu'
  nl_abs_tol = 1.0e-8
[]

[Outputs]
  [out]
    type = Exodus
  []

  print_linear_residuals = false
[]
