IF(WIN32)
	include(FetchContent)
	
	SET(EXT_PATH "${CMAKE_BINARY_DIR}/Plugins/Modules/LibUsb")
	
	FetchContent_Declare(LibUsbDL
	  URL https://github.com/libusb/libusb/releases/download/v1.0.25/libusb-1.0.25.7z
	  #SHA256_HASH 3D1C98416F454026034B2B5D67F8A294053898CB70A8B489874E75B136C6674D
	  SOURCE_DIR "${EXT_PATH}"
	)
	
	FetchContent_Populate(LibUsbDL)
	
	add_library (libusb STATIC IMPORTED GLOBAL)
	
	# Cmake will complain if the folder doesnt exist yet
	file(MAKE_DIRECTORY "${EXT_PATH}/include")
	set_target_properties (libusb PROPERTIES
		INTERFACE_INCLUDE_DIRECTORIES "${EXT_PATH}/include/libusb-1.0"
		
		IMPORTED_LOCATION_DEBUG "${EXT_PATH}/VS2019/MS64/Debug/lib/libusb-1.0.lib"
		IMPORTED_LOCATION_RELWITHDEBINFO "${EXT_PATH}/VS2019/MS64/Debug/lib/libusb-1.0.lib"
		
		IMPORTED_LOCATION_RELEASE "${EXT_PATH}/VS2019/MS64/Release/lib/libusb-1.0.lib"
		IMPORTED_LOCATION_MINSIZEREL "${EXT_PATH}/VS2019/MS64/Release/lib/libusb-1.0.lib"
	)
	
	
ELSE()
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(LIBUSB REQUIRED libusb-1.0)
ENDIF()
