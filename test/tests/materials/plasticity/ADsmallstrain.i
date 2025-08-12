[GlobalParams]
  displacements = 'disp_x disp_y'
  end_time = 1.0
  dt = 0.05
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
    use_automatic_differentiation = true
    new_system = false
    add_variables = true
    generate_output = 'stress_xx strain_xx plastic_strain_xx'
    planar_formulation = weak_plane_stress
    out_of_plane_strain = strain_zz
  []
[]

[Variables]
  [strain_zz]
  []
[]

[Materials]
  [elasticity]
    type = ADComputeIsotropicElasticityTensor
    youngs_modulus = 200e3
    poissons_ratio = 0.3
  []
  [plasticity]
    type = ADIsotropicPlasticStress
    hardening_material = hardening
  []
  [hardening]
    type = ADPowerLawHardening
    initial_yield_stress = 200.0
    youngs_modulus = 200e3
    hardening_exponent = 0.1
  []
[]

[BCs]
  [right]
      type = LinearRampDirichletBC
      variable = disp_x
      boundary = right
      value = 0.1
  []
  [left]
      type = DirichletBC
      variable = disp_x
      boundary = left
      value = 0.
  []
  [bottom]
      type = DirichletBC
      variable = disp_y
      boundary = bottom
      value = 0.
  []
[]

[Executioner]
  type = Transient
  solve_type = "NEWTON"
  petsc_options_iname = '-pc_type'
  petsc_options_value = 'lu'
  nl_abs_tol = 1e-6
  line_search = none
[]

[Outputs]
  print_linear_residuals = false
  [out]
    type = Exodus
    output_nonlinear = true
  []
[]