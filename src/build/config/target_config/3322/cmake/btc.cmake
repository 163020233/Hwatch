#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
#===============================================================================
set(COMPONENT_LIST "bgtp" "bgtp_rom")

foreach(COMPONENT ${COMPONENT_LIST})
    set(COMPONENT_NAME ${COMPONENT})
    if (NOT ${COMPONENT_NAME} IN_LIST TARGET_COMPONENT)
        continue()
    endif()

    set(LIBS ${BIN_DIR}/${CHIP}/libs/bluetooth/btc/${TARGET_COMMAND}${PRE_ROM}/lib${COMPONENT_NAME}.a)
    set(WHOLE_LINK true)
    build_component()
    install_sdk(${BIN_DIR}/${CHIP}/libs/bluetooth/btc/${TARGET_COMMAND} "*.a")
endforeach()