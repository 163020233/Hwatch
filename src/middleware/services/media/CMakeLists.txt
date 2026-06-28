#[[
Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd.. 2023-2023. All rights reserved.
Description: CMake entries of sub-directories.
Author: Media Software Group
Create: 2023-03-25
]]

set(PUBLIC_HEADER

)

set(COMPONENT_PUBLIC_CCFLAGS

)

set(CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES "")

set(ROOTDIR ${ROOT_DIR})
if (RAM_COMPONENT_SET MATCHES "media")
    set(MODULE_NAME "media")
    set(AUTO_DEF_FILE_ID TRUE)
    add_subdirectory_if_exist(dfx/chr)
    add_subdirectory_if_exist(foundation)
    set(COMPONENT_FFMPEG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/component)
    if (EXISTS ${COMPONENT_FFMPEG_DIR})
        add_subdirectory(${COMPONENT_FFMPEG_DIR})
    endif()
    set(COMPONENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/component/hmf_demuxer)
    if (EXISTS ${COMPONENT_DIR})
        add_subdirectory(${COMPONENT_DIR})
    endif()
    set(HAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/hal)
    if (EXISTS ${HAL_DIR})
        add_subdirectory(${HAL_DIR})
    endif()
    # hal_release depends hal, so the position should be after hal
if(NOT "${CHIP}" STREQUAL "socmn1")
    add_subdirectory(hal_release)
    install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/hal_release/CMakeLists.txt "*.txt")
endif()
endif()