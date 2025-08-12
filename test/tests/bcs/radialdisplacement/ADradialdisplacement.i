[GlobalParams]
  displacements = 'disp_x disp_y'
[]

[Mesh]
  file = pipe.msh
[]


[Physics/SolidMechanics/QuasiStatic]
  [all]
    strain = small
    incremental = false
    add_variables = true
    generate_output = 'vonmises_stress stress_xx'
    use_automatic_differentiation = true
  []
[]


[Materials]
  [elasticity]
    type = ADComputeIsotropicElasticityTensor
    youngs_modulus = 200e3
    poissons_ratio = 0.3
  []
  [stress]
    type = ADLinearElasticStress
  []
[]

[BCs]

  [disp_r1]
    type = ADRadialDisplacementBC
    boundary = Inner
    variable = disp_x 
    component = x
    value = 0.01
    penalty = 2e8
  []
  [disp_r2]
    type = ADRadialDisplacementBC
    boundary = Inner
    variable = disp_y 
    component = y
    value = 0.01
    penalty = 2e8
  []
  [fix_rot]
    type = DirichletBC 
    boundary = BottomInner
    variable = disp_x
    value = 0.0
  []
[]

[Executioner]
  type = Steady

  solve_type = NEWTON
  petsc_options_iname = '-pc_type'
  petsc_options_value = ' lu'
   
  nl_abs_tol = 1e-4
[]

[Outputs]
  exodus = true
  print_linear_residuals = false
[]