#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "appverify_base")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${OHOS_DIR}/commonlibrary/utils_lite/include
    ${OHOS_DIR}/base/security/appverify/interfaces/innerkits/appverify_lite/include
    ${OHOS_DIR}/base/security/appverify/interfaces/innerkits/appverify_lite/products/ipcamera
    ${OHOS_DIR}/base/startup/init/interfaces/innerkits/include/syspara
    ${OHOS_DIR}/base/startup/init/interfaces/innerkits/include
    ${OHOS_DIR}/base/startup/init/interfaces/innerkits/include/token
    ${OHOS_DIR}/base/startup/init/interfaces/innerkits/include/param
    ${OHOS_DIR}/base/hiviewdfx/hiview_lite
    ${OHOS_DIR}/foundation/systemabilitymgr/samgr_lite/interfaces/kits/samgr
    ${OHOS_DIR}/base/hiviewdfx/hilog_lite/interfaces/native/kits/hilog_lite
    ${OHOS_DIR}/base/hiviewdfx/hievent_lite/interfaces/native/innerkits
)

set(SOURCES
    ${OHOS_DIR}/base/security/appverify/interfaces/innerkits/appverify_lite/products/default/app_verify_default.c
    ${OHOS_DIR}/base/security/appverify/interfaces/innerkits/appverify_lite/products/ipcamera/app_verify_base.c
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-strict-prototypes
)

set(COMPONENT_CXXCFLAGS
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

