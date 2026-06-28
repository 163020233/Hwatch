/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#ifndef FUZZ_DEMO_H
#define FUZZ_DEMO_H


#include <stdio.h>
#include <stdint.h>

typedef enum State {
    FAILED,
    ASRCTC,
    NLU,
    FINISH
} State;

typedef struct Hyps {
    int prefix[120];
    int len;
} Hyps;

int32_t fuzzEnvInit(const char *asrctcModelFile, const char *asrctcModelOffsetPath, const char *nluModelFile, const char *posEmbFile);
State audioMemProcess(void *audioMem, uint32_t audioMemSize, Hyps *hyps);
State audioFileProcess(const char *audioFile, Hyps *hyps);
int32_t getNluResultIndex(Hyps *hyps);
void fuzzEnvFinalize(void);

int32_t fuzzyCommandWordMain(void);

#endif /* FUZZ_DEMO_H */