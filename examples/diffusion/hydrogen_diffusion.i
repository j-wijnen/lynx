hydrogen_outer = 1.0

[GlobalParams]
  displacements = 'disp_x disp_y'
[]

[Mesh]
  file = mesh/sent.msh
[]

[Physics/SolidMechanics/QuasiStatic]
  [all]
    strain = small
    add_variables = true
    generate_output = ''
  []
[]

[Variables]
  [hydrogen]
    order = FIRST
    family = LAGRANGE
    initial_condition = ${hydrogen_outer}
  []
  [hydrostatic_stress]
    order = FIRST 
    family = LAGRANGE
  []
[]

[Kernels]
  [diffusion]
    type = MatDiffusion
    variable = hydrogen
    diffusivity = 1e-6
  []
  [timederivative]
    type = TimeDerivative
    variable = hydrogen
  []
  [stress_diffusion]
    type = StressDiffusion 
    diffusivity = 1e-6
    variable = hydrogen 
    hydrostatic_stress = hydrostatic_stress
  []
  [hydrostatic_stress]
    type = MaterialPropertyValue 
    variable = hydrostatic_stress 
    prop_name = hydrostatic_stress
  []
[]

[Materials]
  [elasticity]
    type = ComputeIsotropicElasticityTensor
    youngs_modulus = 200e3
    poissons_ratio = 0.3
  []
  [stress]
    type = LinearElasticStress
  []
  [hydrostatic_stress]
    type = RankTwoInvariant
    property_name = hydrostatic_stress 
    rank_two_tensor = stress 
    invariant = Hydrostatic
  []
[]

[BCs]
    [hydrogen_left]
        type = DirichletBC
        variable = hydrogen
        boundary = Left
        value = ${hydrogen_outer}
    []
    [hydrogen_right]
        type = DirichletBC
        variable = hydrogen
        boundary = Right
        value = ${hydrogen_outer}
    []
    [displacement_top]
        type = DirichletBC
        variable = disp_y
        boundary = Top
        value = 0.1
    []
    [displacement_bottom]
        type = DirichletBC
        variable = disp_y
        boundary = Bottom
        value = 0.0
    []
    [displacement_left]
      type = DirichletBC 
      variable = disp_x 
      boundary = Left 
      value = 0.0 
    []
[]

[Executioner]
  type = Transient
  solve_type = "NEWTON"
  petsc_options_iname = '-pc_type -pc_factor_mat_solver_type'
  petsc_options_value = 'lu mumps'

  l_tol = 1e-8
  nl_abs_tol = 1e-6
  nl_rel_tol = 1e-6

  end_time = 10e7
  dt = 1e7
[]

[Outputs]
  [out]
    type = Exodus
  []
  print_linear_residuals = false
[]