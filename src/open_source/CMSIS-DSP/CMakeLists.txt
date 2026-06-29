# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

# This CMakeLists.txt in the root of the repository ensures that, when another
# project fetches CMSIS-DSP by calling FetchContent_MakeAvailable(), the call
# automatically adds CMSIS-DSP as a subdirectory. To maintain compatibility with
# existing projects that assume CMSIS-DSP's entry point to be Source/, we only
# add a redirection in the root CMakeLists.txt.
if (NOT DEFINED RISCVV)
option(RISCVV "RISCV V extension support" OFF)
endif()

if (COMMAND build_component)
# build_component
add_subdirectory(riscv/Source)
else()
#build lib
cmake_minimum_required (VERSION 3.14)
cmake_policy(SET CMP0077 NEW)
project(CMSISDSP)
if (RISCVV)
add_subdirectory(riscv/Source)
else()
add_subdirectory(Source)
endif()

endif()

