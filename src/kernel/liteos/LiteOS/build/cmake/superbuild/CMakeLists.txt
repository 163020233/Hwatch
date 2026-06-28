cmake_minimum_required(VERSION 3.16.5)
project(integrate)
string(REGEX MATCH "(^.*)/build/cmake/superbuild$" RTOS_LITE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(RTOS_LITE_SOURCE_DIR ${CMAKE_MATCH_1})
set(LITEOS_SOURCE_DIR ${RTOS_LITE_SOURCE_DIR}/self_src)
message("RTOS_LITE_SOURCE_DIR: ${RTOS_LITE_SOURCE_DIR}")
message("LITEOS_SOURCE_DIR: ${LITEOS_SOURCE_DIR}")

include(ExternalProject)

message("PATH = $ENV{PATH}")
MESSAGE(STATUS CMAKE_BINARY_DIR "${CMAKE_BINARY_DIR}")
message("CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")

if (CMAKE_INSTALL_PREFIX STREQUAL /usr/local)
    set(CMAKE_INSTALL_PREFIX ${RTOS_LITE_SOURCE_DIR}/output CACHE STRING "path for install()" FORCE)
    message(STATUS "No install prefix selected, default to ${CMAKE_INSTALL_PREFIX}.")
endif()

set(SUPERBUILD_PATH ${RTOS_LITE_SOURCE_DIR}/build/cmake/superbuild)
include(${SUPERBUILD_PATH}/ascend.cmake)
include(${SUPERBUILD_PATH}/kirin.cmake)
