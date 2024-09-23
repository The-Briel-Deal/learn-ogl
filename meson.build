project(
  'learn-ogl',
  ['cpp', 'c'],
  version: '0.1',
  default_options: ['warning_level=3', 'cpp_std=c++14'],
)

glfw = dependency('glfw3')
include = include_directories('include')

glad = static_library('glad', sources: ['src/glad.c'], build_by_default: true, include_directories: include, override_options: ['warning_level=0'])

exe = executable(
  'learn-ogl',
  ['learn_ogl.cpp'],
  link_with: glad,
  install: true,
  dependencies: glfw,
  include_directories: include,
)

test('basic', exe)
