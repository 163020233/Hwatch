#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "abilitymanager")

set(PUBLIC_HEADER
    ${OHOS_DIR}/foundation/ability/ability_lite/frameworks/abilitymgr_lite/include/slite
    ${OHOS_DIR}/foundation/ability/ability_lite/interfaces/inner_api/abilitymgr_lite
    ${OHOS_DIR}/foundation/ability/ability_lite/interfaces/inner_api/abilitymgr_lite/slite
    ${OHOS_DIR}/foundation/ability/ability_lite/interfaces/kits/ability_lite
    ${OHOS_DIR}/foundation/ability/ability_lite/interfaces/kits/ability_lite/slite
    ${OHOS_DIR}/foundation/ability/ability_lite/interfaces/kits/want_lite
    
)

set(PRIVATE_HEADER
    ${OHOS_DIR}/foundation/ability/ability_lite/interfaces/inner_api/abilitymgr_lite
    ${OHOS_DIR}/foundation/ability/ability_lite/interfaces/inner_api/abilitymgr_lite/slite
    ${OHOS_DIR}/foundation/ability/ability_lite/interfaces/kits/ability_lite
    ${OHOS_DIR}/foundation/ability/ability_lite/interfaces/kits/ability_lite/slite
    ${OHOS_DIR}/foundation/ability/ability_lite/interfaces/kits/want_lite
    ${OHOS_DIR}/foundation/ability/ability_lite/services/abilitymgr_lite/include/util
    ${OHOS_DIR}/foundation/bundlemanager/bundle_framework_lite/interfaces/kits/bundle_lite
    ${OHOS_DIR}/foundation/bundlemanager/bundle_framework_lite/utils/bundle_lite
    ${OHOS_DIR}/commonlibrary/utils_lite/include
    ${OHOS_DIR}/commonlibrary/utils_lite/memory/include
    ${OHOS_DIR}/foundation/systemabilitymgr/samgr_lite/interfaces/kits/registry
    ${OHOS_DIR}/foundation/systemabilitymgr/samgr_lite/interfaces/kits/samgr
    ${OHOS_DIR}/foundation/ability/ability_lite/frameworks/abilitymgr_lite/include/slite
    ${OHOS_DIR}/base/hiviewdfx/hilog_lite/interfaces/native/kits/hilog_lite
    ${OHOS_DIR}/base/hiviewdfx/hilog_lite/interfaces/native/innerkits/hilog
    ${OHOS_DIR}/base/hiviewdfx/hilog_lite/interfaces/native/innerkits
    ${OHOS_DIR}/foundation/ability/ability_lite/services/abilitymgr_lite/include/slite
    ${OHOS_DIR}/foundation/ability/ability_lite/services/abilitymgr_lite/include
    ${OHOS_DIR}/foundation/ability/ability_lite/frameworks/want_lite/include
    ${OHOS_DIR}/foundation/ability/ability_lite/frameworks/abilitymgr_lite/include
    ${OHOS_DIR}/foundation/ability/ability_lite/frameworks/ability_lite/include
)

set(SOURCES
    ${OHOS_DIR}/foundation/ability/ability_lite/frameworks/abilitymgr_lite/src/slite/ability_manager.cpp
    ${OHOS_DIR}/foundation/ability/ability_lite/frameworks/abilitymgr_lite/src/slite/ability_manager_client.cpp
    ${OHOS_DIR}/foundation/ability/ability_lite/frameworks/abilitymgr_lite/src/slite/ability_manager_inner.cpp
    ${OHOS_DIR}/foundation/ability/ability_lite/frameworks/abilitymgr_lite/src/slite/ability_record_state_data.cpp
    ${OHOS_DIR}/foundation/ability/ability_lite/frameworks/abilitymgr_lite/src/slite/abilityms_slite_client.cpp
    ${OHOS_DIR}/foundation/ability/ability_lite/frameworks/abilitymgr_lite/src/slite/mission_info.cpp
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
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

install_sdk(${OHOS_DIR}/foundation/ability/ability_lite/frameworks/ability_lite/include "*.h")
install_sdk(${OHOS_DIR}/foundation/ability/ability_lite/frameworks/abilitymgr_lite/include "*.h")
install_sdk(${OHOS_DIR}/foundation/ability/ability_lite/frameworks/want_lite/include "*.h")
install_sdk(${OHOS_DIR}/foundation/ability/ability_lite/services/abilitymgr_lite/include "*.h")
install_sdk(${OHOS_DIR}/foundation/ability/ability_lite/services/abilitymgr_lite/include/slite "*.h")
install_sdk(${OHOS_DIR}/foundation/ability/ability_lite/services/abilitymgr_lite/include/util "*.h")
install_sdk(${OHOS_DIR}/foundation/ability/ability_lite/interfaces/kits/want_lite "*.h")
install_sdk(${OHOS_DIR}/foundation/ability/ability_lite/interfaces/inner_api/abilitymgr_lite "*.h")