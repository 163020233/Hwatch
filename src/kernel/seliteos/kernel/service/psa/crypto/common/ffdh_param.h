/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: ffdh default parameters header file.
 *
 * Create: 2025-10-23
 */

#ifndef FFDH_PARAM_H
#define FFDH_PARAM_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
    extern "C" {
#endif
#endif

#define FFDH_2048_LENGTH    256
#define FFDH_3072_LENGTH    384
#define FFDH_4096_LENGTH    512

typedef struct {
    const uint8_t *g;
    const uint8_t *p;
    const uint8_t *q;
    uint32_t length;
} ffdh_param;

extern const ffdh_param g_ffdh_2048;

extern const ffdh_param g_ffdh_3072;

extern const ffdh_param g_ffdh_4096;

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif  /* FFDH_PARAM_H */