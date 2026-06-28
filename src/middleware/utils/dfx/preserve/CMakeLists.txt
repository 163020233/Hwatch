#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "dfx_preserve")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/private/preserve.c
    ${CMAKE_CURRENT_SOURCE_DIR}/private/crash_data.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/public
)

set(PRIVATE_HEADER
)

if("SUPPORT_EXCEPTION_EXPORT_COMPRESS" IN_LIST DEFINES)
    list(APPEND PUBLIC_HEADER
        ${ROOT_DIR}/open_source/lz4/
    )
    list(APPEND PRIVATE_HEADER
        _STANDALONE
    )
endif()

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
    SUPPORT_DFX_PRESERVE
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

install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/public/" "*.h")

build_component()