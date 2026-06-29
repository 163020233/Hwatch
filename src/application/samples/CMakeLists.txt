#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "samples")

set(SOURCES
)

set(PUBLIC_HEADER ${ROOT_DIR}/middleware/utils/app_init
)

if(DEFINED CONFIG_ENABLE_BT_SAMPLE OR "CONFIG_ENABLE_BT_SAMPLE" IN_LIST DEFINES)
    add_subdirectory_if_exist(bt)
endif()
if("CONFIG_ENABLE_PERIPHERAL_SAMPLE" IN_LIST DEFINES)
    add_subdirectory_if_exist(peripheral)
endif()
if(DEFINED CONFIG_ENABLE_WIFI_SAMPLE)
    add_subdirectory_if_exist(wifi_test)
endif()
if(DEFINED CONFIG_ENABLE_PRODUCTS_SAMPLE)
    add_subdirectory_if_exist(products)
endif()
if(DEFINED CONFIG_ENABLE_RADAR_SAMPLE)
    add_subdirectory_if_exist(radar_test)
endif()
if(DEFINED CONFIG_ENABLE_NFC_SAMPLE)
    add_subdirectory_if_exist(nfc)
endif()
if(DEFINED CONFIG_ENABLE_PSA_SAMPLE)
    add_subdirectory_if_exist(psa)
endif()
if(DEFINED CONFIG_ENABLE_SECURITY_UNIFIED_SAMPLE)
    add_subdirectory_if_exist(security_unified)
endif()
if(DEFINED CONFIG_ENABLE_AI_SAMPLE)
    add_subdirectory_if_exist(ai)
endif()
if("CONFIG_ENABLE_DITING_GNSS_SAMPLE" IN_LIST DEFINES)
    add_subdirectory_if_exist(gnss)
endif()
if("CONFIG_ENABLE_GPIO_SAMPLE" IN_LIST DEFINES)
    add_subdirectory_if_exist(gpio)
endif()
if("CONFIG_ENABLE_PWM_SAMPLE" IN_LIST DEFINES)
    add_subdirectory_if_exist(pwm)
endif()
if("CONFIG_ENABLE_TIMER_SAMPLE" IN_LIST DEFINES)
    add_subdirectory_if_exist(timer)
endif()
if("CONFIG_ENABLE_WATCHDOG_SAMPLE" IN_LIST DEFINES)
    add_subdirectory_if_exist(watchdog)
endif()
if("CONFIG_VOICE_CLOUD_LLM_ENABLE" IN_LIST DEFINES)
    add_subdirectory_if_exist(voice_cloud_LLM)
endif()
add_subdirectory_if_exist(custom)

set(PRIVATE_HEADER
    ${PRIVATE_HDR}
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

if(${CHIP_FAMILY} MATCHES "bs2x" OR ${CHIP} MATCHES "bs25")
    install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/peripheral" "*")
    install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/products" "*")
    install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/nfc" "*")
    install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/bt" "*")
elseif(${CHIP} MATCHES "bs27a")
    install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/peripheral" "*")
    install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/psa" "*")
elseif(${CHIP_FAMILY} MATCHES "sw21")
# no install to do
elseif(${CHIP} MATCHES "3322")
# no install to do
elseif(${CHIP} MATCHES "sw22")
# no install to do
elseif(${CHIP} MATCHES "sw39")
# no install to do
elseif(${CHIP} MATCHES "socmn2")
# no install to do
else()
    install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/bt" "*")
    install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/peripheral" "*")
    install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/products" "*")
    install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/wifi_test/sta_sample" "*")
    install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/wifi_test/alilink_sample" "*")
    install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/wifi_test/softap_sample" "*")
    install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/wifi_test/tcp_sample" "*")
    install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/wifi_test/ohos_connect" "*")
    install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/wifi_test/hilink_indie_upgrade" "*")
    install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/wifi_test/ble_wifi_cfg_sample" "*")
    install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/radar_test" "*")
    install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/nfc" "*")
endif()

build_component()

set(COMPONENT_NAME "mesh_samples")

set(SOURCES
)

set(PUBLIC_HEADER
)

set(COMPONENT_CCFLAGS
)

if((DEFINED CONFIG_ENABLE_BT_SAMPLE) AND (DEFINED CONFIG_SAMPLE_SUPPORT_SLE_MESH) AND (${CHIP} MATCHES "ws63"))
    add_subdirectory_if_exist(bt/sle/mesh)
endif()

set(PRIVATE_HEADER
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)
if((DEFINED CONFIG_ENABLE_BT_SAMPLE) AND (DEFINED CONFIG_SAMPLE_SUPPORT_SLE_MESH) AND (${CHIP} MATCHES "ws63"))
    install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/bt/sle/mesh" "*")
endif()
 
build_component()