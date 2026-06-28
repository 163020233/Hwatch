#[[
Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd.. 2021-2021. All rights reserved.
Description: CMake entries of sub-directories.
Author: Media Software Group
Create: 2021-03-15
]]
cmake_minimum_required(VERSION 3.10)
add_subdirectory_if_exist(common)
add_subdirectory_if_exist(audio)
add_subdirectory_if_exist(format)
if (${CHIP} MATCHES "brandy|3322")
    add_subdirectory_if_exist(codec)
    add_subdirectory_if_exist(camera)
    add_subdirectory_if_exist(camera_lite)
endif()