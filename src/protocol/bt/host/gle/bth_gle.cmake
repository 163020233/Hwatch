#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "bth_gle")

if(${CHIP} MATCHES "sw39|sw21|1a71e")
    set(BTH_ROM_VERSION true)
else()
    set(BTH_ROM_VERSION false)
endif()

set(BTH_RAM_LIST  "" CACHE INTERNAL "" FORCE)
set(BTH_PUBLIC_HEADER_LIST "" CACHE INTERNAL "" FORCE)
set(BTH_PRIVATE_HEADER_LIST "" CACHE INTERNAL "" FORCE)
add_subdirectory_if_exist(dft)
add_subdirectory_if_exist(host)
add_subdirectory_if_exist(ahi)
add_subdirectory_if_exist(samples)
add_subdirectory_if_exist(sdk)
add_subdirectory_if_exist(service)
add_subdirectory_if_exist(utils)
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
)

MESSAGE("BTH_PUBLIC_HEADER_LIST=" ${BTH_PUBLIC_HEADER_LIST})

set(PUBLIC_HEADER
    ${BTH_PUBLIC_HEADER_LIST}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/L0
    ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/sdk_common
)

if(${CHIP} MATCHES "sw39")
    set(PRIVATE_HEADER
        ${BTH_PRIVATE_HEADER_LIST}
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/nv/common/inc
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/tladapter/sle_only
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/os/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/ahi_a
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/at
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/gdk_common
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/hci
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/host
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/host/stack
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/config/sw39
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/log/sw39
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/service
    )
elseif(${CHIP} MATCHES "1a71e")
    set(PRIVATE_HEADER
        ${BTH_PRIVATE_HEADER_LIST}
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/nv/common/inc
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/tladapter/sle_only
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/os/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/ahi_a
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/at
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/gdk_common
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/hci
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/host
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/host/stack
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/config/1a71e
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/log/1a71e
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/service
    )
elseif(${CHIP} MATCHES "sw21")
    set(PRIVATE_HEADER
        ${BTH_PRIVATE_HEADER_LIST}
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/config/sw21
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/log/sw21
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/host
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/os/include
    )
elseif("BGH_WSTP" IN_LIST DEFINES)
    set(PRIVATE_HEADER
        ${BTH_PRIVATE_HEADER_LIST}
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/nv/common/inc
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/os/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/at
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/hci
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/host
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/host/stack
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/config/sw22_entire_sys
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/log/sw22_entire_sys
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/service
    )
elseif(${CHIP} MATCHES "sw22" AND "SUPPORT_POWER_OFF_FIND" IN_LIST DEFINES)
    set(PRIVATE_HEADER
        ${BTH_PRIVATE_HEADER_LIST}
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/nv/common/inc
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/os/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/at
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/hci
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/host
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/host/stack
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/config/sw22/sw22_power_off_find
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/log/sw22
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/service
        ${CMAKE_CURRENT_SOURCE_DIR}/service/self_find/profile
    )
elseif(${CHIP} MATCHES "sw22")
    set(PRIVATE_HEADER
        ${BTH_PRIVATE_HEADER_LIST}
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/config/sw22
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/log/sw22
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/host
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/os/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/nv
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/tladapter/sle_only
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/ipc/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/hci
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/host/stack
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/gdk_common
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/sdk_common
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/service
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/at
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/product/init
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/dfx
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/vendor
    )
elseif(${CHIP} MATCHES "3322")
    set(PRIVATE_HEADER
        ${BTH_PRIVATE_HEADER_LIST}
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/nv/common/inc
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/os/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/at
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/hci
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/host
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/host/stack
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/config/3322
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/log/3322
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/service
    )
elseif(${CHIP} MATCHES "socmn2")
    set(PRIVATE_HEADER
        ${BTH_PRIVATE_HEADER_LIST}
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/nv/common/inc
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/tladapter/sle_only
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/os/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/ahi_a
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/at
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/gdk_common
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/hci
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/host
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/host/stack
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/config/socmn2
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/log/socmn2
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/service
    )
else()
    set(PRIVATE_HEADER
        ${BTH_PRIVATE_HEADER_LIST}
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../drivers/drivers/driver/ipc/ipc_v0/shared
        ${CMAKE_CURRENT_SOURCE_DIR}/../private/gle/host/stack/c_frame
    )
if((NOT ${CHIP} MATCHES "ws53|bs25"))
    set(PRIVATE_HEADER
        ${PRIVATE_HEADER}
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/ipc/include/product
        ${CMAKE_CURRENT_SOURCE_DIR}/../private/gle/service/low_latency
        ${CMAKE_CURRENT_SOURCE_DIR}/../private/gle/service/ssap_low_latency
    )
endif()
endif()

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

if(${CHIP} MATCHES "sw39|1a71e")
    set(COMPONENT_CCFLAGS
        "-Werror"
        "-Werror=uninitialized"
        "-Werror=int-conversion"
        "-Werror=long-long"
        "-Werror=unused-value"
        "-Werror=sign-compare"
        "-Werror=unused-parameter"
        "-Werror=tautological-compare"
        "-Werror=incompatible-pointer-types"
        "-Werror=implicit-function-declaration"
    )
elseif(${CHIP} MATCHES "sw22" AND "SUPPORT_POWER_OFF_FIND" IN_LIST DEFINES)
    set(COMPONENT_CCFLAGS
        # GMSSL引用了stdio.h里的stderr和fprintf等定义, 芯片环境下下无法编译,
        # 但GMSSL开源软件不允许直接修改, 因此通过强制定义头文件保护宏然后通过编译选项插入本文件方式, 强制替换函数宏定义
        "-include${CMAKE_CURRENT_SOURCE_DIR}/service/self_find/profile/gmssl_adapter.h"
    )
else()
    set(COMPONENT_CCFLAGS
    )
endif()

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

if(${CHIP} MATCHES "sw39|1a71e")
    set(LIB_OUT_PATH ${BIN_DIR}/${CHIP}/libs/bluetooth/sleh/${TARGET_COMMAND})
else()
    set(LIB_OUT_PATH ${BIN_DIR}/${CHIP}/libs/bluetooth/bth/${TARGET_COMMAND})
endif()

build_component()

MESSAGE("BTH_ROM_VERSION=" ${BTH_ROM_VERSION})
if(BTH_ROM_VERSION)
    set(COMPONENT_NAME "bth_gle_rom")
    set(SOURCES
        ${BTH_ROM_LIST}
    )
    MESSAGE("bth_gle_rom BTH_ROM_LIST:" ${BTH_ROM_LIST})
    set(PRIVATE_HEADER
        ${BTH_PRIVATE_HEADER_LIST}
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/nv/common/inc
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/os/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/ahi_a
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/gdk_common
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/hci
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/host
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/host/stack
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/service
    )
    if(${CHIP} MATCHES "sw39")
        set(PRIVATE_HEADER
            ${PRIVATE_HEADER}
            ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/config/sw39
            ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/log/sw39
        )
    elseif(${CHIP} MATCHES "1a71e")
        set(PRIVATE_HEADER
            ${PRIVATE_HEADER}
            ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/config/1a71e
            ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/log/1a71e
        )
    elseif(${CHIP} MATCHES "sw21")
        set(PRIVATE_HEADER
            ${PRIVATE_HEADER}
            ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/config/sw21
            ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/portings/log/sw21
        )
    endif()
    set(PUBLIC_HEADER
        ${BTH_PUBLIC_HEADER_LIST}
    )
    set(PRIVATE_DEFINES
    )
    set(PUBLIC_DEFINES
    )
    # use this when you want to add ccflags like -include xxx
    set(COMPONENT_PUBLIC_CCFLAGS
    )

if(${CHIP} MATCHES "sw39|1a71e")
    set(COMPONENT_CCFLAGS
        "-Werror"
        "-Werror=uninitialized"
        "-Werror=int-conversion"
        "-Werror=long-long"
        "-Werror=unused-value"
        "-Werror=sign-compare"
        "-Werror=unused-parameter"
        "-Werror=tautological-compare"
        "-Werror=incompatible-pointer-types"
        "-Werror=implicit-function-declaration"
    )
else()
    set(COMPONENT_CCFLAGS
    )
endif()

    set(WHOLE_LINK
        true
    )
    set(MAIN_COMPONENT
        false
    )
    build_component()
endif()
