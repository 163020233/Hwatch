/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description:smart spk augment
 * Author:audio
 * Create:2014
 * Update:2019-12-17
 */

#ifndef AUDIO_VQE_DEFINE_H
#define AUDIO_VQE_DEFINE_H

#include "audio_vqe_typedef.h"
#include "audio_vqe_float_fft_neon.h"
#include "audio_vqe_aec_api.h"
#include "audio_vqe_fmp_api.h"
#include "audio_vqe_eq_fir_api.h"
#include "audio_vqe_agc_api.h"
#include "audio_vqe_dmnr_api.h"
#include "audio_vqe_limiter_api.h"
#include "audio_vqe_drc_api.h"
#include "audio_vqe_sinpmwf_api.h"
#include "audio_vqe_hs_api.h"
#include "audio_ai_wnr_api.h"
#include "audio_vqe_voice_openfft_fp32.h"
#ifdef __cplusplus
extern "C" {
#endif

#define AUDIO_VQE_OUT_MAX_LEN 320 /* 1frm 16k 20ms */
#define AUDIO_VQE_HDMI_ON 0x1
#define AUDIO_VQE_MEM_PROTECT_CODE 0x5A5A5A5A /* Memory anti - step sign */
#define AUDIO_VQE_USE_AEC_DEREVERB 1          /* if AEC, FMP, DEREVERB on */

#define AUDIO_VQE_MAX_UINT32 4294967295LL
#define AUDIO_VQE_PROTED_INT 0x5a5a5a5a
#ifdef FRAME_16MS
#define AUDIO_VQE_FRAMELAP_16_K 256
#define AUDIO_VQE_FRAMESIZE_16_K 512
#else
#define AUDIO_VQE_FRAMELAP_16_K 160
#define AUDIO_VQE_FRAMESIZE_16_K 480
#endif
#define AUDIO_VQE_NRSHIFT_16_K 96
#define AUDIO_VQE_FFTSIZE_16_K 512
#define AUDIO_VQE_FFT_HALFSIZE_16_K 256

#define AUDIO_VQE_FRAMELAP_16_K_1024 256
#define AUDIO_VQE_FRAMESIZE_16_K_1024 1024
#define AUDIO_VQE_AEC_OUT_BUFF_LEN 352  /* 22ms */
#define AUDIO_VQE_DATAIN_BUFF 1280      /* 160*8 */
#define AUDIO_VQE_DATAIN_BUFF_12_MS 192 /* sRD_Pointer, */
                                        /* 80-68=12ms ，delay 68ms */
#define AUDIO_VQE_DATAIN_BUFF_62_MS 992
#define AUDIO_VQE_DATAIN_BUFF_8_MS 128
#define AUDIO_VQE_DATAIN_BUFF_2_MS 32

/* sample rate */
#define AUDIO_VQE_SAMPLERATE_8_K 8000
#define AUDIO_VQE_SAMPLERATE_16_K 16000
#define AUDIO_VQE_SAMPLEMASK_8_K 0  /* 8K */
#define AUDIO_VQE_SAMPLEMASK_16_K 1 /* 16k */

#define AUDIO_VQE_ENABLE 1  /* on */
#define AUDIO_VQE_DISABLE 0 /* off */
#define AUDIO_VQE_AIDRB_ENABLE 1  /* on */
#define AUDIO_VQE_AINR_ENABLE 0 /* off */
/* different mic num */
#define AUDIO_VQE_MIC_ONE 1
#define AUDIO_VQE_MIC_TWO 2
#define AUDIO_VQE_MIC_THREE 3
#define AUDIO_VQE_MIC_FOUR 4
#define AUDIO_VQE_MIC_FIVE 5
#define AUDIO_VQE_MIC_SIX 6
#define AUDIO_VQE_MIC_SEVEN 7

#define AUDIO_VQE_DEVICEMODE_MINNUM 1  /* Minimum number of patterns */
#define AUDIO_VQE_DEVICEMODE_MAXNUM 31 /* Maximum number of patterns */

#define AUDIO_VQE_CHAN_MINNUM 1 /* Minimum output */
#define AUDIO_VQE_CHAN_MAXNUM 3 /* Maximum output */

#define AUDIO_VQE_REF_CHAN_MINNUM 0 /* min ref num */
#define AUDIO_VQE_REF_CHAN_MAXNUM 2 /* max ref num */

#define AUDIO_VQE_MIC_INITGAIN_MIN 0  /* min gain init */
#define AUDIO_VQE_MIC_INITGAIN_MAX 50 /* max gain init */

#define AUDIO_VQE_MIC_CHANGE_MIN 0 /* Mic switching min */
#define AUDIO_VQE_MIC_CHANGE_MAX 1 /* Mic switching max */

#define AUDIO_VQE_MINPOSTFIXGAIN 0   /* Post Fixed Gain Range Minimum */
#define AUDIO_VQE_MAXPOSTFIXGAIN 250 /* Post Fixed Gain Range Max */

#define AUDIO_VQE_8_ALIGN 1    /* eight-byte alignment */
#define AUDIO_VQE_UN_8_ALIGN 0 /* non eight-byte alignment */

#define AUDIO_VQE_TAB_LEN_TWO 2
#define AUDIO_VQE_TAB_LEN_THREE 3
#define AUDIO_VQE_TAB_LEN_FOUR 4
#define AUDIO_VQE_TAB_LEN_FIVE 5
#define AUDIO_VQE_TAB_LEN_SIX 6
#define AUDIO_VQE_TAB_LEN_SEVEN 7
#define AUDIO_VQE_TAB_LEN_EIGHT 8
#define AUDIO_VQE_TAB_LEN_NINE 9
#define AUDIO_VQE_TAB_LEN_TEN 10
#define AUDIO_VQE_TAB_LEN_ELEVEN 11
#define AUDIO_VQE_TAB_LEN_TWELVE 12
#define AUDIO_VQE_TAB_LEN_THIRTEEN 13

#define AUDIO_VQE_TAB_LEN_TWENTY 20
#define AUDIO_VQE_TAB_LEN_THIRTY 30
#define AUDIO_VQE_TAB_LEN_FIFTY 50

#ifndef AUDIO_VQE_SVN_VERSION
#define AUDIO_VQE_SVN_VERSION 133782
#endif

#define CC_PRE_EMPHASIS 31785   // 0.97 * (2 ^ 15)
/* version info */
static const stru_vqe_version audio_vqe_version_tab = { { "arm-linux" },
                                                        { "Camera3A.V3.130.0" },
                                                        { "2023-12-01 18:00:00" },
                                                        0xa5a5a5a5,
                                                        AUDIO_VQE_SVN_VERSION };

typedef struct audio_vqe_stru_params {
    /* res para,32 */
    imedia_int16 s_pad[8];     /* res para 8 The second is a harmonic protection switch */
    imedia_int8 i_mic_in;      /* alg input num */
    imedia_int8 i_mic_out;     /* alg output num */
    imedia_int8 i_ref_num;     /* ref num */
    imedia_int8 sample_rate;   /* sample rate */
    imedia_int8 init_mic_gain; /* mic gain */
    imedia_int8 mic_change;
    imedia_int8 pad4;
    imedia_uint8 reserved[6]; /* 8 res */

    imedia_int8 aec_enable;
    imedia_int8 fmp_enable;
    imedia_int8 agc_enable;
    imedia_int8 eq_enable;
    imedia_int8 drc_enable;
    imedia_int8 limiter_enable;
    imedia_int8 dereverb_enable;
    imedia_int8 dmnr_enable;
    imedia_int8 sinpmwf_enable;
    imedia_int8 aiwnr_enable;
    imedia_int8 hs_enable;

    stru_aec_params aec_params;   /* 8 */
    stru_fmp_params fmp_params;   /* 8 */
    stru_agc_params agc_params;   /* 48 */
    stru_eq_fir_params eq_params; /* 64 */
    stru_dmnr_params dmnr_param;
    stru_sinpmwf_params sinpmwf_params;
    stru_drc_params drc_param;
    stru_limiter_params limiter_param;
    stru_hs_params hs_cfg;
    stru_aiwnr_params aiwnr_cfg;
} stru_vqe_params, *pst_vqe_params;

/* alg chan variable */
typedef struct {
    imedia_int32 i_check_head;
    imedia_uint32 status;

    imedia_void *aec_handle;
    imedia_void *aec_handle2;
    imedia_void *fmp_handle;
    imedia_void *agc_handle;

    imedia_void *eq_handle;
    imedia_void *drc_handle;
    imedia_void *limiter_handle;

    imedia_void *dereverb_handle;
    imedia_void *dereverb_handle2;
    imedia_void *dereverb_handle3;
    imedia_void *dereverb_handle4;

    imedia_void *dmnr_handle;
    imedia_void *scratch_buf;
    imedia_void *sin_pmwf_handle;
    imedia_void *aiwnr_handle;
    imedia_void *pst_hs_handle;
    imedia_uint32 frm_cnt;

    imedia_int16 pri_fft_frame[AUDIO_VQE_FRAMESIZE_16_K];
    imedia_int16 sec_fft_frame[AUDIO_VQE_FRAMESIZE_16_K];

    imedia_int16 aec_out1_fft_frame[AUDIO_VQE_FRAMESIZE_16_K];
    imedia_int16 aec_out2_fft_frame[AUDIO_VQE_FRAMESIZE_16_K];

    imedia_float pf_out_delay[AUDIO_VQE_FFTSIZE_16_K];

    imedia_int16 en_aec_cnt; /* aec sequence count frm */
    imedia_int16 ref_count; /* unuse para */
    imedia_int8 sample_rate; /* sample rate 1--16k */
    imedia_uint8 ref_num; /* i_ref_num */
    imedia_int8 alg_in_chan_num; /* i_mic_in */
    imedia_int8 reserved1;

    imedia_uint32 i_check_one;
    imedia_uint32 i_check_one_more;

    stru_vqe_params voice_param;

    imedia_uint32 i_check_two;
    imedia_uint32 i_check_two_more;
    imedia_int32 i_frame_lap;
    imedia_int32 i_frame_size;

    /* FFT NEON */
    imedia_int8 ac_fft_buf512[3072]; /* 512 neon FFT buf 3072 */
    imedia_int8 ac_fft_buf256[1536]; /* 256 neon FFT buf 1536 */

    audio_vqe_fft_state_float32t fft_cfg256;
    audio_vqe_fft_state_float32t fft_cfg512;

    audio_vqe_voice_float_st_fft_state_float32_t fft_cfg;
    int8_t ac_fft_buf_half[8 * 512]; /* cespturm 512point 1536*2 */

    imedia_int32 reserved2;
    imedia_int32 i_check_tail; /* chan variable */
} stru_vqe_head_chan, *pst_vqe_head_chan;

#ifdef __cplusplus
}
#endif

#endif
