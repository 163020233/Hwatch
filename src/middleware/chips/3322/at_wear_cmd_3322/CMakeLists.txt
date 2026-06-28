#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================


set(COMPONENT_NAME "wear_at_3322")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/at/at_wear.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/at_wear_cmd_register.c
    ${CMAKE_CURRENT_SOURCE_DIR}/at/at_wear_fs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_data_trans/sle_receiver.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_data_trans/sle_transmitter.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_data_trans/sle_trans_at_init.c
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/middleware/utils/at/at
    ${ROOT_DIR}/middleware/utils/at/at/include
    ${ROOT_DIR}/middleware/utils/at/at/src
    ${ROOT_DIR}/middleware/utils/at/at_wifi_cmd/at
    ${ROOT_DIR}/application/wearable/service/heart_rate
    ${ROOT_DIR}/application/wearable/service/gesture
    ${ROOT_DIR}/application/3322/3322_ai_engine/ai_sample/public/fuzzy_command_word/inc
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_data_trans/include
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
    AT_COMMAND_WEAR
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
    true
)

build_component()
