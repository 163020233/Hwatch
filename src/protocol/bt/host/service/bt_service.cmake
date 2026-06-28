#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2025. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "bt_service")
set(MODULE_NAME "bgh")

set(BTH_RAM_LIST  "" CACHE INTERNAL "" FORCE)
set(BTH_PUB_HDR_LIST "" CACHE INTERNAL "" FORCE)
set(BTH_PRI_HDR_LIST "" CACHE INTERNAL "" FORCE)

add_subdirectory_if_exist(util)
add_subdirectory_if_exist(dft)
add_subdirectory_if_exist(ble_service)
add_subdirectory_if_exist(br_service)

set(SOURCES
    ${BTH_RAM_LIST}
)

MESSAGE("BTH_PUB_HDR_LIST=" ${BTH_PUB_HDR_LIST})
MESSAGE("BTH_RAM_LIST=" ${BTH_RAM_LIST})

set(PUBLIC_HEADER
    ${BTH_PUB_HDR_LIST}
    ${CMAKE_CURRENT_SOURCE_DIR}/
)

if(${CHIP} MATCHES "3322")
    set(PRIVATE_HEADER
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/log/3322
    )
elseif(${CHIP} MATCHES "socmn2")
    set(PRIVATE_HEADER
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/log/socmn2
    )
endif()

set(PRIVATE_HEADER
    ${PRIVATE_HEADER}
    ${BTH_PRI_HDR_LIST}
    ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/host
    ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/os/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/bs_service
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/sle/sle_service
)

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
