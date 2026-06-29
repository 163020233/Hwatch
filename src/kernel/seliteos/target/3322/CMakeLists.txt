#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
#===============================================================================

set(COMPONENT_NAME "target")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/board.c
    ${CMAKE_CURRENT_SOURCE_DIR}/init.c
    ${CMAKE_CURRENT_SOURCE_DIR}/reset_vector.S
    ${CMAKE_CURRENT_SOURCE_DIR}/lowpower/lowpower.S
    ${CMAKE_CURRENT_SOURCE_DIR}/lowpower/lowpower.c
    ${CMAKE_CURRENT_SOURCE_DIR}/pmp_cfg/pmp_cfg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rapc/rapc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/fapc/fapc.c

    ${CMAKE_CURRENT_SOURCE_DIR}/../common/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/chips_porting_common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ioctl/chips_porting_ioctl.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/pmp_cfg
    ${CMAKE_CURRENT_SOURCE_DIR}/rapc
    ${CMAKE_CURRENT_SOURCE_DIR}/fapc
    ${CMAKE_CURRENT_SOURCE_DIR}/arch
    ${CMAKE_CURRENT_SOURCE_DIR}/lowpower
    ${CMAKE_CURRENT_SOURCE_DIR}/ioctl
    ${CMAKE_CURRENT_SOURCE_DIR}/../common
)

set(PRIVATE_HEADER
)

set(PRIVATE_DEFINES
    -DLOSCFG_ARCH_LINX_M
    -DLOSCFG_ARCH_RISCV_TES
    -DTZPC_PROTECT_CONFIG
    -DCONFIG_TEE_EFUSE_SUPPORT
)

set(PUBLIC_DEFINES
    # enable tee log
    -DLOSCFG_LOG_ENABLE=1

    # print error log level
    -DPRINT_LEVEL=2

    # psa support
    -DCONFIG_PSA_CRYPTO_SUPPORT
    -DCONFIG_PSA_KEY_DERIVATION
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    true
)

add_subdirectory_if_exist(tee_efuse)

build_component()
