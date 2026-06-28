#===============================================================================
# @brief    cmake open_source
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================

include(${CMAKE_DIR}/open_source/gmssl.cmake)
include(${CMAKE_DIR}/open_source/mbedtls.cmake)
include(${CMAKE_DIR}/open_source/hitls.cmake)
include(${CMAKE_DIR}/open_source/littlefs.cmake)
include(${CMAKE_DIR}/open_source/libcoap.cmake)
if (EXISTS ${ROOT_DIR}/protocol/wifi/CMakeLists.txt)
include(${CMAKE_DIR}/open_source/wpa_supplicant.cmake)
include(${CMAKE_DIR}/open_source/wpa_supplicant2.11.cmake)
endif()
include(${CMAKE_DIR}/open_source/qcbor.cmake)
include(${CMAKE_DIR}/open_source/mqtt.cmake)
include(${CMAKE_DIR}/open_source/lwip.cmake)