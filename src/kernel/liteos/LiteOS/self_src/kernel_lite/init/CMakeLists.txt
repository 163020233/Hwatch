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
    ${LITEOSTOPDIR}/kernel_lite/extended/include
    ${LITEOSTOPDIR}/kernel_lite/extended/dynload/include
)

if(LOSCFG_KERNEL_MEM_BESTFIT_LITTLE)
    list(APPEND MODULE_INCLUDE_PRI ${LITEOSTOPDIR}/kernel_lite/base/mem/bestfit_little)
endif()

if(LOSCFG_KERNEL_MEM_BESTFIT)
    list(APPEND MODULE_INCLUDE_PRI ${LITEOSTOPDIR}/kernel_lite/base/mem/bestfit)
endif()

if(LOSCFG_TEST)
    list(APPEND MODULE_INCLUDE_PRI ${LITEOSTOPDIR}/test/include)
endif()
set(MODULE_COPTS_PRI ${LITEOS_NON_SECURE_LOCAL_OPTS})
set(MODULE_ASOPTS_PRI ${LITEOS_NON_SECURE_LOCAL_OPTS})
set(MODULE_CXXOPTS_PRI ${LITEOS_NON_SECURE_LOCAL_OPTS})
set(MODULE_CMACRO_PRI)
set(MODULE_ASMACRO_PRI)
set(MODULE_CXXMACRO_PRI)


# add srcs to private target #
set(LOCAL_SRCS_y)
list(APPEND "LOCAL_SRCS_${LOSCFG_KERNEL_MIN}" los_init.c)
if(LOSCFG_LASLR AND NOT LOSCFG_LASLR_EXTERNAL_RELOC)
    list(APPEND "LOCAL_SRCS_y" los_aslr.c)
endif()

set(LOCAL_SRCS ${LOCAL_SRCS_y})  # module.cmake will sort LOCAL_SRCS files
include(${MODULE})
