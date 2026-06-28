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
    ${LITEOSTOPDIR}/kernel_lite/extended/perf
)
set(MODULE_COPTS_PRI ${LITEOS_NON_SECURE_LOCAL_OPTS})
set(MODULE_ASOPTS_PRI ${LITEOS_NON_SECURE_LOCAL_OPTS})
set(MODULE_CXXOPTS_PRI ${LITEOS_NON_SECURE_LOCAL_OPTS})
set(MODULE_CMACRO_PRI)
set(MODULE_ASMACRO_PRI)
set(MODULE_CXXMACRO_PRI)


# add srcs to private target #
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} LOCAL_SRCS_y)
list(APPEND LOCAL_SRCS_${LOSCFG_PERF_HW_PMU} pmu/perf_hw_pmu.c)
list(APPEND LOCAL_SRCS_${LOSCFG_PERF_HW_PMU} pmu/perf_hw_shellcmd.c)
list(APPEND LOCAL_SRCS_${LOSCFG_PERF_TIMED_PMU} pmu/perf_timed_pmu.c)
list(APPEND LOCAL_SRCS_${LOSCFG_PERF_SW_PMU} pmu/perf_sw_pmu.c)

set(LOCAL_SRCS ${LOCAL_SRCS_y})  # module.cmake will sort LOCAL_SRCS files
include(${MODULE})
