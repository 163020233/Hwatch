#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "cipher_static")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${OHOS_DIR}/commonlibrary/utils_lite/include
    ${OHOS_DIR}/base/security/appverify/include
    ${OHOS_DIR}/commonlibrary/utils_lite/memory/include
)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cipher_sha256.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cipher_aes.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cipher_base64.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cipher_hmac_sha256.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cipher_md5.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cipher_nv_store.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cipher_trng.c
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-format-extra-args
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
