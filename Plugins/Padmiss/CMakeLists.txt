cmake_minimum_required(VERSION 3.19)
get_directory_property(hasParent PARENT_DIRECTORY)

set(PLUGIN_NAME "PadmissPlugin")

if(NOT hasParent)
	#project(Padmiss)
	
	get_filename_component(FOLDER_UP ${CMAKE_SOURCE_DIR} DIRECTORY)
	get_filename_component(FOLDER_UP ${FOLDER_UP} DIRECTORY)
	
	set(SM_BASE "${FOLDER_UP}")
	set(SM_LIBS "${SM_BASE}/Build/extern/${CMAKE_BUILD_TYPE_INIT}")
	
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
else()
	set(SM_BASE "${CMAKE_SOURCE_DIR}")
	set(SM_LIBS "${PROJECT_BINARY_DIR}/extern/${CMAKE_BUILD_TYPE_INIT}")
endif()


add_library(${PLUGIN_NAME} SHARED
	${PLUGIN_NAME}.cpp
	${PLUGIN_NAME}.h
	ScreenOptionsManageProfilesPM.cpp
	ScreenOptionsManageProfilesPM.h
	NotesWriterPadmiss.cpp
	NotesWriterPadmiss.h
	PadmissClient.cpp
	PadmissClient.h
)

if(hasParent)
	set_property(TARGET ${PLUGIN_NAME} PROPERTY FOLDER "Plugins")
endif()

target_compile_definitions(${PLUGIN_NAME} PRIVATE 
	GLOBALS_IMPORT 
	CMAKE_POWERED 
	LTM_DESC
	LTC_BASE64 
	LTC_BASE64_URL
)

target_include_directories(${PLUGIN_NAME} PUBLIC
	"${SM_BASE}/src"
	"${SM_BASE}/src/generated"
	"${SM_BASE}/extern/dynalo/include"
	"${SM_BASE}/extern/jsoncpp/include"
	"${SM_BASE}/src/libtomcrypt/src/headers"
	"${CMAKE_CURRENT_SOURCE_DIR}/lib/curl/include"
)

target_link_libraries(${PLUGIN_NAME}
	setupapi
	
	"${SM_LIBS}/tomcrypt.lib"
	"${SM_LIBS}/lua-5.1.lib"
	"${SM_LIBS}/jsoncpp.lib"
	
	"${CMAKE_CURRENT_SOURCE_DIR}/lib/curl/b/lib/Debug/libcurl-d_imp.lib"
)
		 
if(WIN32)
	set_target_properties(${PLUGIN_NAME} PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY					"${SM_BASE}/Plugins/"
		RUNTIME_OUTPUT_DIRECTORY_RELEASE			"${SM_BASE}/Plugins/"
		RUNTIME_OUTPUT_DIRECTORY_DEBUG				"${SM_BASE}/Plugins/"
		RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL			"${SM_BASE}/Plugins/"
		RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO		"${SM_BASE}/Plugins/"
	)

	string(TOLOWER ${CMAKE_BUILD_TYPE_INIT} BUILT_TYPE_LOWER)
	target_link_libraries(${PLUGIN_NAME}
		"${PROJECT_BINARY_DIR}/extern/${CMAKE_BUILD_TYPE_INIT}/StepMania-${BUILT_TYPE_LOWER}.lib"
	)
else()
	set_target_properties(${PLUGIN_NAME} PROPERTIES
		LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../"
		PREFIX ""
	)
endif()