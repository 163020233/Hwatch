/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: header file for sea alg0 api
 * Author: audio
 */

#ifndef __SEA_ALG0_API_H__
#define __SEA_ALG0_API_H__

#include "audio_alg.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define KEYWORDNUM 200

typedef enum {
    AAI_ALG_CMD_GET_VERSION = 0x0600,
    AAI_ALG_CMD_SET_PARAM,
    AAI_ALG_CMD_GET_SCR_BUF_SIZE,
    AAI_ALG_CMD_SET_SCR_BUF,
    AAI_ALG_CMD_MAX,
} aai_alg_cmd_id;

typedef struct {
    td_u32 score[KEYWORDNUM];   /* keyword分值 */
    td_u32 iswakeup;            /* 是否唤醒 */
    td_u32 index;               /* 唤醒的keyword索引 */
} audio_kws_out;

audio_alg_component *aai_alg0_report_entry(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __SEA_ALG0_API_H__ */
