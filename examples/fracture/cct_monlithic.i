[GlobalParams]
  displacements = 'disp_x disp_y'
  end_time = 7.42
[]

[Mesh]
  file = cct.msh
[]

[Physics/SolidMechanics/QuasiStatic]
  [all]
    strain = small
    add_variables = true
    incremental = false
    generate_output = 'stress_xx stress_yy stress_xy vonmises_stress strain_xx strain_yy strain_xy'
    use_automatic_differentiation = true
  []
[]

[Variables]
  [phi]
    initial_condition = 0.0
  []
[]

[AuxVariables]
  [bounds_dummy]
  []
[]

# [Kernels]
#   [phinonlocal]
#     type = PhaseFieldFracture
#     variable = phi
#     length_scale = 0.3
#   []
# []

# [Materials]
#   [elasticity]
#     type = ComputeIsotropicElasticityTensor
#     youngs_modulus = 210e3
#     poissons_ratio = 0.3
#   []
#   [stress]
#     type = ComputePFFStress
#   []
#   [degradation]
#     type = ComputeDegradationFunction
#     variable = phi
#   []
#   [driving_force]
#     type = ComputePFFDrivingForce
#     variable = phi
#     Gc = 2.5
#   []
# []

[Kernels]
  [phinonlocal]
    type = ADPhaseFieldFracture
    variable = phi
    length_scale = 1.5
  []
[]

[Materials]
  [elasticity]
    type = ADComputeIsotropicElasticityTensor
    youngs_modulus = 210e3
    poissons_ratio = 0.3
  []
  [stress]
    type = ADComputeElasticPFFStress
  []
  [degradation]
    type = ADDerivativeParsedMaterial
    property_name = degradation
    coupled_variables = 'phi'
    expression = '(1.0-phi)^2*(1.0 - eta) + eta'
    constant_names       = 'eta'
    constant_expressions = '1e-8'
    derivative_order = 1
  []
  [drivingforce]
    type = ADParsedMaterial
    property_name = crack_driving_force
    constant_names = 'gc'
    constant_expressions = '2.5'
    material_property_names = 'strain_energy dg:=D[degradation,phi]'
    expression = 'dg * strain_energy / gc'
  []
[]

[BCs]
    [top]
        type = LinearRampDirichletBC
        variable = disp_y
        boundary = Top
        value = 0.00742
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
      variable = phi
      boundary = Crack
      value = 1.
    []
[]

[Executioner]
  type = Transient
  dt = 0.0742
  solve_type = "NEWTON"
  petsc_options_iname = '-pc_type -pc_factor_mat_solver_type'
  petsc_options_value = 'lu mumps'
  #petsc_options_iname = '-pc_type -pc_hypre_type'
  #petsc_options_value = 'hypre boomeramg'
  l_tol = 1e-8
  nl_abs_tol = 1e-6
  nl_rel_tol = 1e-4
  line_search = none
[]

[Outputs]
  [out]
    type = Exodus
    time_step_interval = 4
  []
  print_linear_residuals = false
[]