#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
#===============================================================================
enable_language(CXX)
set(COMPONENT_NAME "stream_source_player_service")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/stream_source_player_service.cpp
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set(PRIVATE_HEADER
)

set(PRIVATE_HEADER_COM ${PRIVATE_HEADER})
list(APPEND PRIVATE_HEADER_COM
    ${CMAKE_CURRENT_SOURCE_DIR}/../../utils/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../player/interfaces/kits)
if ("ENABLE_UIKIT" IN_LIST DEFINES)
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

set(COMPONENT_FLAGS)
list(REMOVE_ITEM COMPONENT_FLAGS -std=gnu99 -std=c99 -std=gnu11 -std=c11)
list(APPEND COMPONENT_FLAGS -std=c++11 -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables -nostdlibinc)

set(COMPONENT_CCFLAGS
    ${COMPONENT_FLAGS}
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
