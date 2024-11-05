#####
# Define constants

#active_subdomains = 'Base_QUAD8 Weld1_QUAD8 Weld2_QUAD8'

temp_heating = 1500
temp_outer = 20

time_heating = 3.0
time_cooldown = 20.0

dt_heating = 0.03
dt_cooldown_start = 0.0001
dt_cooldown_end = 5.0

heating1_start = 0.0
heating1_end = ${fparse ${heating1_start} + ${time_heating} }
cooldown1_start = ${heating1_end}
cooldown1_end = ${fparse ${cooldown1_start} + ${time_cooldown} }

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
        block = 'Base_QUAD8 Weld1_QUAD8 Weld2_QUAD8'
    []
[]

[ICs]
    [weld_ic]
        type = ConstantIC
        variable = temp
        value = ${temp_heating}
        block = 'Weld1_QUAD8 Weld2_QUAD8' 
    []
    [base_ic]
        type = ConstantIC
        variable = temp
        value = ${temp_outer}
        block = Base_QUAD8
    []
[]

[Kernels]
    [heat_conduction]
        type = HeatConduction
        variable = temp
        block = 'Base_QUAD8 Weld1_QUAD8 Weld2_QUAD8'
    []
    [heat_timederivative]
        type = HeatCapacityConductionTimeDerivative
        variable = temp
        block = 'Base_QUAD8 Weld1_QUAD8 Weld2_QUAD8'
    []
[]

[Materials]
    [thermal]
        type = HeatConductionMaterial
        thermal_conductivity = 0.028
        specific_heat = 0.59
    []
    [density]
        type = GenericConstantMaterial
        prop_names = 'density heat_capacity'
        prop_values = '0.0076 0.004484'
    []
    [sspt_base]
        type = SSPTSteel
        block = Base_QUAD8
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
        block = 'Weld1_QUAD8 Weld2_QUAD8'
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
    [heating1_temp]
        type = LinearRampDirichletBC
        variable = temp
        boundary = 'Heating1Inner'
        #function = '20 + t/${heating1_end} * 1480'
        value = ${temp_heating}
        end_time = ${heating1_end}
    []
[]

[MeshModifiers]
    [GlobalSubdomainModifier]
        type = 'TimedSubdomainModifier'
        times =       '${heating1_end}'
        blocks_from = 'Weld1_INACTIVE'
        blocks_to =   'Weld1_QUAD8'
        execute_on = 'INITIAL TIMESTEP_BEGIN'
    []
[]

[Controls]
    [heating1]
        type = TimePeriod
        enable_objects = 'BCs::heating1_temp'
        start_time = ${heating1_start}
        end_time = ${heating1_end}
        set_sync_times = true
    []
[]

[Problem]
  type = FEProblem
  kernel_coverage_check = false
  boundary_restricted_elem_integrity_check = false
[]

[Executioner]
    type = Transient
    end_time = ${cooldown1_end}

    solve_type = NEWTON
    petsc_options_iname = '-pc_type'
    petsc_options_value = 'lu'
    nl_rel_tol = 1e-8
    nl_abs_tol = 1e-8

    [TimeSteppers]
        [step_ends]
            type = TimeSequenceStepper
            time_sequence = '${heating1_end} ${cooldown1_end}'
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