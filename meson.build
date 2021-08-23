project(
    'hashfn-benchmark',
    'cpp',
    default_options: [
        'buildtype=release',
        'optimization=3',
        'debug=true',
        'cpp_std=c++20',
        'warning_level=3',
    ],
)

cpp_args = [
  '-msse4.2',
]
add_project_arguments(cpp_args, language: 'cpp')

dependencies = [
#     dependency('absl_container', default_options: ['debug=true']),
]

srcs = [
  'main.cpp',
  'city.cpp',
  'xxhash.cpp',
  'xorwow.cpp',
]

include_directories = [
]

executable(
  'hashfn-benchmark',
  srcs,
  dependencies: dependencies,
  include_directories: include_directories,
)