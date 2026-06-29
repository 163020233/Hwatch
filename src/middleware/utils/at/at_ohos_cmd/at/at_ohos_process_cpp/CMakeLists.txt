set(COMPONENT_NAME "ohos_at_cpp")

if("JS_ENABLE" IN_LIST DEFINES)
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/ohosfwk_at_process.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ohosfwk_at_netstack_dfx.cpp
)
endif()

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(PRIVATE_HEADER
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/base
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/modules/netstack/http/include
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/modules/netstack/websocket/include
    ${OHOS_DIR}/foundation/ability/ability_lite/frameworks/abilitymgr_lite/include/slite
    ${OHOS_DIR}/foundation/ability/ability_lite/interfaces/inner_api/abilitymgr_lite/slite
    ${OHOS_DIR}/foundation/ability/ability_lite/interfaces/kits/ability_lite/slite
    ${OHOS_DIR}/commonlibrary/utils_lite/memory/include
    ${OHOS_DIR}/base/global/resource_management_lite/interfaces/inner_api/include
    ${OHOS_DIR}/foundation/bundlemanager/bundle_framework_lite/services/bundlemgr_lite/tools/signmode
    ${OHOS_DIR}/foundation/bundlemanager/bundle_framework_lite/services/bundlemgr_lite/include
    ${OHOS_DIR}/foundation/bundlemanager/bundle_framework_lite/utils/bundle_lite
    ${ROOT_DIR}/application/wearable/ohos_deps
    ${ROOT_DIR}/application/wearable/nativeapp/nativeui/include
    ${ROOT_DIR}/application/wearable/nativeapp/nativelauncher/include
    ${ROOT_DIR}/application/wearable/nativeapp/nativeui/uiservice/include
    ${OHOS_DIR}/third_party/cJSON
    ${OHOS_DIR}/foundation/graphic/graphic_utils_lite/wearable/interfaces/kits
    ${ROOT_DIR}/middleware/services/gui/uikit/config/wearable
    ${OHOS_DIR}/foundation/graphic/graphic_utils_lite/wearable/interfaces/innerkits
    ${OHOS_DIR}/base/location/location_lite/sample
    ${OHOS_DIR}/base/sensors/sensor_lite/sample
    ${OHOS_DIR}/base/security/permission_lite/services/pms/include/hals
    ${OHOS_DIR}/commonlibrary/utils_lite/include
    ${OHOS_DIR}/commonlibrary/utils_lite/hals/file
)

set(PRIVATE_DEFINES
    OHOS_DEBUG
)

if("JS_ENABLE" IN_LIST DEFINES)
set(PUBLIC_DEFINES
    SUPPORT_AT_JSINSTALL
)
endif()

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -std=c++11
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
