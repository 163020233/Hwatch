#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================

enable_language(C)
set(COMPONENT_NAME "audio_utils")
if (${CHIP} MATCHES "socmn1|socmn2")
    set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/am_list.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/audio_utils.c
    )
elseif (${CHIP} MATCHES "brandy|3322")
    set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/am_list.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/audio_utils.c
    )
endif()

set(AUDIO_UTILS_HEADER
    ${ROOTDIR}/utils/dfx/log/include
    ${ROOTDIR}/middleware/chips/brandy/dfx/include
)

set(PUBLIC_HEADER
)


if(${CHIP_TYPE} MATCHES "socmn1|brandy")
    list(APPEND AUDIO_UTILS_HEADER
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${ROOTDIR}/open_source/openharmony/foundation/multimedia/utils/include
    )
else()
    list(APPEND AUDIO_UTILS_HEADER
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${ROOTDIR}/ohos/foundation/multimedia/media_utils_lite/interfaces/kits
    )
endif()

set(PRIVATE_HEADER
    ${AUDIO_UTILS_HEADER}
)

set(PRIVATE_DEFINES
    ${CDEFINES}
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_FLAGS ${ccflags})
list(APPEND COMPONENT_FLAGS -nostdlibinc)

set(COMPONENT_CCFLAGS
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib/media/${BOARD}_${APPLICATION}/${COMPONENT_NAME})

build_component()