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
set(COMPONENT_NAME "bt_app")
set(BTA_RAM_LIST  "" CACHE INTERNAL "" FORCE)
set(BTH_PUBLIC_HDR_LIST  "" CACHE INTERNAL "" FORCE)
set(BTH_PRIVATE_HDR_LIST  "" CACHE INTERNAL "" FORCE)

if(${CHIP} MATCHES "ws53|ws63|bs20|bs21|bs21e|bs21a|bs22|bs26") # 单双核差异
    if(${CHIP} MATCHES "ws53|ws63|bs27a") # 新项目使用
        add_subdirectory_if_exist(dft)
        add_subdirectory_if_exist(service)
    else()
        add_subdirectory_if_exist(dft)
        add_subdirectory_if_exist(service/bs2x)
    endif()
elseif(${CHIP} MATCHES "bs25")
    add_subdirectory_if_exist(dft)
    add_subdirectory_if_exist(service/bs2x)
    add_subdirectory_if_exist(ahi/ahi_a)
elseif(${CHIP} MATCHES "bs27a")
    add_subdirectory_if_exist(dft)
    add_subdirectory_if_exist(service)
    add_subdirectory_if_exist(ahi/ahi_a)
else()
    add_subdirectory_if_exist(dft)
    add_subdirectory_if_exist(service)
    add_subdirectory_if_exist(ahi/ahi_a)
endif()

# for sdk closed_component compile
if("${BTA_RAM_LIST}" STREQUAL "")
    if(DEFINED CONFIG_SLE_BLE_SUPPORT AND SUPPORT_MULTI_LIBS IN_LIST DEFINES)
        set(LIBS ${CMAKE_CURRENT_SOURCE_DIR}/${PKG_CHIP}-${CONFIG_SLE_BLE_SUPPORT}/lib${COMPONENT_NAME}.a)
    elseif(DEFINED CONFIG_SUPPORT_SLE_BLE_CENTRAL_DEFAULT)
        set(LIBS ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET_COMMAND}/lib${COMPONENT_NAME}.a)
    else()
        set(SOURCES "__null__")
    endif()
else()
    set(SOURCES
        ${BTA_RAM_LIST}
    )
    if(${CHIP} MATCHES "bs20|bs21|bs21e|bs21a|bs22|bs26")
        set(LOG_DEF
            ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/log/bs21/log_def_sdk_bth.h
        )
    elseif(${CHIP} MATCHES "bs27a")
        set(LOG_DEF
            ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/log/bs27a/log_def_sdk_bth.h
        )
    elseif(${CHIP} MATCHES "3322")
        set(LOG_DEF
            ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/log/3322/log_def_sdk_bgh.h
        )
    elseif("BGH_WSTP" IN_LIST DEFINES)
        set(LOG_DEF
            ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/log/sw22_entire_sys/log_def_sdk_bgh.h
        )
    else()
        set(LOG_DEF
            ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/log/log_def_sdk_bth.h
        )
    endif()
endif()
if(${CHIP} MATCHES "ws53|ws63")
    set(PUBLIC_HEADER
        ${BTH_PUBLIC_HDR_LIST}
        ${CMAKE_CURRENT_SOURCE_DIR}/host/stack_api/include
        ${CMAKE_CURRENT_SOURCE_DIR}/host/stack/include/br
    )
elseif(${CHIP} MATCHES "bs20|bs21|bs21e|bs21a|bs22|bs26|bs25")
    set(PUBLIC_HEADER
        ${BTH_PUBLIC_HDR_LIST}
        ${CMAKE_CURRENT_SOURCE_DIR}/host/stack_api/bs2x/ram/include
    )
    if(${CHIP} MATCHES "bs25")
        set(PUBLIC_HEADER
            ${BTH_PUBLIC_HDR_LIST}
            ${CMAKE_CURRENT_SOURCE_DIR}/host/bs25/inc
        )
    endif()
elseif(${CHIP} MATCHES "bs27a")
    set(PUBLIC_HEADER
        ${BTH_PUBLIC_HDR_LIST}
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ble/L2
        ${CMAKE_CURRENT_SOURCE_DIR}/host/inc
        ${CMAKE_CURRENT_SOURCE_DIR}/host/bs27/ram/include
        ${CMAKE_CURRENT_SOURCE_DIR}/sdk/L2/ble
    )
elseif(${CHIP} MATCHES "3322")
    set(PUBLIC_HEADER
        ${BTH_PUBLIC_HDR_LIST}
        ${CMAKE_CURRENT_SOURCE_DIR}/sdk/L2/br
        ${CMAKE_CURRENT_SOURCE_DIR}/sdk/L2/ble
    )
    set(LOG_DEF
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/log/3322/log_def_sdk_bgh.h
    )
elseif("BGH_WSTP" IN_LIST DEFINES)
    set(PUBLIC_HEADER
        ${BTH_PUBLIC_HDR_LIST}
        ${CMAKE_CURRENT_SOURCE_DIR}/sdk/L2/br
        ${CMAKE_CURRENT_SOURCE_DIR}/sdk/L2/ble
    )
    set(LOG_DEF
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/log/sw22_entire_sys/log_def_sdk_bgh.h
    )
else()
    set(PUBLIC_HEADER
        ${BTH_PUBLIC_HDR_LIST}
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ble/L2
        ${CMAKE_CURRENT_SOURCE_DIR}/include/br/L0
        ${CMAKE_CURRENT_SOURCE_DIR}/include/br
        ${CMAKE_CURRENT_SOURCE_DIR}/include/br/L2
        ${CMAKE_CURRENT_SOURCE_DIR}/sdk/L2/br
        ${CMAKE_CURRENT_SOURCE_DIR}/sdk/L2/common
        ${CMAKE_CURRENT_SOURCE_DIR}/host/stack_api/include
        ${CMAKE_CURRENT_SOURCE_DIR}/host/stack/include/br
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../middleware/chips/socmn2/product/tws/
    )
    set(LOG_DEF
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/log/log_def_sdk_bth.h
    )
endif()

if(${CHIP} MATCHES "3322")
    set(PRIVATE_HEADER
        ${PRIVATE_HEADER}
        ${BTH_PRIVATE_HDR_LIST}
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/log/3322
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/host
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/os/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/nv
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ble/L2
        ${CMAKE_CURRENT_SOURCE_DIR}/include/br/L0
        ${CMAKE_CURRENT_SOURCE_DIR}/include/br
        ${CMAKE_CURRENT_SOURCE_DIR}/include/br/L2
        ${CMAKE_CURRENT_SOURCE_DIR}/sdk/L2/common
        ${CMAKE_CURRENT_SOURCE_DIR}/host/stack_api/include
    )
elseif("BGH_WSTP" IN_LIST DEFINES)
    set(PRIVATE_HEADER
        ${PRIVATE_HEADER}
        ${BTH_PRIVATE_HDR_LIST}
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/log/sw22_entire_sys
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/host
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/os/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ble/L2
        ${CMAKE_CURRENT_SOURCE_DIR}/include/br/L0
        ${CMAKE_CURRENT_SOURCE_DIR}/include/br
        ${CMAKE_CURRENT_SOURCE_DIR}/include/br/L2
        ${CMAKE_CURRENT_SOURCE_DIR}/sdk/L2/common
        ${CMAKE_CURRENT_SOURCE_DIR}/host/stack_api/include
    )
elseif(${CHIP} MATCHES "bs27a")
    set(PRIVATE_HEADER
        ${PRIVATE_HEADER}
        ${BTH_PRIVATE_HDR_LIST}
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/log/bs27a
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/host
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/os/include
        ${CMAKE_CURRENT_SOURCE_DIR}/host/stack_api/sapi/include
    )
elseif(${CHIP} MATCHES "ws63|ws53")
    set(PRIVATE_HEADER
        ${PRIVATE_HEADER}
        ${BTH_PRIVATE_HDR_LIST}
    )
endif()

if(${CHIP} MATCHES "bs27a")
    set(PUBLIC_HEADER
        ${PUBLIC_HEADER}
        ${CMAKE_CURRENT_SOURCE_DIR}/host/stack_api/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/common/L0
        ${CMAKE_CURRENT_SOURCE_DIR}/include/common/bs27a #bs27a编译自己的文件夹
        ${CMAKE_CURRENT_SOURCE_DIR}/include/common
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ble
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ble/L0
        ${CMAKE_CURRENT_SOURCE_DIR}/sdk/L2/ble
        ${CMAKE_CURRENT_SOURCE_DIR}/sdk/L2/common
    )
else()
    set(PUBLIC_HEADER
        ${PUBLIC_HEADER}
        ${CMAKE_CURRENT_SOURCE_DIR}/host/stack_api/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/common/L0
        ${CMAKE_CURRENT_SOURCE_DIR}/include/common/common
        ${CMAKE_CURRENT_SOURCE_DIR}/include/common
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ble
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ble/L0
        ${CMAKE_CURRENT_SOURCE_DIR}/sdk/L2/ble
        ${CMAKE_CURRENT_SOURCE_DIR}/sdk/L2/common
    )
endif()

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
MESSAGE("BT APP PUBLIC_HEADER:" ${PUBLIC_HEADER})
build_component()
