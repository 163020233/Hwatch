set(COMPONENT_NAME "bth_saf_private")
if(${CHIP} MATCHES "sw39|sw21|socmn2|sw22|3322|1a71e")
    set(MODULE_NAME "bgh")
else()
    set(MODULE_NAME "bth")
endif()

set(SAF_SRC_LIST  "" CACHE INTERNAL "" FORCE)
set(SAF_HEADER_LIST "" CACHE INTERNAL "" FORCE)

add_subdirectory_if_exist(config)
add_subdirectory_if_exist(src)

if("${SAF_SRC_LIST}" STREQUAL "")
    if(DEFINED CONFIG_SLE_BLE_SUPPORT AND SUPPORT_MULTI_LIBS IN_LIST DEFINES)
        set(LIBS ${CMAKE_CURRENT_SOURCE_DIR}/${PKG_CHIP}-${CONFIG_SLE_BLE_SUPPORT}/lib${COMPONENT_NAME}.a)
    elseif(DEFINED CONFIG_SUPPORT_SLE_BLE_CENTRAL_DEFAULT)
        set(LIBS ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET_COMMAND}/lib${COMPONENT_NAME}.a)
    else()
        set(SAF_SRC_LIST "__null__")
    endif()
endif()

set(SOURCES
    ${SAF_SRC_LIST}
)

set(PUBLIC_HEADER
    ${SAF_CONFIG_HEADER_LIST}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ape
    ${CMAKE_CURRENT_SOURCE_DIR}/src/asm
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ccp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cdsm
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gcp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mcp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/micp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nip
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vap
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vcp
)

if(${CHIP} MATCHES "sw39")
    set(PRIVATE_HEADER
        ${SAF_HEADER_LIST}
        ${CMAKE_CURRENT_SOURCE_DIR}/../../bg_common/include/host
        ${CMAKE_CURRENT_SOURCE_DIR}/../../bg_common/include/host/stack
        ${CMAKE_CURRENT_SOURCE_DIR}/../../bg_common/include/config/sw39
        ${CMAKE_CURRENT_SOURCE_DIR}/../../bg_common/portings/os/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../bg_common/portings/log/sw39
        ${CMAKE_CURRENT_SOURCE_DIR}/../../bg_common/include/service
        ${CMAKE_CURRENT_SOURCE_DIR}/../../bg_common/bg_common/include/nv
        ${CMAKE_CURRENT_SOURCE_DIR}/../../bg_common/portings/os/include
    )
elseif(${CHIP} MATCHES "1a71e")
    set(PRIVATE_HEADER
        ${SAF_HEADER_LIST}
        ${CMAKE_CURRENT_SOURCE_DIR}/../../bg_common/include/host
        ${CMAKE_CURRENT_SOURCE_DIR}/../../bg_common/include/host/stack
        ${CMAKE_CURRENT_SOURCE_DIR}/../../bg_common/include/config/1a71e
        ${CMAKE_CURRENT_SOURCE_DIR}/../../bg_common/portings/os/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../bg_common/portings/log/1a71e
        ${CMAKE_CURRENT_SOURCE_DIR}/../../bg_common/include/service
        ${CMAKE_CURRENT_SOURCE_DIR}/../../bg_common/bg_common/include/nv
        ${CMAKE_CURRENT_SOURCE_DIR}/../../bg_common/portings/os/include
    )
elseif(${CHIP} MATCHES "sw21")
    set(PRIVATE_HEADER
        ${SAF_HEADER_LIST}
        ${CMAKE_CURRENT_SOURCE_DIR}/../../bg_common/include/host
        ${CMAKE_CURRENT_SOURCE_DIR}/../../bg_common/include/host/stack
        ${CMAKE_CURRENT_SOURCE_DIR}/../../bg_common/include/config/sw21
        ${CMAKE_CURRENT_SOURCE_DIR}/../../bg_common/portings/os/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../bg_common/portings/log/sw21
        ${CMAKE_CURRENT_SOURCE_DIR}/../../bg_common/include/service
        ${CMAKE_CURRENT_SOURCE_DIR}/../../bg_common/bg_common/include/nv
        ${CMAKE_CURRENT_SOURCE_DIR}/../../bg_common/portings/os/include
    )
elseif(${CHIP} MATCHES "sw22")
    set(PRIVATE_HEADER
        ${SAF_HEADER_LIST}
        ${CMAKE_CURRENT_SOURCE_DIR}/../../bg_common/include/host
        ${CMAKE_CURRENT_SOURCE_DIR}/../../bg_common/include/host/stack
        ${CMAKE_CURRENT_SOURCE_DIR}/../../bg_common/include/config/sw22
        ${CMAKE_CURRENT_SOURCE_DIR}/../../bg_common/include/sdk_common
        ${CMAKE_CURRENT_SOURCE_DIR}/../../bg_common/include/config
        ${CMAKE_CURRENT_SOURCE_DIR}/../../bg_common/include/utils
        ${CMAKE_CURRENT_SOURCE_DIR}/../../bg_common/portings/os/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../bg_common/portings/log/sw22
        ${CMAKE_CURRENT_SOURCE_DIR}/../../bg_common/include/service
        ${CMAKE_CURRENT_SOURCE_DIR}/../../bg_common/bg_common/include/nv
        ${CMAKE_CURRENT_SOURCE_DIR}/../../bg_common/portings/os/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../gle/sdk/L2/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../gle/utils
        ${CMAKE_CURRENT_SOURCE_DIR}/../../gle/include
    )
elseif(${CHIP} MATCHES "socmn2")
    set(PRIVATE_HEADER
        ${SAF_HEADER_LIST}
        ${CMAKE_CURRENT_SOURCE_DIR}/../../bg_common/include/host
        ${CMAKE_CURRENT_SOURCE_DIR}/../../bg_common/include/host/stack
        ${CMAKE_CURRENT_SOURCE_DIR}/../../bg_common/include/config/socmn2
        ${CMAKE_CURRENT_SOURCE_DIR}/../../bg_common/portings/os/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../bg_common/portings/log/socmn2
        ${CMAKE_CURRENT_SOURCE_DIR}/../../bg_common/include/service
        ${CMAKE_CURRENT_SOURCE_DIR}/../../bg_common/bg_common/include/nv
        ${CMAKE_CURRENT_SOURCE_DIR}/../../bg_common/portings/os/include
    )
elseif(${CHIP} MATCHES "3322")
    set(PRIVATE_HEADER
        ${SAF_HEADER_LIST}
        ${CMAKE_CURRENT_SOURCE_DIR}/../../bg_common/include/host
        ${CMAKE_CURRENT_SOURCE_DIR}/../../bg_common/include/host/stack
        ${CMAKE_CURRENT_SOURCE_DIR}/../../bg_common/include/config/3322
        ${CMAKE_CURRENT_SOURCE_DIR}/../../bg_common/portings/os/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../bg_common/portings/log/3322
        ${CMAKE_CURRENT_SOURCE_DIR}/../../bg_common/include/service
        ${CMAKE_CURRENT_SOURCE_DIR}/../../bg_common/bg_common/include/nv
        ${CMAKE_CURRENT_SOURCE_DIR}/../../bg_common/portings/os/include
    )
else()
    set(PRIVATE_HEADER
        ${SAF_HEADER_LIST}
    )
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