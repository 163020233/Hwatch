/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: crypto common macro header. \n
 *
 * History: \n
 * 2023-03-22, Create file. \n
 */
#ifndef CRYPTO_COMMON_MACRO_H
#define CRYPTO_COMMON_MACRO_H

#include <stdbool.h>
#include <stdint.h>

#define crypto_cpu_to_be32(v) ((((uint32_t)(v)) >> 24) | ((((uint32_t)(v)) >> 8) & 0xff00) \
    | ((((uint32_t)(v)) << 8) & 0xff0000) | (((uint32_t)(v)) << 24))

#define crypto_u8_to_word(u8, word, i) ((word) |= ((u8)[(i) * 4] & 0xFF) | (((u8)[(i) * 4 + 1] & 0xFF) << 8) \
    | (((u8)[(i) * 4 + 2] & 0xFF) << 16) | (((u8)[(i) * 4 + 3] & 0xFF) << 24))

#define crypto_word_to_u8(word, u8, i) do { \
    (u8)[(i) * 4] = (word) & 0xFF; \
    (u8)[(i) * 4 + 1] = ((word) >> 8) & 0xFF; \
    (u8)[(i) * 4 + 2] = ((word) >> 16) & 0xFF; \
    (u8)[(i) * 4 + 3] = ((word) >> 24) & 0xFF; \
} while (0)

inline uint32_t crypto_u8_to_u32(const uint8_t data[4])
{
    uint32_t word = 0;
    word |= data[0];
    word |= (data[1] << 8);     // 1, 8: bit[8:15]
    word |= (data[2] << 16);    // 2, 16: bit[16:23]
    word |= (data[3] << 24);    // 3, 24: bit[24:31]
    return word;
}

#define crypto_max(a, b)    ((a) > (b)) ? (a) : (b)
#define crypto_min(a, b)    ((a) > (b)) ? (b) : (a)

#ifndef crypto_unused
#define crypto_unused(x)    (void)(x)
#endif

#define crypto_array_size(arr)  (sizeof(arr) / sizeof((arr)[0]))

#define crypto_set_bits(src, bit)   ((src) |= (1 << (bit)))
#define crypto_clear_bits(src, bit) ((src) &= ~(1 << (bit)))
/* Error Check. */
#define crypto_chk_print(cond, ...) do {      \
    if (cond) {                                         \
        crypto_log_err(__VA_ARGS__);                       \
    }                                                   \
} while (0)

#define crypto_chk_return(cond, err_ret, ...) do {      \
    if (cond) {                                         \
        crypto_log_err(__VA_ARGS__);                    \
        return err_ret;                                 \
    }                                                   \
} while (0)

#define crypto_chk_return_void(cond, ...) do { \
    if (cond) { \
        crypto_log_err(__VA_ARGS__);                       \
        return; \
    }   \
} while (0)

#define crypto_chk_goto(cond, label, ...) do {          \
    if (cond) {                                         \
        crypto_log_err(__VA_ARGS__);                    \
        goto label;                                     \
    }                                                   \
} while (0)

/* Input Params Check. */
#define crypto_param_require(cond)  do {                \
    if (!(cond)) {                                      \
        crypto_log_err("Param Check %s failed\n", #cond);  \
        return CRYPTO_FAILURE;                          \
    }                                                   \
} while (0)

#define crypto_param_check(cond)  do {                  \
    if (cond) {                                         \
        crypto_log_err("Param Check %s failed\n", #cond);  \
        return CRYPTO_FAILURE;                          \
    }                                                   \
} while (0)

#define crypto_chk_goto_with_ret(ret, cond, label, err_ret, ...) do {           \
    if (cond) {                                                                 \
        crypto_log_err(__VA_ARGS__);                                        \
        ret = err_ret;                                                          \
        goto label;                                                             \
    }                                                                           \
} while (0)

#if defined(CONFIG_CRYPTO_LOG_PTR_CHK_INFO_DISABLE)
#define crypto_null_ptr_chk(ptr, errno)    \
    crypto_chk_return((ptr) == NULL, errno, "param is NULL\n")
#else
#define crypto_null_ptr_chk(ptr, errno)    \
    crypto_chk_return((ptr) == NULL, errno, #ptr " is NULL\n")
#endif

#if defined(CRYPTO_HAL_FUNC_TRACE_ENABLE)
#define crypto_hal_func_enter()        crypto_log_trace("%s===>Enter\n", __func__)
#define crypto_hal_func_exit()         crypto_log_trace("%s<===Exit\n", __func__)
#else
#define crypto_hal_func_enter()
#define crypto_hal_func_exit()
#endif

#if defined(CRYPTO_KAPI_FUNC_TRACE_ENABLE)
#define crypto_kapi_func_enter()        crypto_log_trace("%s===>Enter\n", __func__)
#define crypto_kapi_func_exit()         crypto_log_trace("%s<===Exit\n", __func__)
#else
#define crypto_kapi_func_enter()
#define crypto_kapi_func_exit()
#endif

#if defined(CRYPTO_DRV_FUNC_TRACE_ENABLE)
#define crypto_drv_func_enter()        crypto_print("%s===>Enter\n", __func__)
#define crypto_drv_func_exit()         crypto_print("%s<===Exit\n", __func__)
#else
#define crypto_drv_func_enter()
#define crypto_drv_func_exit()
#endif

#if defined(CRYPTO_DISPATCH_FUNC_TRACE_ENABLE)
#define crypto_dispatch_func_enter()        crypto_log_trace("%s===>Enter\n", __func__)
#define crypto_dispatch_func_exit()         crypto_log_trace("%s<===Exit\n", __func__)
#else
#define crypto_dispatch_func_enter()
#define crypto_dispatch_func_exit()
#endif

#if defined(CRYPTO_UAPI_TRNG_DEBUG_ENABLE)
#define crypto_uapi_func_enter()    crypto_log_trace("%s ===>Enter\n", __func__)
#define crypto_uapi_func_exit()     crypto_log_trace("%s <===Exit\n", __func__)
#else
#define crypto_uapi_func_enter()
#define crypto_uapi_func_exit()
#endif

#ifndef crypto_print_func_err
#define crypto_print_func_err(_func, _err_code) do { \
    crypto_log_err("%s failed! error code: 0x%x \r\n", #_func, _err_code); \
} while (0)
#endif

#ifndef crypto_chk_func_return
#define crypto_chk_func_return(func, ret) do { \
    if ((ret) != CRYPTO_SUCCESS) { \
        crypto_print_func_err(func, ret);   \
        return ret; \
    }   \
} while (0)
#endif

#ifndef crypto_check_param
#define crypto_check_param(err_level, module, cond, err_code) do { \
    if (cond) { \
        crypto_log_err("pke invalid parameter: %s \r\n", #cond);   \
        return CRYPTO_COMPAT_ERRNO(CRYPTO_ERROR_ENV, err_level, module, err_code);    \
    }   \
} while (0)
#endif

#ifndef crypto_check_param_null
#define crypto_check_param_null(err_level, module, _val) do { \
    crypto_check_param(err_level, module, ((_val) == NULL), ERROR_PARAM_IS_NULL); \
} while (0)
#endif

#ifndef drv_crypto_check_param
#define drv_crypto_check_param(module, cond) do { \
    crypto_check_param(ERROR_LAYER_DRV, module, cond, ERROR_INVALID_PARAM); \
} while (0)
#endif

#ifndef drv_crypto_pke_check_param
#define drv_crypto_pke_check_param(cond) do { \
    drv_crypto_check_param(ERROR_MODULE_PKE, cond); \
} while (0)
#endif

#ifndef drv_crypto_pke_check_param_null
#define drv_crypto_pke_check_param_null(_val) do { \
    drv_crypto_check_param_null(ERROR_MODULE_PKE, _val); \
} while (0)
#endif

#if defined(CONFIG_ROMBOOT_ENV)
#define ROM_SHARE_BSS  __attribute__((section(".rom_share_bss")))
#else
#define ROM_SHARE_BSS
#endif

#endif
