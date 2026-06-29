/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: _WINDOW_coeff_H_
 * Author: czc
 * Create: 2025-1-5
 */

#ifndef _WINDOW_coeff_H_
#define _WINDOW_coeff_H_

#include "audio_vqe_typedef.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define FBANK_LEN_80 80
#define CTC_CHUNK_SIZE_8 8 /* 8 120 */
#define CTC_FEATURE_FRAME_LEN_120 120 /* 8 120 */
#define CTC_FEATURE_LEN_64 64
#define CTC_FC_LEN_65 65
#define CTC_CHAR_LEN_1289 1289
#define PREFIX_BEAMSEARCH_LEN_10 10
#define MAX_SENTENCE_LEN 120
#define MAX_SENTENCE_CANDIDATE_LEN 200
#define NEG_INF_VALUE (-3.40282347e+38)
#define POS_EMB_ROW  64
#define POS_EMB_COL  64
#define NLU_INPUT_LENGTH 25


extern const imedia_uint16 prove_coeff_q16[400];

extern const imedia_float mel_bin2[257][2];

extern imedia_int16 mel_start[80];

extern imedia_int16 mel_end[80];

typedef struct {
    float *ctc_out;
#ifdef FLOAT_OUTPUT
    imedia_float pos_emb_pe[320000];
#else
    imedia_uint16 *pos_emb_pe;  /* 5000x64 */
#endif
#ifdef FLOAT_CTC_LINEAR
    imedia_float ctc_fc_coeff[65][1289];
#endif
}asr_pre_post_para, *asr_pre_post_para_pst;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif