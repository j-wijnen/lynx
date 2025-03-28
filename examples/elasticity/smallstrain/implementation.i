[GlobalParams]
  end_time = 1.0
  dt = 0.05
  displacements = 'disp_x disp_y'
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
    strain = small
    add_variables = true
    generate_output = 'stress_xx stress_yy stress_xy 
      vonmises_stress strain_yy strain_xy'
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
    type = LinearElasticStress
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
        type = LinearRampDirichletBC
        variable = disp_x
        boundary = right
        value = 0.5
    []
[]

[Postprocessors]
  [force]
    type = NodalSum
    variable = force_x
    boundary = right
    outputs = force
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

[Outputs]
  [out]
    type = Exodus
  []
  [force]
    type = CSV
  []
  print_linear_residuals = false
[]