#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "hctest")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${OHOS_DIR}/test/xts/tools/lite/hctest/include
    ${OHOS_DIR}/third_party/unity/src
    ${OHOS_DIR}/commonlibrary/utils_lite/include
    ${OHOS_DIR}/foundation/systemabilitymgr/samgr_lite/interfaces/kits/samgr
    ${OHOS_DIR}/base/startup/bootstrap_lite/services/source
    ${OHOS_DIR}/base/startup/init/interfaces/innerkits/include/syspara
    ${OHOS_DIR}/test/xts/device_attest_lite/interfaces/kit/js/include
    ${OHOS_DIR}/test/xts/device_attest_lite/interfaces/innerkits
    ${OHOS_DIR}/test/xts/device_attest_lite/framework/mini
)

set(SOURCES
    ${OHOS_DIR}/third_party/unity/src/unity.c
    ${OHOS_DIR}/test/xts/tools/lite/hctest/src/hctest.c
    ${OHOS_DIR}/test/xts/tools/lite/hctest/src/hctest_service.c
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-error
)

set(COMPONENT_CXXCFLAGS
)

set(PUBLIC_DEFINES
)

set(PRIVATE_DEFINES
    UNITY_INCLUDE_CONFIG_H
    INTER_ATTEST_MINI_MODULE
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()

