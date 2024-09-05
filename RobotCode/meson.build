project(
  'kingLearBot',
  'cpp',
  version: '1.0.0',
  license: 'MIT',
  default_options: [],
)

add_global_arguments('-DSOME_TOKEN=value', language: 'cpp')

inc = include_directories('include', '/usr/include/opencv4', '/usr/local/include/apriltag')

proj_sources = [
  'src/main.cpp',
  'src/driverStation.cpp',
  'src/robotActuation.cpp',
  'src/robotControl.cpp',
  'src/robotState.cpp',
  'src/robotVision.cpp',
  'src/util.cpp'
]

asio = subproject('asio')
asio_dep = asio.get_variable('asio_dep')

thread_dep = dependency('threads')
opencv_dep = dependency('opencv4')

link_args = ['-lapriltag']
if build_machine.system() == 'windows'
  link_args += '-lwsock32' # Required by ASIO  
endif

executable(
  'botCode',
  proj_sources,
  include_directories: inc,
  dependencies: [asio_dep, thread_dep, opencv_dep],
  link_args: link_args
)
