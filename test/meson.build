test_names = [
  'arguments_test',
  'cli',
  'dynamic',
  'event_d_test',
  'fs_watcher',
  'inotify',
  'logs',
  'memstream',
  'network_test',
  'property',
  'reader',
  'reallobuffer',
  'sh_process',
  'space_test',
  'uri_test',
  'util',
  'writer_test'
]

test_files = []

foreach test_name : test_names
  test_files += files(test_name + '.cpp')
  test_exe = executable(test_name + '.test', test_files[-1], link_with : lib, include_directories : [includes, src], dependencies : module_deps)
  test(meson.project_name() + '-' + test_name, test_exe, suite : 'unit')
endforeach
