#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd.. 2023. All rights reserved.
#===============================================================================
if(${CHIP} MATCHES "3322" OR "BGH_WSTP" IN_LIST DEFINES)
    set(MODULE_NAME "bgh")
else()
    set(MODULE_NAME "bth")
endif()
set(AUTO_DEF_FILE_ID FALSE)
set(COMPONENT_NAME "bt_host")

set(BTH_RAM_LIST  "" CACHE INTERNAL "" FORCE)
set(BTA_RAM_LIST  "" CACHE INTERNAL "" FORCE)

set(BTH_PUBLIC_HDR_LIST  "" CACHE INTERNAL "" FORCE)
set(BTH_PRIVATE_HDR_LIST  "" CACHE INTERNAL "" FORCE)

set(BTH_ROM_LIST  "" CACHE INTERNAL "" FORCE)
set(BTA_ROM_LIST  "" CACHE INTERNAL "" FORCE)

if(DEFINED ROM_COMPONENT OR DEFINED ROM_SYM_PATH)
    set(BT_ROM_VERSION true)
else()
    set(BT_ROM_VERSION false)
endif()

if("${CHIP}" MATCHES "brandy|bs27a")
    add_subdirectory_if_exist(ahi/ahi_b)
endif()

add_subdirectory_if_exist(host)
add_subdirectory_if_exist(util)

if("${BTH_RAM_LIST}" STREQUAL "")
    if(DEFINED CONFIG_SLE_BLE_SUPPORT AND SUPPORT_MULTI_LIBS IN_LIST DEFINES)
        set(LIBS ${CMAKE_CURRENT_SOURCE_DIR}/${PKG_CHIP}-${CONFIG_SLE_BLE_SUPPORT}/lib${COMPONENT_NAME}.a)
    elseif(DEFINED CONFIG_SUPPORT_SLE_BLE_CENTRAL_DEFAULT)
        set(LIBS ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET_COMMAND}/lib${COMPONENT_NAME}.a)
    else()
        set(BTH_RAM_LIST "__null__")
    endif()
endif()
set(SOURCES
    ${BTH_RAM_LIST}
    ${BTS_RAM_LIST}
)

if(${CHIP} MATCHES "bs27a")
    set(PUBLIC_HEADER
        ${BTH_PUBLIC_HDR_LIST}
        ${CMAKE_CURRENT_SOURCE_DIR}/ahi/ahi_b/ahi_vendor/inc
        ${CMAKE_CURRENT_SOURCE_DIR}/ahi/ahi_b/ahi_common/inc
        ${CMAKE_CURRENT_SOURCE_DIR}/host/stack/include/br
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ble/L0
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ble
        ${CMAKE_CURRENT_SOURCE_DIR}/include/common/L0
        ${CMAKE_CURRENT_SOURCE_DIR}/include/common/bs27a #bs27a优先编译自己的文件夹
        ${CMAKE_CURRENT_SOURCE_DIR}/include/common
        ${CMAKE_CURRENT_SOURCE_DIR}/include/br/L0
        ${CMAKE_CURRENT_SOURCE_DIR}/include/br
    )
else()
    set(PUBLIC_HEADER
        ${BTH_PUBLIC_HDR_LIST}
        ${CMAKE_CURRENT_SOURCE_DIR}/ahi/ahi_b/ahi_vendor/inc
        ${CMAKE_CURRENT_SOURCE_DIR}/ahi/ahi_b/ahi_br/inc
        ${CMAKE_CURRENT_SOURCE_DIR}/ahi/ahi_b/ahi_common/inc
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ble/L0
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ble
        ${CMAKE_CURRENT_SOURCE_DIR}/include/br/L0
        ${CMAKE_CURRENT_SOURCE_DIR}/include/br
        ${CMAKE_CURRENT_SOURCE_DIR}/include/common/L0
        ${CMAKE_CURRENT_SOURCE_DIR}/include/common/common
        ${CMAKE_CURRENT_SOURCE_DIR}/include/common
        ${CMAKE_CURRENT_SOURCE_DIR}/sdk/L2/common
        ${CMAKE_CURRENT_SOURCE_DIR}/sdk/L2/br
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../middleware/chips/socmn2/product/tws/
)
endif()

if(${CHIP} MATCHES "3322" OR "BGH_WSTP" IN_LIST DEFINES)
    set(PRIVATE_HEADER
        ${PRIVATE_HEADER}
        ${BTH_PRIVATE_HDR_LIST}
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/log/3322
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/host
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/os/include
    )
elseif(${CHIP} MATCHES "bs27a")
    set(PRIVATE_HEADER
        ${PRIVATE_HEADER}
        ${BTH_PRIVATE_HDR_LIST}
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/log/bs27a
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/host
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/os/include
    )
endif()

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

set(COMPONENT_NAME "bt_host_rom")

if("${BTH_ROM_LIST}" STREQUAL "")
    set(BTH_ROM_LIST "__null__")
endif()

set(SOURCES
    ${BTH_ROM_LIST}
)

set(PRIVATE_HEADER
    ${BTH_PRIVATE_HEADER_LIST}
    ${CMAKE_CURRENT_SOURCE_DIR}/../private/gle/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../private/gle/host/stack/include
)

set(PUBLIC_HEADER
    ${BTH_PUBLIC_HDR_LIST}
)

set(PRIVATE_DEFINES
    BUILD_ROM_COMPILE
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
MESSAGE("BTH_ROM_LIST:" ${BTH_ROM_LIST})
set(LIB_OUT_PATH ${BIN_DIR}/${CHIP}/libs/bluetooth/bth/${TARGET_COMMAND})

build_component()
