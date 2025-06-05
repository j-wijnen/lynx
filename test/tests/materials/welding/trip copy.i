[GlobalParams]
  displacements = 'disp_x disp_y'
[]

[Mesh]
  [generated]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 10
    ny = 2
    xmax = 5
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
    initial_condition = 20
  []
  [fraction_austenite]
    family = lagrange
    order = first 
    initial_condition = 1.0
  []
  [fraction_ferrite]
    family = lagrange
    order = first 
    initial_condition = 0.0
  []

  # Output
  [stress_xx]
    family = monomial
    order = first 
  []
  [strain_xx]
    family = monomial
    order = first 
  []
  [plasticstrain_xx]
    family = monomial 
    order = first
  []
  [plasticstrain]
    family = monomial 
    order = first
  []
[]

[AuxKernels]
  [temperature]
    type = FunctionAux
    variable = temperature
    function = temperature
    execute_on = timestep_begin
  []
  [fraction_austenite]
    type = FunctionAux
    variable = fraction_austenite 
    function = fraction_austenite
    execute_on = timestep_begin
  []
  [fraction_ferrite]
    type = FunctionAux
    variable = fraction_ferrite
    function = fraction_ferrite
    execute_on = timestep_begin
  []

  # Output
  [out_stress_xx]
    type = RankTwoAux 
    rank_two_tensor = stress
    variable = stress_xx
    index_i = 0
    index_j = 0
    execute_on = timestep_end
  []
  [out_strain_xx]
    type = RankTwoAux 
    rank_two_tensor = total_strain
    variable = strain_xx
    index_i = 0
    index_j = 0
    execute_on = timestep_end
  []
  [out_plasticstrain_xx]
    type = RankTwoAux 
    rank_two_tensor = plastic_strain
    variable = plasticstrain_xx
    index_i = 0
    index_j = 0
    execute_on = timestep_end
  []
  [out_plasticstrain]
    type = MaterialRealAux
    variable = plasticstrain
    property = effective_plastic_strain
  []
[]

[Physics/SolidMechanics/QuasiStatic]
  [all]
    strain = small
    incremental = false
    add_variables = true
  []
[]

[Materials]
  [fraction_ferrite]
    type = MaterialFromVariable 
    property = fraction_ferrite
    coupled_variable = fraction_ferrite 
  []
  [fraction_austenite]
    type = MaterialFromVariable 
    property = fraction_austenite
    coupled_variable = fraction_austenite
  []
  [generic]
    type = GenericConstantMaterial
    prop_names = 'fraction_pearlite fraction_bainite fraction_martensite'
    prop_values = '0.0 0.0 0.0'
  []
  [elasticity]
    type = ComputeIsotropicElasticityTensor
    youngs_modulus = 210e3
    poissons_ratio = 0.3
  []
  [stress]
    type = IsotropicPlasticTRIPStress
    yield_stress = 1e20
  []
[]

[Functions]
  [temperature]
    type = ParsedFunction
    expression = '20 + (1 - x/5)*t*1480'
  []
  [fraction_austenite]
    type = ParsedFunction
    #expression = '(1 - x/5)*t'
    expression = '1.0 - t'
  []
  [fraction_ferrite]
    type = ParsedFunction
    #expression = '1.0 - (1 - x/5)*t'
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
  dt = 0.01
  petsc_options_iname = '-pc_type'
  petsc_options_value = 'lu'
  nl_abs_tol = 1.0e-8
[]

[Outputs]
  exodus=true
  print_linear_residuals = false
[]
