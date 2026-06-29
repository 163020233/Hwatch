/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: mbedtls harden common macro defines.
 *
 * Create: 2026-05-19
 */

#ifndef MBEDTLS_ALT_COMMON_DEF_H
#define MBEDTLS_ALT_COMMON_DEF_H

#define mbedtls_harden_check_return(cond, errcode, fmt, ...) do { \
    if (cond) { \
        mbedtls_printf(fmt, ##__VA_ARGS__);  \
        return errcode; \
    } \
} while (0)

#define mbedtls_harden_check_goto_with_ret(ret, cond, label, errcode, ...) do { \
    if (cond) { \
        mbedtls_printf(__VA_ARGS__);  \
        ret = errcode;  \
        goto label; \
    } \
} while (0)

#endif