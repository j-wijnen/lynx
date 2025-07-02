[GlobalParams]
  end_time = 1.0
  dt = 0.05
  displacements = 'disp_x disp_y'
  large_kinematics = true
[]

[Mesh]
  [generated]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 10
    ny = 5
    xmax = 2
    ymax = 1
  []
[]

[Physics/SolidMechanics/QuasiStatic]
  [all]
    strain = finite
    new_system = true
    add_variables = true
    use_automatic_differentiation = false
    formulation = total
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
  [stress]
    type = IADHenckyStress
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
        boundary = left
        value = 0.
    []
    [right]
        type = FunctionDirichletBC
        variable = disp_x
        boundary = right
        function = 't'
    []
[]

[Executioner]
  type = Transient
  solve_type = "NEWTON"
  petsc_options_iname = '-pc_type -pc_factor_mat_solver_type'
  petsc_options_value = 'lu mumps'
  l_tol = 1e-8
  nl_abs_tol = 1e-3
  nl_rel_tol = 1e-4
  nl_max_its = 200
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