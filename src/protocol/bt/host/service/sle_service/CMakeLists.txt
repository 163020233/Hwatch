#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2025. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "sle_service")
set(MODULE_NAME "bgh")

set(SLE_RAM_LIST  "" CACHE INTERNAL "" FORCE)
set(SLE_PUB_HDR_LIST "" CACHE INTERNAL "" FORCE)
set(SLE_PRI_HDR_LIST "" CACHE INTERNAL "" FORCE)

add_subdirectory_if_exist(include)
add_subdirectory_if_exist(init)
add_subdirectory_if_exist(as)
add_subdirectory_if_exist(at)
add_subdirectory_if_exist(autorate)
add_subdirectory_if_exist(battery)
add_subdirectory_if_exist(call)
add_subdirectory_if_exist(media)
add_subdirectory_if_exist(manager)
add_subdirectory_if_exist(connection)
add_subdirectory_if_exist(device)
add_subdirectory_if_exist(config)
add_subdirectory_if_exist(acquire)
add_subdirectory_if_exist(util)
add_subdirectory_if_exist(subprocess)
add_subdirectory_if_exist(port)
add_subdirectory_if_exist(persistence)
add_subdirectory_if_exist(volume)
add_subdirectory_if_exist(dis)
add_subdirectory_if_exist(ssap)

set(SOURCES
    ${SLE_RAM_LIST}
)

set(PUBLIC_HEADER
    ${SLE_PUB_HDR_LIST}
    ${CMAKE_CURRENT_SOURCE_DIR}/
)

if (${CHIP} MATCHES "socmn2")
    list(APPEND PUBLIC_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/compile_config/socmn2
    )
elseif (${CHIP} MATCHES "3322")
    list(APPEND PUBLIC_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/compile_config/3322
    )
endif()

set(PRIVATE_HEADER
    ${SLE_PRI_HDR_LIST}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/sle/L2
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/sle/sle_service
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/sle/sle_audio
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/sle/sdm
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/bs_service
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../middleware/utils/at/at_bt_cmd/src/
    ${CMAKE_CURRENT_SOURCE_DIR}/config/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../include/driver/security_unified
)

set(LOG_DEF
    ${CMAKE_CURRENT_SOURCE_DIR}/include/log_def_sle_service_bgh.h
)

MESSAGE(STATUS "sle service sources: ${SOURCES}")
MESSAGE(STATUS "sle service public header: ${PUBLIC_HEADER}")
MESSAGE(STATUS "sle service private header: ${PRIVATE_HEADER}")

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
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

set(LIB_OUT_PATH ${BIN_DIR}/${CHIP}/libs/bluetooth/bth/${TARGET_COMMAND})
build_component()
