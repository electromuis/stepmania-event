set(CPPNETLIB_SRC
   "cpp-netlib/libs/network/src/client.cpp"
)

set(CPPNETLIB_HPP
    "cpp-netlib/include/boost/network.hpp"
)

source_group("" FILES ${CPPNETLIB_SRC})
source_group("" FILES ${CPPNETLIB_HPP})

add_library("cppnetlib" STATIC ${CPPNETLIB_SRC} ${CPPNETLIB_HPP})

set_property(TARGET "cppnetlib" PROPERTY FOLDER "External Libraries")

if(MSVC)
  sm_add_compile_definition("cppnetlib" _CRT_SECURE_NO_WARNINGS)
endif()

disable_project_warnings("cppnetlib")

target_include_directories("cppnetlib" PUBLIC "cpp-netlib/include")