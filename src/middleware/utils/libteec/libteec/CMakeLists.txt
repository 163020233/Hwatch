#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "libteec")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/tee_client_api.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/kernel/seliteos/kernel/base/communication/tesvec/include
)

set(PRIVATE_DEFINES
    -DGP_BENCHMARK=0
    -DMAX_TEMP_BUFFER_LEN=1024
)

set(PUBLIC_DEFINES
    -DLIBTEEC_SUPPORT
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
