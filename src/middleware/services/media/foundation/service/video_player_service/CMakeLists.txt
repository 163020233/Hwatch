#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
#===============================================================================
enable_language(CXX)
set(COMPONENT_NAME "video_player_service")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/video_player_service.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/media_video_player.cpp
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/../../audiomanager/frameworks/audiostream/include/cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set(OPENMG_DIR ${ROOT_DIR}/ohos)
if (${CHIP} MATCHES "brandy|3322")
    set(PRIVATE_HEADER_COM ${PRIVATE_HEADER})
    list(APPEND PRIVATE_HEADER_COM
        ${CMAKE_CURRENT_SOURCE_DIR}/../../utils/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../player/interfaces/kits
        ${ROOT_DIR}/middleware/services/media/hal/camera/include
        ${OPENMG_DIR}/drivers/peripheral/display/interfaces/include
    )
    if ("ENABLE_LVGL" IN_LIST DEFINES)
        set(LVGL_DIR ${ROOT_DIR}/middleware/services/gui/lvgl)
        list(APPEND PRIVATE_HEADER_COM ${CMAKE_CURRENT_SOURCE_DIR}
            ${LVGL_DIR}
            ${LVGL_DIR}/src
            ${LVGL_DIR}/src/widgets
            ${LVGL_DIR}/config
            ${LVGL_DIR}/proprietary/service
            ${LVGL_DIR}/proprietary/src/utils
            ${LVGL_DIR}/proprietary/include/font
            ${LVGL_DIR}/proprietary/include/widgets
        )
    elseif ("ENABLE_UIKIT" IN_LIST DEFINES)
        set(UIKIT_DIR ${ROOT_DIR}/middleware/services/gui/uikit)
        list(APPEND PRIVATE_HEADER_COM ${CMAKE_CURRENT_SOURCE_DIR}
            ${UIKIT_DIR}/ui/interfaces/innerkits
            ${UIKIT_DIR}/utils/interfaces/kits
            ${UIKIT_DIR}/utils/interfaces/innerkits
            ${UIKIT_DIR}/surface/interfaces/kits
            ${UIKIT_DIR}/ui/interfaces/kits
            ${UIKIT_DIR}/proprietary/src/utils/innerkits
            ${UIKIT_DIR}/proprietary/include
        )
    endif()
    set(PRIVATE_HEADER
        ${PRIVATE_HEADER_COM}
    )
endif()

set(CCDEFINES ${CCDEFINES} "_GNU_SOURCE" "_XOPEN_SOURCE=700")
list(REMOVE_ITEM CCDEFINES VERSION_STANDARD)
set(PRIVATE_DEFINES
    ${CCDEFINES}
)
set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

if (${CHIP} MATCHES "brandy|3322")
    set(COMPONENT_FLAGS)
    list(REMOVE_ITEM COMPONENT_FLAGS -std=gnu99 -std=c99 -std=gnu11 -std=c11)
    list(APPEND COMPONENT_FLAGS -std=c++11 -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables -nostdlibinc)
endif()
set(COMPONENT_CCFLAGS
    ${COMPONENT_FLAGS}
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib/media/${BOARD}_${APPLICATION})

build_component()
