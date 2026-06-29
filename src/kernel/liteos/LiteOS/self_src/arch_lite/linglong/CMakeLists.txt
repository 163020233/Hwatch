set(MODULE_NAME ${LOSCFG_ARCH_CPU})

# FPU compile options
if(LOSCFG_ARCH_FPU_HARD)
    set(LITEOS_FLOAT_OPTS -mfloat-abi=hard)
endif()

if(LOSCFG_KERNEL_MIN)
set(LITEOS_INT_AHEAD_LIBS targets init CACHE STRING "LiteOS libs place ahead")
else()
set(LITEOS_INT_AHEAD_LIBS targets CACHE STRING "LiteOS libs place ahead")
endif()

# add global include, compile macro and options to public target #
set(MODULE_INCLUDE_PUB)
set(MODULE_CXXINCLUDE_PUB)
set(MODULE_CXXOPTS_PUB)
set(MODULE_CXXMACRO_PUB)

set(LITEOS_CPU_OPTS --target=${LOSCFG_ARCH_CPU})

set(ARCH_C_OPT)
list(APPEND ARCH_C_OPT -mllvm -no-stack-coloring=false)         # optimize stack size
list(APPEND ARCH_C_OPT -fno-short-wchar)                        # wchar_t compatible for musl

set(MODULE_COPTS_PUB   ${LITEOS_CPU_OPTS} ${LITEOS_FLOAT_OPTS} ${ARCH_C_OPT})
set(MODULE_ASOPTS_PUB  ${LITEOS_CPU_OPTS})
set(MODULE_CXXOPTS_PUB ${LITEOS_CPU_OPTS} ${LITEOS_FLOAT_OPTS})
set(MODULE_CMACRO_PUB)
set(MODULE_ASMACRO_PUB)
set(MODULE_CXXMACRO_PUB)

set(GSYM_CFLAG ${GSYM_CFLAG} ${LITEOS_CPU_OPTS} CACHE INTERNAL "CSYM Add Arch Option" FORCE)

# add local include, compile macro and options to private target #
set(MODULE_INCLUDE_PRI
    ${LITEOSTOPDIR}/kernel_lite/extended/include
    ${LITEOSTOPDIR}/kernel_lite/extended/dynload/include
    ${LITEOSTOPDIR}/kernel_lite/extended/perf
)

set(MODULE_CXXINCLUDE_PRI ${MODULE_INCLUDE_PRI})

set(MODULE_COPTS_PRI ${LITEOS_NON_SECURE_LOCAL_OPTS})
set(MODULE_ASOPTS_PRI ${LITEOS_NON_SECURE_LOCAL_OPTS})
set(MODULE_CXXOPTS_PRI ${LITEOS_NON_SECURE_LOCAL_OPTS})
set(MODULE_CMACRO_PRI)
set(MODULE_ASMACRO_PRI)
set(MODULE_CXXMACRO_PRI)

# add srcs to private target #
list(APPEND LOCAL_SRCS_y
    src/cpu.c
    src/fault.c
    src/task.c
    src/atomic.c
    src/canary.c
)

list(APPEND LOCAL_SRCS_${LOSCFG_PERF_HW_PMU} src/pmu.c)
list(APPEND LOCAL_ASMSRCS_LIST_TEMP_y
    "src/dispatch.S"
    "src/exception.S"
    "src/reset_vector.S"
    "src/vector.S"
)
if(LOSCFG_LASLR)
    list(APPEND LOCAL_ASMSRCS_LIST_TEMP_y
        "aslr/aslr.c"
        "aslr/aslr_vector.S"
        "aslr/symbol.S"
    )
endif()



list(APPEND LOCAL_ASMSRCS_LIST_TEMP_${LOSCFG_KERNEL_SMP}      "src/spinlock.S")
list(APPEND LOCAL_ASMSRCS_LIST_TEMP_${LOSCFG_KERNEL_LOWPOWER} "src/deepsleep.S")
list(APPEND LOCAL_SRCS_${LOSCFG_KERNEL_DYNLOAD}               "src/dynload.c")

list(APPEND LOCAL_SRCS_y ${LOCAL_ASMSRCS_LIST_TEMP_y})

set(LOCAL_SRCS ${LOCAL_SRCS_y})  # module.cmake will sort LOCAL_SRCS files
include(${MODULE})
