/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: audio calib cmd type header file
 * Author : AuthorNameMagicTag
 * Create : 2023-07-10
 */

#ifndef FAC_TLV_H
#define FAC_TLV_H

#ifdef __cplusplus
extern "C" {
#endif

/* 协议5.7.8 type类型定义 */
typedef enum {
    MIC_LOOP_IDX_TYPE = 1,
    MIC_LOOP_SWITCH_TYPE = 2
} mic_loop_type_t;
/* 协议5.7.9 type类型定义 */
typedef enum {
    SPK_CALIB_VALUE_TYPE = 1,
} spk_calib_type_t;

/* 协议5.7.10 type类型定义 */
typedef enum {
    SPK_CALIB_VALUE_SAVE_TYPE = 1,
} spk_calib_save_type_t;

/* 协议5.7.11 type类型定义 */
typedef enum {
    MIC_SWITCH_TYPE = 1,
} mic_switch_type_t;
 
/* 协议5.7.12 type类型定义 */
typedef enum {
    MIC_CALIB_IDX_TYPE = 1,
    MIC_CALIB_VALUE_TYPE = 2,
} mic_calib_type_t;
 
/* 协议5.7.13 type类型定义 */
typedef enum {
    MIC_CALIB_SAVE_IDX_TYPE = 1,
    MIC_CALIB_SAVE_VALUE_TYPE = 2,
} mic_calib_save_type_t;

#ifdef __cplusplus
}
#endif

#endif