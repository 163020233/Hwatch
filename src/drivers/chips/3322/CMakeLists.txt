#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================

add_subdirectory_if_exist(porting)
add_subdirectory_if_exist(pmu)
add_subdirectory_if_exist(pwm_aon)
add_subdirectory_if_exist(boot)
add_subdirectory_if_exist(rom)
add_subdirectory_if_exist(clocks)
add_subdirectory_if_exist(seliteos_init)
add_subdirectory_if_exist(include)
add_subdirectory_if_exist(burn_tools)
add_subdirectory_if_exist(sec_trng/master)
add_subdirectory_if_exist(sec_trng/slave)
add_subdirectory_if_exist(feature_table)
set(COMPONENT_NAME "chip_3322")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/arch/riscv/${ARCH}/interrupt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/arch/riscv/${ARCH}/interrupt_handler.c
    ${CMAKE_CURRENT_SOURCE_DIR}/interrupt/${CORE}/interrupt_adapter.c
    ${CMAKE_CURRENT_SOURCE_DIR}/interrupt/${CORE}/interrupt_os_adapter.c
    ${CMAKE_CURRENT_SOURCE_DIR}/interrupt/${CORE}/vectors.c
    ${CMAKE_CURRENT_SOURCE_DIR}/config/pmp_config.c
)

if("NON_OS_ENABLE_TEE" IN_LIST DEFINES)
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/arch/riscv/${ARCH}/noos_exception_riscv.c)
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/arch/riscv/${ARCH}/vectors_tee.s)
else()
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/arch/riscv/${ARCH}/vectors.s)
endif()

if(DEFINED OS AND NOT ${OS} STREQUAL "non-os")
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/arch/riscv/${ARCH}/oam_trace.c)
endif()

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/arch/riscv/${ARCH}
)

set(PRIVATE_HEADER
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()

set(COMPONENT_NAME "sfc_3322")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/sfc/drivers/sfc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sfc/hal/hal_sfc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sfc/hal/hal_sfc_v150.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sfc/hal/hal_sfc_v150_regs_op.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/sfc/drivers
    ${CMAKE_CURRENT_SOURCE_DIR}/sfc/hal
)

set(PRIVATE_HEADER
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
    SUPPORT_CLOCKS
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()

set(COMPONENT_NAME "fmc_3322")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/fmc/common/nand_fmc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/fmc/common/nand_ids.c
    ${CMAKE_CURRENT_SOURCE_DIR}/fmc/common/nand_ops.c
    ${CMAKE_CURRENT_SOURCE_DIR}/fmc/common/nand_scan.c
    ${CMAKE_CURRENT_SOURCE_DIR}/fmc/common/nand.c
    ${CMAKE_CURRENT_SOURCE_DIR}/fmc/nand/nand_init.c
    ${CMAKE_CURRENT_SOURCE_DIR}/fmc/nand/hifmc100/hifmc100.c
    ${CMAKE_CURRENT_SOURCE_DIR}/fmc/nand/hifmc100/hifmc_common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/fmc/nand/nandflash_ds35x1g.c
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/fmc/nand
    ${CMAKE_CURRENT_SOURCE_DIR}/fmc/nand/hifmc100/include
    ${ROOT_DIR}/kernel/liteos/LiteOS/self_src/fs/yaffs2/direct
    ${ROOT_DIR}/kernel/liteos/LiteOS/self_src/fs/yaffs2/os_adapt/adapt_liteos
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
    FMC_SUPPORT
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-unused-parameter
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}" "*.h")

build_component()

set(COMPONENT_NAME "rom_callback")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/rom_config/${CORE}/output/rom_callback.S
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

if(${BUILD_ROM_CALLBACK} OR rom_callback IN_LIST TARGET_COMPONENT)
    execute_process(
        COMMAND ${CP} ${CMAKE_CURRENT_SOURCE_DIR}/rom_config/${CORE}/output/function.lds ${CMAKE_BINARY_DIR}
        COMMAND ${CP} ${CMAKE_CURRENT_SOURCE_DIR}/rom_config/${CORE}/output/data.lds ${CMAKE_BINARY_DIR}
    )
endif()

if(rom_callback IN_LIST TARGET_COMPONENT)
    include(${CMAKE_CURRENT_SOURCE_DIR}/rom_config/${CORE}/output/rom_callback_wrap.cmake)
    target_link_options(${TARGET_NAME}
        PRIVATE
        ${CMAKE_CB_FLGAS}
    )
endif()
build_component()

add_subdirectory_if_exist(patch)
