[GlobalParams]
  displacements = 'disp_x disp_y'
  end_time = 1.0
  num_steps = 20
[]

[Mesh]
  [pipe]
    type = FileMeshGenerator
    file = pipeline_linear.msh
  []
  [pipe_crack]
    type = LineNodeSetGenerator
    input = pipe 
    new_boundary = crack 
    point1 = '0 0 0'
    point2 = '0 505 0'
    width = 0.25
  []
[]

[Physics/SolidMechanics/QuasiStatic]
  [all]
    strain = small
    add_variables = true
    generate_output = 'vonmises_stress strain_xx'
    extra_vector_tags = ref
  []
[]

[Variables]
  [damage]
    family = lagrange
  []
[]

[Kernels]
  [pff_kernel]
    type = PhaseFieldFractureAT2
    variable = damage
  []
[]

[Materials]
  [properties]
    type = GenericConstantMaterial
    prop_names = 'fracture_toughness length_scale'
    prop_values = '2.5 0.5'
  []
  [elasticity]
    type = ComputeIsotropicElasticityTensor
    youngs_modulus = 210e3
    poissons_ratio = 0.3
  []
  [stress]
    type = PFFLinearElasticStress
  []
  [pff_degradation]
    type = PFFDegradationFunction
    variable = damage
  []
  [pff_driving]
    type = PFFDrivingForce
    variable = damage
  []
[]

[BCs]
    [disp_x]
        type = RadialDisplacementBC
        variable = disp_x
        component = x
        boundary = inner_boundary
        value = 0.4
        penalty = 1e6
    []
    [disp_y]
        type = RadialDisplacementBC
        variable = disp_y
        component = y
        boundary = inner_boundary
        value = 0.4
        penalty = 1e6
    []
    [fix_y]
        type = DirichletBC
        variable = disp_y
        boundary = inner_left 
        value = 0.0
    []
    [crack]
      type = DirichletBC
      variable = damage
      boundary = crack 
      value = 1.0
    []
[]

[Postprocessors]
  [cracktip]
    type = CrackTipLocation
    variable = damage
    direction = y 
    outputs = data
  []
  [iterations]
    type = NumNonlinearIterations
    outputs = data
  []
[]

[Problem]
  extra_tag_vectors = ref 
[]

[Executioner]
  type = Transient
  solve_type = "NEWTON"
  petsc_options_iname = '-pc_type -pc_factor_mat_solver_type'
  petsc_options_value = ' lu       mumps'
  line_search = none
  nonlinear_convergence = conv
[]

[Convergence]
  [conv]
    type = ReferenceResidualConvergence
    reference_vector = ref 
    normalization_type = global_Linf
    nl_rel_tol = 1e-3
    nl_max_its = 1000
    n_max_nonlinear_pingpong = 1000
  []
[]

[Outputs]
  print_linear_residuals = false
  [out]
    type = Exodus
  []
  [data]
    type = CSV
  []
[]