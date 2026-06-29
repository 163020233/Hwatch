#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
add_subdirectory_if_exist(battery)
add_subdirectory_if_exist(brandy_demo)
add_subdirectory_if_exist(charger)
add_subdirectory_if_exist(dut)
add_subdirectory_if_exist(monitor)
add_subdirectory_if_exist(ota)
if(EXISTS ${ROOT_DIR}/../samples/native_samples/CMakeLists.txt)
    # 外部源码，强制指定二进制构建目录
    set(SAMPLES_OUTER_DIR ${ROOT_DIR}/../samples/native_samples)
    get_filename_component(SAMPLES_OUTER_ABS ${ROOT_DIR}/../samples/native_samples ABSOLUTE)
    add_subdirectory("${SAMPLES_OUTER_ABS}" "${CMAKE_BINARY_DIR}/outer_samples_build")
endif()
add_subdirectory_if_exist(samples)
add_subdirectory_if_exist(ux)
add_subdirectory_if_exist(wstp)
add_subdirectory_if_exist(cfbb)
add_subdirectory_if_exist(usb)
add_subdirectory_if_exist(remote_mt_app)

if("${CHIP}" STREQUAL "socmn1" OR "${CHIP}" STREQUAL "socmn2")
    add_subdirectory_if_exist(melody)
else()
    add_subdirectory_if_exist(${CHIP})
endif()

if("${CHIP}" STREQUAL "socmn1" OR "${CHIP}" STREQUAL "brandy" OR "${CHIP}" STREQUAL "socmn2" OR "${CHIP}" STREQUAL "hi2132")
    add_subdirectory_if_exist(audio)
endif()

if (${CHIP} STREQUAL "sw21" OR ${CHIP} STREQUAL "3322" OR ${CHIP} STREQUAL "sw22")
    if (IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/audio)
        add_subdirectory(audio)
    elseif(EXISTS ${ROOT_DIR}/build/config/target_config/${CHIP}/cmake/audio.cmake)
        include(${ROOT_DIR}/build/config/target_config/${CHIP}/cmake/audio.cmake)
    endif()
endif()

add_subdirectory_if_exist(wearable)
