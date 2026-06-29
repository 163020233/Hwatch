#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "module_ActsHuksHalFunctionTest")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${OHOS_DIR}/base/security/huks/interfaces/inner_api/huks_standard/main/include
    ${OHOS_DIR}/base/iothardware/peripheral/interfaces/inner_api
    ${OHOS_DIR}/base/security/huks/frameworks/huks_standard/main/common/include
    ${OHOS_DIR}/test/xts/acts/security_lite/huks/common/include
    ${OHOS_DIR}/commonlibrary/utils_lite/include
    ${OHOS_DIR}/test/xts/tools/lite/hctest/include
    ${OHOS_DIR}/third_party/unity/src
    ${OHOS_DIR}/kernel/liteos_m/kal
    ${OHOS_DIR}/foundation/systemabilitymgr/samgr_lite/interfaces/kits/samgr
)

set(SOURCES
    ${OHOS_DIR}/test/xts/acts/security_lite/huks/liteos_m_adapter/hks_aes_test.c
    ${OHOS_DIR}/test/xts/acts/security_lite/huks/liteos_m_adapter/hks_agreement_test.c
    ${OHOS_DIR}/test/xts/acts/security_lite/huks/liteos_m_adapter/hks_bn_exp_mod_test.c
    ${OHOS_DIR}/test/xts/acts/security_lite/huks/liteos_m_adapter/hks_cipher_test.c
    ${OHOS_DIR}/test/xts/acts/security_lite/huks/liteos_m_adapter/hks_curve25519_test.c
    ${OHOS_DIR}/test/xts/acts/security_lite/huks/liteos_m_adapter/hks_delete_test.c
    ${OHOS_DIR}/test/xts/acts/security_lite/huks/liteos_m_adapter/hks_derive_test.c
    ${OHOS_DIR}/test/xts/acts/security_lite/huks/liteos_m_adapter/hks_exist_test.c
    ${OHOS_DIR}/test/xts/acts/security_lite/huks/liteos_m_adapter/hks_generate_key_test.c
    ${OHOS_DIR}/test/xts/acts/security_lite/huks/liteos_m_adapter/hks_generate_random_test.c
    ${OHOS_DIR}/test/xts/acts/security_lite/huks/liteos_m_adapter/hks_hash_test.c
    ${OHOS_DIR}/test/xts/acts/security_lite/huks/liteos_m_adapter/hks_mac_test.c
    ${OHOS_DIR}/test/xts/acts/security_lite/huks/liteos_m_adapter/hks_others_test.c
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -D_CUT_ED25519_
    -D_CUT_LOG_
    -Wno-missing-field-initializers
    -Wno-strict-prototypes
    -Wno-macro-redefined
    -Wno-unused-but-set-variable
    -Wno-unused-function
)

set(COMPONENT_CXXCFLAGS
)

set(PUBLIC_DEFINES
)

set(PRIVATE_DEFINES
    UNITY_INCLUDE_CONFIG_H
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()

