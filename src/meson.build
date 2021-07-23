sources = files([
  'arguments.cpp',
  'cli.cpp',
  'expandbuff.cpp',
  'log.cpp',
  'parse_essentials.cpp',
  'uri.cpp',
  'uuid.cpp'
])

src = include_directories('.')

lib = library(meson.project_name(), sources, include_directories : includes, install: true, dependencies: module_deps)
