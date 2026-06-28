#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================

enable_language(CXX)
set(COMPONENT_NAME "audio_manager")
if (${CHIP} MATCHES "socmn1|socmn2")
    if ("MELODY_DEMO_BMIC_RX" IN_LIST DEFINES OR "MELODY_DEMO_BMIC_TX" IN_LIST DEFINES)
        set(SOURCES
            ${CMAKE_CURRENT_SOURCE_DIR}/c/audio_manager.c
        )
    else()
        set(SOURCES
            ${CMAKE_CURRENT_SOURCE_DIR}/c/audio_callback_dispatcher.c
            ${CMAKE_CURRENT_SOURCE_DIR}/c/audio_manager.c
        )
        set(CDEFINES ${CDEFINES} "SOPPORT_DISPATCHER_CALL_BACK")
        set(PRIVATE_DEFINES
            ${CDEFINES}
        )
    endif()

    set(PUBLIC_HEADER
        ${CMAKE_CURRENT_SOURCE_DIR}/../../interfaces/kits/audiocommon/
        ${CMAKE_CURRENT_SOURCE_DIR}/../../interfaces/kits/audiomanager/c
        
    )
    
    set(PUBLIC_DEFINES
    )
elseif (${CHIP} MATCHES "brandy|3322")
    set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/cpp/audio_callback_dispatcher.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/cpp/audio_manager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/cpp/audio_manager_impl.cpp
    )

    set(PUBLIC_HEADER
        ${CMAKE_CURRENT_SOURCE_DIR}/../../interfaces/kits/audiocommon/
        ${CMAKE_CURRENT_SOURCE_DIR}/../../interfaces/kits/audiomanager/cpp
    )
    set(PRIVATE_DEFINES
    )
    set(PUBLIC_DEFINES
        "SUPPORT_MEDIA_AUDIO_MANAGER"
    )
endif()
set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../frameworks/audioutils/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../frameworks/audioservice/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../utils/interfaces/kits
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

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib/media/${BOARD}_${APPLICATION}/${COMPONENT_NAME})

build_component()