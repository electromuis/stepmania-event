set(PLUGIN_NAME "GrooveStatsPlugin")
REGISTER_PLUGIN(${CMAKE_CURRENT_SOURCE_DIR} ${PLUGIN_NAME})

FIND_PACKAGE("LibCurl" REQUIRED)
FIND_PACKAGE("CurlPP" REQUIRED)

target_sources(${PLUGIN_NAME} PRIVATE
	"${CMAKE_CURRENT_SOURCE_DIR}/GrooveStatsClient.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/GrooveStatsClient.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/GSReguests.h"
)

target_link_libraries(${PLUGIN_NAME}
	CURL::libcurl
	libcurlpp
)
