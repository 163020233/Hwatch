#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "3322_loaderboot_fmc")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/fmc/common/nand_fmc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/fmc/common/nand_ids.c
    ${CMAKE_CURRENT_SOURCE_DIR}/fmc/common/nand_ops.c
    ${CMAKE_CURRENT_SOURCE_DIR}/fmc/common/nand_scan.c
    ${CMAKE_CURRENT_SOURCE_DIR}/fmc/common/nand.c
    ${CMAKE_CURRENT_SOURCE_DIR}/fmc/nand/nand_init.c
    ${CMAKE_CURRENT_SOURCE_DIR}/fmc/nand/hifmc100/hifmc100.c
    ${CMAKE_CURRENT_SOURCE_DIR}/fmc/nand/hifmc100/hifmc_common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/porting/fmc/fmc_porting.c
    ${CMAKE_CURRENT_SOURCE_DIR}/fmc/nand/nandflash_ds35x1g.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/fmc
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/fmc/common/include
    ${CMAKE_CURRENT_SOURCE_DIR}/fmc/nand/hifmc100/include
    ${CMAKE_CURRENT_SOURCE_DIR}/fmc/nand
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
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

set(COMPONENT_NAME "3322_loaderboot_emmc")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/porting/emmc/mmc_porting.c
    ${CMAKE_CURRENT_SOURCE_DIR}/emmc/disk.c
    ${CMAKE_CURRENT_SOURCE_DIR}/emmc/mmc_init.c
    ${CMAKE_CURRENT_SOURCE_DIR}/emmc/core/block.c
    ${CMAKE_CURRENT_SOURCE_DIR}/emmc/core/mmc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/emmc/core/sdio.c
    ${CMAKE_CURRENT_SOURCE_DIR}/emmc/core/mmcsd.c
    ${CMAKE_CURRENT_SOURCE_DIR}/emmc/host/sdhci/sdhci_init.c
    ${CMAKE_CURRENT_SOURCE_DIR}/emmc/host/sdhci/mmc_sdhci.c
    ${CMAKE_CURRENT_SOURCE_DIR}/emmc/host/sdhci/sdhci.c
    ${CMAKE_CURRENT_SOURCE_DIR}/emmc/host/sdhci/adma2.c
    ${CMAKE_CURRENT_SOURCE_DIR}/emmc/host/hci.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/emmc
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/emmc/host/sdhci
    ${CMAKE_CURRENT_SOURCE_DIR}/emmc/include
    ${CMAKE_CURRENT_SOURCE_DIR}/emmc/core
    ${CMAKE_CURRENT_SOURCE_DIR}/porting/emmc
)

set(PRIVATE_DEFINES
LOSCFG_DRIVERS_MMC_RW_IN_BOOT_AND_EXC
LOFCFG_DRIVERS_MMC_CUSTOM_TUNING
LOSCFG_DRIVERS_MMC_HISDEMMC
LOSCFG_DRIVERS_MMC_DMA_CACHE_DISABLE
)

set(PUBLIC_DEFINES
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