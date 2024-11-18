[GlobalParams]
  displacements = 'disp_x disp_y'
  end_time = 1.0
  dt = 0.01
  #family = lagrange
  #order = first
[]

[Mesh]
  [generated]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 1
    ny = 1
    xmax = 1
    ymax = 1
  []
  [leftbottom]
    type = BoundingBoxNodeSetGenerator
    new_boundary = leftbottom
    bottom_left = '-0.5 -0.5 0.0'
    top_right = '0.5 0.5 0'
    input = generated
  []
[]

[Variables]
  [phi]
    initial_condition = 0.0
  []
[]

[Physics/SolidMechanics/QuasiStatic]
  [all]
    strain = small
    use_automatic_differentiation = true
    add_variables = true
    incremental = false
    generate_output = 'stress_xx strain_xx'
  []
[]

[Kernels]
  [phinonlocal]
    type = ADPhaseFieldFracture
    variable = phi
    length_scale = 0.1
  []
[]

[Materials]
  active = 'elasticity stress driving_force degradation'
  #active = 'elasticity stress driving_force degradationparsed derivativedegradationparsed'
  [elasticity]
    type = ADComputeIsotropicElasticityTensor
    youngs_modulus = 210e3
    poissons_ratio = 0.3
  []
  [degradation]
    type = ADComputeDegradation
    variable = phi
  []
  [stress]
    type = ADComputeElasticPFFStress
  []
  [driving_force]
    type = ADComputePFFDrivingForce
    Gc = 1.0
  []
  [degradationparsed]
    type = ADDerivativeParsedMaterial
    property_name = degradation
    coupled_variables = 'phi'
    expression = '(1.0-phi)^2*(1.0 - eta) + eta'
    constant_names       = 'eta'
    constant_expressions = '1e-8'
    derivative_order = 2
  []
  [derivativedegradationparsed]
    type = ADDerivativeParsedMaterial
    property_name = d_degradation
    coupled_variables = 'phi'
    material_property_names = 'dg:=D[degradation,phi]'
    expression = 'dg'
    derivative_order = 1
  []
  # [./pfbulkmat]
  #   type = GenericConstantMaterial
  #   prop_names = 'gc_prop l visco'
  #   prop_values = '1e-3 0.04 1e-4'
  # [../]
  # [./define_mobility]
  #   type = ParsedMaterial
  #   material_property_names = 'gc_prop visco'
  #   property_name = L
  #   expression = '1.0/(gc_prop * visco)'
  # [../]
  # [./define_kappa]
  #   type = ParsedMaterial
  #   material_property_names = 'gc_prop l'
  #   property_name = kappa_op
  #   expression = 'gc_prop * l'
  # [../]
  # [./damage_stress]
  #   type = ComputeLinearElasticPFFractureStress
  #   c = c
  #   E_name = 'elastic_energy'
  #   D_name = 'degradation'
  #   F_name = 'local_fracture_energy'
  #   decomposition_type = strain_spectral
  # [../]
  # [./degradation]
  #   type = DerivativeParsedMaterial
  #   property_name = degradation
  #   coupled_variables = 'c'
  #   expression = '(1.0-c)^2*(1.0 - eta) + eta'
  #   constant_names       = 'eta'
  #   constant_expressions = '0.0'
  #   derivative_order = 2
  # [../]
  # [./local_fracture_energy]
  #   type = DerivativeParsedMaterial
  #   property_name = local_fracture_energy
  #   coupled_variables = 'c'
  #   material_property_names = 'gc_prop l'
  #   expression = 'c^2 * gc_prop / 2 / l'
  #   derivative_order = 2
  # [../]
  # [./fracture_driving_energy]
  #   type = DerivativeSumMaterial
  #   coupled_variables = c
  #   sum_materials = 'elastic_energy local_fracture_energy'
  #   derivative_order = 2
  #   property_name = F
  # [../]
[]

[BCs]
    [right]
        type = LinearRampDirichletBC
        variable = disp_x
        boundary = right
        value = 0.02
    []
    [left]
        type = DirichletBC
        variable = disp_x
        boundary = left
        value = 0.
    []
    [bottomleft]
        type = DirichletBC
        variable = disp_y
        boundary = leftbottom
        value = 0.
    []
[]

[Problem]
  type = FEProblem
[]

[Executioner]
  type = Transient
  solve_type = "NEWTON"
  petsc_options_iname = '-pc_type -pc_factor_mat_solver_type'
  petsc_options_value = 'lu mumps'
  l_tol = 1e-8
  nl_abs_tol = 1e-6
  nl_rel_tol = 1e-2
[]

[Outputs]
  exodus = true
[]