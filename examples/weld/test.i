# Example 3

[Mesh]
  [generated]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 4
    ny = 1
    xmax = 2
    ymax = 1
  []
[]

[Variables]
  [temperature]
    order = FIRST
    family = LAGRANGE
    initial_condition = 20
  []
[]

[Kernels]
  [heat_conduction]
    type = HeatConduction
    variable = temperature
  []
  [heat_timederivative]
    type = HeatConductionTimeDerivative
    variable = temperature
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
        prop_values = '0.0078'
    []
    [sspt]
        type = SSPTSteel
        variable = temperature
        frac_f = 1.0
        output_properties = 'frac_a frac_f'
        outputs = out
    []
[]

[BCs]
  [right]
    type = LinearRampDirichletBC
    variable = temperature
    boundary = "right"
    value = 1500
    end_time = 0.5
  []
[]

[Executioner]
  type = Transient
  end_time = 1
  dt = .1
  solve_type = "NEWTON"
  petsc_options_iname = '-pc_type'
  petsc_options_value = 'lu'
  nl_rel_tol = 1.0e-4
  nl_abs_tol = 1.0e-4
[]

[Outputs]
    [out]
        type = Exodus
        time_step_interval = 2
    []
[]
