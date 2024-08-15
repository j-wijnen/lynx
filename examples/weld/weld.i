#####
# Define constants

active_subdomains = 'Base_QUAD8 Weld1_QUAD8 Weld2_QUAD8'

temp_heating = 1500
temp_outer = 20

emissivity = 0.8
stefan_boltzmann_constant = 5.67e-14
heat_transfer_coefficient = 2.5e-5

time_heating = 2.0
time_cooldown = 10.0
time_cooldown_final = 60.0

dt_heating = 0.2
dt_cooldown_start = 0.2
dt_cooldown_end = 1.0
dt_cooldown_end_final = 10.0

heating1_start = 0.0
heating1_end = ${fparse ${heating1_start} + ${time_heating} }
cooldown1_start = ${heating1_end}
cooldown1_end = ${fparse ${cooldown1_start} + ${time_cooldown} }
heating2_start = ${cooldown1_end}
heating2_end = ${fparse ${heating2_start} + ${time_heating} }
cooldown2_start = ${heating2_end}
cooldown2_end = ${fparse ${cooldown2_start} + ${time_cooldown_final} }

######

[Mesh]
    construct_side_list_from_node_list=true

    [pipe]
        type = FileMeshGenerator
        file = pipeline.inp
    []

    [rename]
      type = RenameBlockGenerator
      input = pipe
      old_block = 'Weld1_QUAD8 Weld2_QUAD8'
      new_block = 'Weld1_INACTIVE Weld2_INACTIVE'
    []

    add_subdomain_names = 'Weld1_QUAD8 Weld2_QUAD8'
[]

[Variables]
    [temp]
        order = SECOND
        block = '${active_subdomains}'
    []
[]

[ICs]
    [base_ic]
        type = ConstantIC
        variable = temp
        value = ${temp_outer}
        block = Base_QUAD8
    []
    [weld_ic]
        type = ConstantIC
        variable = temp
        value = ${temp_heating}
        block = 'Weld1_QUAD8 Weld2_QUAD8' 
    []
[]

[Kernels]
    [heat_conduction]
        type = HeatConduction
        variable = temp
        block = ${active_subdomains}
    []
    [heat_timederivative]
        type = HeatConductionTimeDerivative
        variable = temp
        block = ${active_subdomains}
    []
[]

[Materials]
    [thermal]
        type = HeatConductionMaterial
        thermal_conductivity = 0.048
        specific_heat = 0.45
    []
    [density]
        type = GenericConstantMaterial
        prop_names = 'density'
        prop_values = 0.0078
    []
[]

[BCs]
    [heating1_temp]
        type = LinearRampDirichletBC
        variable = temp
        boundary = 'Heating1Inner'
        value = ${temp_heating}
        end_time = ${heating1_end}
    []
    [heating1_convection]
        type = ConvectiveHeatFluxBC
        variable = temp
        boundary = 'Heating1Outer'
        T_infinity = ${temp_outer}
        heat_transfer_coefficient = ${heat_transfer_coefficient}
    []
    [heating1_radiation]
        type = FunctionRadiativeBC
        variable = temp
        boundary = 'Heating1Outer'
        Tinfinity = ${temp_outer}
        emissivity_function = ${emissivity}
        stefan_boltzmann_constant = ${stefan_boltzmann_constant}
    []

    [cooldown1_convection]
        type = ConvectiveHeatFluxBC
        variable = temp
        boundary = 'Cooldown1Outer'
        T_infinity = ${temp_outer}
        heat_transfer_coefficient = ${heat_transfer_coefficient}
    []
    [cooldown1_radiation]
        type = FunctionRadiativeBC
        variable = temp
        boundary = 'Cooldown1Outer'
        Tinfinity = ${temp_outer}
        emissivity_function = ${emissivity}
        stefan_boltzmann_constant = ${stefan_boltzmann_constant}
    []

    [heating2_temp]
        type = LinearRampDirichletBC
        variable = temp
        boundary = 'Heating2Inner'
        value = ${temp_heating}
        end_time = ${heating1_end}
    []
    [heating2_convection]
        type = ConvectiveHeatFluxBC
        variable = temp
        boundary = 'Heating2Outer'
        T_infinity = ${temp_outer}
        heat_transfer_coefficient = ${heat_transfer_coefficient}
    []
    [heating2_radiation]
        type = FunctionRadiativeBC
        variable = temp
        boundary = 'Heating2Outer'
        Tinfinity = ${temp_outer}
        emissivity_function = ${emissivity}
        stefan_boltzmann_constant = ${stefan_boltzmann_constant}
    []

    [cooldown2_convection]
        type = ConvectiveHeatFluxBC
        variable = temp
        boundary = 'Cooldown2Outer'
        T_infinity = ${temp_outer}
        heat_transfer_coefficient = ${heat_transfer_coefficient}
    []
    [cooldown2_radiation]
        type = FunctionRadiativeBC
        variable = temp
        boundary = 'Cooldown2Outer'
        Tinfinity = ${temp_outer}
        emissivity_function = ${emissivity}
        stefan_boltzmann_constant = ${stefan_boltzmann_constant}
    []
[]

[UserObjects]
   [GlobalSubdomainModifier]
       type = 'TimedSubdomainModifier'
       times =       '${heating1_end}   ${heating2_end}'
       blocks_from = 'Weld1_INACTIVE    Weld2_INACTIVE'
       blocks_to =   'Weld1_QUAD8       Weld2_QUAD8'
       execute_on = 'INITIAL TIMESTEP_BEGIN'
   []
[]

[Controls]
    [heating1]
        type = TimePeriod
        enable_objects = 'BCs::heating1_temp BCs::heating1_convection BCs::heating1_radiation'
        start_time = ${heating1_start}
        end_time = ${heating1_end}
    []
    [cooldown1]
        type = TimePeriod
        enable_objects = 'BCs::cooldown1_convection BCs::cooldown1_radiation'
        start_time = ${cooldown1_start}
        end_time = ${cooldown1_end}
    []
    [heating2]
        type = TimePeriod
        enable_objects = 'BCs::heating2_temp BCs::heating2_convection BCs::heating2_radiation'
        start_time = ${heating2_start}
        end_time = ${heating2_end}
    []
    [cooldown2]
        type = TimePeriod
        enable_objects = 'BCs::cooldown2_convection BCs::cooldown2_radiation'
        start_time = ${cooldown2_start}
        end_time = ${cooldown2_end}
    []
[]

[Problem]
  type = FEProblem
  kernel_coverage_check = false
  boundary_restricted_elem_integrity_check = false
[]

[Executioner]
    type = Transient
    end_time = ${cooldown2_end}

    solve_type = NEWTON
    petsc_options_iname = '-pc_type'
    petsc_options_value = 'lu'
    nl_rel_tol = 1e-8
    nl_abs_tol = 1e-8

    [TimeSteppers]
        [step_ends]
            type = TimeSequenceStepper
            time_sequence = '${heating1_end} ${cooldown1_end} ${heating2_end} ${cooldown2_end}'
        []
        [heating1]
            type = FunctionDT
            function = 'if(t >= ${heating1_start} & t < ${heating1_end}, ${dt_heating}, 100.0)'
        []
        [cooldown1]
            type = FunctionDT
            function = 'if(t >= ${cooldown1_start} & t < ${cooldown1_end}, 
            ${dt_cooldown_start} + (t-${cooldown1_start})/(${cooldown1_end}-${cooldown1_start})*(${dt_cooldown_end} -${dt_cooldown_start}),
             100.0)'
        []
        [heating2]
            type = FunctionDT
            function = 'if(t >= ${heating2_start} & t < ${heating2_end}, ${dt_heating}, 100.0)'
        []
        [cooldown2]
            type = FunctionDT
            function = 'if(t >= ${cooldown2_start} & t < ${cooldown2_end}, 
            ${dt_cooldown_start} + (t-${cooldown2_start})/(${cooldown2_end}-${cooldown2_start})*(${dt_cooldown_end_final} -${dt_cooldown_start}),
             100.0)'
        []
    []

[]

[Preconditioning]
    [smp]
       type = SMP
  []
[]

[Outputs]
    [out]
        type = Exodus
        time_step_interval = 2
    []
[]