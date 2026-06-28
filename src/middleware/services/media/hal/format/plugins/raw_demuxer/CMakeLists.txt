#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "plugin_demuxer_raw")

# use build env and option camke
include(../../../../build/build_env.cmake)
include(../../../../build/build_option.cmake)

if (${CHIP} MATCHES "brandy|3322")
    set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/raw_format.c
        ${CMAKE_CURRENT_SOURCE_DIR}/silk_demuxer.c
        ${CMAKE_CURRENT_SOURCE_DIR}/sbc_demuxer.c
    )
elseif (${CHIP} MATCHES "socmn1|socmn2")
    set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/raw_format.c
        ${CMAKE_CURRENT_SOURCE_DIR}/sbc_demuxer.c
    )
endif()

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../common/include
)

set(NEWDEFINES "ENABLE_PLUGIN_DEMUXER_RAW")
if(${CHIP} MATCHES "brandy|3322")
    set(NEWDEFINES ${NEWDEFINES} "SILK_FILE_SUPPORT")
endif()
set(PUBLIC_DEFINES
    ${NEWDEFINES}
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
    false
)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib/media/${BOARD}_${APPLICATION}/${OS}/${COMPONENT_NAME})

build_component()