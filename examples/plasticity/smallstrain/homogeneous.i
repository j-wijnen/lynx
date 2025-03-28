[GlobalParams]
  displacements = 'disp_x disp_y'
  end_time = 1.0
  dt = 0.1
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
    new_system = false
    add_variables = true
    generate_output = 'stress_xx strain_xx plastic_strain_xx'
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
  [plasticity]
    type = IsotropicPlasticStress
    youngs_modulus = 200e3
    poisson_ratio = 0.3
    yield_stress = 200.0
    hardening_law = powerlaw
    hardening_exponent = 0.1
  []
[]

[BCs]
    [right]
        type = LinearRampDirichletBC
        variable = disp_x
        boundary = right
        value = 0.005
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

[Postprocessors]
  [force]
    type = NodalSum
    variable = force_x
    boundary = right
    outputs = force
  []
[]

[Outputs]
  print_linear_residuals = false
  [out]
    type = Exodus
  []
  [force]
    type = CSV
  []
[]