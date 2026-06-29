/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: FP32_16 EXCHANGE
 * Author: czc
 * Create: 2025-1-5
 */

#ifndef FP16_CONVERT_H
#define FP16_CONVERT_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

void fp322fp16(short *__restrict out, const float in);

void fp162fp32(float *__restrict out, const short in);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif