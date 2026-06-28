/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: NLU_SLOT_POSTPROC_H
 * Author: zcl
 * Create: 2025-10-15
 */
#ifndef NLU_SLOT_POSTPROC_H
#define NLU_SLOT_POSTPROC_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NLU_SLOT_NUM 229
#define NLU_POSTPROC_IN_MAX 800
#define AUDIO_SLOTS_ARABIC_MAX 500

int evaluate_single_unitfiller(void *handle, char *slots_arabic);
int nlu_net_mapto_postproc(int *nlu_input_seq_data, int nlu_out_index,
    int *nlu_out_slot_data, int seq_len, char *nlu_postproc_in);
int nlu_slot_postproc(void *handle, char *nlu_net_out, char *fuzz_result);
void getSlotMax(float *nlu_net_slot_out, int length, int *nlu_out_slot_data);
#ifdef __cplusplus
}
#endif

#endif