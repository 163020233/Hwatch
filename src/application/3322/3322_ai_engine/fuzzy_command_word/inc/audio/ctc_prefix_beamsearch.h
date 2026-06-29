/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: CTC_PREFIX_BEAMSEARCH
 * Author: czc
 * Create: 2025-1-5
 */

#ifndef CTC_PREFIX_BEAMSEARCH
#define CTC_PREFIX_BEAMSEARCH

#include <math.h>
#include <string.h>
#include "window_coeff.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct sin_hyps {
    int prefix[MAX_SENTENCE_LEN];
    int len;
} sin_hyps;

typedef struct hyps_prob {
    int prefix[MAX_SENTENCE_LEN];
    int len;
    float n_pb;
    float n_pnb;
} hyps_prob;

typedef struct long_hyps {
    hyps_prob candidate[MAX_SENTENCE_CANDIDATE_LEN];
    int len;
} long_hyps;

typedef struct nlu_in {
    int seq_data[NLU_INPUT_LENGTH];
    int seq_len;
} nlu_in;

extern float before_softmax[CTC_FEATURE_FRAME_LEN_120][CTC_CHAR_LEN_1289];
void ctc_prefix_beamsearch(float *ctc_out, long_hyps *cur_hyps, int chunk_num);

void renew_cur_hyps(long_hyps *cur_hyps, long_hyps *next_hyps, int len, int *score_index_range);

int check_dictionary_key(long_hyps *dict, int *key, int key_len);

float log_add(float *in, int log_len);

void sort_topk(float *a, int length, int k, int *b);

#ifdef __cplusplus
}
#endif
#endif