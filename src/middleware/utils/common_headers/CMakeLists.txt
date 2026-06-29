#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "common_headers")

set(SOURCES
)
set(MCP_COMMON_HEADERS)
add_subdirectory_if_exist(mcp)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/media
    ${CMAKE_CURRENT_SOURCE_DIR}/gpu/external
    ${CMAKE_CURRENT_SOURCE_DIR}/gpu/internal
    ${CMAKE_CURRENT_SOURCE_DIR}/venc/external
    ${CMAKE_CURRENT_SOURCE_DIR}/venc/internal
    ${CMAKE_CURRENT_SOURCE_DIR}/vdec/external
    ${CMAKE_CURRENT_SOURCE_DIR}/vdec/internal
    ${CMAKE_CURRENT_SOURCE_DIR}/native
    ${CMAKE_CURRENT_SOURCE_DIR}/osal
    ${MCP_COMMON_HEADERS}
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
if("_PRE_FEATURE_WS53_DEVICE_MODE" IN_LIST DEFINES OR "_PRE_FEATURE_SYSCHANNEL_LITEOS" IN_LIST DEFINES)
    list(APPEND COMPONENT_PUBLIC_CCFLAGS -Wno-typedef-redefinition)
endif()

set(COMPONENT_CCFLAGS
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()


set(COMPONENT_NAME "common_headers_base")

set(SOURCES
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/native
    ${CMAKE_CURRENT_SOURCE_DIR}/osal
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

build_component()