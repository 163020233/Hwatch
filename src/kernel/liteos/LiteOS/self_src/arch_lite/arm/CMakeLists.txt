
# extra definition for other module
set(LITEOS_CPU_TYPE ${LOSCFG_ARCH_CPU} CACHE STRING "CPU_TYPE" FORCE)
set(LITEOS_ARM_ARCH -march=${LOSCFG_ARCH_ARM_VER} CACHE STRING "ARM_ARCH" FORCE)

if(LOSCFG_ARCH_ARM_CORTEX_M)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/cortex_m)
else()
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/cortex_a_r)
endif()
