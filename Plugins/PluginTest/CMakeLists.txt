add_library(PluginTest SHARED
	Plugin.cpp
	Plugin.h
)

target_compile_definitions(PluginTest PRIVATE PLUGIN__API_EXPORT__ CMAKE_POWERED)

target_include_directories(PluginTest PUBLIC
"${CMAKE_SOURCE_DIR}/src"
"${CMAKE_SOURCE_DIR}/src/generated"
"${CMAKE_SOURCE_DIR}/extern/dynalo/include"
)
		 
if(WIN32)
	set_target_properties(PluginTest PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY					"${CMAKE_CURRENT_SOURCE_DIR}/../"
		RUNTIME_OUTPUT_DIRECTORY_RELEASE			"${CMAKE_CURRENT_SOURCE_DIR}/../"
		RUNTIME_OUTPUT_DIRECTORY_DEBUG				"${CMAKE_CURRENT_SOURCE_DIR}/../"
		RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL			"${CMAKE_CURRENT_SOURCE_DIR}/../"
		RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO		"${CMAKE_CURRENT_SOURCE_DIR}/../"
	)

	string(TOLOWER ${CMAKE_BUILD_TYPE_INIT} BUILT_TYPE_LOWER)
			 
	target_link_libraries(PluginTest
	"${PROJECT_BINARY_DIR}/extern/${CMAKE_BUILD_TYPE_INIT}/StepMania-${BUILT_TYPE_LOWER}.lib"
	)
else()
	set_target_properties(PluginTest PROPERTIES
		LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../"
		PREFIX ""
	)
endif()