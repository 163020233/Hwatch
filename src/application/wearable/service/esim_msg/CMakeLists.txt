#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "esim_msg")

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/
)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/esim_base_interface.c
    ${CMAKE_CURRENT_SOURCE_DIR}/esim_bind_interface.c
    ${CMAKE_CURRENT_SOURCE_DIR}/esim_lpa_interface.c
    ${CMAKE_CURRENT_SOURCE_DIR}/esim_lpa.c
    ${CMAKE_CURRENT_SOURCE_DIR}/esim_bind.c
    ${CMAKE_CURRENT_SOURCE_DIR}/esim_stub.c
)

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

set(PUBLIC_DEFINES
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/protocol/ims/include/
    ${ROOT_DIR}/ohos/foundation/arkui/ace_engine_lite/frameworks/src/core/modules/netstack/http/include/
)

set(PRIVATE_DEFINES
)

build_component()

