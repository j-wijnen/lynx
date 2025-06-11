[Mesh]
  [generated]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 5
    ny = 5
    xmax = 1
    ymax = 1
  []
[]

[AuxVariables]
  [temperature]
    order = first
    family = LAGRANGE
  []
  [density]
    order = first
    family = monomial
  []
  [thermal_conductivity]
    order = first
    family = monomial
  []
[]

[AuxKernels]
  [output_density]
    type = MaterialRealAux
    property = density
    variable = density
    execute_on = timestep_end
  []
  [output_thermal_conductivity]
    type = MaterialRealAux
    property = thermal_conductivity
    variable = thermal_conductivity
    execute_on = timestep_end
  []
[]

[Functions]
  [temperature]
    type = ParsedFunction
    expression = '20 + 1480*sqrt(x)*sqrt(y)'
  []
[]

[ICs]
  [temperature]
      type = FunctionIC
      variable = temperature
      function = temperature
  []
[]

[Materials]
  [fractions]
    type = GenericConstantMaterial
    prop_names = 'fraction_ferrite fraction_pearlite fraction_bainite
      fraction_martensite fraction_austenite'
    prop_values = '0.2 0.2 0.2 0.2 0.2'
  []
  [phaseproperties]
    type = PhaseFractionMaterialFromCSV
    temperature_variable = temperature
    prop_names = 'density thermal_conductivity'
    prop_files = 'properties.csv'
  []
[]

[Problem]
  solve = false
[]

[Executioner]
  type = Steady
[]

[Outputs]
  exodus = true
[]
