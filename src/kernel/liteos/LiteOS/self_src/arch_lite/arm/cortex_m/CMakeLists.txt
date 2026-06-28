set(MODULE_NAME ${LOSCFG_ARCH_CPU})

if(LOSCFG_KERNEL_MIN)
set(LITEOS_INT_AHEAD_LIBS ${LOSCFG_ARCH_CPU} targets init CACHE STRING "LiteOS libs place ahead")
else()
set(LITEOS_INT_AHEAD_LIBS ${LOSCFG_ARCH_CPU} targets CACHE STRING "LiteOS libs place ahead")
endif()

# add global include, compile macro and options to public target #
set(MODULE_INCLUDE_PUB)
set(MODULE_CXXINCLUDE_PUB)

set(ARCH_COPTS)
set(ARCH_ASOPTS)
set(ARCH_CMACRO)
set(ARCH_ASMACRO)

# FPU compile options: hard/soft/softfp, use hard as default
if(LOSCFG_ARCH_CORTEX_M0)
# cortex-m0 don't support fpu
    set(EXTENSION +nofp)
elseif(LOSCFG_ARCH_CORTEX_M3)
    set(LITEOS_FLOAT_OPTS -mfloat-abi=softfp)
else()
    if(LOSCFG_ARCH_FPU_ENABLE)
        if(LOSCFG_ARCH_FPU_SOFTFP)
            set(LITEOS_FLOAT_OPTS -mfloat-abi=softfp)
        else()
            set(LITEOS_FLOAT_OPTS -mfloat-abi=hard)
        endif()
        set(LITEOS_FPU_OPTS -mfpu=${LOSCFG_ARCH_FPU})
    else()
        set(EXTENSION +nofp)
        set(LITEOS_FLOAT_OPTS -mfloat-abi=soft)
    endif()
endif()

# CPU compile options
set(LITEOS_CPU_OPTS -mcpu=${LOSCFG_ARCH_CPU}${EXTENSION})
if(LOSCFG_TRUSTZONE)
    list(APPEND LITEOS_CPU_OPTS -mcmse)
endif()

# gcc libc folder style is combine with core and fpu, use thumb as default
# for example, cortex-m7 with softfp abi and thumb is: thumb/v7e-m+fp/softfp
# attention: for v6e, can use thumb/v6-m/nofp/libgcc.a
if(LOSCFG_ARCH_CORTEX_M0)
    set(LITEOS_GCCLIB_TEMP thumb/v6-m/nofp)
elseif(LOSCFG_ARCH_CORTEX_M55)
    set(LITEOS_GCCLIB_TEMP thumb/v8.1-m.main+mve/hard)
else()
    if(LOSCFG_ARCH_FPU_ENABLE)
        set(LITEOS_GCCLIB_TEMP thumb/v7e-m+fp/${LITEOS_FLOAT_OPTS})
        string(REGEX REPLACE "-mfloat-abi=" "" LITEOS_GCCLIB_TEMP ${LITEOS_GCCLIB_TEMP})
    else()
        set(LITEOS_GCCLIB_TEMP thumb/v7-m/nofp)
    endif()
endif()
set(LITEOS_GCCLIB ${LITEOS_GCCLIB_TEMP} CACHE STRING "LITEOS_GCCLIB" FORCE)

# linux style macros
set(LINUX_ARCH_${LOSCFG_ARCH_ARM_V6M} __LINUX_ARM_ARCH__=6)
set(LINUX_ARCH_${LOSCFG_ARCH_ARM_V7M} __LINUX_ARM_ARCH__=7)
set(LINUX_ARCH_${LOSCFG_ARCH_ARM_V8M} __LINUX_ARM_ARCH__=8)
set(AS_OBJS_LIBC_FLAGS ${AS_OBJS_LIBC_FLAGS} ${LINUX_ARCH_y}
    CACHE STRING "liteos asm flags" FORCE)

set(MODULE_COPTS_PUB   ${LITEOS_CPU_OPTS} ${LITEOS_FLOAT_OPTS} ${LITEOS_FPU_OPTS} ${ARCH_COPTS})
set(MODULE_ASOPTS_PUB  ${LITEOS_CPU_OPTS})
set(MODULE_CXXOPTS_PUB ${MODULE_COPTS_PUB})
set(MODULE_CMACRO_PUB  ${ARCH_CMACRO})
set(MODULE_ASMACRO_PUB ${ARCH_ASMACRO})
set(MODULE_CXXMACRO_PUB)

set(GSYM_CFLAG ${GSYM_CFLAG}${MODULE_COPTS_PUB} CACHE INTERNAL "CSYM Add Arch Option" FORCE)

# add local include, compile macro and options to private target #
set(MODULE_INCLUDE_PRI
    ${LITEOSTOPDIR}/kernel_lite/base/include
    ${LITEOSTOPDIR}/kernel_lite/extended/include
    ${LITEOSTOPDIR}/kernel_lite/extended/dynload/include
)
set(MODULE_CXXINCLUDE_PRI ${MODULE_INCLUDE_PRI})

set(MODULE_COPTS_PRI ${LITEOS_NON_SECURE_LOCAL_OPTS})
set(MODULE_ASOPTS_PRI ${LITEOS_NON_SECURE_LOCAL_OPTS})
set(MODULE_CXXOPTS_PRI ${LITEOS_NON_SECURE_LOCAL_OPTS})
set(MODULE_CMACRO_PRI)
set(MODULE_ASMACRO_PRI)
set(MODULE_CXXMACRO_PRI)

if(LOSCFG_PERF_HW_PMU)
    list(APPEND MODULE_INCLUDE_PRI ${LITEOSTOPDIR}/kernel_lite/extended/perf)
endif()

if(LOSCFG_GDB)
    list(APPEND MODULE_COPTS_PRI ${AS_OBJS_LIBC_FLAGS})
    list(APPEND MODULE_ASOPTS_PRI ${AS_OBJS_LIBC_FLAGS})
    list(APPEND MODULE_CXXOPTS_PRI ${AS_OBJS_LIBC_FLAGS})
endif()

# add srcs to private target #
if(LOSCFG_SECURE_REGION)
    file(GLOB LOCAL_SRCS_LIST_TEMP RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/secure/*.c" )
    list(APPEND LOCAL_SRCS_y ${LOCAL_SRCS_LIST_TEMP})

    file(GLOB LOCAL_ASMSRCS_LIST_TEMP RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/secure/*.S" )
    list(APPEND LOCAL_SRCS_y ${LOCAL_ASMSRCS_LIST_TEMP})
    list(APPEND LOCAL_SRCS_y src/canary.c)
else()
    set(LOCAL_DIRS_y src)
    file(GLOB LOCAL_ASMSRCS_LIST_TEMP RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.S" )
    list(APPEND LOCAL_SRCS_y ${LOCAL_ASMSRCS_LIST_TEMP})

    FOREACH(CUR_DIR ${LOCAL_DIRS_y})
        aux_source_directory(${CUR_DIR} LOCAL_SRCS_y)
    ENDFOREACH(CUR_DIR)

    if(NOT LOSCFG_KERNEL_LOWPOWER)
        list(REMOVE_ITEM LOCAL_SRCS_y src/intermit.S)
    endif()
    if(NOT LOSCFG_FPB_ENABLE)
        list(REMOVE_ITEM LOCAL_SRCS_y src/fpb.c)
    endif()
    if(NOT LOSCFG_APC_ENABLE)
        list(REMOVE_ITEM LOCAL_SRCS_y src/mpu.c)
    endif()
    if(NOT LOSCFG_PERF_HW_PMU)
        list(REMOVE_ITEM LOCAL_SRCS_y src/pmu.c)
    endif()
endif()

list(APPEND LOCAL_SRCS_y ${LITEOSTOPDIR}/arch_lite/arm/common/thumb_backtrace.c)

set(LOCAL_SRCS ${LOCAL_SRCS_y})

include(${MODULE})
