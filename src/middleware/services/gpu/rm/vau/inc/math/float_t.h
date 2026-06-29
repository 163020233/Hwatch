/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: rm float convert to float_t (24bit) - 将float转成24bit
 */

#ifndef GPU_API_RM_FLOAT_T_H
#define GPU_API_RM_FLOAT_T_H

#include "td_base.h"

#ifdef __cplusplus
#if __cplusplus
    extern "C" {
#endif
#endif

/*-------------------------------- macro define -------------------------------------------*/
#define MAN_BIT 15
#define INT_BIT (1 + 8 + MAN_BIT)
#define RES_BIT (32 - INT_BIT)

/*-------------------------------- struct define ------------------------------------------*/
typedef union {
    struct {
        unsigned int reserved : RES_BIT;
        unsigned int integer : INT_BIT;
    };

    unsigned int u32;
    float f32;
} float_t_u;

/*-------------------------------- func declares ------------------------------------------*/
td_float_t float_convert_to_float_t(td_float a);
td_float_t float_t_div(td_float_t a, td_float_t b);
td_float_t float_t_mul(td_float_t a, td_float_t b);
td_float_t float_t_add(td_float_t a, td_float_t b);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* GPU_API_RM_FLOAT_T_H */