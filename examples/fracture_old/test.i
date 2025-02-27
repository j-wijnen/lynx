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

[Physics/SolidMechanics/QuasiStatic]
  [all]
    strain = small
    add_variables = true
    generate_output = 'stress_xx strain_xx'
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

[Kernels]
  [phinonlocal]
    type = PhaseFieldFracture
    variable = phi
    length_scale = 1.5
  []
[]

[Materials]
  [elasticity]
    type = ComputeIsotropicElasticityTensor
    youngs_modulus = 210e3
    poissons_ratio = 0.3
  []
  [stress]
    type = ComputePFFStress
  []
  [degradation]
    type = ComputeDegradationFunction
    variable = phi
  []
  [driving_force]
    type = ComputePFFDrivingForce
    variable = phi
    Gc = 2.5
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

[Bounds]
  [./upper]
    type = ConstantBounds
    variable = bounds_dummy
    bounded_variable = phi
    bound_type = upper
    bound_value = 1
  [../]
  [./lower]
    type = VariableOldValueBounds
    variable = bounds_dummy
    bounded_variable = phi
    bound_type = lower
  [../]
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
  print_linear_residuals=false
[]