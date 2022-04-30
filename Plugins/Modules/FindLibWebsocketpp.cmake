IF(WIN32)
	include(FetchContent)
	
	SET(EXT_PATH "${CMAKE_BINARY_DIR}/Plugins/Modules/Websocketpp")
	
	FetchContent_Declare(AsioDL
		GIT_REPOSITORY https://github.com/chriskohlhoff/asio
		SOURCE_DIR "${EXT_PATH}/asio"
	)
	FetchContent_Populate(AsioDL)
	
	ExternalProject_Add(Websocketpp
	  #CMAKE_ARGS
		#-DCURL_INCLUDE_DIR=${CURL_INCLUDE_DIR}
		#-DCURL_LIBRARY=${CURL_LIBRARY}
		#-DCURLPP_BUILD_SHARED_LIBS=OFF
		#-DCMAKE_CXX_FLAGS_DEBUG=/MTd
	  GIT_REPOSITORY https://github.com/zaphoyd/websocketpp
	  GIT_TAG master
	  UPDATE_COMMAND ""
	  INSTALL_COMMAND ""
	  SOURCE_DIR "${EXT_PATH}/src"
	  BINARY_DIR "${EXT_PATH}/build"
	)
	
	add_library(websocketpp::websocketpp INTERFACE IMPORTED)
	add_dependencies(websocketpp::websocketpp Websocketpp)
	
	set_target_properties(websocketpp::websocketpp PROPERTIES 
		INTERFACE_INCLUDE_DIRECTORIES "${EXT_PATH}/src;${EXT_PATH}/asio/asio/include"
	)
ELSE()
	find_package(websocketpp REQUIRED)
	SET(ASIO_FILE "/usr/include/asio.hpp")
	IF(NOT EXISTS "${ASIO_FILE}")
		message(SEND_ERROR "Asio not found! ${ASIO_FILE}")
	ENDIF()
ENDIF()
