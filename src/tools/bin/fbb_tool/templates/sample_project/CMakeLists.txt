#===============================================================================
# @brief    cmake entry — out-of-tree project template
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026. All rights reserved.
#===============================================================================

cmake_minimum_required(VERSION 3.14.1)

set(FBB_PROJECT_COMPONENT_NAME "samplepage")
# Option A: direct include (no CMake package registry needed)
#   include("$ENV{FBB_SDK_DIR}/build/cmake/project.cmake")
#
# Option B: find_package (used here — cleaner when the SDK path is well-known)
find_package(fbb REQUIRED HINTS 
    "$ENV{FBB_SDK_DIR}/build/cmake"
    "$ENV{FBB_SDK_ROOT_DIR}/build/cmake")

project(sample_project)
