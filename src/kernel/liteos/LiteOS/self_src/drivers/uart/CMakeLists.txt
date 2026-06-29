STRING( REGEX REPLACE ".*/(.*)" "\\1" CURRENT_FOLDER ${CMAKE_CURRENT_SOURCE_DIR} )
set(MODULE_NAME ${CURRENT_FOLDER})

if(NOT LOSCFG_DRIVERS_UART_VENDOR)
    if(LOSCFG_DRIVERS_UART_ARM_PL011)
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/arm_pl011)
    endif()
    if(LOSCFG_DRIVERS_UART_HQ_UART)
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/hq_uart_v5xx)
    endif()
    if(LOSCFG_DRIVERS_UART_VUART_RPQ)
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vuart_rpq)
    endif()
endif()

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
set(MODULE_INCLUDE_PRI)
set(MODULE_COPTS_PRI ${LITEOS_NON_SECURE_LOCAL_OPTS})
set(MODULE_ASOPTS_PRI ${LITEOS_NON_SECURE_LOCAL_OPTS})
set(MODULE_CXXOPTS_PRI ${LITEOS_NON_SECURE_LOCAL_OPTS})
set(MODULE_CMACRO_PRI)
set(MODULE_ASMACRO_PRI)
set(MODULE_CXXMACRO_PRI)


# add srcs to private target #
list(APPEND LOCAL_SRCS_y uart_core.c)
if(LOSCFG_DRIVERS_UART)
    list(APPEND LOCAL_SRCS_y uart_ops.c)
    list(APPEND LOCAL_SRCS_${LOSCFG_FS_VFS} uart_vfs.c)
endif()

set(LOCAL_SRCS ${LOCAL_SRCS_y})
include(${MODULE})
