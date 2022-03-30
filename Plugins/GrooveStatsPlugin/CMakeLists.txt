set(PLUGIN_NAME "GrooveStatsPlugin")
REGISTER_PLUGIN(${CMAKE_CURRENT_SOURCE_DIR} ${PLUGIN_NAME})

target_sources(${PLUGIN_NAME} PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/GrooveStatsClient.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/GrooveStatsClient.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/GSReguests.h"
)

target_link_libraries(${PLUGIN_NAME}
	${CURL_LIBRARY}
	libcurlpp
)

add_dependencies(${PLUGIN_NAME} CurlPP)