[GlobalParams]
  displacements = 'disp_x disp_y'
  end_time = 1.0
  dt = 0.04
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
    [right]
        type = LinearRampDirichletBC
        variable = disp_x
        boundary = right
        value = 0.01
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

[Preconditioning]
  [smp]
    type = SMP
  []
[]

[Executioner]
  type = Transient
  solve_type = "NEWTON"
  petsc_options_iname = '-pc_type -pc_factor_mat_solver_type'
  petsc_options_value = 'lu mumps'
  l_tol = 1e-8
  nl_abs_tol = 1e-6
  nl_rel_tol = 1e-4
  line_search = none
[]

[Outputs]
  print_linear_residuals=false
  exodus = true
[]