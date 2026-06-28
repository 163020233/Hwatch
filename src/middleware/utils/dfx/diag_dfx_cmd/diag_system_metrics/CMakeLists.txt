#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================

set(COMPONENT_NAME "dfx_system_metrics")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/diag_log_hook.c
    ${CMAKE_CURRENT_SOURCE_DIR}/diag_metrics_common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/diag_os_metrics.c
    ${CMAKE_CURRENT_SOURCE_DIR}/diag_connectivity_metrics.c
    ${CMAKE_CURRENT_SOURCE_DIR}/diag_device_info.c
    ${CMAKE_CURRENT_SOURCE_DIR}/diag_sensor_metrics.c
    ${CMAKE_CURRENT_SOURCE_DIR}/diag_data_mock.c
    ${CMAKE_CURRENT_SOURCE_DIR}/diag_report_control.c
    ${CMAKE_CURRENT_SOURCE_DIR}/diag_metrics_cmd.c
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    # -Wno-unused-parameter
    # -Wno-incompatible-pointer-types
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

set(AUTO_ADD_PATH
    false
)

build_component()
