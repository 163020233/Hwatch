#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
add_subdirectory_if_exist(chips)
add_subdirectory_with_alias_if_exist(services ${PROJECT_BINARY_DIR}/services)
add_subdirectory_with_alias_if_exist(utils ${PROJECT_BINARY_DIR}/utils)

if (${CHIP} STREQUAL "sw22")
    if (EXISTS ${ROOT_DIR}/middleware/ims/CMakeLists.txt)
        add_subdirectory(ims)
    elseif(EXISTS ${ROOT_DIR}/libs_url/${CHIP}/cmake/ims.cmake)
        include(${ROOT_DIR}/libs_url/${CHIP}/cmake/ims.cmake)
    endif()
endif()