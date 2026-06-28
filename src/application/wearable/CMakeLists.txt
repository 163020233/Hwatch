#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
add_subdirectory_if_exist(nativeabilityfwk)
add_subdirectory_if_exist(nativeapp)
add_subdirectory_if_exist(ohos_deps)
add_subdirectory_if_exist(service)
add_subdirectory_if_exist(ohos_startup)
add_subdirectory_if_exist(xts_adapt)
if("SUPPORT_ALIPAY_SEC" IN_LIST DEFINES OR "SUPPORT_ALIPAY_HSC32I1" IN_LIST DEFINES)
add_subdirectory_if_exist(alipay)
endif()
if("SUPPORT_AUTO_OTA" IN_LIST DEFINES)
    add_subdirectory_if_exist(auto_ota)
endif()
add_subdirectory_if_exist(dsoftbus_deps)
add_subdirectory_with_alias_if_exist(SystemView ${PROJECT_BINARY_DIR}/systemview)
add_subdirectory_if_exist(svr_dynload)
add_subdirectory_if_exist(virtual_sensor)
add_subdirectory_if_exist(mapsdk)