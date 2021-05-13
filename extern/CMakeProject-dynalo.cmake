set(DYNALO_DIR "${SM_EXTERN_DIR}/dynalo")

list(APPEND LIBUV_HPP
		  "${DYNALO_DIR}/include/dynalo/dynalo.hpp")

source_group("Header Files" FILES ${DYNALO_HPP})


include_directories("${DYNALO_DIR}/include")