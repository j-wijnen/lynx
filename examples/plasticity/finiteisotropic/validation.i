[GlobalParams]
  end_time = 1.0
  dt = 0.05
  displacements = 'disp_x disp_y'
[]

[Mesh]
  [generated]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 5
    ny = 5
    xmax = 1
    ymax = 1
  []
  [leftbottom]
    type = BoundingBoxNodeSetGenerator
    new_boundary = leftbottom
    bottom_left = '-0.01 -0.01 0.0'
    top_right = '0.01 0.01 0'
    input = generated
  []
[]

[Physics/SolidMechanics/QuasiStatic]
  [all]
    new_system = true
    strain = finite 
    add_variables = true
    generate_output = 'pk1_stress_xx strain_xx'
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
  [elasticity]
    type = ComputeIsotropicElasticityTensor
    youngs_modulus = 210e3
    poissons_ratio = 0.3
  []
  #[./elasticity_tensor]
  #  type = ComputeElasticityTensor
  #  C_ijkl = '1.684e5 0.176e5 0.2e5 1.153e5 0.1e5 1.3512e5 0.7e5 0.9e5 0.4e5'
  #  fill_method = symmetric9
  #[../]
  [stress]
    type = IADIsotropicCrystalPlasticityStress
    large_kinematics = true
    rate_sensitivity = 0.05
    lamdot0 = 0.01
  []
[]

[BCs]
    [leftx]
        type = DirichletBC
        variable = disp_x
        boundary = left
        value = 0.
    []
    [lefty]
        type = DirichletBC
        variable = disp_y
        boundary = leftbottom
        value = 0.
    []
    [right]
        type = FunctionDirichletBC
        variable = disp_x
        boundary = right
        function = '0.01*t'
    []
[]

[Executioner]
  type = Transient
  solve_type = "NEWTON"
  petsc_options_iname = '-pc_type -pc_factor_mat_solver_type'
  petsc_options_value = 'lu mumps'
  l_tol = 1e-8
  nl_abs_tol = 1e-8
  nl_rel_tol = 1e-4
[]

[Postprocessors]
  [disp]
    type = NodalMaxValue 
    variable = disp_x 
    boundary = right
    outputs = force 
  []
  [force]
    type = NodalSum
    variable = force_x
    boundary = right
    outputs = force
  []
  [strain]
    type = ElementAverageValue
    variable = strain_xx
    outputs = force 
  []
  [stress]
    type = ElementAverageValue
    variable = pk1_stress_xx
    outputs = force 
  []
[]

[Outputs]
  [out]
    type = Exodus
  []
  [force]
    type = CSV
  []
  print_linear_residuals = false
[]