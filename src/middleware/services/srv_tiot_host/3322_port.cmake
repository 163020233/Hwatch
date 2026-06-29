#[[
Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd.. 2025-2025. All rights reserved.
Description: CMake tiot driver module.
Author: tiot driver
Create: 2025-09-15
]]

if(DEFINED TIOT_DEVICE_LIST AND "caxx" IN_LIST TIOT_DEVICE_LIST AND "gnss" IN_LIST TIOT_DEVICE_LIST)
    set(TIOT_PORTING_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tiot_driver/product_porting/3322_hiditing/gnss_nfc)
    set(TIOT_PORTING_CONFIG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tiot_driver/product_porting/3322_hiditing/gnss_nfc)
elseif(DEFINED TIOT_DEVICE_LIST AND "gnss" IN_LIST TIOT_DEVICE_LIST)
    set(TIOT_PORTING_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tiot_driver/product_porting/3322_hiditing/gnss)
    set(TIOT_PORTING_CONFIG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tiot_driver/product_porting/3322_hiditing/gnss)
endif()
