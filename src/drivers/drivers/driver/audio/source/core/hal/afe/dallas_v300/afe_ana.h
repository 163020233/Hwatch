/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: header of afe ana driver
 * Create: 2024-11-12
 */

#ifndef __AFE_ANA_H__
#define __AFE_ANA_H__

#include "td_type.h"

#define AFE_ADC_PGA_MAX     24
#define AFE_ADC_PGA_MIN     0
#define AFE_ADC_PGA_STEP    2

#define AFE_LPADC_PGA_MAX   24
#define AFE_LPADC_PGA_MIN   0
#define AFE_LPADC_PGA_STEP  2

typedef enum {
    AFE_ADC_CH0 = 0,
    AFE_ADC_CH1,
    AFE_ADC_CH_MAX = AFE_ADC_CH1,
} afe_adc_chn;

typedef enum {
    AFE_LPADC_CH0 = 0,
    AFE_LPADC_CH_MAX = AFE_LPADC_CH0,
} afe_lpadc_chn;

typedef enum {
    AFE_DAC_CH0 = 0,
    AFE_DAC_CH1,
    AFE_DAC_CH_MAX = AFE_DAC_CH1,
} afe_dac_chn;

typedef enum {
    AFE_ADC_SRC_MIC0 = 0,
    AFE_ADC_SRC_MIC1,
    AFE_ADC_SRC_BUTT,
} afe_adc_source_sel;

typedef enum {
    AFE_LPADC_SRC_VPU = 0,
    AFE_LPADC_SRC_MIC0,
    AFE_LPADC_SRC_MIC1,
    AFE_LPADC_SRC_BUTT,
} afe_lpadc_source_sel;

td_s32 afe_adc_open(afe_adc_chn adc_ch);
td_s32 afe_adc_close(afe_adc_chn adc_ch);
td_s32 afe_adc_set_gain(afe_adc_chn adc_ch, td_s32 gain);
td_s32 afe_adc_get_gain(afe_adc_chn adc_ch, td_s32 *gain);
td_s32 afe_adc_set_source(afe_adc_chn adc_ch, afe_adc_source_sel src_sel);
td_s32 afe_adc_get_source(afe_adc_chn adc_ch, afe_adc_source_sel *src_sel);

td_s32 afe_lpadc_open(afe_lpadc_chn lpadc_ch);
td_s32 afe_lpadc_close(afe_lpadc_chn lpadc_ch);
td_s32 afe_lpadc_set_gain(afe_lpadc_chn lpadc_ch, td_s32 gain);
td_s32 afe_lpadc_get_gain(afe_lpadc_chn lpadc_ch, td_s32 *gain);
td_s32 afe_lpadc_set_source(afe_lpadc_chn lpadc_ch, afe_lpadc_source_sel src_sel);
td_s32 afe_lpadc_get_source(afe_lpadc_chn lpadc_ch, afe_lpadc_source_sel *src_sel);

td_s32 afe_dac_open(afe_dac_chn dac_ch);
td_s32 afe_dac_close(afe_dac_chn dac_ch);

#endif /* __AFE_ANA_H__ */
