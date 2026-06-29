#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
#===============================================================================


set(COMPONENT_NAME "nfc_at")
if("SUPPORT_ALIPAY_HSC32I1" IN_LIST DEFINES)
    set(ALIPAY_SDK_DIR ${ROOT_DIR}/vendor/alipay_sdk/rv70_llvm_HSC32I1)
elseif("SUPPORT_ALIPAY_SEC" IN_LIST DEFINES)
    set(ALIPAY_SDK_DIR ${ROOT_DIR}/vendor/alipay_sdk/alipay_sec_sdk_v3.0.0/lib/rv70_llvm)
endif()
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/at_nfc_cmd_register.c
    ${CMAKE_CURRENT_SOURCE_DIR}/at/at_nfc.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/at
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/at
    ${ROOT_DIR}/middleware/utils/at/at
    ${ROOT_DIR}/middleware/utils/at/at/include
    ${ROOT_DIR}/middleware/utils/at/at/src
    ${ALIPAY_SDK_DIR}/share/common/inc
    ${ROOT_DIR}/application/wearable/alipay/service
)

if("SUPPORT_ALIPAY_TAP" IN_LIST DEFINES)
    list(APPEND PRIVATE_HEADER
        ${ALIPAY_SDK_DIR}/app/nfc_offline_pay/inc
        ${ALIPAY_SDK_DIR}/app/scan_pay/inc
    )
endif()

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
    AT_COMMAND_NFC
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    true
)

build_component()

install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/at "*.h")
install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/src "*.h")
