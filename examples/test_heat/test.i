t_heat = 2.0
t_total = 120

[Mesh]
  [generated]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 100
    ny = 10
    xmax = 100
    ymax = 10
  []
  [subdomains]
    type = ParsedSubdomainMeshGenerator
    input = generated
    combinatorial_geometry = 'x > 90.0'
    block_id = 2
  []
  [middle_sideset]
    type = SideSetsBetweenSubdomainsGenerator
    input = subdomains
    new_boundary = 'middle'
    primary_block = 0
    paired_block = 2
  []
  [topleft_sideset]
    type = SideSetsAroundSubdomainGenerator
    input = middle_sideset
    new_boundary = topleft
    block = 0
    normal = '0 1 0'
  []
  [bottomleft_sideset]
    type = SideSetsAroundSubdomainGenerator
    input = topleft_sideset
    new_boundary = bottomleft
    block = 0
    normal = '0 -1 0'
  []
  [topright_sideset]
    type = SideSetsAroundSubdomainGenerator
    input = bottomleft_sideset
    new_boundary = topright
    block = 2
    normal = '0 1 0'
  []
  [bottomright_sideset]
    type = SideSetsAroundSubdomainGenerator
    input = topright_sideset
    new_boundary = bottomright
    block = 2
    normal = '0 -1 0'
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
        value = 1500
        block = 1
    []
[]

[Kernels]
  [heat_conduction]
      type = HeatConduction
      variable = temp
      #block = '0 1'
  []
  [heat_timederivative]
      type = HeatConductionTimeDerivative
      variable = temp
      #block = '0 1'
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
    [sspt_base]
        type = SSPTSteel
        block = 0
        variable = temp
        output_properties = 'frac_a frac_f frac_p frac_b frac_m'
        outputs = out
        frac_f = 1.0
        comp_C = 0.176
        comp_Si = 0.217
        comp_Mn = 1.37
        comp_P = 0.016
        comp_V = 0.053
        comp_Ti = 0.002
        comp_Cr = 0.015
        comp_Ni = 0.02
        comp_Mo = 0.002
        comp_As = 0.011 
        comp_Al = 0.026
        temp_Ae3 = 823.2
        temp_Ae1 = 706.7
        temp_Bs = 605.6
        temp_Ms = 417.8
    []
    [sspt_weld]
        type = SSPTSteel
        block = 1
        variable = temp
        output_properties = 'frac_a frac_f frac_p frac_b frac_m'
        outputs = out
        frac_a = 1.0
        comp_C = 0.176
        comp_Si = 0.217
        comp_Mn = 1.37
        comp_P = 0.016
        comp_V = 0.053
        comp_Ti = 0.002
        comp_Cr = 0.015
        comp_Ni = 0.02
        comp_Mo = 0.002
        comp_As = 0.011 
        comp_Al = 0.026
        temp_Ae3 = 823.2
        temp_Ae1 = 706.7
        temp_Bs = 605.6
        temp_Ms = 417.8
    []
[]

[BCs]
  [heating]
    type = LinearRampDirichletBC
    variable = temp
    boundary = 'middle'
    value = 1500
    end_time = ${t_heat}
  []
  [convection1]
    type = ConvectiveHeatFluxBC
    variable = temp
    boundary = 'topleft left bottomleft'
    T_infinity = 20
    heat_transfer_coefficient = 2.5e-4
  []
  [convection2]
    type = ConvectiveHeatFluxBC
    variable = temp
    boundary = 'right bottomright topright'
    T_infinity = 20
    heat_transfer_coefficient = 2.5e-4
  []
  [radiation1]
    type = FunctionRadiativeBC
    variable = temp
    boundary = 'topleft left bottomleft'
    Tinfinity = 20
    emissivity_function = 0.8
    stefan_boltzmann_constant = 5.67e-14
  []
  [radiation2]
    type = FunctionRadiativeBC
    variable = temp
    boundary = 'topleft left bottomleft'
    Tinfinity = 20
    emissivity_function = 0.8
    stefan_boltzmann_constant = 5.67e-14
  []
[]

[MeshModifiers]
    active = 'insert'
   [insert]
       type = 'TimedSubdomainModifier'
       times =       '${t_heat}'
       blocks_from = '2'
       blocks_to =   '1'
       execute_on = 'INITIAL TIMESTEP_BEGIN'
       #apply_initial_conditions = true
   []
[]

[Controls]
  [step1]
    type = TimePeriod
    enable_objects = '*::step1 BCs::heating BCs::convection1 BCs::radiation1'
    end_time = ${t_heat}
  []
  [step2]
    type = TimePeriod
    enable_objects = '*::step2 BCs::convection2 BCs::radiation2'
    start_time = ${t_heat}
  []
[]

[Problem]
  type = FEProblem
  kernel_coverage_check = false
  material_coverage_check = false
  boundary_restricted_elem_integrity_check = false
[]

[Executioner]
  type = Transient
  end_time = ${t_total}
  solve_type = NEWTON
  petsc_options_iname = '-pc_type'
  petsc_options_value = 'lu'
  l_tol = 1.0e-6
  nl_rel_tol = 1.0e-4
  nl_abs_tol = 1.0e-4

  [TimeSteppers]
    [step1]
        type = ConstantDT
        dt = 0.2
    []
    [step2]
        type = IncreasingDT
        dt = 0.1
        max_dt = 10.
    []
  []
[]

[Outputs]
    [out]
        type = Exodus
        time_step_interval = 2
    []
[]
