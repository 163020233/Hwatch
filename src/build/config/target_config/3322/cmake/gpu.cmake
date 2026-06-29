#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
#===============================================================================

if (EXISTS ${ROOT_DIR}/middleware/services/gpu/CMakeLists.txt)
set(COMPONENT_LIST "gfx_rm")
else()
set(COMPONENT_LIST "gpu" "gfx_rm")
endif()

set(LIBRARY_OUTPUT_PATH ${BIN_DIR}/${CHIP}/libs/gpu/${TARGET_COMMAND})
foreach(COMPONENT ${COMPONENT_LIST})
    set(COMPONENT_NAME ${COMPONENT})
    if (NOT ${COMPONENT_NAME} IN_LIST TARGET_COMPONENT)
        continue()
    endif()
    find_library(LIBS${COMPONENT_NAME} "lib${COMPONENT_NAME}.a" ${LIBRARY_OUTPUT_PATH})
    target_link_libraries(${TARGET_NAME} PRIVATE -Wl,--whole-archive  ${LIBS${COMPONENT_NAME}} -Wl,--no-whole-archive )
    build_component()
    install_sdk(${BIN_DIR}/${CHIP}/libs/gpu/${TARGET_COMMAND} "*.a")
endforeach()
