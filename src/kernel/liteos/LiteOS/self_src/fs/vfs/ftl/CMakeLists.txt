STRING( REGEX REPLACE ".*/(.*)" "\\1" CURRENT_FOLDER ${CMAKE_CURRENT_SOURCE_DIR} )
set(MODULE_NAME ${CURRENT_FOLDER})

# add global include, compile macro and options to public target #
set(MODULE_INCLUDE_PUB)
set(MODULE_CXXINCLUDE_PUB)
set(MODULE_COPTS_PUB)
set(MODULE_ASOPTS_PUB)
set(MODULE_CXXOPTS_PUB)
set(MODULE_CMACRO_PUB)
set(MODULE_ASMACRO_PUB)
set(MODULE_CXXMACRO_PUB)

# add local include, compile macro and options to private target #
set(MODULE_INCLUDE_PRI
    ${LITEOSTOPDIR}/fs/vfs/include/ftl
)

set(MODULE_COPTS_PRI ${LITEOS_NON_SECURE_LOCAL_OPTS})
set(MODULE_ASOPTS_PRI ${LITEOS_NON_SECURE_LOCAL_OPTS})
set(MODULE_CXXOPTS_PRI ${LITEOS_NON_SECURE_LOCAL_OPTS})
set(MODULE_CMACRO_PRI)
set(MODULE_ASMACRO_PRI)
set(MODULE_CXXMACRO_PRI)

list(APPEND MODULE_COPTS_PRI -fno-strict-aliasing)
list(APPEND MODULE_CXXOPTS_PRI -fno-strict-aliasing)

# add srcs to private target #
set(LOCAL_SRCS_y src/fblock.c src/ftl.c)

if (LOSCFG_FS_FAT_FTL_COMPRESS)
    list(APPEND LOCAL_SRCS_y src/ftl_compress.c)
endif()

set(LOCAL_SRCS ${LOCAL_SRCS_y})  # module.cmake will sort LOCAL_SRCS files
include(${MODULE})
