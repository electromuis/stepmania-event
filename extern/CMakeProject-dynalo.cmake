set(DYNALO_DIR "${SM_EXTERN_DIR}/dynalo")
set(DYNALO_INCLUDE_DIRS "${DYNALO_DIR}/include" PARENT_SCOPE)

list(APPEND LIBUV_HPP
		  "${DYNALO_DIR}/include/dynalo/dynalo.hpp")

source_group("Header Files" FILES ${DYNALO_HPP})