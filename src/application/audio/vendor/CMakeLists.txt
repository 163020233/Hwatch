#[[
Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
Description: CMake audio sample module.
Author: audio
Create: 2023-06-09
]]

#===================================================================================
set(COMPONENT_NAME "audio_vendor")
if(NOT ${COMPONENT_NAME} IN_LIST TARGET_COMPONENT)
    return()
endif()
#===================================================================================
set(SAP_TOP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../drivers/drivers/driver/audio/source)
get_filename_component(SAP_TOP_DIR ${SAP_TOP_DIR} ABSOLUTE)
#===================================================================================
include(${SAP_TOP_DIR}/build/cmake/audio_config.cmake)
#===================================================================================
set(MODULE_NAME "dsp")
set(AUTO_DEF_FILE_ID TRUE)
#===================================================================================
set(COMPONENT_SRC )
#===================================================================================
set(COMPONENT_INC
    ${SAP_TOP_DIR}/include
    ${SAP_TOP_DIR}/core/common/include
    ${CMAKE_CURRENT_SOURCE_DIR}/common
)
#===================================================================================

if("${CFG_SAP_VENDOR_DRIVER_ACM8623_SUPPORT}" STREQUAL "y")
set(COMPONENT_SRC ${COMPONENT_SRC}
    ${CMAKE_CURRENT_SOURCE_DIR}/acm8623/acm8623.c
)
set(COMPONENT_INC ${COMPONENT_INC}
    ${CMAKE_CURRENT_SOURCE_DIR}/acm8623
    ${CMAKE_CURRENT_SOURCE_DIR}/acm8623/arch/${CFG_SAP_CHIP_TYPE}
)
endif()

if("${CFG_SAP_VENDOR_DRIVER_ACM8625S_SUPPORT}" STREQUAL "y")
set(COMPONENT_SRC ${COMPONENT_SRC}
    ${CMAKE_CURRENT_SOURCE_DIR}/acm8625s/acm8625s.c
)
set(COMPONENT_INC ${COMPONENT_INC}
    ${CMAKE_CURRENT_SOURCE_DIR}/acm8625s
    ${CMAKE_CURRENT_SOURCE_DIR}/acm8625s/arch/${CFG_SAP_CHIP_TYPE}
)
endif()

if("${CFG_SAP_VENDOR_DRIVER_ACM8815_SUPPORT}" STREQUAL "y")
set(COMPONENT_SRC ${COMPONENT_SRC}
    ${CMAKE_CURRENT_SOURCE_DIR}/acm8815/acm8815.c
)
set(COMPONENT_INC ${COMPONENT_INC}
    ${CMAKE_CURRENT_SOURCE_DIR}/acm8815
    ${CMAKE_CURRENT_SOURCE_DIR}/acm8815/arch/${CFG_SAP_CHIP_TYPE}
)
endif()

if("${CFG_SAP_VENDOR_DRIVER_AW85825_SUPPORT}" STREQUAL "y")
set(COMPONENT_SRC ${COMPONENT_SRC}
    ${CMAKE_CURRENT_SOURCE_DIR}/aw85825/aw85825.c
)
set(COMPONENT_INC ${COMPONENT_INC}
    ${CMAKE_CURRENT_SOURCE_DIR}/aw85825
    ${CMAKE_CURRENT_SOURCE_DIR}/aw85825/arch/${CFG_SAP_CHIP_TYPE}
)
endif()

if("${CFG_SAP_VENDOR_DRIVER_AW88262_SUPPORT}" STREQUAL "y")
set(COMPONENT_SRC ${COMPONENT_SRC}
    ${CMAKE_CURRENT_SOURCE_DIR}/aw88262/aw88262.c
)
set(COMPONENT_INC ${COMPONENT_INC}
    ${CMAKE_CURRENT_SOURCE_DIR}/aw88262
    ${CMAKE_CURRENT_SOURCE_DIR}/aw88262/arch/${CFG_SAP_CHIP_TYPE}
)
endif()

if("${CFG_SAP_VENDOR_DRIVER_AW88166_SUPPORT}" STREQUAL "y")
set(COMPONENT_SRC ${COMPONENT_SRC}
    ${CMAKE_CURRENT_SOURCE_DIR}/aw88166/aw88166.c
    ${CMAKE_CURRENT_SOURCE_DIR}/aw88166/aw883xx.c
    ${CMAKE_CURRENT_SOURCE_DIR}/aw88166/aw883xx_calib.c
    ${CMAKE_CURRENT_SOURCE_DIR}/aw88166/aw883xx_device.c
    ${CMAKE_CURRENT_SOURCE_DIR}/aw88166/product_init_files/aw88166/aw883xx_pid_2066_init.c
)
set(COMPONENT_INC ${COMPONENT_INC}
    ${CMAKE_CURRENT_SOURCE_DIR}/aw88166
    ${CMAKE_CURRENT_SOURCE_DIR}/aw88166/product_init_files/aw88166
    ${CMAKE_CURRENT_SOURCE_DIR}/aw88166/config/aw88166/mono/16bit
)
endif()

if("${CFG_SAP_VENDOR_DRIVER_ANC_CODEC_SUPPORT}" STREQUAL "y")
set(COMPONENT_SRC ${COMPONENT_SRC}
    ${CMAKE_CURRENT_SOURCE_DIR}/anc_codec/anc_codec.c
)
set(COMPONENT_INC ${COMPONENT_INC}
    ${CMAKE_CURRENT_SOURCE_DIR}/anc_codec
    ${CMAKE_CURRENT_SOURCE_DIR}/anc_codec/sai01_dac_32.768k
)
endif()
#===================================================================================
set(COMPONENT_DEF
    "SAP_PROC_SUPPORT"
)
#===================================================================================
include(${SAP_TOP_DIR}/build/cmake/audio_common.cmake)
