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
set(MODULE_INCLUDE_PRI ${LITEOSTOPDIR}/kernel_lite/extended/hpl/include)

set(MODULE_COPTS_PRI ${LITEOS_NON_SECURE_LOCAL_OPTS})
set(MODULE_ASOPTS_PRI ${LITEOS_NON_SECURE_LOCAL_OPTS})
set(MODULE_CXXOPTS_PRI ${LITEOS_NON_SECURE_LOCAL_OPTS})
set(MODULE_CMACRO_PRI)
set(MODULE_ASMACRO_PRI)
set(MODULE_CXXMACRO_PRI)

# add srcs to private target #
set(LOCAL_SRCS_y src/los_hpl_logger_common.c)

if (LOSCFG_KERNEL_HPL_TASK_LOCK_FREE)
    list(APPEND LOCAL_SRCS_y
         src/task_lock_free/los_hpl_ringbuf.c
         src/task_lock_free/los_hpl_logger.c
         src/task_lock_free/los_hpl_compression.c)
else()
    list(APPEND LOCAL_SRCS_y src/task_locked/los_hpl_logger.c)
endif()

list(APPEND LOCAL_SRCS_${LOSCFG_DEBUG_HPL_EMULATION} src/los_hpl_decompression.c src/los_hpl_emulation.c)

if (NOT LOSCFG_KERNEL_HPL_SCRIPT_PREPROCESSOR)
    list(APPEND LOCAL_SRCS_y src/los_hpl_c_preprocessor.c)
endif()

set(LOCAL_SRCS ${LOCAL_SRCS_y})  # module.cmake will sort LOCAL_SRCS files
add_subdirectory(mkDecompressor)
include(${MODULE})
