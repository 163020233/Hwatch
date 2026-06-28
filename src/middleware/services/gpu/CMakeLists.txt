#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================

set(COMPONENT_NAME "gpu")

set(PUBLIC_DEFINES
)

set(PRIVATE_DEFINES
    CONFIG_GFX_PROC_SUPPORT
    CONFIG_GPU_DFX_ERROR
    CONFIG_VAU_DRAW_DIRECTLY
)

if (DEFINES MATCHES "SW21_CHIP_V100" OR DEFINES MATCHES "SW21_PRODUCT_FPGA")
    list(APPEND PUBLIC_DEFINES "GPU_1A21" "GPU_HDR_SUPPORT" "GPU_SRE_SUPPORT" "GPU_DRAW_CLR_SUPPORT" "GPU_DUMP_SUPPORT")
endif()

if (DEFINES MATCHES "HI3322_CHIP_V100" OR DEFINES MATCHES "HI3322_PRODUCT_FPGA" )
    list(APPEND PUBLIC_DEFINES "GPU_3322" "GPU_ZME_SUPPORT" "GPU_SUPPORT_CHR" "GPU_SRE_SUPPORT" "GPU_HDR_SUPPORT"
         "GPU_SUBNODE_SUPPORT")
endif()

if (DEFINES MATCHES "SW22_CHIP_V100" OR DEFINES MATCHES "SW22_PRODUCT_FPGA")
    list(APPEND PUBLIC_DEFINES "GPU_1A22" "GPU_ZME_SUPPORT" "GPU_DDR_SUPPORT" "GPU_HDR_SUPPORT" "GPU_SRE_SUPPORT"
         "GPU_SDF_SUPPORT" "GPU_DUMP_SUPPORT")
endif()

if (DEFINES MATCHES "SW21_CHIP_V100")
    list(APPEND PUBLIC_DEFINES "GPU_ASIC_1A21")
elseif (DEFINES MATCHES "SW21_PRODUCT_FPGA")
    list(APPEND PUBLIC_DEFINES "GPU_FPGA_1A21")
elseif (DEFINES MATCHES "SUQIN_PRODUCT_FPGA")
    list(APPEND PUBLIC_DEFINES "GPU_FPGA_3321")
elseif (DEFINES MATCHES "HI3322_CHIP_V100")
    list(APPEND PUBLIC_DEFINES "GPU_ASIC_3322")
elseif (DEFINES MATCHES "HI3322_PRODUCT_FPGA")
    list(APPEND PUBLIC_DEFINES "GPU_FPGA_3322")
elseif (DEFINES MATCHES "SW22_CHIP_V100")
    list(APPEND PUBLIC_DEFINES "GPU_ASIC_1A22")
elseif (DEFINES MATCHES "SW22_PRODUCT_FPGA")
    list(APPEND PUBLIC_DEFINES "GPU_FPGA_1A22")
else()
    list(APPEND PUBLIC_DEFINES "GPU_FOR_ASIC")
endif()

if (DEFINES MATCHES "CONFIG_PSRAM_SUPPORT_LPM")
   list(APPEND PUBLIC_DEFINES "GPU_PSRAM_SUPPORT")
endif()

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    "-Werror=out-of-line-declaration"
    "-Werror=uninitialized"
    "-Werror=int-conversion"
    "-Werror=enum-conversion"
    "-Werror=long-long"
    "-Werror=unused-variable"
    "-Werror=unused-value"
    "-Werror=unused-function"
    "-Werror=unused-but-set-variable"
    "-Werror=sign-compare"
    "-Werror=unused-parameter"
    "-Werror=tautological-compare"
    "-Werror=bad-function-cast"
    "-Werror=incompatible-pointer-types"
    "-Werror=implicit-function-declaration"
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

add_subdirectory_if_exist(api)
add_subdirectory_if_exist(drv)
add_subdirectory_if_exist(dfx)
add_subdirectory_if_exist(rm)

set(SOURCES
    ${GPU_SRC_LIST}
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${GPU_INC_LIST}
)

set(MODULE_NAME "gpu")

set(AUTO_DEF_FILE_ID TRUE)

set(LIBRARY_OUTPUT_PATH ${BIN_DIR}/${CHIP}/libs/gpu/${TARGET_COMMAND})

build_component()

if (DEFINES MATCHES "HI3322_CHIP_V100" OR DEFINES MATCHES "HI3322_PRODUCT_FPGA" )
install_sdk(${BIN_DIR}/${CHIP}/libs/gpu/${TARGET_COMMAND} "*.a")
endif()
