IF(WIN32)
	IF(NOT TARGET Curl)
		INCLUDE("ExternalProject")
		
		SET(EXT_PATH "${CMAKE_BINARY_DIR}/Plugins/Modules/Curl")
		
		ExternalProject_Add(Curl
		  CMAKE_ARGS
			-DCURL_USE_SCHANNEL=ON
			#-DCMAKE_CXX_FLAGS_DEBUG=/MTd
			-DCURL_STATIC_CRT=ON
			-DBUILD_SHARED_LIBS=ON
			-DBUILD_CURL_EXE=OFF
		  GIT_REPOSITORY https://github.com/curl/curl
		  GIT_TAG master
		  UPDATE_COMMAND ""
		  INSTALL_COMMAND ""
		  SOURCE_DIR "${EXT_PATH}/src"
		  BINARY_DIR "${EXT_PATH}/build"
		)
		
		add_library (libcurl STATIC IMPORTED GLOBAL)
		add_dependencies(libcurl Curl)

		# Cmake will complain if the folder doesnt exist yet
		file(MAKE_DIRECTORY "${EXT_PATH}/src/include")
		set_target_properties (libcurl PROPERTIES
			INTERFACE_INCLUDE_DIRECTORIES "${EXT_PATH}/src/include"
		)
		
		FOREACH(RELEASE_TYPE ${CMAKE_CONFIGURATION_TYPES})
			STRING(TOUPPER ${RELEASE_TYPE} RELEASE_TYPE_UPPER)
			
			SET(LIB_POSTFIX "")
			IF(RELEASE_TYPE MATCHES "Debug")
				SET(LIB_POSTFIX "-d")
			ENDIF()
			
			set_target_properties (libcurl PROPERTIES
				IMPORTED_LOCATION_${RELEASE_TYPE_UPPER}
				"${EXT_PATH}/build/lib/${RELEASE_TYPE}/libcurl${LIB_POSTFIX}_imp.lib"
			)
		ENDFOREACH()
		
		SET(CURL_LIBRARY "${EXT_PATH}/build/lib/Debug/libcurl-d_imp.lib")
		SET(CURL_INCLUDE_DIR "${EXT_PATH}/src/include")
		
	ENDIF()
ELSE()
	FIND_PACKAGE("CURL" REQUIRED)
	
	IF(NOT libcurl)
		add_library (libcurl STATIC IMPORTED GLOBAL)
		set_target_properties (libcurl PROPERTIES
			INTERFACE_INCLUDE_DIRECTORIES "${CURL_INCLUDE_DIR}"
			IMPORTED_LOCATION "${CURL_LIBRARY}"
		)
	ENDIF()
ENDIF()
