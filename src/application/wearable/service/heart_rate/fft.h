/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Create Date : 2025-08
 */

#ifndef FFT_H
#define FFT_H

#include "ppg_heart_variable.h"


fft_cfg* fft_alloc(int nfft);
void fft(fft_cfg *cfg, const fft_cpx *fin, fft_cpx *fout);

#endif