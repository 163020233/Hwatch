#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
#===============================================================================

set(COMPONENT_LIST "bg_common" "bth_gle" "bt_app" "bt_host" "bth_audio_manager_wrapper" "bth_sample" "bth_saf" "bth_saf_private")

foreach(COMPONENT ${COMPONENT_LIST})
    set(COMPONENT_NAME ${COMPONENT})
    if (NOT ${COMPONENT_NAME} IN_LIST TARGET_COMPONENT)
        continue()
    endif()

    set(LIBS ${BIN_DIR}/${CHIP}/libs/bluetooth/bth/${TARGET_COMMAND}/lib${COMPONENT_NAME}.a)
    set(WHOLE_LINK true)
    build_component()
    install_sdk(${BIN_DIR}/${CHIP}/libs/bluetooth/bth/${TARGET_COMMAND} "*.a")
endforeach()