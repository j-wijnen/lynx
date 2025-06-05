[GlobalParams]
  displacements = 'disp_x disp_y'
[]

[Mesh]
  [generated]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 10
    ny = 2
    xmax = 5
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

[AuxVariables]
  [density]
    family = lagrange
    order = first
    initial_condition = 1.0
  []
  # Output
  [stress_triaxial]
    family = monomial
    order = first 
  []
  [stress_yy]
    family = monomial
    order = first
  []
  [strain_volumetric]
    family = monomial
    order = first 
  []
  [strain_yy]
    family = monomial
    order = first
  []
  [eigenstrain_volumetric]
    family = monomial
    order = first 
  []
[]

[AuxKernels]
  [density]
    type = FunctionAux
    variable = density
    function = density
    execute_on = timestep_begin
  []

  # Output
  [out_stress_triaxial]
    type = RankTwoScalarAux
    rank_two_tensor = stress
    variable = stress_triaxial
    scalar_type = Hydrostatic
    execute_on = timestep_end
  []
  [out_stress_yy]
    type = RankTwoAux 
    rank_two_tensor = stress
    variable = stress_yy
    index_i = 1
    index_j = 1
    execute_on = timestep_end
  []
  [out_strain_volumentric]
    type = RankTwoScalarAux
    rank_two_tensor = total_strain
    variable = strain_volumetric
    scalar_type = FirstInvariant
    execute_on = timestep_end
  []
  [out_strain_yy]
    type = RankTwoAux 
    rank_two_tensor = stress
    variable = strain_yy
    index_i = 1
    index_j = 1
    execute_on = timestep_end
  []
  [out_eigenstrain_volumentric]
    type = RankTwoScalarAux
    rank_two_tensor = eigenstrain
    variable = eigenstrain_volumetric
    scalar_type = FirstInvariant
    execute_on = timestep_end
  []
[]

[Physics/SolidMechanics/QuasiStatic]
  [all]
    strain = small
    incremental = false
    add_variables = true
    eigenstrain_names = eigenstrain
  []
[]

[Materials]
  [density]
    type = MaterialFromVariable 
    property = density
    coupled_variable = density
  []
  [phase_expansion]
    type = VolumetricExpansionFromDensity
    eigenstrain_name = eigenstrain
    reference_density = 1.0
  []
  [elasticity]
    type = ComputeIsotropicElasticityTensor
    youngs_modulus = 210e3
    poissons_ratio = 0.3
  []
  [stress]
    type = LinearElasticStress
  []
[]

[Functions]
  [density]
    type = ParsedFunction
    expression = '1 - 0.5*(1 - x/5)*t'
  []
[]

[BCs]
  [fix_x]
    type = DirichletBC
    variable = disp_x
    boundary = left
    value = 0
  []
  [fix_y]
    type = DirichletBC
    variable = disp_y
    boundary = leftbottom
    value = 0
  []
[]

[Executioner]
  type = Transient
  solve_type = NEWTON
  end_time = 1
  dt = 0.5
  petsc_options_iname = '-pc_type'
  petsc_options_value = 'lu'
  nl_abs_tol = 1.0e-4
[]

[Outputs]
  exodus=true
  print_linear_residuals = false
[]
