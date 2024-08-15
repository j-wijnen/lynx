[Mesh]
  [generated]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 20
    ny = 10
    xmax = 20
    ymax = 10
  []
[]

[Variables]
  [temp]
    order = first
    family = LAGRANGE
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
    []
    [density]
        type = GenericConstantMaterial
        prop_names = 'density'
        prop_values = 0.0078
    []
[]

[BCs]
  [left_u]
    type = LinearRampDirichletBC
    variable = temp
    boundary = "left"
    value = 1500
    end_time = 10
  []
[]

[Controls]
    [heating]
        type = TimePeriod
        enable_objects = 'BCs::left_u'
        start_time = 0.0
        end_time = 10
    []
[]

[Executioner]
  type = Transient
  end_time = 20
  dt = 1
  solve_type = "NEWTON"
  petsc_options_iname = '-pc_type'
  petsc_options_value = 'hypre'
  l_tol = 1.0e-6
  nl_rel_tol = 1.0e-4
  nl_abs_tol = 1.0e-4
[]

[Outputs]
  exodus = true
[]
