#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "tee_psa_storage")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/dispatch_psa_storage.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/middleware/utils/psa/storage
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
