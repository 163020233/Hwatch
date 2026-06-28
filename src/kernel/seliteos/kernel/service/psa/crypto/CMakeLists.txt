#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "tee_psa_crypto")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/dispatch/dispatch_psa_crypto_algorithm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dispatch/dispatch_psa_crypto_key_manage.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dispatch/dispatch_psa_crypto_inner.c

    ${CMAKE_CURRENT_SOURCE_DIR}/common/psa_crypto_common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/ffdh_param.c

    ${CMAKE_CURRENT_SOURCE_DIR}/wrap/psa_crypto_wrap_driver_common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/wrap/psa_crypto_wrap_driver_crc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/wrap/psa_crypto_wrap_driver_ecc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/wrap/psa_crypto_wrap_driver_ffdh.c
    ${CMAKE_CURRENT_SOURCE_DIR}/wrap/psa_crypto_wrap_driver_hash.c
    ${CMAKE_CURRENT_SOURCE_DIR}/wrap/psa_crypto_wrap_driver_rsa.c
    ${CMAKE_CURRENT_SOURCE_DIR}/wrap/psa_crypto_wrap_driver_symc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/wrap/psa_crypto_wrap_driver_trng.c
    ${CMAKE_CURRENT_SOURCE_DIR}/wrap/psa_crypto_wrap_driver_kdf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/wrap/psa_crypto_wrap_secure_storage.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/dispatch
    ${CMAKE_CURRENT_SOURCE_DIR}/common
    ${CMAKE_CURRENT_SOURCE_DIR}/wrap
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/middleware/utils/libteec/psa/include
    ${ROOT_DIR}/include/middleware/utils
    ${ROOT_DIR}/kernel/seliteos/kernel/base/communication/tesvec/include
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
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
    false
)

build_component()
