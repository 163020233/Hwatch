#[[
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
# Description: CMake entries of sub-directories.
# Create: 2021-03-15
# ]]
add_subdirectory_if_exist(dpu)
add_subdirectory_if_exist(vau)
if("SUPPORT_GPU_GMMU" IN_LIST DEFINES)
    add_subdirectory_if_exist(gmmu)
endif()
if("SUPPORT_GPU_JPEG" IN_LIST DEFINES)
    add_subdirectory_if_exist(jpeg)
endif()
add_subdirectory_if_exist(comm)
add_subdirectory_if_exist(include)
