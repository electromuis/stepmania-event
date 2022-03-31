cmake_minimum_required(VERSION 3.16)

IF(WIN32)
	add_compile_options(
		$<$<CONFIG:>:/MT> #---------|
		$<$<CONFIG:Debug>:/MTd> #---|-- Statically link the runtime libraries
		$<$<CONFIG:Release>:/MT> #--|
	)
ENDIF()

get_directory_property(hasParent PARENT_DIRECTORY)
if(hasParent)
  SET(SM_BASE "${CMAKE_CURRENT_SOURCE_DIR}/..")
  SET(SM_LIBS "${CMAKE_BINARY_DIR}/extern")
else()
  project(Plugins)
  SET(SM_BASE "C:/dev/stepmania/SDK")
  SET(SM_LIBS "${SM_BASE}/lib")
endif()

SET(PLUGINS_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

MACRO(SUBDIRLIST result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
      LIST(APPEND dirlist ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO()


MACRO(REGISTER_PLUGIN SOURCE_DIR PLUGIN_NAME)
	add_library(${PLUGIN_NAME} SHARED
		"${SOURCE_DIR}/${PLUGIN_NAME}.cpp"
		"${SOURCE_DIR}/${PLUGIN_NAME}.h"
	)
	
	#set_property(TARGET ${PLUGIN_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
	
	set_property(TARGET ${PLUGIN_NAME} PROPERTY FOLDER "Plugins")
	
	target_compile_definitions(${PLUGIN_NAME} PRIVATE 
		GLOBALS_IMPORT 
		CMAKE_POWERED 
		LTM_DESC
		LTC_BASE64 
		LTC_BASE64_URL
		PLUGIN_NAME="${PLUGIN_NAME}"
	)
	
	target_include_directories(${PLUGIN_NAME} PUBLIC
		"${SM_BASE}/src"
		"${SM_BASE}/src/generated"
		"${SM_BASE}/extern/dynalo/include"
		"${SM_BASE}/extern/jsoncpp/include"
	)
	
	target_link_libraries(${PLUGIN_NAME} setupapi)
	
	#target_link_libraries(${PLUGIN_NAME} Debug
#		"${SM_LIBS}/Debug/jsoncpp.lib"
#		"${SM_LIBS}/Debug/lua-5.1.lib"
#		"${SM_LIBS}/Debug/StepMania-debug.lib"
#	)
	
	target_link_libraries(${PLUGIN_NAME} $<$<CONFIG:Debug>:${SM_LIBS}/Debug/StepMania-debug.lib>)
	target_link_libraries(${PLUGIN_NAME} $<$<CONFIG:MinSizeRel>:${SM_LIBS}/MinSizeRel/StepMania-min-size.lib>)
	target_link_libraries(${PLUGIN_NAME} $<$<CONFIG:Release>:${SM_LIBS}/Release/StepMania.lib>)
	target_link_libraries(${PLUGIN_NAME} $<$<CONFIG:RelWithDebInfo>:${SM_LIBS}/RelWithDebInfo/StepMania-release-symbols.lib>)
	
	
	
	IF(WIN32)
		set_target_properties(${PLUGIN_NAME} PROPERTIES
			RUNTIME_OUTPUT_DIRECTORY					"${PLUGINS_DIR}"
			RUNTIME_OUTPUT_DIRECTORY_RELEASE			"${PLUGINS_DIR}"
			RUNTIME_OUTPUT_DIRECTORY_DEBUG				"${PLUGINS_DIR}"
			RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL			"${PLUGINS_DIR}"
			RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO		"${PLUGINS_DIR}"
		)
	ELSE()
		set_target_properties(${PLUGIN_NAME} PROPERTIES
			LIBRARY_OUTPUT_DIRECTORY "${PLUGINS_DIR}"
			PREFIX ""
		)
	ENDIF()
ENDMACRO()


include (lib/Curl.txt)

#set(CURLPP_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/curlpp/include")
#set(CURLPP_LIBRARY "${SM_LIBS}/libcurlpp.lib")

#SET(CURLPP_BUILD_SHARED_LIBS OFF)
#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/lib/curlpp")

SUBDIRLIST(SUBDIRS ${CMAKE_CURRENT_LIST_DIR})

FOREACH(subdir ${SUBDIRS})
	#IF(${subdir} MATCHES "Plugin" AND EXISTS "${CMAKE_CURRENT_LIST_DIR}/${subdir}/CMakePlugin.txt")
  IF(${subdir} MATCHES "Plugin")
	#INCLUDE("${CMAKE_CURRENT_LIST_DIR}/${subdir}/CMakePlugin.txt")
	ADD_SUBDIRECTORY(${subdir})
  ENDIF()
ENDFOREACH()