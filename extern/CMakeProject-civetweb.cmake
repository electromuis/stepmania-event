list(APPEND CIVETWEB_SRC
  "civetweb/src/CivetServer.cpp"
  "civetweb/src/civetweb.c"
)

list(APPEND CIVETWEB_HPP
  "civetweb/include/CivetServer.h"
  "civetweb/include/civetweb.h"
)

source_group("" FILES ${CIVETWEB_SRC} ${CIVETWEB_HPP})

add_library("civetweb" STATIC ${CIVETWEB_SRC} ${CIVETWEB_HPP})

set_property(TARGET "civetweb" PROPERTY FOLDER "External Libraries")

if(MSVC)
    sm_add_compile_definition("civetweb" _CRT_SECURE_NO_WARNINGS)
endif()

disable_project_warnings("civetweb")

target_include_directories("civetweb" PUBLIC "civetweb/include")

