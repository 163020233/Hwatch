/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: ASR_H
 * Author: czc
 * Create: 2025-1-5
 */

#ifndef ASR_H
#define ASR_H

#include "audio_vqe_typedef.h"
#include "window_coeff.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define ASR_STRIDE                    32
#define ASR_CHUNK_FRM_LEN            35
#define ASR_POSEMB_LEN            4096
#define ASR_POSEMB_FEA_LEN        64
#define ASR_POSEMB_STRIDE        8
#define ASR_STRIDE_OVERLAP            (ASR_CHUNK_FRM_LEN - ASR_STRIDE)
#define ASR_CHUNK_NUM            15
#define ASR_MAX_FRM_NUM            350000000


typedef struct {
#ifdef FLOAT_OUTPUT
    imedia_float *fbank_in;
    imedia_float *pos_emb;
#else
    imedia_uint16 *fbank_in;
    imedia_uint16 *pos_emb;
#endif
    imedia_float frm_cnt;
    imedia_int32 chunk_cnt;
    imedia_int16 stride;
    imedia_int16 chunk_size;
    imedia_int16 flag;
    imedia_int16 cache_t1;
    imedia_int16 attention_key_size;
    imedia_int16 offset;
    imedia_int16 ctc_cnt;
    imedia_int16 ctc_flag;
} stru_asr_in;

#ifdef FLOAT_OUTPUT
void get_encoder_init(stru_asr_in *asr_in, imedia_float *fbankbuf, imedia_float *pos_emb_buff);

void get_encoder_input(imedia_float *ffbank_f, stru_asr_in *asr_in);
#else
void get_encoder_init(stru_asr_in *asr_in, imedia_uint16 *fbankbuf, imedia_uint16 *pos_emb_buff);

void get_encoder_input(imedia_uint16 *ffbank_f, stru_asr_in *asr_in, asr_pre_post_para *asr_coeff);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif