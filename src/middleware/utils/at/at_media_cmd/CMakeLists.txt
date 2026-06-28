#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================


set(COMPONENT_NAME "media_at")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/at/at_media.c
    ${CMAKE_CURRENT_SOURCE_DIR}/at/media_at_process.c
    ${CMAKE_CURRENT_SOURCE_DIR}/at/media_cmd_manager.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/at_media_cmd_register.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/at
    ${ROOT_DIR}/middleware/utils/at/at
    ${ROOT_DIR}/middleware/utils/at/at/include
    ${ROOT_DIR}/middleware/utils/at/at/src
    ${ROOT_DIR}/middleware/utils/at/wstp_at/inc
    ${ROOT_DIR}/drivers/drivers/driver/audio/include
    ${ROOT_DIR}/middleware/services/media/foundation/sample/wrapper
    ${ROOT_DIR}/middleware/services/media/foundation/audiomanager/interfaces/kits/audiocommon
    ${ROOT_DIR}/middleware/utils/common_headers/native
)

if("audio_modem_port" IN_LIST RAM_COMPONENT)
    set(CDEFINES ${CDEFINES} "MEDIA_VOICE_CALL_VOLTE_AT")
endif()

set(PRIVATE_DEFINES
    ${CDEFINES}
)

set(PUBLIC_DEFINES
    AT_COMMAND_MEDIA
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