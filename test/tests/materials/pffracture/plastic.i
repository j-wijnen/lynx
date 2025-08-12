[GlobalParams]
  displacements = 'disp_x disp_y'
  end_time = 1.0
  dt = 0.01
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
[]

[Physics/SolidMechanics/QuasiStatic]
  [all]
    strain = small
    add_variables = true
    generate_output = 'strain_xx strain_yy'
    planar_formulation = weak_plane_stress 
    out_of_plane_strain = strain_zz
  []
[]

[Variables]
  [strain_zz]
    family = lagrange
  []
  [damage]
      family = lagrange
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

[Kernels]
  [pff]
    type = PhaseFieldFractureAT2
    variable = damage
  []
[]

[Materials]
  [elasticity]
    type = ComputeIsotropicElasticityTensor
    youngs_modulus = 210e3
    poissons_ratio = 0.3
  []
  [properties]
    type = GenericConstantMaterial
    prop_names = 'fracture_toughness length_scale'
    prop_values = '20 1.0'
  []
  [degradation]
    type = PFFDegradationFunction
    variable = damage
  []
  [stress]
    type = PFFIsotropicPlasticStress
    hardening_material = hardening
    beta = 0.1
  []
  [hardening]
    type = PowerLawHardening
    initial_yield_stress = 200
    youngs_modulus = 210e3
    hardening_exponent = 0.1
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
        value = 0.3
    []
    [left]
        type = DirichletBC
        variable = disp_x
        boundary = left
        value = 0.
    []
    [bottom]
        type = DirichletBC
        variable = disp_y
        boundary = bottom
        value = 0.
    []
[]

[Executioner]
  type = Transient
  solve_type = "NEWTON"
  petsc_options_iname = '-pc_type'
  petsc_options_value = 'lu'
  nl_abs_tol = 1e-2
  line_search = none
  nl_max_its = 500
  n_max_nonlinear_pingpong = 500
[]

[Outputs]
  exodus = true
[]