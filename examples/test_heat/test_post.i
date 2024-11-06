[Mesh]
  [generated]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 40
    ny = 10
    xmax = 40
    ymax = 10
  []
  [subdomains]
    type = ParsedSubdomainMeshGenerator
    input = generated
    combinatorial_geometry = 'x > 20.0'
    block_id = 2
  []
  [middle_sideset]
    type = SideSetsBetweenSubdomainsGenerator
    input = subdomains
    new_boundary = 'middle'
    primary_block = 0
    paired_block = 2
  []

  add_subdomain_ids = 1
[]

[Variables]
  [temp]
    order = first
    family = LAGRANGE
    block = '0 1'
  []
[]

[ICs]
    [base_ic]
        type = ConstantIC
        variable = temp
        value = 20
        block = 0
    []
    [weld_ic]
        type = ConstantIC
        variable = temp
        value = 1000
        block = 1
    []
[]

[Kernels]
  [heat_conduction]
      type = HeatConduction
      variable = temp
      block = '0 1'
  []
  [heat_timederivative]
      type = HeatConductionTimeDerivative
      variable = temp
      block = '0 1'
  []
[]

[Materials]
    [thermal]
        type = HeatConductionMaterial
        thermal_conductivity = 0.048
        specific_heat = 0.45
        block = '0 1'
    []
    [density]
        type = GenericConstantMaterial
        prop_names = 'density'
        prop_values = 0.0078
        block = '0 1'
    []
[]

[BCs]
  active = 'temp_middle'
  [temp_middle]
    type = FunctionDirichletBC
    variable = temp
    boundary = 'middle'
    function = '20 + 980 * t/10'
  []
[]

[Controls]
    active = 'disableheating'
    [disableheating]
      type = ConditionalFunctionEnableControl
      conditional_function = 't >= 10'
      disable_objects = 'BCs::temp_middle'
      enable_objects = ''
      execute_on = 'INITIAL TIMESTEP_BEGIN'
    []
[]

[UserObjects]
    active = 'insert'
   [insert]
       type = 'TimedSubdomainModifier'
       times =       '10'
       blocks_from = '2'
       blocks_to =   '1'
       execute_on = 'INITIAL TIMESTEP_END'
       apply_initial_conditions = true
   []
[]

[Problem]
  type = FEProblem
  kernel_coverage_check = false
  material_coverage_check = false
[]

[Executioner]
  type = Transient
  end_time = 50
  dt = 1
  solve_type = "NEWTON"
  petsc_options_iname = '-pc_type'
  petsc_options_value = 'lu'
  l_tol = 1.0e-6
  nl_rel_tol = 1.0e-4
  nl_abs_tol = 1.0e-4
[]

[Outputs]
  exodus = true
[]
