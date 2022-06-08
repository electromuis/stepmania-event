FIND_PACKAGE("LibCurl" REQUIRED)

IF(WIN32)
	IF(NOT TARGET CurlPP)
		include("ExternalProject")
		
		SET(EXT_PATH "${CMAKE_BINARY_DIR}/Plugins/Modules/CurlPP")
		
		ExternalProject_Add(CurlPP
		  CMAKE_ARGS
			-DCURL_INCLUDE_DIR=${CURL_INCLUDE_DIR}
			-DCURL_LIBRARY=${CURL_LIBRARY}
			-DCURLPP_BUILD_SHARED_LIBS=OFF
			-DCMAKE_CXX_FLAGS_DEBUG=/MTd
		  GIT_REPOSITORY https://github.com/jpbarrette/curlpp
		  GIT_TAG master
		  UPDATE_COMMAND ""
		  INSTALL_COMMAND ""
		  SOURCE_DIR "${EXT_PATH}/src"
		  BINARY_DIR "${EXT_PATH}/build"
		)
		add_dependencies(CurlPP CURL::libcurl)
		
		add_library (libcurlpp STATIC IMPORTED GLOBAL)
		add_dependencies(libcurlpp CurlPP)
		
		# Cmake will complain if the folder doesnt exist yet
		file(MAKE_DIRECTORY "${EXT_PATH}/src/include")
		set_target_properties (libcurlpp PROPERTIES
			INTERFACE_INCLUDE_DIRECTORIES "${EXT_PATH}/src/include"
		)
		
		FOREACH(RELEASE_TYPE ${CMAKE_CONFIGURATION_TYPES})
			STRING(TOUPPER ${RELEASE_TYPE} RELEASE_TYPE_UPPER)
			set_target_properties (libcurlpp PROPERTIES
				IMPORTED_LOCATION_${RELEASE_TYPE_UPPER}
				"${EXT_PATH}/build/${RELEASE_TYPE}/libcurlpp.lib"
			)
		ENDFOREACH()
	ENDIF()
ELSE()
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(curlpp REQUIRED IMPORTED_TARGET GLOBAL curlpp)
	set_target_properties (PkgConfig::curlpp PROPERTIES
		INTERFACE_INCLUDE_DIRECTORIES ""
	)
	add_library(libcurlpp ALIAS PkgConfig::curlpp)
ENDIF()
