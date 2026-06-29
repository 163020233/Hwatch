/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: NLU_POSTPROC_TAB_H
 * Author: zcl
 * Create: 2025-10-15
 */
#ifndef NLU_POSTPROC_TAB_H
#define NLU_POSTPROC_TAB_H

#include <string.h>
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define ASR_TOKENIZER_NUM 1315
#define INTENT_TOKENIZER_NUM 105
#define BIO_TOKENIZER_NUM 228

extern const char* intent_list_lowercase[INTENT_TOKENIZER_NUM - 1];
extern const char* asr_tokenizer_list[ASR_TOKENIZER_NUM];
extern const char* intent_tokenizer_list[INTENT_TOKENIZER_NUM];
extern const char* bio_tokenizer_list[BIO_TOKENIZER_NUM];

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif


#endif