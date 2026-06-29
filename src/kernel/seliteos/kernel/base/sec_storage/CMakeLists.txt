#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "sec_storage")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/base/sec_storage.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dispatch/sec_storage_dispatch.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ioctl/sec_storage_ioctl.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(PRIVATE_HEADER
)

set(TA_PRIVATE_DEFINES
    -DLOSCFG_BOX    # support syscall
    -DLOSCFG_SUPPORT_TA    # support TA
)

set(PRIVATE_DEFINES
)

if(${CONFIG_SUPPORT_TA} STREQUAL "y")
    list(APPEND PRIVATE_DEFINES ${TA_PRIVATE_DEFINES})
endif()

set(PUBLIC_DEFINES
    -DTEE_SEC_STORAGE
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

install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/include "*")

build_component()
