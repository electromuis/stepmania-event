set(PLUGIN_NAME "PadmissPlugin")
REGISTER_PLUGIN(${CMAKE_CURRENT_SOURCE_DIR} ${PLUGIN_NAME})

target_sources(${PLUGIN_NAME} PRIVATE
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


FIND_PACKAGE("LibCurl" REQUIRED)
FIND_PACKAGE("CurlPP" REQUIRED)
FIND_PACKAGE("LibUsb" REQUIRED)

target_include_directories(${PLUGIN_NAME} PUBLIC
	"${PLUGINS_DIR}/WebSocketPlugin"
)

target_link_libraries(${PLUGIN_NAME}
	tomcrypt
	tommath
	WebSocketPlugin
	
	libcurl
	libcurlpp
	libusb
)
