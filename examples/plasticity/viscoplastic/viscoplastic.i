csv_file = 'viscoplastic_00025s'

[GlobalParams]
  displacements = 'disp_x disp_y'
  end_time = 0.0025
  num_steps = 50
[]

[Mesh]
  [generated]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 2
    ny = 2
    xmax = 1
    ymax = 1
  []
[]

[Physics/SolidMechanics/QuasiStatic]
  [all]
    strain = small
    new_system = false
    add_variables = true
    volumetric_locking_correction = false
    generate_output = 'stress_xx strain_xx plastic_strain_xx effective_plastic_strain'
    save_in = 'force_x force_y'
  []
[]

[AuxVariables]
  [force_x]
    family = lagrange
  []
  [force_y]
    family = lagrange
  []
[]

[Materials]
  [elsasticity]
    type = ComputeIsotropicElasticityTensor
    youngs_modulus = 200e3
    poissons_ratio = 0.3
  []
  [plasticity]
    type = IsotropicViscoPlasticStress
    hardening_material = hardening
    viscosity = 1e-1
    #viscosity_law = peric
    #rate_sensitivity = 0.5
  []
  [hardening]
    type = PowerLawHardening
    initial_yield_stress = 200
    youngs_modulus = 200e3
    hardening_exponent = 0.1
  []
[]

[BCs]
    [top]
        type = LinearRampDirichletBC
        variable = disp_x
        boundary = right
        value = 0.1
    []
    [bottom]
        type = DirichletBC
        variable = disp_x
        boundary = left
        value = 0.
    []
    [left]
        type = DirichletBC
        variable = disp_y
        boundary = bottom
        value = 0.
    []
[]

[Preconditioning]
  [smp]
    type = SMP
    full = true
  []
[]

[Executioner]
  type = Transient
  solve_type = "NEWTON"
  petsc_options_iname = '-pc_type'
  petsc_options_value = 'lu'
  nl_abs_tol = 1e-6
  nl_rel_tol = 1e-6
  line_search = none
[]

[Postprocessors]
  [force]
    type = NodalSum
    variable = force_x
    boundary = right
    outputs = data
  []
  [displacement]
    type = AverageNodalVariableValue
    variable = disp_x
    boundary = right
    outputs = data
  []
  [stress]
    type = ElementAverageValue
    variable = stress_xx
    outputs = data
  []
  [strain]
    type = ElementAverageValue
    variable = strain_xx
    outputs = data
  []
  [iterations]
    type = NumNonlinearIterations
    outputs = data
  []
[]

[Outputs]
  print_linear_residuals = false
  [out]
    type = Exodus
    time_step_interval = 10
  []
  [data]
    type = CSV
    file_base = ${csv_file}
  []
[]