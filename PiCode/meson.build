project(
  'kingLearBot',
  'cpp',
  version : '1.0.0',
  license : 'MIT',
  default_options : []
)

add_global_arguments('-DSOME_TOKEN=value', language : 'cpp')

inc = include_directories('include')

proj_sources = ['src/main.cpp', 'src/communicator.cpp', 'src/robotActuation.cpp', 'src/robotControl.cpp']
proj_headers = ['include/communicator.h', 'include/robotActuation.h','include/robotControl.h', 'include/structs.h']
install_headers(proj_headers)

asio = subproject('asio')
asio_dep = asio.get_variable('asio_dep')

executable('botCode', proj_sources, include_directories : inc, dependencies : asio_dep)
