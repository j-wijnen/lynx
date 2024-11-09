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

[AuxVariables]
  [temp]
    family = monomial
    order = constant
    initial_condition = 750
  []
[]

[AuxKernels]
  [temp_constant]
    type = ConstantAux
    variable = temp 
    value = 750
  []
[]

[Materials]
    [sspt]
        type = SSPTSteel
        variable = temp
        output_properties = 'frac_a frac_f'
        outputs = exodus
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
    []
[]

[Problem]
  type = FEProblem
  solve = False
[]

[Executioner]
  type = Transient
  end_time = 80
  dt = 0.80
[]

[Outputs]
  exodus=true
  time_step_interval = 10
[]
