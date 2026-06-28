/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Fbank
 * Author: czc
 * Create: 2025-1-5
 */

#ifndef FBANK_H
#define FBANK_H

#include "audio_vqe_typedef.h"
#include "window_coeff.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define  FBANK_FFT_LEN             512
#define  FBANK_FFT_HALF_LEN        256
#define  FBANK_SPECT257_LEN        257
#define  FBANK_FRAME_LEN           400
#define  FBANK_BIN_LEN             80
#define  FFT_Q9                    9
#define  PRE_EMPHA_COEFF_0P97      (0.97)
#define  FBANK_THRESH              0.000001
void cal_ffbank(imedia_int16 *data_in, imedia_uint16 *ffbank);
void cal_ffbank_mel(imedia_float *fft_result, imedia_uint16 *ffbank);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif