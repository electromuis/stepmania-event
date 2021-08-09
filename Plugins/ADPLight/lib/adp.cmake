cmake_minimum_required (VERSION 3.6)

if(WIN32)
	set(CMAKE_GENERATOR_PLATFORM win32)
endif()

set(ADP_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib/adp")

include_directories(
	${ADP_PATH}
)

file(GLOB_RECURSE adp_sources
     "${ADP_PATH}/*.h"
     "${ADP_PATH}/*.cpp"
)

add_library(adp
	${adp_sources}
)

set_target_properties(adp PROPERTIES
	CXX_STANDARD 17
	CXX_EXTENSIONS OFF
)