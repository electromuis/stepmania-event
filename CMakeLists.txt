cmake_minimum_required(VERSION 3.4)

project(StepMania)

include(StepmaniaCore.cmake)

# The external libraries need to be included.
add_subdirectory(extern)

# The internal libraries and eventual executable to be used.
add_subdirectory(src)

# Optional plugins people might want to compile
add_subdirectory(Plugins)