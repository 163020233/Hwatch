#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "bundle")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${OHOS_DIR}/foundation/bundlemanager/bundle_framework_lite/services/bundlemgr_lite/include
    ${OHOS_DIR}/foundation/bundlemanager/bundle_framework_lite/interfaces/inner_api/bundlemgr_lite/slite
    ${OHOS_DIR}/foundation/bundlemanager/bundle_framework_lite/interfaces/inner_api/bundlemgr_lite
    ${OHOS_DIR}/foundation/bundlemanager/bundle_framework_lite/interfaces/kits/bundle_lite/slite
    ${OHOS_DIR}/foundation/bundlemanager/bundle_framework_lite/interfaces/kits/bundle_lite
    ${OHOS_DIR}/foundation/bundlemanager/bundle_framework_lite/utils/bundle_lite
    ${OHOS_DIR}/foundation/bundlemanager/bundle_framework_lite/frameworks/bundle_lite/include
    ${OHOS_DIR}/base/hiviewdfx/hilog_lite/interfaces/native/kits/hilog_lite
    ${OHOS_DIR}/base/security/permission_lite/interfaces/kits
    ${OHOS_DIR}/base/security/permission_lite/services/pms/include
    ${OHOS_DIR}/foundation/ability/ability_lite/frameworks/want_lite/include
    ${OHOS_DIR}/foundation/ability/ability_lite/interfaces/kits/want_lite
    ${OHOS_DIR}/foundation/ability/ability_lite/interfaces/kits/ability_lite/slite
    ${OHOS_DIR}/foundation/ability/ability_lite/interfaces/inner_api/abilitymgr_lite
    ${OHOS_DIR}/foundation/systemabilitymgr/samgr_lite/interfaces/kits/samgr
    ${OHOS_DIR}/foundation/systemabilitymgr/samgr_lite/interfaces/kits/registry
    ${OHOS_DIR}/commonlibrary/utils_lite/include
    ${OHOS_DIR}/commonlibrary/utils_lite/memory/include
    ${OHOS_DIR}/third_party/cJSON
    ${OHOS_DIR}/base/hiviewdfx/hilog_lite/interfaces/native/innerkits/hilog
    ${OHOS_DIR}/base/hiviewdfx/hilog_lite/interfaces/native/innerkits
)

set(SOURCES
    ${OHOS_DIR}/foundation/bundlemanager/bundle_framework_lite/frameworks/bundle_lite/src/ability_info.cpp
    ${OHOS_DIR}/foundation/bundlemanager/bundle_framework_lite/frameworks/bundle_lite/src/ability_info_utils.cpp
    ${OHOS_DIR}/foundation/bundlemanager/bundle_framework_lite/frameworks/bundle_lite/src/bundle_info.cpp
    ${OHOS_DIR}/foundation/bundlemanager/bundle_framework_lite/frameworks/bundle_lite/src/bundle_info_utils.cpp
    ${OHOS_DIR}/foundation/bundlemanager/bundle_framework_lite/frameworks/bundle_lite/src/element_name.cpp
    ${OHOS_DIR}/foundation/bundlemanager/bundle_framework_lite/frameworks/bundle_lite/src/module_info.cpp
    ${OHOS_DIR}/foundation/bundlemanager/bundle_framework_lite/frameworks/bundle_lite/src/slite/bundle_manager.cpp
    ${OHOS_DIR}/foundation/bundlemanager/bundle_framework_lite/frameworks/bundle_lite/src/slite/bundle_manager_inner.cpp
    ${OHOS_DIR}/foundation/bundlemanager/bundle_framework_lite/frameworks/bundle_lite/src/slite/bundlems_slite_client.cpp
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-non-c-typedef-for-linkage
    -Wno-error=sign-compare
)

set(COMPONENT_CXXCFLAGS
    -std=c++11
)

set(PUBLIC_DEFINES
)

set(PRIVATE_DEFINES
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()

