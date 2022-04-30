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

link_directories(${SM_LIBS})
list(PREPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Modules")
SET(PLUGINS_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
SET(LOADED_PLUGINS "")
SET(TARGET_DEP_HOOKS "")
SET(TARGET_NAME "")
include("Modules/CMakeRC.cmake")

MACRO(REGISTER_PLUGIN SOURCE_DIR PLUGIN_NAME)
	SET(LOADED_PLUGINS ${LOADED_PLUGINS} "${PLUGIN_NAME}" PARENT_SCOPE)
	SET(TARGET_NAME "${PLUGIN_NAME}" PARENT_SCOPE)
	SET(TARGET_DEP_HOOKS "" PARENT_SCOPE)
	
	MESSAGE("${PLUGIN_NAME} regisered")

	IF(NOT WITH_PLUGINS_EMBEDDED)
		add_library(${PLUGIN_NAME} SHARED
			"${SOURCE_DIR}/${PLUGIN_NAME}.cpp"
			"${SOURCE_DIR}/${PLUGIN_NAME}.h"
		)
	ELSE()
		add_library(${PLUGIN_NAME} STATIC
			"${SOURCE_DIR}/${PLUGIN_NAME}.cpp"
			"${SOURCE_DIR}/${PLUGIN_NAME}.h"
		)
	ENDIF()
	
	set_property(TARGET ${PLUGIN_NAME} PROPERTY FOLDER "Plugins")
	
	SET(PLUGIN_DEFS
		CMAKE_POWERED 
		LTM_DESC
		LTC_BASE64 
		LTC_BASE64_URL	
		PLUGIN_NAME="${PLUGIN_NAME}"
	)
	
	IF(LINUX)
		list(APPEND PLUGIN_DEFS "UNIX")
	ENDIF()
	
	IF(WITH_PLUGINS_EMBEDDED)
		list(APPEND PLUGIN_DEFS "WITH_PLUGINS_EMBEDDED")
	ELSE()
		list(APPEND PLUGIN_DEFS "GLOBALS_IMPORT")
	ENDIF()
	
	target_compile_definitions(${PLUGIN_NAME} PRIVATE ${PLUGIN_DEFS})
	
	target_include_directories(${PLUGIN_NAME} PUBLIC
		"${SM_BASE}/src"
		"${SM_BASE}/src/libtomcrypt/src/headers"
		"${SM_BASE}/src/generated"
		"${SM_BASE}/extern/dynalo/include"
		"${SM_BASE}/extern/jsoncpp"
		"${SM_BASE}/extern/cmrc"
	)
	
	
	
	IF(NOT WITH_PLUGINS_EMBEDDED)
	
		SET(SMDATA_LINK_LIB
			"jsoncpp"
			"lua-5.1"
		)
		
		IF(EXISTS "${SOURCE_DIR}/resources")
			FILE(GLOB_RECURSE resources "${SOURCE_DIR}/resources/*.*")
			cmrc_add_resource_library("${PLUGIN_NAME}Resources" ${resources})
			list(APPEND SMDATA_LINK_LIB "${PLUGIN_NAME}Resources")
			set_property(TARGET "${PLUGIN_NAME}Resources" PROPERTY FOLDER "Plugins")
		ENDIF()
		
		IF(WIN32)
			list(APPEND SMDATA_LINK_LIB "setupapi")
			
		
			FOREACH(RELEASE_TYPE ${CMAKE_CONFIGURATION_TYPES})
				STRING(TOUPPER ${RELEASE_TYPE} RELEASE_TYPE_UPPER)
				
				# Link to Stepmania lib file
				LIST(
					APPEND
					SMDATA_LINK_LIB 
					$<$<CONFIG:${RELEASE_TYPE}>:${SM_NAME_${RELEASE_TYPE_UPPER}}.lib>
				)
				
				# Set Dll files to output in the Plugins folder
				SET_TARGET_PROPERTIES(${PLUGIN_NAME} PROPERTIES "RUNTIME_OUTPUT_DIRECTORY_${RELEASE_TYPE_UPPER}" "${PLUGINS_DIR}")
			ENDFOREACH()
		ELSE()
			set_target_properties(${PLUGIN_NAME} PROPERTIES
				LIBRARY_OUTPUT_DIRECTORY "${PLUGINS_DIR}"
				PREFIX ""
			)
		ENDIF()
		
		TARGET_LINK_LIBRARIES(${PLUGIN_NAME} ${SMDATA_LINK_LIB})
	ENDIF()
		
	IF(WITH_PLUGINS_EMBEDDED)
		SET(PLUGIN_HOOK_FILE "${CMAKE_CURRENT_BINARY_DIR}/generated/Embedded${PLUGIN_NAME}.cpp")
		CONFIGURE_FILE("${PLUGINS_DIR}/Embed.in.cpp" ${PLUGIN_HOOK_FILE})
		
		target_sources(StepMania PRIVATE ${PLUGIN_HOOK_FILE})
		target_link_libraries(${SM_EXE_NAME} ${PLUGIN_NAME})
	ENDIF()
	
ENDMACRO()

# Load all Plugin subdirectories which in turn call our REGISTER_PLUGIN() macro
FILE(GLOB subdirs RELATIVE ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/*Plugin)
FOREACH(subdir ${subdirs})
	SET(TARGET_NAME "")
	SET(TARGET_DEP_HOOKS "")

	ADD_SUBDIRECTORY(${subdir})

	IF(TARGET_NAME AND TARGET_DEP_HOOKS)
		add_dependencies("${TARGET_NAME}" ${TARGET_DEP_HOOKS})
	ENDIF()
ENDFOREACH()