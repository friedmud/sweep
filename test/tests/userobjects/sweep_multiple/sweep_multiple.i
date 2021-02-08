[Mesh]
  [./oned]
    type = GeneratedMeshGenerator
    dim = 1
    nx = 10
  []
  [./makez]
    type = TransformGenerator
    input = oned
    transform = ROTATE
    vector_value = '90 90 0'
  []
  [./multiple]
    type = PatternedMeshGenerator
    inputs = makez
    pattern = '0 0 0 0; 0 0 0 0; 0 0 0 0; 0 0 0 0'
    x_width = 1
    y_width = 1
  []
[]

[Variables]
  [u]
  []
[]

[AuxVariables]
  [s]
  []
[]

[Kernels]
  [diff]
    type = Diffusion
    variable = u
  []
[]

[BCs]
  [left]
    type = DirichletBC
    variable = u
    boundary = left
    value = 0
  []
  [right]
    type = DirichletBC
    variable = u
    boundary = right
    value = 1
  []
[]

[Executioner]
  type = Steady
  solve_type = 'PJFNK'
  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre boomeramg'
[]

[Outputs]
  exodus = true
[]

[UserObjects]
  [./sweep]
    type = SweepUserObject
    boundary = left
    variable = s
  []
[]
