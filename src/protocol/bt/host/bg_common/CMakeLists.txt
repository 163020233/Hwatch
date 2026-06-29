#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd.. 2023. All rights reserved.
#===============================================================================
# Temporarily Released
if(${CHIP} MATCHES "bisheng|mp17c|mp12")
include(gle_host_offload_noneos.cmake)
else()
if(${CHIP} MATCHES "sw39|sw21|3322|socmn2|sw22|1a71e")
    set(MODULE_NAME "bgh")
else()
    set(MODULE_NAME "bth")
endif()
set(AUTO_DEF_FILE_ID FALSE)
set(COMPONENT_NAME "bg_common")
set(BTH_PUBLIC_HEADER_LIST "" CACHE INTERNAL "" FORCE)

set(BTH_RAM_LIST  "" CACHE INTERNAL "" FORCE)
set(BTH_ROM_LIST  "" CACHE INTERNAL "" FORCE)

if(DEFINED ROM_COMPONENT OR DEFINED ROM_SYM_PATH)
    set(BT_ROM_VERSION true)
else()
    set(BT_ROM_VERSION false)
endif()

if(${CHIP_FAMILY} MATCHES "bs2x")
    add_subdirectory_if_exist(chips)
    add_subdirectory_if_exist(src/utils)
    add_subdirectory_if_exist(src/product)
    if (SUPPORT_CHIP_N1200 IN_LIST DEFINES)
        set(PUBLIC_HEADER
            ${BTH_PUBLIC_HDR_LIST}
            ${CMAKE_CURRENT_SOURCE_DIR}/portings/log/oml
            ${CMAKE_CURRENT_SOURCE_DIR}/portings/log
            ${CMAKE_CURRENT_SOURCE_DIR}/include/product/init
        )
    else()
        set(PUBLIC_HEADER
            ${BTH_PUBLIC_HDR_LIST}
            ${CMAKE_CURRENT_SOURCE_DIR}/portings/log/bs21
            ${CMAKE_CURRENT_SOURCE_DIR}/include/product/init
        )
    endif()

elseif((${CHIP} MATCHES "bs25") AND ("bt_host_rom" IN_LIST TARGET_COMPONENT))
    set(MODULE_NAME "bt")
    add_subdirectory_if_exist(chips)
    add_subdirectory_if_exist(src/product)
    set(PUBLIC_HEADER
        ${BTH_PUBLIC_HDR_LIST}
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/log/bs25
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/ipc/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/product/init
    )
elseif(${CHIP} MATCHES "ws63")
    add_subdirectory_if_exist(chips)
    add_subdirectory_if_exist(src)
    set(PUBLIC_HEADER
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/nv
        ${CMAKE_CURRENT_SOURCE_DIR}/include/gdk_common
        ${CMAKE_CURRENT_SOURCE_DIR}/include/hci
        ${CMAKE_CURRENT_SOURCE_DIR}/include/host
        ${CMAKE_CURRENT_SOURCE_DIR}/include/host/stack
        ${CMAKE_CURRENT_SOURCE_DIR}/include/config/ws63
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/log
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/log/ws63
        ${CMAKE_CURRENT_SOURCE_DIR}/include/sdk_common
        ${CMAKE_CURRENT_SOURCE_DIR}/include/service
        ${CMAKE_CURRENT_SOURCE_DIR}/../host/inc
        ${CMAKE_CURRENT_SOURCE_DIR}/include/at
        ${CMAKE_CURRENT_SOURCE_DIR}/include/dfx
        ${CMAKE_CURRENT_SOURCE_DIR}/include/product/init
    )
elseif(${CHIP} MATCHES "ws53")
    add_subdirectory_if_exist(src)
    set(PUBLIC_HEADER
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/nv
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/tladapter/hcc
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/tladapter/os
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/os/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/gdk_common
        ${CMAKE_CURRENT_SOURCE_DIR}/include/hci
        ${CMAKE_CURRENT_SOURCE_DIR}/include/host
        ${CMAKE_CURRENT_SOURCE_DIR}/include/host/stack
        ${CMAKE_CURRENT_SOURCE_DIR}/include/config/ws53
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/log
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/log/uapi
        ${CMAKE_CURRENT_SOURCE_DIR}/include/sdk_common
        ${CMAKE_CURRENT_SOURCE_DIR}/include/service
        ${CMAKE_CURRENT_SOURCE_DIR}/include/at
        ${CMAKE_CURRENT_SOURCE_DIR}/include/product/init
        ${CMAKE_CURRENT_SOURCE_DIR}/include/dfx
    )
elseif(${CHIP} MATCHES "sw21")
    add_subdirectory_if_exist(src)
    set(PRIVATE_HEADER
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/log/sw21
        ${CMAKE_CURRENT_SOURCE_DIR}/include/host
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/os/include
    )
    set(PUBLIC_HEADER
        ${CMAKE_CURRENT_SOURCE_DIR}/include/config/sw21
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/nv
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/tladapter/sle_only
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/ipc/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ahi_common
        ${CMAKE_CURRENT_SOURCE_DIR}/include/gdk_common
        ${CMAKE_CURRENT_SOURCE_DIR}/include/hci
        ${CMAKE_CURRENT_SOURCE_DIR}/include/host/stack
        ${CMAKE_CURRENT_SOURCE_DIR}/include/sdk_common
        ${CMAKE_CURRENT_SOURCE_DIR}/include/service
        ${CMAKE_CURRENT_SOURCE_DIR}/include/at
        ${CMAKE_CURRENT_SOURCE_DIR}/include/product/init
        ${CMAKE_CURRENT_SOURCE_DIR}/include/dfx
        ${CMAKE_CURRENT_SOURCE_DIR}/include/vendor
    )
elseif(${CHIP} MATCHES "sw39")
    add_subdirectory_if_exist(src)
    set(PRIVATE_HEADER
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/nv
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/tladapter/sle_only
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/os/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/gdk_common
        ${CMAKE_CURRENT_SOURCE_DIR}/include/hci
        ${CMAKE_CURRENT_SOURCE_DIR}/include/host
        ${CMAKE_CURRENT_SOURCE_DIR}/include/host/stack
        ${CMAKE_CURRENT_SOURCE_DIR}/include/config/sw39
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/log/sw39
        ${CMAKE_CURRENT_SOURCE_DIR}/include/sdk_common
        ${CMAKE_CURRENT_SOURCE_DIR}/include/service
        ${CMAKE_CURRENT_SOURCE_DIR}/include/at
        ${CMAKE_CURRENT_SOURCE_DIR}/include/vendor
    )
    set(PUBLIC_HEADER
        ${CMAKE_CURRENT_SOURCE_DIR}/include/dfx
    )
elseif(${CHIP} MATCHES "1a71e")
    add_subdirectory_if_exist(src)
    set(PRIVATE_HEADER
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/nv
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/tladapter/sle_only
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/os/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/gdk_common
        ${CMAKE_CURRENT_SOURCE_DIR}/include/hci
        ${CMAKE_CURRENT_SOURCE_DIR}/include/host
        ${CMAKE_CURRENT_SOURCE_DIR}/include/host/stack
        ${CMAKE_CURRENT_SOURCE_DIR}/include/config/1a71e
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/log/1a71e
        ${CMAKE_CURRENT_SOURCE_DIR}/include/sdk_common
        ${CMAKE_CURRENT_SOURCE_DIR}/include/service
        ${CMAKE_CURRENT_SOURCE_DIR}/include/at
        ${CMAKE_CURRENT_SOURCE_DIR}/include/vendor
    )
    set(PUBLIC_HEADER
        ${CMAKE_CURRENT_SOURCE_DIR}/include/dfx
    )
elseif(${CHIP} MATCHES "3322")
    add_subdirectory_if_exist(src)
    set(PRIVATE_HEADER
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/os/include
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/nv
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/tladapter/sle_only
        ${CMAKE_CURRENT_SOURCE_DIR}/include/host
        ${CMAKE_CURRENT_SOURCE_DIR}/include/config/featrue_conf.h
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/log/3322
        ${CMAKE_CURRENT_SOURCE_DIR}/include/sdk_common
        ${CMAKE_CURRENT_SOURCE_DIR}/include/at
    )
    set(PUBLIC_HEADER
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/tladapter/ipc
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/ipc/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/hci
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ahi_a
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ahi_b
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ahi_common
        ${CMAKE_CURRENT_SOURCE_DIR}/include/gdk_common
        ${CMAKE_CURRENT_SOURCE_DIR}/include/host/stack
        ${CMAKE_CURRENT_SOURCE_DIR}/include/config/3322
        ${CMAKE_CURRENT_SOURCE_DIR}/include/service
        ${CMAKE_CURRENT_SOURCE_DIR}/include/dfx
        ${CMAKE_CURRENT_SOURCE_DIR}/include/product/init
        ${CMAKE_CURRENT_SOURCE_DIR}/include/product/customize
        ${CMAKE_CURRENT_SOURCE_DIR}/include/vendor
    )
elseif("BGH_WSTP" IN_LIST DEFINES)
    add_subdirectory_if_exist(src)
    set(PRIVATE_HEADER
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/os/include
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/nv
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/tladapter/sle_only
        ${CMAKE_CURRENT_SOURCE_DIR}/include/host
        ${CMAKE_CURRENT_SOURCE_DIR}/include/config/featrue_conf.h
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/log/sw22_entire_sys
        ${CMAKE_CURRENT_SOURCE_DIR}/include/sdk_common
        ${CMAKE_CURRENT_SOURCE_DIR}/include/at
    )
    set(PUBLIC_HEADER
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/tladapter/ipc
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/ipc/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/hci
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ahi_a
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ahi_b
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ahi_common
        ${CMAKE_CURRENT_SOURCE_DIR}/include/gdk_common
        ${CMAKE_CURRENT_SOURCE_DIR}/include/host
        ${CMAKE_CURRENT_SOURCE_DIR}/include/host/stack
        ${CMAKE_CURRENT_SOURCE_DIR}/include/config/sw22_entire_sys
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/log/sw22_entire_sys
        ${CMAKE_CURRENT_SOURCE_DIR}/include/service
        ${CMAKE_CURRENT_SOURCE_DIR}/include/dfx
        ${CMAKE_CURRENT_SOURCE_DIR}/include/product/init
        ${CMAKE_CURRENT_SOURCE_DIR}/include/vendor
    )
elseif(${CHIP} MATCHES "sw22" AND "SUPPORT_POWER_OFF_FIND" IN_LIST DEFINES)
    add_subdirectory_if_exist(src)
    set(PRIVATE_HEADER
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/os/include
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/nv
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/tladapter/sle_only
        ${CMAKE_CURRENT_SOURCE_DIR}/include/host
        ${CMAKE_CURRENT_SOURCE_DIR}/include/config/featrue_conf.h
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/log/sw22
        ${CMAKE_CURRENT_SOURCE_DIR}/include/sdk_common
        ${CMAKE_CURRENT_SOURCE_DIR}/include/at
    )
    set(PUBLIC_HEADER
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/tladapter/ipc
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/ipc/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/hci
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ahi_a
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ahi_b
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ahi_common
        ${CMAKE_CURRENT_SOURCE_DIR}/include/gdk_common
        ${CMAKE_CURRENT_SOURCE_DIR}/include/host/stack
        ${CMAKE_CURRENT_SOURCE_DIR}/include/config/sw22/sw22_power_off_find
        ${CMAKE_CURRENT_SOURCE_DIR}/include/service
        ${CMAKE_CURRENT_SOURCE_DIR}/include/dfx
        ${CMAKE_CURRENT_SOURCE_DIR}/include/product/init
        ${CMAKE_CURRENT_SOURCE_DIR}/include/product/customize
        ${CMAKE_CURRENT_SOURCE_DIR}/include/vendor
    )
elseif(${CHIP} MATCHES "sw22")
    add_subdirectory_if_exist(src)
    set(PRIVATE_HEADER
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/log/sw22
        ${CMAKE_CURRENT_SOURCE_DIR}/include/host
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/os/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/config/sw22
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/nv
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/tladapter/sle_only
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/ipc/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/hci
        ${CMAKE_CURRENT_SOURCE_DIR}/include/host/stack
        ${CMAKE_CURRENT_SOURCE_DIR}/include/gdk_common
        ${CMAKE_CURRENT_SOURCE_DIR}/include/sdk_common
        ${CMAKE_CURRENT_SOURCE_DIR}/include/service
        ${CMAKE_CURRENT_SOURCE_DIR}/include/at
        ${CMAKE_CURRENT_SOURCE_DIR}/include/dfx
    )
    set(PUBLIC_HEADER
        ${CMAKE_CURRENT_SOURCE_DIR}/include/product/init
        ${CMAKE_CURRENT_SOURCE_DIR}/include/vendor
    )
elseif(${CHIP} MATCHES "bs25|brandy")
    add_subdirectory_if_exist(src)
    set(PUBLIC_HEADER
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/nv/bs25/inc
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/ipc/include
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/os/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ahi_a
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ahi_b
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ahi_common
        ${CMAKE_CURRENT_SOURCE_DIR}/include/gdk_common
        ${CMAKE_CURRENT_SOURCE_DIR}/include/hci
        ${CMAKE_CURRENT_SOURCE_DIR}/include/host
        ${CMAKE_CURRENT_SOURCE_DIR}/include/host/stack
        ${CMAKE_CURRENT_SOURCE_DIR}/include/config/default
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/log
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/log/oml
        ${CMAKE_CURRENT_SOURCE_DIR}/include/sdk_common
        ${CMAKE_CURRENT_SOURCE_DIR}/include/service
        ${CMAKE_CURRENT_SOURCE_DIR}/include/at
        ${CMAKE_CURRENT_SOURCE_DIR}/include/dfx
    )
elseif(${CHIP} MATCHES "socmn1|socmn2")
    add_subdirectory_if_exist(src)
    set(PUBLIC_HEADER
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/nv
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/tladapter/ipc
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/ipc/include
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/os/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ahi_a
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ahi_b
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ahi_common
        ${CMAKE_CURRENT_SOURCE_DIR}/include/gdk_common
        ${CMAKE_CURRENT_SOURCE_DIR}/include/hci
        ${CMAKE_CURRENT_SOURCE_DIR}/include/host
        ${CMAKE_CURRENT_SOURCE_DIR}/include/host/stack
        ${CMAKE_CURRENT_SOURCE_DIR}/include/config/socmn2
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/log/socmn2
        ${CMAKE_CURRENT_SOURCE_DIR}/include/sdk_common
        ${CMAKE_CURRENT_SOURCE_DIR}/include/service
        ${CMAKE_CURRENT_SOURCE_DIR}/include/at
        ${CMAKE_CURRENT_SOURCE_DIR}/include/product/init
        ${CMAKE_CURRENT_SOURCE_DIR}/include/dfx
        ${CMAKE_CURRENT_SOURCE_DIR}/include/vendor
    )
elseif(${CHIP} MATCHES "bs27a")
    add_subdirectory_if_exist(chips)
    add_subdirectory_if_exist(src)
    set(PUBLIC_HEADER
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/os/include
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/tladapter/onetrack
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/ipc/include
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/nv
        ${CMAKE_CURRENT_SOURCE_DIR}/include/hci
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ahi_common
        ${CMAKE_CURRENT_SOURCE_DIR}/include/gdk_common
        ${CMAKE_CURRENT_SOURCE_DIR}/include/host
        ${CMAKE_CURRENT_SOURCE_DIR}/include/host/stack
        ${CMAKE_CURRENT_SOURCE_DIR}/include/config/bs27a
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/log/bs27a
        ${CMAKE_CURRENT_SOURCE_DIR}/include/sdk_common
        ${CMAKE_CURRENT_SOURCE_DIR}/include/service
        ${CMAKE_CURRENT_SOURCE_DIR}/include/product/init
        ${CMAKE_CURRENT_SOURCE_DIR}/include/product/lowpower
        ${CMAKE_CURRENT_SOURCE_DIR}/include/dfx
        ${CMAKE_CURRENT_SOURCE_DIR}/include/vendor
        ${CMAKE_CURRENT_SOURCE_DIR}/include/utils
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ahi_b
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ahi_a
    )
endif()
add_subdirectory_if_exist(portings)
set(PUBLIC_HEADER
    ${PUBLIC_HEADER}/
    ${BTH_PUBLIC_HEADER_LIST}
    ${CMAKE_CURRENT_SOURCE_DIR}/include/config
    ${CMAKE_CURRENT_SOURCE_DIR}/include/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/portings/ipc/include # 清除 porting 以外代码对 IPC 的依赖后，此项可删除
    ${CMAKE_CURRENT_SOURCE_DIR}/portings/algo/ecdh/include # ecdh
    ${CMAKE_CURRENT_SOURCE_DIR}/portings/high_perf/wear # high perf
    ${CMAKE_CURRENT_SOURCE_DIR}/portings/algo/aes/include # aes
)
if("${BTH_RAM_LIST}" STREQUAL "")
    if(DEFINED CONFIG_SLE_BLE_SUPPORT AND SUPPORT_MULTI_LIBS IN_LIST DEFINES)
        set(LIBS ${CMAKE_CURRENT_SOURCE_DIR}/${PKG_CHIP}-${CONFIG_SLE_BLE_SUPPORT}/lib${COMPONENT_NAME}.a)
    elseif(DEFINED CONFIG_SUPPORT_SLE_BLE_CENTRAL_DEFAULT)
        set(LIBS ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET_COMMAND}/lib${COMPONENT_NAME}.a)
    else()
        set(BTH_RAM_LIST "__null__")
    endif()
elseif(${CHIP} MATCHES "bs25")
    set(LOG_DEF
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/log/bs25/log_def_bth_rom.h
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/log/bs25/log_def_bth.h
    )
elseif(${CHIP} MATCHES "bs27a")
    set(LOG_DEF
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/log/bs27a/log_def_bth.h
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/log/bs27a/log_def_sdk_bth.h
    )
elseif(${CHIP_FAMILY} MATCHES "bs2x")
    if (SUPPORT_CHIP_N1200 IN_LIST DEFINES) # ROM-EC后
        set(LOG_DEF
            ${CMAKE_CURRENT_SOURCE_DIR}/portings/log/log_def_bth.h
            ${CMAKE_CURRENT_SOURCE_DIR}/portings/log/log_def_sdk_bth.h
        )
    else() # ROM-EC前
        set(LOG_DEF
            ${CMAKE_CURRENT_SOURCE_DIR}/portings/log/bs21/log_def_bth.h
            ${CMAKE_CURRENT_SOURCE_DIR}/portings/log/bs21/log_def_sdk_bth.h
        )
    endif()
elseif(${CHIP} MATCHES "sw39")
    set(LOG_DEF
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/log/sw39/log_def_bgh.h
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/log/sw39/log_def_sdk_bgh.h
    )
elseif(${CHIP} MATCHES "1a71e")
    set(LOG_DEF
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/log/1a71e/log_def_bgh.h
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/log/1a71e/log_def_sdk_bgh.h
    )
elseif(${CHIP} MATCHES "sw21")
    set(LOG_DEF
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/log/sw21/log_def_bgh.h
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/log/sw21/log_def_sdk_bgh.h
    )
elseif("BGH_WSTP" IN_LIST DEFINES)
    set(LOG_DEF
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/log/sw22_entire_sys/log_def_bgh.h
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/log/sw22_entire_sys/log_def_sdk_bgh.h
    )
elseif(${CHIP} MATCHES "sw22")
    set(LOG_DEF
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/log/sw22/log_def_bgh.h
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/log/sw22/log_def_sdk_bgh.h
    )
elseif(${CHIP} MATCHES "3322")
    set(LOG_DEF
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/log/3322/log_def_bgh.h
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/log/3322/log_def_sdk_bgh.h
    )
elseif(${CHIP} MATCHES "mp17c")
    set(LOG_DEF
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/log/log_def_bth.h
    )
elseif(${CHIP} MATCHES "socmn1|socmn2")
    set(LOG_DEF
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/log/socmn2/log_def_bgh.h
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/log/socmn2/log_def_sdk_bgh.h
    )
else()
    set(LOG_DEF
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/log/log_def_bth.h
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/log/log_def_sdk_bth.h
    )
endif()

set(SOURCES
    ${BTH_RAM_LIST}
)

if((NOT "ipc_old" IN_LIST TARGET_COMPONENT) AND (NOT "bs25_bgle" IN_LIST TARGET_COMPONENT) AND
    (NOT ${CHIP} MATCHES "sw21|sw22|3322|socmn2|bs27a"))
    set(PRIVATE_HEADER
        ${PRIVATE_HEADER}
        # Temporarily Released
        # 清除 porting 以外代码对 IPC 的依赖后，此项可删除
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../drivers/drivers/driver/ipc/ipc_v0/shared
    )
if((NOT ${CHIP} MATCHES "ws53|bs25"))
    set(PRIVATE_HEADER
        ${PRIVATE_HEADER}
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/ipc/include/product
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
install_sdk(${BIN_DIR}/${CHIP}/libs/bluetooth/bth/${TARGET_COMMAND} "*.a")


set(COMPONENT_NAME "bg_common_rom")

if("${BTH_ROM_LIST}" STREQUAL "")
    set(BTH_ROM_LIST "__null__")
endif()

MESSAGE("BG COMMON BTH_ROM_LIST:" ${BTH_ROM_LIST})
set(SOURCES
    ${BTH_ROM_LIST}
)

set(PUBLIC_HEADER
    ${BTH_PUBLIC_HDR_LIST}
)

set(PRIVATE_HEADER
    ${BTH_PRIVATE_HDR_LIST}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../drivers/drivers/driver/ipc/ipc_v0/shared
)

if((NOT ${CHIP} MATCHES "ws53|bs25"))
    set(PRIVATE_HEADER
        ${PRIVATE_HEADER}
        ${CMAKE_CURRENT_SOURCE_DIR}/portings/ipc/include/product
    )
endif()

set(PRIVATE_DEFINES
    BUILD_ROM_COMPILE
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
MESSAGE("BTH_ROM_LIST:" ${BTH_ROM_LIST})
set(LIB_OUT_PATH ${BIN_DIR}/${CHIP}/libs/bluetooth/bth/${TARGET_COMMAND})

build_component()

endif()
