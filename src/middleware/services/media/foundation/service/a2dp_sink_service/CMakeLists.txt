#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
#===============================================================================
enable_language(CXX)
set(COMPONENT_NAME "a2dp_sink_service")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/a2dp_sink_service.cpp
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/../../audiomanager/frameworks/audiostream/include/cpp
)

set(PRIVATE_DEFINES
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
