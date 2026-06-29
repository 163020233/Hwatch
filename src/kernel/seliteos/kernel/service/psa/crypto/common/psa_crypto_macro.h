/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: psa crypto common macro. \n
 *
 * History: \n
 * 2025-03-14, Create file. \n
 */

#ifndef PSA_CRYPTO_MACRO_H
#define PSA_CRYPTO_MACRO_H

#include "los_base.h"
#include "los_mem.h"
#include "pmp_mgr.h"

#define PSA_CRYPTO_BIT_ALIGNED_IN_BYTE(bit)  (((bit) + 7) / 8)
#define PSA_ECC_PUBLIC_KEY_GET_BITS(data_bits) ((data_bits) / 2)

#ifndef psa_crypto_print
#ifdef PSA_CRYPTO_PAKE_DEBUG
#define psa_crypto_print(fmt, ...)    LOS_Printf("[%s:%d]: " fmt, __func__, __LINE__, ##__VA_ARGS__)
#else
#define psa_crypto_print(...)
#endif
#endif
#define psa_crypto_malloc(param)       los_kmalloc(param)
#define psa_crypto_free(param)         los_kfree(param)

#define psa_crypto_array_size(arr)  (sizeof(arr) / sizeof((arr)[0]))

#define psa_unused(param)    (void)(param)

#define psa_crypto_param_chk(cond)  do {                     \
    if (cond) {                                              \
        psa_crypto_print("Param Check %s failed\n", #cond);  \
        return PSA_ERROR_INVALID_ARGUMENT;                   \
    }                                                        \
} while (0)

#define psa_crypto_param_alg_chk(cond)  do {                     \
    if (cond) {                                              \
        psa_crypto_print("Param Check %s failed\n", #cond);  \
        return PSA_ERROR_NOT_SUPPORTED;                   \
    }                                                        \
} while (0)

#define psa_crypto_param_length_chk(cond)  do {                     \
    if (cond) {                                              \
        psa_crypto_print("Param Check %s failed\n", #cond);  \
        return PSA_ERROR_BUFFER_TOO_SMALL;                   \
    }                                                        \
} while (0)

#define psa_crypto_chk_return(cond, err_ret, ...) do {      \
    if (cond) {                                             \
        psa_crypto_print(__VA_ARGS__);                      \
        return err_ret;                                     \
    }                                                       \
} while (0)

#define  psa_crypto_chk_goto_with_ret(ret, cond, label, err_ret, ...) do {           \
    if (cond) {                                                                      \
        psa_crypto_print(__VA_ARGS__);                                               \
        ret = err_ret;                                                               \
        goto label;                                                                  \
    }                                                                                \
} while (0)

#define  psa_crypto_chk_goto(cond, label, ...) do {           \
    if (cond) {                                                                      \
        psa_crypto_print(__VA_ARGS__);                                               \
        goto label;                                                                  \
    }                                                                                \
} while (0)

#define null_param_chk_1(a) do {                         \
    if ((a) == NULL) {                                   \
        psa_crypto_print("param is NULL\n");             \
        return PSA_ERROR_INVALID_ARGUMENT;               \
    }                                                    \
} while (0)

#define null_param_chk_2(a, b) do {                      \
    if ((a) == NULL || (b) == NULL) {                    \
        psa_crypto_print("param is NULL\n");             \
        return PSA_ERROR_INVALID_ARGUMENT;               \
    }                                                    \
} while (0)

#define null_param_chk_3(a, b, c) do {                   \
    if ((a) == NULL || (b) == NULL || (c) == NULL) {     \
        psa_crypto_print("param is NULL\n");             \
        return PSA_ERROR_INVALID_ARGUMENT;               \
    }                                                    \
} while (0)

#define null_param_chk_4(a, b, c, d) do {                              \
    if ((a) == NULL || (b) == NULL || (c) == NULL || (d) == NULL) {    \
        psa_crypto_print("param is NULL\n");                           \
        return PSA_ERROR_INVALID_ARGUMENT;                             \
    }                                                                  \
} while (0)

#define null_param_chk_5(a, b, c, d, e) do {                                                \
    if ((a) == NULL || (b) == NULL || (c) == NULL || (d) == NULL || (e) == NULL) {          \
        psa_crypto_print("param is NULL\n");                                                \
        return PSA_ERROR_INVALID_ARGUMENT;                                                  \
    }                                                                                       \
} while (0)

#define null_param_chk_inner(a, ...)       null_param_chk_##a(__VA_ARGS__)
#define null_param_chk(...)                null_param_chk_inner(__VA_ARGS__)

/* get __VA_ARGS__ param number */
#define count_args_inner(dummy, a, b, c, d, e, f, g, h, i, j, count, ...) count
#define null_param_count_args(...)         count_args_inner(dummy, ##__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

/* psa_crypto_null_param_chk macro actually defined here */
#define psa_crypto_null_param_chk(...)     null_param_chk(null_param_count_args(__VA_ARGS__), __VA_ARGS__)

#endif
