#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================

enable_language(C)
set(COMPONENT_NAME "media_chr")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/media_chr_log.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(PRIVATE_HEADER
)
if("${CHIP}" STREQUAL "3322")
    set(CDEFINES ${CDEFINES} "MEDIA_CHR_SUPPORT")
endif()
set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
   ${CDEFINES}
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