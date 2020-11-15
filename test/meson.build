test_names = [
  'arguments_test',
  'cli',
  'dynamic',
  'fs_watcher',
  'inotify',
  'logs',
  'memstream',
  'network_test',
  'property',
  'reader',
  'reallobuffer',
  'sh_process',
  'uri_test',
  'util',
  'writer_test'
]

test_files = []

foreach test_name : test_names
  test_files += files(test_name + '.cpp')
  test_exe = executable(test_name + '.test', test_files[-1], link_with : lib, include_directories : [includes, src], dependencies : module_deps)
  test(test_name, test_exe, suite : 'regular')
endforeach
