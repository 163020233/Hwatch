set(COMPONENT_NAME "netstack_http")
# set(MODULE_NAME "ohos")
# set(AUTO_DEF_FILE_ID TRUE)

set(PUBLIC_HEADER
)

message(STATUS "OHOS_DIR: ${OHOS_DIR}")

set(PRIVATE_HEADER
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/modules/netstack/http/include
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/modules/netstack/http/internal
)

if(${CHIP} MATCHES "sw22")
list(APPEND PRIVATE_HEADER
    ${ROOT_DIR}/open_source/lwip/lwip_v2.2.0/src/include
    ${ROOT_DIR}/open_source/lwip/lwip_adapter/sw22/src/include
)
else()
list(APPEND PRIVATE_HEADER
    ${OHOS_DIR}/third_party/lwip/lwip_v2.1.3/src/include
    ${OHOS_DIR}/third_party/lwip/lwip_adapter/liteos/src/include
)
endif()

if("SUPPORT_LWIP" IN_LIST DEFINES)
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/http_api.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/http_client.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/http_wrapper.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/http_form_data.c
)
endif()

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-error
    -Wno-unused-parameter
    -fno-exceptions
    -fno-unwind-tables
    -fno-asynchronous-unwind-tables
    -Wno-undef
    -Wno-bitwise-op-parentheses
    -Wno-shift-op-parentheses
    -nostdlibinc
    -nostdinc
    -nostdlib
    -Wno-error=unused-function
    # -DLWIP_COMPAT_SOCKETS=2
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
   false
)

set(PUBLIC_DEFINES
    CONFIG_USE_LWIP
    CONFIG_HTTP_SECURE
)

set(PRIVATE_DEFINES
    CONFIG_HTTP_FORM_DATA
    CONFIG_HTTP_FILE_OPERATE
)

# install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/interfaces" "*")
# install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/include" "*")

build_component()