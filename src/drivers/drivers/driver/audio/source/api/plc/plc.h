/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd.. 2021-2022. All rights reserved.
 * Description: plc.h header file
 * Author: audio
 */

#ifndef __PLC_H__
#define __PLC_H__

#include "td_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define FRAMESZ 480                /* 5ms at 480KHz */
#define PITCH_MIN (FRAMESZ >> 1)   /* minimum allowed pitch */
#define PITCH_MAX (PITCH_MIN * 3) /* maximum allowed pitch 15ms */
#define PITCH_DIFF (PITCH_MAX - PITCH_MIN)
#define OVERLAP_MAX (PITCH_MIN)              /* maximum pitch OLA window */

#define HISTORY_LEN FRAMESZ /* history buffer length */
#ifdef __HIFI_OPT_ADV__
#define NDEC 4                                    /* 4:1 decimation */
#define CORR_LEN (FRAMESZ >> 1)                    /* 5 ms correlation length */
#else
#define NDEC 2                                    /* 2:1 decimation */
#define CORR_LEN (FRAMESZ >> 1)                    /* 20 ms correlation length */
#endif
#define CORR_BUF_LEN (HISTORY_LEN - CORR_LEN)       /* correlation buffer length */
#define CORR_MIN_POWER (250)                      /* minimum power */
#define OVERLAP_INCR (24 * FRAMESZ / 60)          /* end OLA increment per frame, 4 ms */
#define ATTEN_FAC (6553)                          /* ((double).2) attenuation factor per 10 ms frame */
#define ATTEN_INCR (ATTEN_FAC / FRAMESZ)          /* attenuation per sample */
#define PCM_BIT_DEPTH_BYTE 4
#define PCM_BEST_FLAG_SCALE 8

#ifndef MAX_INT32
#define MAX_INT32 ((int)0x7fffffff)
#endif
#ifndef MIN_INT32
#define MIN_INT32 ((int)0x80000000)
#endif

#ifndef MAX_INT16
#define MAX_INT16 ((short)0x7fff)
#endif
#ifndef MIN_INT16
#define MIN_INT16 ((short)0x8000)
#endif

#define PLC_VAR_1 1
#define PLC_VAR_2 2
#define PLC_VAR_3 3
#define PLC_BIT_SHIFT_8 8
#define PLC_BIT_SHIFT_16 16
#define PLC_BIT_SHIFT_24 24
#define PLC_BIT_DEPTH_16 16
#define PLC_BIT_DEPTH_24 24
#define PCM_DEC_VAR_31 31
#define PCM_DEC_VAR_32 32

#define sap_err_log_fun(a, b) printf("%s:%d fun ret: %d \n", __FILE__, __LINE__, b)

typedef struct pcm_plc_state *plc_handle;

td_void plc_deinit(plc_handle h_state);
td_s32 plc_init(plc_handle *h_state, td_u32 samples_per_frame, td_u32 ch, td_u32 bit_depth);
td_void plc_add_history_data(plc_handle h_state, td_s32 *s);
td_void plc_decode_frame(plc_handle h_state, td_s32 *out);

#ifndef VAR_1
#define VAR_1 1
#endif

#ifndef VAR_2
#define VAR_2 2
#endif

#ifndef VAR_32
#define VAR_32 32
#endif

#ifndef VAR_3
#define VAR_3 3
#endif

#ifndef VAR_15
#define VAR_15 15
#endif

#ifndef VAR_16
#define VAR_16 16
#endif

#ifndef AE_BIT_32
#define AE_BIT_32 32
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* end of plc.h */
