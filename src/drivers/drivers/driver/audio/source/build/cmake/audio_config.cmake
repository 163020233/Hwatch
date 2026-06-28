#[[
Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
Description: CMake audio protocol module.
Author: audio
Create: 2022-06-05
]]

cmake_minimum_required(VERSION 3.14.1)

#===================================================================================
set(CFG_SAP_SDK_VERSION SAP_SDKV100R001)
set(SAP_CFGFILE "")
set(SAP_FEATURE_CONFIGS "")
set(PROJECT_ROOT_DIR ${ROOTDIR})
#===================================================================================
get_filename_component(SAP_SDK_DIR ${SAP_TOP_DIR}/../../../../.. ABSOLUTE)
set(SAP_UAPI_DIR ${SAP_SDK_DIR}/drivers/drivers/driver/audio/include)
#===================================================================================
include(${SAP_TOP_DIR}/build/cmake/read-config.cmake)

set(SAP_BOARD ${BOARD})
# Switch audio config mak for brandy evb4
if("BRANDY_PRODUCT_EVB4" IN_LIST DEFINES)
    string(APPEND SAP_BOARD "4")
endif() #BRANDY_PRODUCT_EVB4

# Mini version:: Switch audio config mak:
if("${DSP_VERSION}" STREQUAL "mini")
    string(APPEND SAP_BOARD "mini")
endif()

# Max version:: Switch audio config mak
if("${DSP_VERSION}" STREQUAL "max")
    string(APPEND SAP_BOARD "max")
endif()

# Nano version:: Switch audio config mak:
if("${DSP_VERSION}" STREQUAL "nano")
    string(APPEND SAP_BOARD "nano")
endif()

if ("MELODY_DEMO_BMIC_RX" IN_LIST DEFINES)
    set(SAP_BOARD "bmicrx")
endif()

if ("MELODY_DEMO_BMIC_TX" IN_LIST DEFINES)
    set(SAP_BOARD "bmictx")
endif()

if ("MELODY_DEMO_EARPHONE" IN_LIST DEFINES)
    set(SAP_BOARD "earphone")
endif()

if ("MELODY_DEMO_SOUND_BOX" IN_LIST DEFINES)
    set(SAP_BOARD "soundbox")
endif()

if ("MELODY_DEMO_DONGLE" IN_LIST DEFINES OR "MELODY_DEMO_SOUNDBAR" IN_LIST DEFINES)
    set(SAP_BOARD "dongle")
endif()

if ("MELODY_DEMO_HA" IN_LIST DEFINES)
    set(SAP_BOARD "ha")
endif()

set(SAP_CFGFILE ${SAP_TOP_DIR}/build/configs/${CHIP}/${CHIP}_${SAP_BOARD}_liteos_cfg.mak)
message("audio SAP_CFGFILE = " ${SAP_CFGFILE})

read_config(${SAP_CFGFILE})
install_sdk(${SAP_TOP_DIR}/build/configs/${CHIP} "*.mak")
#===================================================================================
