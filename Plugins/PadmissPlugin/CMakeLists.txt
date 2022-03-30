set(PLUGIN_NAME "PadmissPlugin")
REGISTER_PLUGIN(${CMAKE_CURRENT_SOURCE_DIR} ${PLUGIN_NAME})

target_sources(${PLUGIN_NAME} PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/ScreenPadmiss.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/ScreenPadmiss.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/NotesWriterPadmiss.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/NotesWriterPadmiss.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/PadmissClient.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/PadmissClient.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/Reader.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Reader.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/ProfileState.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/ProfileState.h"
)

target_include_directories(${PLUGIN_NAME} PUBLIC
	"${CURL_INCLUDE_DIR}"
	"${CURLPP_INCLUDE_DIR}"
	
	"${CMAKE_CURRENT_SOURCE_DIR}/lib/libusb/include/libusb-1.0"
	"${CMAKE_CURRENT_SOURCE_DIR}/../WebSocket"
)

target_link_libraries(${PLUGIN_NAME}
	WebsocketPlugin
	
	"${CURL_LIBRARY}"
	"${CURLPP_LIBRARY}"
	
	"${CMAKE_CURRENT_SOURCE_DIR}/lib/libusb/lib/libusb-1.0.lib"
)