add_library(PluginTest SHARED
	Plugin.cpp
	Plugin.h
)

target_include_directories(PluginTest PUBLIC
"${CMAKE_SOURCE_DIR}/src"
"${CMAKE_SOURCE_DIR}/src/generated"
"${CMAKE_SOURCE_DIR}/extern/dynalo/include"
)

set_target_properties(PluginTest PROPERTIES
         LIBRARY_OUTPUT_DIRECTORY ../)