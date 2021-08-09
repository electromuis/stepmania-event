include(lib/hidapi.cmake)
include(lib/adp.cmake)

add_library(ADPLights SHARED
	Plugin.cpp
	Plugin.h
)

target_compile_definitions(ADPLights PRIVATE PLUGIN__API_EXPORT__ CMAKE_POWERED GLOBALS_IMPORT)

target_include_directories(ADPLights PUBLIC
"${CMAKE_SOURCE_DIR}/src"
"${CMAKE_SOURCE_DIR}/src/generated"
"${CMAKE_SOURCE_DIR}/extern/dynalo/include"
)

target_link_libraries(ADPLights
	hidapi
	adp
	setupapi
	"lua-5.1"
)
		 
if(WIN32)
	set_target_properties(ADPLights PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY					"${CMAKE_CURRENT_SOURCE_DIR}/../"
		RUNTIME_OUTPUT_DIRECTORY_RELEASE			"${CMAKE_CURRENT_SOURCE_DIR}/../"
		RUNTIME_OUTPUT_DIRECTORY_DEBUG				"${CMAKE_CURRENT_SOURCE_DIR}/../"
		RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL			"${CMAKE_CURRENT_SOURCE_DIR}/../"
		RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO		"${CMAKE_CURRENT_SOURCE_DIR}/../"
	)

	string(TOLOWER ${CMAKE_BUILD_TYPE_INIT} BUILT_TYPE_LOWER)
			 
	target_link_libraries(ADPLights
		"${PROJECT_BINARY_DIR}/extern/${CMAKE_BUILD_TYPE_INIT}/StepMania-${BUILT_TYPE_LOWER}.lib"
	)
else()
	set_target_properties(ADPLights PROPERTIES
		LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../"
		PREFIX ""
	)
endif()