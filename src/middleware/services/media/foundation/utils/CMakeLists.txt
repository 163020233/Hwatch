#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================

enable_language(CXX)

set(COMPONENT_NAME "media_common")
if(${CHIP} MATCHES "socmn1|socmn2")
    set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/c/media_log.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/c/media_thread_adapt.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/c/media_mem.c
    )
elseif (${CHIP} MATCHES "brandy|3322")
    set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/format.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/cpp/media_log.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/cpp/media_thread_adapt.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/source.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/cpp/media_mem.cpp
    )
endif()
    set(PUBLIC_HEADER
        ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/kits
    )

    set(PRIVATE_HEADER
    )

    set(CCDEFINES ${CCDEFINES} "_GNU_SOURCE" "_XOPEN_SOURCE=700" "CMSIS_SUPPORT")

    if(${CHIP} MATCHES "socmn1|socmn2")
        set(CCDEFINES ${CCDEFINES} "CMSIS_SUPPORT_JOIN")
    endif()

    if (${CHIP} MATCHES "3322" OR DEFINED BUILD_TIME)
        set(CCDEFINES ${CCDEFINES} "ENABLE_INFO_LEVEL_LOG")
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
