#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "mcp")

set(PUBLIC_DEFINES
)

set(PRIVATE_DEFINES
	CONFIG_CHIP_PLATFORM_WEAR
	ISP_OSAL_I2C
	NEW_CMP_PARA
	CONFIG_OT_DIS_SUPPORT
)

if (DEFINES MATCHES "HI3322_CHIP_V100" OR DEFINES MATCHES "HI3322_PRODUCT_FPGA" )
    list(APPEND PUBLIC_DEFINES "CONFIG_CHIP_3322")
    set(MCP_CHIP_NAME "3322")
endif()

if (DEFINES MATCHES "SW22_CHIP_V100" OR DEFINES MATCHES "SW22_PRODUCT_FPGA")
    list(APPEND PUBLIC_DEFINES "CONFIG_CHIP_1A22" "SOCT_GDC_SUPPORT" "SOCT_ISP_SUPPORT" 
        "CONFIG_EXT_ISP_COLOR_SECTOR_SUPPORT" "CONFIG_SOCT_1A22_SUPPORT")
    set(MCP_CHIP_NAME "1A22")

    if (${CORE} STREQUAL "ccore")
    list(APPEND PUBLIC_DEFINES "CONFIG_MCP_CCORE_SUPPORT")
    else()
    list(APPEND PUBLIC_DEFINES "CONFIG_MCP_ACORE_SUPPORT")
    endif()

    set(MCP_WORK_DEPLOYING "ccpu" CACHE STRING "Deployment target")
    set(PRIVATE_DEFINES "${PRIVATE_DEFINES};SOCT_DIS_SUPPORT")
endif()

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

if (DEFINES MATCHES "HI3322_CHIP_V100" OR DEFINES MATCHES "HI3322_PRODUCT_FPGA" )
set(COMPONENT_CCFLAGS
)
endif()

if (DEFINES MATCHES "SW22_CHIP_V100" OR DEFINES MATCHES "SW22_PRODUCT_FPGA")
set(COMPONENT_CCFLAGS
    -Wno-unused-parameter
    -Wno-unused-variable
    -Wno-unused-value
    -Wno-uninitialized
    -Wno-pointer-to-int-cast
    -Wno-int-to-pointer-cast
    -Wno-sign-compare
    -Wno-switch-enum
    -Wno-pointer-integer-compare
    -Wno-implicit-int  # skip EXPORT_SYMBOL error
    -Wno-int-conversion
    -Wno-pointer-sign
    -Wno-missing-field-initializers
    -Wno-unused-function
    -Wno-gnu-pointer-arith
    -Wno-unused-but-set-variable
    -Wno-tautological-pointer-compare   #skip err: equal to a null pointer is always false.
    -Wno-incompatible-pointer-types-discards-qualifiers #skip err: assigning to 'void *' from 'const td_void *
    -Wno-array-parameter    #skip err: argument 'r_src' of type 'td_u32[1024]'
    -Wno-parentheses-equality   #skip err: if((pInput->out0_en == 1))
    -Wno-comment    #skip err: gc02m1_cmos.h:75:1: error: '/*' within block comment [-Werror,-Wcomment]
)
endif()


set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

add_subdirectory(api)
add_subdirectory(drv)

set(PRIVATE_DEFINES
	${PRIVATE_DEFINES}
	${MCP_DEFINES_LIST}
)

set(SOURCES
    ${MCP_SRC_LIST}
)

set(PUBLIC_HEADER
    ${MCP_INC_LIST}

    ${ROOT_DIR}/middleware/utils/common_headers/mcp/external/base
    ${ROOT_DIR}/middleware/utils/common_headers/mcp/external/cis
    ${ROOT_DIR}/middleware/utils/common_headers/mcp/external/gdc
    ${ROOT_DIR}/middleware/utils/common_headers/mcp/external/isp
    ${ROOT_DIR}/middleware/utils/common_headers/mcp/external/isp/alg
    ${ROOT_DIR}/middleware/utils/common_headers/mcp/external/mipi_rx
    ${ROOT_DIR}/middleware/utils/common_headers/mcp/external/ppu
    ${ROOT_DIR}/middleware/utils/common_headers/mcp/external/viu
    ${ROOT_DIR}/middleware/utils/common_headers/mcp/external/fsd
    ${ROOT_DIR}/middleware/utils/common_headers/mcp/internal
    ${ROOT_DIR}/middleware/utils/common_headers/mcp/internal/base
    ${ROOT_DIR}/middleware/utils/common_headers/mcp/internal/base/hal/${MCP_CHIP_NAME}
    ${ROOT_DIR}/middleware/utils/common_headers/mcp/internal/dis
    ${ROOT_DIR}/middleware/utils/common_headers/mcp/internal/gdc
    ${ROOT_DIR}/middleware/utils/common_headers/mcp/internal/isp
    ${ROOT_DIR}/middleware/utils/common_headers/mcp/internal/mipi_rx
    ${ROOT_DIR}/middleware/utils/common_headers/mcp/internal/platform/wear
    ${ROOT_DIR}/middleware/utils/common_headers/mcp/internal/ppu
    ${ROOT_DIR}/middleware/utils/common_headers/mcp/internal/vgs
    ${ROOT_DIR}/middleware/utils/common_headers/mcp/internal/viu
    ${ROOT_DIR}/middleware/utils/common_headers/mcp/internal/fsd
    ${ROOT_DIR}/middleware/utils/common_headers/native
)

set(PRIVATE_HEADER
    ${MCP_INC_LIST}
)

set(LIBRARY_OUTPUT_PATH ${BIN_DIR}/${CHIP}/libs/mcp/${TARGET_COMMAND})

build_component()

install_sdk(${BIN_DIR}/${CHIP}/libs/mcp/${TARGET_COMMAND} "*.a")
