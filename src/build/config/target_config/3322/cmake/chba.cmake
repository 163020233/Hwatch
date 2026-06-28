#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
#===============================================================================
set(COMPONENT_LIST "sle_netdev" "chba_at")

foreach(COMPONENT ${COMPONENT_LIST})
    set(COMPONENT_NAME ${COMPONENT})
    if (NOT ${COMPONENT_NAME} IN_LIST TARGET_COMPONENT)
        continue()
    endif()

    set(LIBS ${BIN_DIR}/${CHIP}/libs/bluetooth/chba/${TARGET_COMMAND}${PRE_ROM}/lib${COMPONENT_NAME}.a)
    set(WHOLE_LINK true)
    build_component()
    install_sdk(${BIN_DIR}/${CHIP}/libs/bluetooth/chba/${TARGET_COMMAND} "*.a")
endforeach()