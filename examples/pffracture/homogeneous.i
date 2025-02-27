[GlobalParams]
  displacements = 'disp_x disp_y'
  large_kinematics = false
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

[Physics/SolidMechanics/QuasiStatic]
  [all]
    strain = small
    new_system = true
    add_variables = true
    generate_output = 'pk1_stress_xx strain_xx'
  []
[]

[Variables]
  [damage]
      family = lagrange
      order = first
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

[Outputs]
  exodus = true
  print_linear_residuals = false
[]