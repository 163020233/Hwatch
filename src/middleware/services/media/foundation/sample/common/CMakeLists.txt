#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================

enable_language(C)
set(COMPONENT_NAME "stream_source_common")
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/aac.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/avc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/jpg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/stream.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/stream_file.c
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../utils/interfaces/kits
)

set(PRIVATE_DEFINES
    ${CDEFINES}
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib/media/${BOARD}_${APPLICATION}/${COMPONENT_NAME})

build_component()