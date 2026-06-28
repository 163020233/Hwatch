#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "psa_storage")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/psa_ps_file.c
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/kernel/seliteos/kernel/service/psa/ioctl
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
    -DPSA_STORAGE_SUPPORT
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
