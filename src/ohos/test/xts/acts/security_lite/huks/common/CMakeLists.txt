#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "huks_test_common")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${OHOS_DIR}/test/xts/acts/security_lite/huks/common/include
    ${OHOS_DIR}/commonlibrary/utils_lite/include
    ${OHOS_DIR}/base/security/huks/interfaces/inner_api/huks_standard/main/include
    ${OHOS_DIR}/base/hiviewdfx/hilog_lite/interfaces/native/kits/hilog_lite
    ${OHOS_DIR}/base/hiviewdfx/hiview_lite
    ${OHOS_DIR}/foundation/systemabilitymgr/samgr_lite/interfaces/kits/samgr
    ${OHOS_DIR}/base/hiviewdfx/hievent_lite/interfaces/native/innerkits
)

set(SOURCES
    ${OHOS_DIR}/test/xts/acts/security_lite/huks/common/hks_test_aes.c
    ${OHOS_DIR}/test/xts/acts/security_lite/huks/common/hks_test_api_performance.c
    ${OHOS_DIR}/test/xts/acts/security_lite/huks/common/hks_test_cipher.c
    ${OHOS_DIR}/test/xts/acts/security_lite/huks/common/hks_test_common.c
    ${OHOS_DIR}/test/xts/acts/security_lite/huks/common/hks_test_curve25519.c
    ${OHOS_DIR}/test/xts/acts/security_lite/huks/common/hks_test_file_operator.c
    ${OHOS_DIR}/test/xts/acts/security_lite/huks/common/hks_test_mem.c
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-missing-field-initializers
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

