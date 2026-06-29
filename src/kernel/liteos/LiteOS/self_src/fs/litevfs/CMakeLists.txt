STRING( REGEX REPLACE ".*/(.*)" "\\1" CURRENT_FOLDER ${CMAKE_CURRENT_SOURCE_DIR} )
set(MODULE_NAME ${CURRENT_FOLDER})

set(MODULE_COPTS_PRI)
set(MODULE_ASOPTS_PRI)
set(MODULE_CXXOPTS_PRI)
set(MODULE_CMACRO_PRI)
set(MODULE_ASMACRO_PRI)
set(MODULE_CXXMACRO_PRI)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ../../../self_src/osal/kal/include
    ../../../self_src/include/uapi/posix)

# add srcs to private target
list(APPEND LOCAL_SRCS_y litevfs_vfs.c)
set(LOCAL_SRCS ${LOCAL_SRCS_y})
include(${MODULE})