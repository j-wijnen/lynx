[GlobalParams]
  displacements = 'disp_x disp_y'
  end_time = 0.2
  dt = 0.002
  order = first
[]

[Mesh]
  file = sent.msh
[]

[Physics/SolidMechanics/QuasiStatic]
  [all]
    strain = small
    add_variables = true
    generate_output = 'stress_yy strain_yy'
  []
[]

[Variables]
  [damage]
      family = lagrange
  []
[]

[Kernels]
  [pff]
    type = PhaseFieldFractureAT2
    variable = damage
  []
[]

[Materials]
  [properties]
    type = GenericConstantMaterial
    prop_names = 'fracture_toughness length_scale'
    prop_values = '2.5 1.0'
  []
  [elasticity]
    type = ComputeIsotropicElasticityTensor
    youngs_modulus = 210e3
    poissons_ratio = 0.3
  []
  [stress]
    type = PFFElasticStress
  []
  [degradation]
    type = PFFDegradationFunction
    variable = damage
  []
  [driving_force]
    type = PFFDrivingForce
    variable = damage
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
    [crack]
      type = DirichletBC
      variable = damage
      boundary = Crack 
      value = 1.0
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
  petsc_options_iname = '-pc_type -pc_factor_mat_solver_type'
  petsc_options_value = 'lu        mumps                    '
  nl_abs_tol = 1e-6
  nl_rel_tol = 1e-3
  line_search = none
  nl_max_its = 5000
  n_max_nonlinear_pingpong = 5000
[]

[Outputs]
  print_linear_residuals = false
  [out]
    type = Exodus
  []
[]