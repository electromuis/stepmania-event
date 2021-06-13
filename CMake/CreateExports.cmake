set(DEF_FILE "${CMAKE_CURRENT_BINARY_DIR}/src/StepMania.dir/Debug/exports.def")

execute_process(COMMAND ${CMAKE_COMMAND}  -E __create_def "${DEF_FILE}" "C:/dev/stepmania/Build/src/StepMania.dir/Debug//objects.txt"
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  OUTPUT_VARIABLE cmd_out)
  
message(STATUS "${cmd_out}")

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