[Mesh]
    construct_side_list_from_node_list=true

    [pipe]
        type = FileMeshGenerator
        file = pipeline.inp
    []

    [outer_base]
        type = SideSetsAroundSubdomainGenerator
        input = pipe
        new_boundary = base_outer
        block = Base_QUAD8
    []
[]

[Variables]
    [temp]
        initial_condition = 20.0
        order = FIRST
    []
[]

[Kernels]
    [heat_conduction]
        type = HeatConduction
        variable = temp
    []
    [heat_timederivative]
        type = HeatConductionTimeDerivative
        variable = temp
    []
[]

[Materials]
    [thermal]
        type = HeatConductionMaterial
        thermal_conductivity = 0.048
        specific_heat = 0.45
        #block = 'Base_QUAD8'
    []
    [density]
        type = GenericConstantMaterial
        prop_names = 'density'
        prop_values = 0.0078
    []
[]

[BCs]
    active = 'test'
    [test]
        type = LinearRampDirichletBC
        variable = temp
        boundary = 'base_outer'
        value = 100
        time = 10
    []
    [heating1]
        type = LinearRampDirichletBC
        variable = temp
        boundary = 'heating1'
        value = 50
        time = 5
    []
    [heating2]
        type = LinearRampDirichletBC
        variable = temp
        boundary = 'heating2'
        value = 1000
        time = 10
    []
    [convection1]
        type = ConvectiveHeatFluxBC
        variable = temp
        boundary = 'heating2'
        T_infinity = 5000
        heat_transfer_coefficient = 2.5e-5
    []
[]

[Controls]
    active = ''
    [heating1]
        type = TimePeriod
        enable_objects = 'BCs::heating1'
        start_time = '0'
        end_time = '5'
    []
    [heating2]
        type = TimePeriod
        enable_objects = 'BCs::heating2'
        start_time = '5'
        end_time = '10'
    []
[]

#[UserObjects]
#    [GlobalSubdomainModifier]
#        type = 'TimedSubdomainModifier'
#        times = '5'
#        blocks_from = 'Weld1'
#        blocks_to = 'part_inactive'
#        execute_on = 'INITIAL TIMESTEP_BEGIN'
#    []
#[]

[Problem]
  type = FEProblem  # This is the "normal" type of Finite Element Problem in MOOSE
[]

[Executioner]
    type = Transient
    end_time = 10
    dt = 1
    solve_type = NEWTON # Perform a Newton solve, uses AD to compute Jacobian terms
    petsc_options_iname = '-pc_type'
    petsc_options_value = 'lu'
    nl_abs_tol = 1e-8
[]

[Preconditioning]
    [smp]
       type = SMP
  []
[]

[Outputs]
    exodus = true
[]