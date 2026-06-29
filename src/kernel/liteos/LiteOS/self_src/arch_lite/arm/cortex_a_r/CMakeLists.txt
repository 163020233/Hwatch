set(MODULE_NAME ${LOSCFG_ARCH_CPU})

set(LITEOS_INT_AHEAD_LIBS ${LOSCFG_ARCH_CPU} targets init CACHE STRING "LiteOS libs place ahead")

# add global include, compile macro and options to public target #
set(MODULE_INCLUDE_PUB)
set(MODULE_CXXINCLUDE_PUB)

set(ARCH_COPTS)
set(ARCH_ASOPTS)
set(ARCH_CMACRO)
set(ARCH_ASMACRO)

# FPU compile options
if(LOSCFG_ARCH_FPU_ENABLE)
    if(LOSCFG_ARCH_FPU_SOFTFP)
        set(LITEOS_FLOAT_OPTS -mfloat-abi=softfp)
    else()
        set(LITEOS_FLOAT_OPTS -mfloat-abi=hard)
    endif()
    set(LITEOS_FPU_OPTS -mfpu=${LOSCFG_ARCH_FPU})
else()
    set(LITEOS_FLOAT_OPTS -mfloat-abi=soft)
endif()

# CPU compile options
set(LITEOS_CPU_OPTS -mcpu=${LOSCFG_ARCH_CPU})
if(LOSCFG_ARCH_ARM_CORTEX_R AND LOSCFG_COMPILER_CLANG)
    list(APPEND LITEOS_CPU_OPTS --target=arm-none-eabi)
endif()

# gcc libc folder
if(LOSCFG_ARCH_ARM_V5)
    set(LITEOS_GCCLIB_TEMP arm/v5te/softfp)
else()
    # for example, cortex-a7 with softfp abi and neon vfp4 is: a7_softfp_neon-vfp4
    set(LITEOS_GCCLIB_TEMP ${LOSCFG_ARCH_CPU}_softfp_${LOSCFG_ARCH_FPU})
    string(REGEX REPLACE "cortex-" "" LITEOS_GCCLIB_TEMP ${LITEOS_GCCLIB_TEMP})
endif()
if(NOT EXISTS ${GCC_GCCLIB_PATH}/${LITEOS_GCCLIB_TEMP})
    set(LITEOS_GCCLIB_TEMP)
endif()
set(LITEOS_GCCLIB ${LITEOS_GCCLIB_TEMP} CACHE STRING "LITEOS_GCCLIB" FORCE)

list(APPEND ARCH_COPTS -mno-unaligned-access)
if(NOT LOSCFG_COMPILER_CLANG)
    list(APPEND ARCH_COPTS -mthumb-interwork)
endif()


# linux style macros
set(LINUX_ARCH_${LOSCFG_ARCH_ARM_V5}  __LINUX_ARM_ARCH__=5)
set(LINUX_ARCH_${LOSCFG_ARCH_ARM_V7A} __LINUX_ARM_ARCH__=7)
set(LINUX_ARCH_${LOSCFG_ARCH_ARM_V7R} __LINUX_ARM_ARCH__=7)
set(LINUX_ARCH_${LOSCFG_ARCH_ARM_V8A} __LINUX_ARM_ARCH__=8)
set(LINUX_ARCH_${LOSCFG_ARCH_ARM_V8R} __LINUX_ARM_ARCH__=8)
set(AS_OBJS_LIBC_FLAGS ${AS_OBJS_LIBC_FLAGS} ${LINUX_ARCH_y}
    CACHE STRING "liteos asm flags" FORCE)

set(MODULE_COPTS_PUB   ${LITEOS_CPU_OPTS} ${LITEOS_FLOAT_OPTS} ${LITEOS_FPU_OPTS} ${ARCH_COPTS})
set(MODULE_ASOPTS_PUB  ${LITEOS_CPU_OPTS})
set(MODULE_CXXOPTS_PUB ${LITEOS_CPU_OPTS} ${LITEOS_FLOAT_OPTS} ${LITEOS_FPU_OPTS} ${ARCH_COPTS})
set(MODULE_CMACRO_PUB  ${ARCH_CMACRO})
set(MODULE_ASMACRO_PUB ${ARCH_ASMACRO})
set(MODULE_CXXMACRO_PUB)


# add local include, compile macro and options to private target #
set(MODULE_INCLUDE_PRI
    ${LITEOSTOPDIR}/kernel_lite/extended/include
    ${LITEOSTOPDIR}/kernel_lite/extended/dynload/include
    ${LITEOSTOPDIR}/arch_lite/arm/cortex_a_r/include
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
set(LOCAL_SRCS_y src/canary.c src/cpu.c
                 src/fault.c src/task.c
)
file(GLOB LOCAL_ASMSRCS_LIST_TEMP RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    "src/*.S"
)
list(APPEND LOCAL_SRCS_y ${LOCAL_ASMSRCS_LIST_TEMP})

list(APPEND LOCAL_SRCS_${LOSCFG_KERNEL_DYNLOAD} src/dynload.c)

list(APPEND LOCAL_SRCS_${LOSCFG_LASLR} src/aslr/aslr.c src/aslr/symbol.S)

if("${LITEOS_ARM_ARCH}" STREQUAL "-march=armv7-a")
    file(GLOB LOCAL_ASMSRCS_LIST_TEMP RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        "src/armv7/*.S"
    )
    list(APPEND LOCAL_SRCS_y ${LOCAL_ASMSRCS_LIST_TEMP})
elseif("${LITEOS_ARM_ARCH}" STREQUAL "-march=armv7-r" OR
       "${LITEOS_ARM_ARCH}" STREQUAL "-march=armv8-r")
    list(APPEND LOCAL_SRCS_y src/armv7/cache.S)
endif()

list(APPEND LOCAL_SRCS_${LOSCFG_APC_ENABLE} src/mmu.c)

list(APPEND LOCAL_SRCS_${LOSCFG_PERF_HW_PMU} src/pmu.c)

if(NOT LOSCFG_KERNEL_LOWPOWER)
list(REMOVE_ITEM LOCAL_SRCS_y "src/runstop.S")
endif()

if(LOSCFG_ARCH_ARM_WITH_THUMB_INSTRUCTION)
    list(APPEND LOCAL_SRCS_y ${LITEOSTOPDIR}/arch_lite/arm/common/thumb_backtrace.c)
    list(APPEND MODULE_COPTS_PUB -mthumb)
endif()

set(LOCAL_SRCS ${LOCAL_SRCS_y})


include(${MODULE})
