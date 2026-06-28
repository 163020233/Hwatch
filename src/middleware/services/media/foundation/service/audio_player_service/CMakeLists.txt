#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
#===============================================================================
enable_language(CXX)
set(COMPONENT_NAME "audio_player_service")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio_player_service.cpp
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if (${CHIP} MATCHES "brandy|3322")
    set(PRIVATE_HEADER_COM 
        ${CMAKE_CURRENT_SOURCE_DIR}/../../audiomanager/frameworks/audiostream/include/cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../../utils/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../player/interfaces/kits
    )
    set(PRIVATE_HEADER
        ${PRIVATE_HEADER_COM}
    )
    set(CCDEFINES ${CCDEFINES} "HMF_DECRYPT_DATA_ENABLE")
    if ("CONFIG_SUPPORT_MEDIA_MEMORY_MINI" IN_LIST DEFINES)
        list(REMOVE_ITEM CCDEFINES "HMF_DECRYPT_DATA_ENABLE")
    endif()
endif()

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
