project( 'sim_8086', 'c', 'cpp', default_options : ['buildtype=debug'] )

include_thirdparty = include_directories( '../thirdparty' )

# add_global_arguments('-E', language : 'cpp')

if get_option('buildtype').startswith('debug')

  add_project_arguments('-DBuild_Debug', language : 'cpp')

endif

executable( 'sim_8086', 'sim_8086.cpp', include_directories : include_thirdparty )
