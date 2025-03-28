[GlobalParams]
  displacements = 'disp_x disp_y'
  end_time = 1.0
  num_steps = 100
[]

[Mesh]
  file = sent.msh
[]

[Physics/SolidMechanics/QuasiStatic]
  [all]
    strain = small
    new_system = false
    add_variables = true
    generate_output = 'stress_yy strain_yy plastic_strain_yy effective_plastic_strain'
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
active = 'plasticity'
  [plasticity]
    type = IsotropicPlasticStress
    youngs_modulus = 200e3
    poissons_ratio = 0.3
    yield_stress = 200
    hardening_law = powerlaw
    hardening_exponent = 0.1
  []
[]

[BCs]
    [top]
        type = LinearRampDirichletBC
        variable = disp_y
        boundary = Top
        value = 0.03
    []
    [bottom]
        type = DirichletBC
        variable = disp_y
        boundary = Bottom
        value = 0.
    []
    [left]
        type = DirichletBC
        variable = disp_x
        boundary = Left
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
  l_tol = 1e-8
  nl_abs_tol = 1e-6
  nl_rel_tol = 1e-4
  line_search = none
[]

[Postprocessors]
  [force]
    type = NodalSum
    variable = force_y
    boundary = Top
    outputs = csv
  []
[]

[Outputs]
  print_linear_residuals = false
  [out]
    type = Exodus
    time_step_interval = 5
  []
  [csv]
    type = CSV
  []
[]