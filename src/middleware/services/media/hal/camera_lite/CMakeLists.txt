#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
#===============================================================================

set(COMPONENT_NAME "camera_hal")

# use build env and option camke
include(../../build/build_env.cmake)
include(../../build/build_option.cmake)


set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_camera.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_camera_pipe.c
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/middleware/utils/common_headers
    ${ROOT_DIR}/ohos/foundation/multimedia/media_utils_lite/hals

    # drv_mcp_ext.h
    ${ROOT_DIR}/middleware/utils/common_headers/mcp/internal

    # drv_venc_intf.h
    ${ROOT_DIR}/middleware/utils/common_headers/venc/internal

    # mpi_viu_ext.h
    ${ROOT_DIR}/middleware/utils/common_headers/mcp/external/viu
    ${ROOT_DIR}/middleware/utils/common_headers/mcp/internal/gdc

    # mpi_cis_ext.h
    ${ROOT_DIR}/middleware/utils/common_headers/mcp/external/cis

    # dfx/dfx_traceui.h
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/include
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

set(AUTO_ADD_PATH
    false
)

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib/media/${BOARD}_${APPLICATION}/${OS}/${COMPONENT_NAME})

build_component()
