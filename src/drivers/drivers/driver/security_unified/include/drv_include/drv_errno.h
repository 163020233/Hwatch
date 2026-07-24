/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 *
 * Description: DFX error-chain support for security_unified.
 */
#ifndef DRV_ERRNO_H
#define DRV_ERRNO_H

#include "crypto_type.h"
#include "drv_file_id.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DRV_ERR_STACK_DEPTH 8

extern int32_t drv_err_top;
extern uint32_t drv_err_stack[DRV_ERR_STACK_DEPTH];

/* Stack entry: return PC low 24 bits in [31:8], low 8-bit error code in [7:0]. */
#define DRV_ERR_PC_LOW_MASK 0x00FFFFFFU
#define DRV_ERR_PC_REGION_MASK 0xFF000000U

#define drv_error_pc(return_pc, err_code) \
    ((((uint32_t)(return_pc) & DRV_ERR_PC_LOW_MASK) << 8) | ((uint32_t)(err_code) & 0xFFU))

#define drv_err_get_return_pc_low(err_code) (((uint32_t)(err_code) >> 8) & DRV_ERR_PC_LOW_MASK)
#define drv_err_get_errcode(err_code) ((uint32_t)(err_code) & 0xFFU)

void drv_set_err_info(uint32_t err_code) __attribute__((noinline, not_tail_called));
void drv_print_err_chain(void);

/* The public call form intentionally stays at two arguments. */
#define drv_set_err(errcode, err_msg) do { \
    (void)(err_msg); \
    drv_set_err_info((uint32_t)(errcode) & 0xFFU); \
    /* Keep a real post-call instruction so the backend preserves an exact return PC. */ \
    __asm__ volatile("nop"); \
} while (0)

/* Variadic tails accept existing printf-style messages without retaining their strings. */
#define drv_err_chk_return(cond, err_ret, ...) do { \
    if (cond) { \
        drv_set_err((err_ret), ""); \
        return (err_ret); \
    } \
} while (0)

#define drv_err_chk_return_void(cond, ...) do { \
    if (cond) { \
        drv_set_err(CRYPTO_FAILURE, ""); \
        return; \
    } \
} while (0)

#define drv_err_chk_goto(cond, label, ...) do { \
    if (cond) { \
        drv_set_err(CRYPTO_FAILURE, ""); \
        goto label; \
    } \
} while (0)

#define drv_err_chk_goto_with_ret(ret, cond, label, err_ret, ...) do { \
    if (cond) { \
        drv_set_err((err_ret), ""); \
        (ret) = (err_ret); \
        goto label; \
    } \
} while (0)

#define drv_err_chk_func_return(func, ret, ...) do { \
    if ((ret) != CRYPTO_SUCCESS) { \
        drv_set_err((ret), ""); \
        return (ret); \
    } \
} while (0)

#ifdef __cplusplus
}
#endif

#endif
