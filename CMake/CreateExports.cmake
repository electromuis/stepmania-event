set(BUILD_DIR "${PROJECT_BINARY_DIR}/src/${SM}.dir/${CMAKE_BUILD_TYPE}")
set(DEF_FILE "${BUILD_DIR}/exports.def")
set(OBJS_FILE "${BUILD_DIR}/objects2.txt")

file(GLOB OBJ_FILES ${BUILD_DIR}/*.obj)
foreach(LINE IN LISTS OBJ_FILES)
	file(APPEND ${OBJS_FILE} "${LINE}\n")
endforeach()
  
execute_process(
	COMMAND ${CMAKE_COMMAND}
		-E __create_def "${DEF_FILE}"
		"${OBJS_FILE}"
		
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	OUTPUT_VARIABLE cmd_out
)

set(pattern "==")

# create list of lines form the contens of a file
file (STRINGS ${DEF_FILE} LINES)

# overwrite the file....
file(WRITE ${DEF_FILE} "")

# loop through the lines,
# remove unwanted parts 
# and write the (changed) line ...
foreach(LINE IN LISTS LINES)
	string(REGEX REPLACE ${pattern} "" STRIPPED "${LINE}")
	file(APPEND ${DEF_FILE} "${STRIPPED}\n")
endforeach()