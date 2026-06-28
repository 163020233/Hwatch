#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "dfx_port_3322")

if(${CORE} STREQUAL "acore")
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/dfx_channel.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dfx_system_init.c
    ${CMAKE_CURRENT_SOURCE_DIR}/diag_adapt_cmd_mem.c
    ${CMAKE_CURRENT_SOURCE_DIR}/last_dump_adapt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sample_data_adapt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/diag_adapt_sdt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/connectivity_log.c
    ${CMAKE_CURRENT_SOURCE_DIR}/diag_sample_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/crash_data_porting.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dfx_reboot.c
    ${CMAKE_CURRENT_SOURCE_DIR}/diag_data_store_adapt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dfx_cloud_report.c
    ${CMAKE_CURRENT_SOURCE_DIR}/diag_audio.c
    ${CMAKE_CURRENT_SOURCE_DIR}/diag_adapt_flash.c
)
else()
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/diag_bt_custom.c
)
endif()

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${ROOT_DIR}/middleware/utils/dfx/diag_dfx_cmd/diag_system_metrics/include
    ${ROOT_DIR}/middleware/utils/dfx/diag/diag_data_store/include
    ${ROOT_DIR}/middleware/utils/dfx/diag/diag_data_transport/include
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/include/middleware/services/bts
    ${ROOT_DIR}/include/middleware/services/bts/ble
    ${ROOT_DIR}/include/middleware/services/bts/br
    ${ROOT_DIR}/include/middleware/services/bts/common
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-implicit-function-declaration
    -Wno-incompatible-function-pointer-types
    -Wno-incompatible-pointer-types-discards-qualifiers
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

set(LOG_DEF
    ${CMAKE_CURRENT_SOURCE_DIR}/include/log_def_gpu.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/log_def_gui.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/log_def_dsp.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/log_def_pf.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/log_def_app.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/log_def_media.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/log_def_btc.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/log_def_wifi.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/log_def_ohos.h
)

build_component()



set(COMPONENT_NAME "dfx_log_3322")

set(SOURCES
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(PRIVATE_HEADER
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

set(LOG_DEF
    ${CMAKE_CURRENT_SOURCE_DIR}/include/log_def_dsp.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/log_def_pf.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/log_def_media.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/log_def_ohos.h
)
if("_PRE_FEATURE_WS53_DEVICE_MODE" IN_LIST DEFINES)
   list(APPEND LOG_DEF ${CMAKE_CURRENT_SOURCE_DIR}/include/log_def_wifi.h)
endif()

build_component()
