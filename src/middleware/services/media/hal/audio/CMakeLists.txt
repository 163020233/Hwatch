#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "audio_hw")
if("${OS_TYPE}" STREQUAL "Linux")
    set(PROJECT_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../..)
    add_subdirectory(plugins/software)
else()
    add_subdirectory(plugins/brandy)
endif()
if("audio_bluetooth_port" IN_LIST RAM_COMPONENT)
    set(NEWDEFINES ${NEWDEFINES} "SUPPORT_BT")
endif()
if("audio_modem_port" IN_LIST RAM_COMPONENT)
    set(NEWDEFINES ${NEWDEFINES} "SUPPORT_MODEM")
endif()
if("audio_usb_port" IN_LIST RAM_COMPONENT)
    set(NEWDEFINES ${NEWDEFINES} "SUPPORT_USB")
endif()

# use build env and option camke
include(../../build/build_env.cmake)
include(../../build/build_option.cmake)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio_hal_manager.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio_adapter.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio_capture.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio_render.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio_plugins_manager/src/audio_plugins_manager.c
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio_plugins_manager/include
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/plugin_include
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/brandy/primary
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/brandy/bluetooth
    ${OHOS_DIR}/drivers/peripheral/audio/interfaces/include
)

set(PRIVATE_DEFINES  
)

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