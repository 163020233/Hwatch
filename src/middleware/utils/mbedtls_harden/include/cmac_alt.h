/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: mbedtls alt cmac header.
 *
*/
#ifndef CMAC_ALT_H
#define CMAC_ALT_H

#include "mbedtls/build_info.h"
#include "mbedtls/platform_util.h"

#include "security_mac.h"
#include "security_km.h"

#ifdef __cplusplus
extern "C" {
#endif

#define  DRV_SYMC_MAC_HANDLE_SIZE  324
struct mbedtls_cmac_context_t {
    uint8_t mac_handle_buf[DRV_SYMC_MAC_HANDLE_SIZE];
};
#ifdef __cplusplus
}
#endif
#endif