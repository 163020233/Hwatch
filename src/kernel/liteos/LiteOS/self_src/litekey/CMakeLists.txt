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
set(MODULE_CMACRO_PRI)
set(MODULE_ASMACRO_PRI)
set(MODULE_CXXMACRO_PRI)

# add srcs to private target #
set(LOCAL_SRCS_y)

macro(add_if_exist_without_cond file_base)
    if(EXISTS "${file_base}")
        list(APPEND LOCAL_SRCS_y "${file_base}")
    else()
        if(NOT _keysrc_included_${CMAKE_CURRENT_SOURCE_DIR})
            include(${LITEOS_TOP_DIR}/build/release/litekey.cmake)
            set(_keysrc_included_${CMAKE_CURRENT_SOURCE_DIR} TRUE)
        endif()
    endif()
endmacro()

macro(add_if_exist_cond feature_switch file_base)
    if("${${feature_switch}}" STREQUAL "y")
        if(EXISTS ${file_base})
            list(APPEND LOCAL_SRCS_y ${file_base})
        else()
            if(NOT _keysrc_included_${CMAKE_CURRENT_SOURCE_DIR})
                include(${LITEOS_TOP_DIR}/build/release/litekey.cmake)
                set(_keysrc_included_${CMAKE_CURRENT_SOURCE_DIR} TRUE)
            endif()
        endif()
    endif()
endmacro()

add_if_exist_cond(LOSCFG_SHARED_STACK ${LITEOSTOPDIR}/kernel_lite/keysrc/los_shared_stack.c)
add_if_exist_cond(LOSCFG_RISCV_HIMIDEERV200_PLIC ${LITEOSTOPDIR}/drivers/interrupt/riscv_himideerv200_key.c)
add_if_exist_cond(LOSCFG_APC_ENABLE ${LITEOSTOPDIR}/arch_lite/riscv/src/pmp.c)
add_if_exist_cond(LOSCFG_PERF_HW_PMU ${LITEOSTOPDIR}/arch_lite/riscv/src/pmu.c)
add_if_exist_without_cond(${LITEOSTOPDIR}/arch_lite/riscv/src/cache.c)
add_if_exist_without_cond(${LITEOSTOPDIR}/arch_lite/riscv/src/fault_ccause.c)

set(MODULE_INCLUDE_PRI
    ${LITEOSTOPDIR}/kernel_lite/base/include
    ${LITEOSTOPDIR}/arch_lite/riscv/include
)
set(LOCAL_SRCS ${LOCAL_SRCS_y})  # module.cmake will sort LOCAL_SRCS files
include(${MODULE})