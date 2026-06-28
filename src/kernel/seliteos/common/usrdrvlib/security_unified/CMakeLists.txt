#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "usrdrv_security_unified")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/drvbox_call.S
    ${CMAKE_CURRENT_SOURCE_DIR}/drvbox_call.c

    ${ROOT_DIR}/drivers/drivers/driver/security_unified/service_layer/sl_common.c
    ${ROOT_DIR}/drivers/drivers/driver/security_unified/service_layer/trng.c
    ${ROOT_DIR}/drivers/drivers/driver/security_unified/service_layer/security_init.c
    ${ROOT_DIR}/drivers/drivers/driver/security_unified/service_layer/cipher_simple.c
    ${ROOT_DIR}/drivers/drivers/driver/security_unified/service_layer/pke.c
    ${ROOT_DIR}/drivers/drivers/driver/security_unified/service_layer/sha256_simple.c
    ${ROOT_DIR}/drivers/drivers/driver/security_unified/service_layer/sm3_simple.c
    ${ROOT_DIR}/drivers/drivers/driver/security_unified/service_layer/km_simple.c
    ${ROOT_DIR}/drivers/drivers/driver/security_unified/service_layer/mac_simple.c
    ${ROOT_DIR}/drivers/drivers/driver/security_unified/service_layer/hmac_sha256_simple.c
    ${ROOT_DIR}/drivers/drivers/driver/security_unified/service_layer/hmac_sm3_simple.c
    ${ROOT_DIR}/drivers/drivers/driver/security_unified/service_layer/crc_simple.c
    ${ROOT_DIR}/drivers/drivers/driver/security_unified/service_layer/hkdf_simple.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/
    ${ROOT_DIR}/include/driver/security_unified
)

set(PRIVATE_HEADER
)

set(PRIVATE_DEFINES
    -DLOSCFG_BOX    # support syscall
    -DSYSCALLS_H
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
