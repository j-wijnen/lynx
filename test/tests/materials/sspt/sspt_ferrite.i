[Mesh]
  [generated]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 1
    ny = 1
    xmax = 1
    ymax = 1
  []
[]

[Variables]
  [temp]
    order = first
    family = LAGRANGE
  []
[]

[ICs]
    [base_ic]
        type = ConstantIC
        variable = temp
        value = 750
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
        thermal_conductivity = 1.0
        specific_heat = 1.0
    []
    [density]
        type = GenericConstantMaterial
        prop_names = 'density'
        prop_values = 1.0
    []
    [sspt]
        type = SSPTSteel
        variable = temp
        output_properties = 'frac_a frac_f frac_p frac_b frac_m'
        outputs = xda
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
[]

[Problem]
  type = FEProblem
[]

[Executioner]
  type = Transient
  end_time = 10
  dt = 0.1
  solve_type = NEWTON
  petsc_options_iname = '-pc_type'
  petsc_options_value = 'lu'
  l_tol = 1.0e-6
  nl_rel_tol = 1.0e-4
  nl_abs_tol = 1.0e-4
[]

[Outputs]
  [xda]
    type = XDA
    execute_on = 'FINAL'
  []
[]
