set(MODULE_NAME oh_mini)

set(LITEOS_EXT_AHEAD_LIBS oh_mini targets init CACHE STRING "LiteOS libs place ahead")

# add global include, compile macro and options to public target #
set(MODULE_INCLUDE_PUB)
set(MODULE_CXXINCLUDE_PUB)
set(MODULE_COPTS_PUB)
set(MODULE_ASOPTS_PUB)
set(MODULE_CXXOPTS_PUB)
set(MODULE_CMACRO_PUB)
set(MODULE_ASMACRO_PUB)
set(MODULE_CXXMACRO_PUB)

set(MODULE_COPTS_PRI ${LITEOS_NON_SECURE_LOCAL_OPTS})
set(MODULE_ASOPTS_PRI ${LITEOS_NON_SECURE_LOCAL_OPTS})
set(MODULE_CXXOPTS_PRI ${LITEOS_NON_SECURE_LOCAL_OPTS})
set(MODULE_CMACRO_PRI)
set(MODULE_ASMACRO_PRI)
set(MODULE_CXXMACRO_PRI)

# add local include, compile macro and options to private target #
set(MODULE_INCLUDE_PRI
    ${LITEOSTOPDIR}/compat
    ${LITEOSTOPDIR}/lib/liteos_libc/pthread
    ${LITEOSTOPDIR}/lib/liteos_libc/mq
    ${LITEOSTOPDIR}/lib/liteos_libc/errno
    ${LITEOSTOPDIR}/lib/liteos_libc/tzdst
    ${LITEOSTOPDIR}/lib/liteos_libc/time
    ${LITEOSTOPDIR}/lib/liteos_libc/lock
    ${LITEOS_LIBC_PATH}/src/include
    ${LITEOS_LIBC_PATH}/src/internal
    ${LITEOS_LIBC_PATH}/src/time
)

list(APPEND MODULE_INCLUDE_PUB
    ${LITEOSTOPDIR}/compat/osal/oh_mini/include
)

# kernel_s is LOSCFG_MEM_BESTFIT_LITTLE, kernel is LOSCFG_KERNEL_MEM_BESTFIT_LITTLE
if(LOSCFG_MEM_BESTFIT_LITTLE OR LOSCFG_KERNEL_MEM_BESTFIT_LITTLE)
    list(APPEND MODULE_INCLUDE_PUB ${LITEOSTOPDIR}/${KERNEL_PATH}/base/mem/bestfit_little)
else()
    list(APPEND MODULE_INCLUDE_PUB ${LITEOSTOPDIR}/${KERNEL_PATH}/base/mem/bestfit)
endif()

set(C_DEFS CONFIG_OH_MINI)
list(APPEND C_DEFS CONFIG_PRIVATE_DIRENT_STRUCT)
if(LOSCFG_PLATFORM_SW21_APP_SE OR LOSCFG_PLATFORM_SW22_APP_SE)
    list(APPEND C_DEFS SUPPORT_MBR_MULTIPLE_PART)
endif()

if(LOSCFG_PLATFORM_SW21_APP OR LOSCFG_PLATFORM_SW22_APP)
    list(APPEND C_DEFS SUPPORT_MBR_MULTIPLE_PART)
endif()

if(LOSCFG_PLATFORM_SH_AN)
    list(APPEND C_DEFS SUPPORT_SH_AN_PART)
endif()

list(APPEND MODULE_CMACRO_PRI ${C_DEFS})

set(LOCAL_SRCS_y
    src/stub.c
    src/shb_exc_info.c
    src/shb_task_switch.c
    src/fs_remove.c
    src/adapt_time.c
)

if(LOSCFG_FS_FAT)
list(APPEND LOCAL_SRCS_y src/disk_partition.c)
list(APPEND MODULE_INCLUDE_PUB ${LITEOS_OPENSOURCE_PATHNAME}/FatFs/source)
endif()

set(LOCAL_SRCS ${LOCAL_SRCS_y})  # module.cmake will sort LOCAL_SRCS files
include(${UMODULE})