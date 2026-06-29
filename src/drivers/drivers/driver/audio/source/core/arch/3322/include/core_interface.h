/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: core interface
 * Author: audio
 */

#ifndef __CORE_INTERFACE_H__
#define __CORE_INTERFACE_H__

#include "audio_struct.h"

#define AI_VOL_MAX 30     /* max 30 dB */
#define AI_VOL_MIN (-120) /* min -120 dB */

#define AUDIO_DSP_FREQ_MIN AUDIO_DSP_FREQ_32M /* 32M for FPGA */

#define ADC0_DEFAULT_SIO_MIC_UPLINK SIO_MIC01_UPLINK
#define ADC0_DEFAULT_MIC_UPLINK MIC01_UPLINK

typedef enum {
    DSP_IF0_UP = 0,
    DSP_IF0_DN = 1,
    DSP_IF1_UP = 2,
    DSP_IF1_DN = 3,
    DSP_IF2_UP = DSP_IF1_UP,
    DSP_IF2_DN = DSP_IF1_DN,
    UART0_TX = 4,
    UART0_RX = 5,
    UART1_TX = 8,
    UART1_RX = 9,
    I2C_MST_TX = 6,
    I2C_MST_RX = 7,
    ANC_TP3 = 12,
    ANC_TP2 = 13,
    ANC_HT = 14,
    ANC_TP1 = 15,
    ANC_TP0 = 16,
    ANC_TOSPK = 17,
    ANC_REF_ERR = 18,
    ANC_FF_FB = 19,
    AUDIO_DNLINK = 8,
    SPA_DNLINK = 9,
    EC_UPLINK = 10,
    VAD_UPLINK = 11,
    MAD_UPLINK = VAD_UPLINK,
    MIC01_UPLINK = 12,
    MIC23_UPLINK = MIC01_UPLINK,
    MIC45_UPLINK = MIC01_UPLINK,
    MIC67_UPLINK = MIC01_UPLINK,
    AUDIO_LD_DNLINK = 13,

    EDMAC_PERI_NUM_MAX = 0xff,
} edmac_peri_num;

typedef enum {
    LOG_MEMORY_REGION_SECTION_0 = 0, /* LOG_MEMORY_REGION_SECTION_0 for bt core */
    LOG_MEMORY_REGION_SECTION_1,     /* LOG_MEMORY_REGION_SECTION_1 for application core */
    LOG_MEMORY_REGION_SECTION_2,     /* LOG_MEMORY_REGION_SECTION_2 for hifi core when dsp exists. */
    LOG_MEMORY_REGION_MAX_NUMBER,    /* LOG_MEMORY_REGION_MAX_NUMBER */
} log_memory_region_section;

#define IPC_PERIOD 0x1FFFF

#define ADEC_MUSIC_IN_BUF_MIN_SIZE 0x0 /* 0K */ /* For local play && BT music(SRC) */
#define ADEC_TONE_IN_BUF_MIN_SIZE 0x0 /* 0K */ /* For tone play */

#define ADEC_MUSIC_ALMOST_EMPTY_SIZE 0x0 /* 0K */ /* For local play && BT music(SRC) */
#define ADEC_TONE_ALMOST_EMPTY_SIZE 0x0 /* 0K */ /* For tone play */

/* need 40ms mute for fade frame and hvs lite delay */
#define AIP_MUTE_FRAME_CNT 4
/* 81000: bt aenc_out_us buffer_size / 72byte * 2670us */
#define BT_MUSIC_AENC_OUT_MAX_US 81000

#define AEF_IV_CH        AUDIO_CH_2
#define AEF_IV_BIT_DEPTH BIT_DEPTH_16

#define TWS_CFG_DEFAULT_MONO_MODE TD_FALSE

#define SCRATCH_BUF_SIZE 0x9000  /* 36k scratch buffer */

#define CORE_AI_DROP_FRAME_MIN_NUM 30

#define SAP_MSG_TASK_SIZE 0xB00

#endif /* __CORE_INTERFACE_H__ */
