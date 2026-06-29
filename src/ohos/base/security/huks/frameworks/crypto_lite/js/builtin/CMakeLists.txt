#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "ace_kit_cipher_static")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${OHOS_DIR}/base/hiviewdfx/hilog_lite/interfaces/native/kits/hilog_lite
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${OHOS_DIR}/base/security/huks/frameworks/crypto_lite/cipher/include
    ${OHOS_DIR}/device/soc/hisilicon/common/hardware/litewearable_base/mem_hal
    ${OHOS_DIR}/base/security/appverify/interfaces/innerkits/appverify_lite/include
    ${OHOS_DIR}/base/security/appverify/interfaces/innerkits/appverify_lite/products/ipcamera
)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cipher_module.cpp
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -std=c++11
)

set(COMPONENT_CXXCFLAGS
)

set(PUBLIC_DEFINES
)

set(PRIVATE_DEFINES
    HUKS_LITEWEARABLE_SUPPORT_ENABLED
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()

