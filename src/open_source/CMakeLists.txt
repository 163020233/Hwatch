#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
add_subdirectory_if_exist(mbedtls)
add_subdirectory_if_exist(7-zip-lzma-sdk)
add_subdirectory_if_exist(coremark)
add_subdirectory_if_exist(libboundscheck)
add_subdirectory_if_exist(lz4)

# cmsis-dsp funtions support RISC-V V extension, default OFF 
option(RISCVV "RISCV V extension support" OFF)
add_subdirectory_if_exist(CMSIS-DSP)

if(DEFINED CONFIG_SUPPORT_OH)
# add oh
elseif("ENABLE_UIKIT" IN_LIST DEFINES)
# add oh
elseif("CFG_NORFLASH_ONLY" IN_LIST DEFINES)
add_subdirectory_if_exist(cjson)
 add_subdirectory_if_exist(mqtt)
 add_subdirectory_if_exist(littlefs)
 add_subdirectory_if_exist(libcoap)
else()
add_subdirectory_if_exist(cjson)
add_subdirectory_if_exist(mqtt)
add_subdirectory_if_exist(littlefs)
add_subdirectory_if_exist(huks)
add_subdirectory_if_exist(deviceauth)
add_subdirectory_if_exist(libcoap)
add_subdirectory_if_exist(openharmony)
endif()

if (EXISTS ${ROOT_DIR}/protocol/wifi/CMakeLists.txt OR "CONFIG_SUPPORT_LWIP" IN_LIST DEFINES)
    add_subdirectory_if_exist(lwip)
endif()

if("CONFIG_SUPPORT_MATTER" IN_LIST DEFINES)
add_subdirectory_if_exist(matter)
endif()
