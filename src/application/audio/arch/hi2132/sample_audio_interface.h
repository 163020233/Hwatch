/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2021. All rights reserved.
 * Description: source file for module operation
 */
 
 
#ifndef __SAMPLE_AUDIO_INTERFACE_H__
#define __SAMPLE_AUDIO_INTERFACE_H__
 
#include "at_uart.h"

#define AI_PORT_DEFAULT         UAPI_AI_PORT_ADC0 /* In port for ai */
#define SND_OUT_PORT_DEFAULT    UAPI_SND_OUT_PORT_DAC0
#define CAT1_OUT_PORT_DEFAULT   UAPI_SND_OUT_PORT_I2S1 /* Out port for CAT1 output */
 
/* sound local output default pcm format */
#define SND_OUT_SAMPLE_RATE_DEF     UAPI_AUDIO_SAMPLE_RATE_16K
#define SND_OUT_BIT_DEPTH_DEF       UAPI_AUDIO_BIT_DEPTH_16
#define SND_OUT_CHANNEL_DEF         UAPI_AUDIO_CHANNEL_1
#define SND_OUT_I2S_SAMPLE_RATE_DEF UAPI_AUDIO_SAMPLE_RATE_16K
#define SND_OUT_I2S_BIT_DEPTH_DEF   UAPI_AUDIO_BIT_DEPTH_16
#define SND_OUT_I2S_CHANNEL_DEF     UAPI_AUDIO_CHANNEL_1
#define SND_OUT_I2S_MST_DEF         TD_TRUE /* TD_TRUE:master;TD_FALSE:slave */
#define SND_OUT_I2S_DATA_EDGE_DEF   TD_TRUE /* TD_TRUE:Rising;TD_FALSE:Falling */
#define SND_OUT_FRAME_MODE_WORD_LEN UAPI_AUDIO_BIT_DEPTH_16 /* 16:for pcm16/i2s32, 32:for pcm32/i2s64 */
 
#define AEF_BYPASS_DEFAULT   TD_FALSE
#define TRACK_MODE_DEFAULT   UAPI_SND_TRACK_DOUBLE_MONO
 
#define DECODE_FRAME_BUFFER_SIZE 0x800 /* 2K */
 
/* 若需将sample_decode由thread_enable切换为adp_event_enable模式，需开启宏PLAY_ENABLE_ADP_EVENT_CTRL */
 
#define FPGA_SAVE_DATA_ADDR 0x3000000
#define FPGA_SAVE_DATA_SIZE 0x70000

#define FPGA_FLASH_ES_DATA_MP3_ADDR FPGA_SAVE_DATA_ADDR
#define FPGA_FLASH_ES_DATA_SBC_ADDR FPGA_SAVE_DATA_ADDR
#define FPGA_FLASH_PCM_DATA_ADDR (FPGA_SAVE_DATA_ADDR)
 
#define FPGA_FLASH_ES_SIZE 0x60000
#define FPGA_FLASH_PCM_SIZE 0x60000
 
#define FPGA_DUMP_DATA_ADDR FPGA_SAVE_DATA_ADDR
#define FPGA_DUMP_DATA_SIZE FPGA_SAVE_DATA_SIZE

#define sap_printf at_uart_log_string
#define SINGLE_CORE_DEPLOY
#define MONO_PROFILE_CUT
#define SAP_FOPS_TYPE_DUMMY
 
#endif /* __SAMPLE_AUDIO_INTERFACE_H__ */
