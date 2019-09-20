sources = files([
  'cli.cpp',
  'descriptor.cpp',
  'epoll_d.cpp',
  'event_d.cpp',
  'expandbuff.cpp',
  'file_d.cpp',
  'imem_stream.cpp',
  'in_stream_d.cpp',
  'inotify_d.cpp',
  'omem_stream.cpp',
  'out_stream_d.cpp',
  'parse_essentials.cpp',
  'reader.cpp',
  'reallobuf.cpp',
  'signal_d.cpp',
  'socket_d.cpp',
  'stdin_d.cpp',
  'timer_d.cpp',
  'uuid.cpp'
])

src = include_directories('.')

sources += configure_file(output : 'config.cpp', input : 'config.cpp.in',
  configuration : {
  'group' : group,
  'project' : meson.project_name(),
  'version' : meson.project_version(),
  'build' : run_command('git', 'describe').stdout().strip(),
  'maintainer' : maintainer,
})

static_lib = static_library(meson.project_name(), sources, include_directories : includes, install: true, dependencies: module_deps)
shared_lib = shared_library(meson.project_name(), sources, include_directories : includes, install: true, dependencies: module_deps)
