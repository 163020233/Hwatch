set(MODULE_NAME ${LOSCFG_ARCH_CPU})

set(LITEOS_INT_AHEAD_LIBS ${LOSCFG_ARCH_CPU} targets init CACHE STRING "LiteOS libs place ahead")

# extra definition for other module
set(LITEOS_CPU_TYPE ${LOSCFG_ARCH_CPU} CACHE STRING "CPU_TYPE" FORCE)

if(LOSCFG_ARCH_ARM_SVE2)
    set(ARCH_EXTENSION +sve2)
endif()
set(LITEOS_ARM_ARCH -march=${LOSCFG_ARCH_ARM_VER}${ARCH_EXTENSION} CACHE STRING "ARM_ARCH" FORCE)

# add global include, compile macro and options to public target #
set(MODULE_INCLUDE_PUB)
set(MODULE_CXXINCLUDE_PUB)

set(ARCH_COPTS)
set(ARCH_ASOPTS)
set(ARCH_CMACRO)
set(ARCH_ASMACRO)

set(LITEOS_CPU_OPTS)

# CPU compile options
if(LOSCFG_COMPILER_CLANG)
    list(APPEND LITEOS_CPU_OPTS --target=aarch64-none-linux-elf)
endif()

# AArch64 has no specific FPU compile options like AArch32 does.
set(EXTENSION)
if(NOT LOSCFG_ARCH_FPU_ENABLE)
    set(EXTENSION +nofp)
endif()
list(APPEND LITEOS_CPU_OPTS ${LITEOS_ARM_ARCH}${EXTENSION})
list(APPEND LITEOS_CPU_OPTS -mcpu=${LOSCFG_ARCH_CPU}${EXTENSION})

# linux style macros
set(LINUX_ARCH_${LOSCFG_ARCH_ARM_V8A} __LINUX_ARM_ARCH__=8)
set(AS_OBJS_LIBC_FLAGS ${AS_OBJS_LIBC_FLAGS} ${LINUX_ARCH_y}
    CACHE STRING "liteos asm flags" FORCE)

set(MODULE_COPTS_PUB   ${LITEOS_CPU_OPTS} ${ARCH_COPTS})
set(MODULE_ASOPTS_PUB  ${LITEOS_CPU_OPTS})
set(MODULE_CXXOPTS_PUB ${LITEOS_CPU_OPTS} ${ARCH_COPTS})
set(MODULE_CMACRO_PUB  ${ARCH_CMACRO})
set(MODULE_ASMACRO_PUB ${ARCH_ASMACRO})
set(MODULE_CXXMACRO_PUB)


# add local include, compile macro and options to private target #
set(MODULE_INCLUDE_PRI
    ${LITEOSTOPDIR}/kernel_lite/extended/include
    ${LITEOSTOPDIR}/kernel_lite/extended/dynload/include
)

if(LOSCFG_PERF_HW_PMU)
    list(APPEND MODULE_INCLUDE_PRI ${LITEOSTOPDIR}/kernel_lite/extended/perf)
endif()
set(MODULE_CXXINCLUDE_PRI ${MODULE_INCLUDE_PRI})
set(MODULE_COPTS_PRI ${LITEOS_NON_SECURE_LOCAL_OPTS})
set(MODULE_ASOPTS_PRI ${LITEOS_NON_SECURE_LOCAL_OPTS})
set(MODULE_CXXOPTS_PRI ${LITEOS_NON_SECURE_LOCAL_OPTS})
set(MODULE_CMACRO_PRI)
set(MODULE_ASMACRO_PRI)
set(MODULE_CXXMACRO_PRI)

if(LOSCFG_GDB)
    list(APPEND MODULE_COPTS_PRI ${AS_OBJS_LIBC_FLAGS})
    list(APPEND MODULE_ASOPTS_PRI ${AS_OBJS_LIBC_FLAGS})
    list(APPEND MODULE_CXXOPTS_PRI ${AS_OBJS_LIBC_FLAGS})
endif()


# add srcs to private target #
set(LOCAL_DIRS_y)
set(LOCAL_SRCS_y src/canary.c src/cpu.c
                 src/fault.c src/task.c
)

list(APPEND LOCAL_SRCS_${LOSCFG_LASLR} src/aslr/aslr.c src/aslr/symbol.S)
list(APPEND LOCAL_SRCS_${LOSCFG_ARCH_MMU_ENABLE} src/mmu.c)
list(APPEND LOCAL_SRCS_${LOSCFG_KERNEL_DYNLOAD} src/dynload.c)
list(APPEND LOCAL_SRCS_${LOSCFG_PERF_HW_PMU} src/pmu.c)
list(APPEND LOCAL_SRCS_${LOSCFG_ARCH_RAS} src/ras.c)
list(APPEND LOCAL_SRCS_${LOSCFG_KERNEL_LP} src/lp_patch.c)
list(APPEND LOCAL_SRCS_${LOSCFG_ARCH_LINXCORE_610} src/cache_lx.c)

file(GLOB LOCAL_ASMSRCS_LIST_TEMP RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    "src/*.S"
)

list(FILTER LOCAL_ASMSRCS_LIST_TEMP EXCLUDE REGEX "src/spinlock(_lse)?\.S")
if(${LOSCFG_ARCH_ARM_AARCH64_LSE_ATOMIC})
    list(APPEND LOCAL_ASMSRCS_LIST_TEMP src/spinlock_lse.S)
else()
    list(APPEND LOCAL_ASMSRCS_LIST_TEMP src/spinlock.S)
endif()

list(APPEND LOCAL_SRCS_y ${LOCAL_ASMSRCS_LIST_TEMP})
FOREACH(CUR_DIR ${LOCAL_DIRS_y})
    aux_source_directory(${CUR_DIR} LOCAL_SRCS_y)
ENDFOREACH(CUR_DIR)

set(LOCAL_SRCS ${LOCAL_SRCS_y})


include(${MODULE})  # module.cmake will sort LOCAL_SRCS files
