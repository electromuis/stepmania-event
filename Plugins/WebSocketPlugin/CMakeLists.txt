set(PLUGIN_NAME "WebSocketPlugin")
REGISTER_PLUGIN(${CMAKE_CURRENT_SOURCE_DIR} ${PLUGIN_NAME})

target_sources(${PLUGIN_NAME} PRIVATE
	"${CMAKE_CURRENT_SOURCE_DIR}/SocketHandler.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/SocketHandler.h"
)

target_include_directories(${PLUGIN_NAME} PRIVATE
	"${SM_BASE}/extern/jsoncpp/include"
	"${CMAKE_CURRENT_SOURCE_DIR}/lib/websocketpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/lib/asio/asio/include"
)