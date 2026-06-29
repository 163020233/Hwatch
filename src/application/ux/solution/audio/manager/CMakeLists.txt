#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "ux_audio_manager")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/ux_audio_manager.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ux_audio_common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ux_audio_manager_adapt.c
)

if("MELODY_DEMO_EARPHONE" IN_LIST DEFINES OR "MELODY_DEMO_HA" IN_LIST DEFINES OR "MELODY_DEMO_SOUND_BOX" IN_LIST DEFINES)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_audio_sound_trigger.c
    )
endif()

set(PUBLIC_HEADER
    
)

set(PRIVATE_HEADER
    ${UX_INTERNAL_HEADERS}
    ${ROOT_DIR}/middleware/services/media/foundation/audiomanager/interfaces/kits/ancmanager
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Werror=all
    -Wconversion
    -Werror=incompatible-pointer-types
    -Werror=unused-parameter
    -Werror=enum-conversion
    -Werror=sign-compare
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
