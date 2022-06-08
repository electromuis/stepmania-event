set(BUILD_DIR "${PROJECT_BINARY_DIR}/src/${SM}.dir/${CMAKE_BUILD_TYPE}")
set(OBJS_FILE "${BUILD_DIR}/objects.txt")

set(pattern "")
set(BUG_FOUND 0)

file (STRINGS ${OBJS_FILE} LINES)
foreach(LINE IN LISTS LINES)
	if("${LINE}" MATCHES "InputHandler_DirectInputHelper")
		set(BUG_FOUND 1)
		break()
	endif()
endforeach()



if(BUG_FOUND)
	message("Removing bugged object for export definition")
	file(WRITE ${OBJS_FILE} "")
	
	foreach(LINE IN LISTS LINES)
		if(NOT "${LINE}" MATCHES "InputHandler_DirectInputHelper")
			file(APPEND ${OBJS_FILE} "${LINE}\n")
		endif()
	endforeach()
endif()