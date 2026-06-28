#[[
Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
Description: CMake entries of sub-directories.
Author: Media Software Group
Create: 2021-03-15
]]

add_subdirectory(audiomanager)
add_subdirectory(audioservice)
add_subdirectory(audiostream)
add_subdirectory(audioutils)
if (${CHIP} MATCHES "socmn1|socmn2")
    add_subdirectory_if_exist(haid_manager)
    add_subdirectory_if_exist(anc_manager)
    add_subdirectory_if_exist(tws_manager)
endif()