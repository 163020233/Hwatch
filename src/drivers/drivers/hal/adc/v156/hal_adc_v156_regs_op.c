/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: Provides v156 adc register \n
 *
 * History: \n
 * 2025-09-15， Create file. \n
 */

#include "common_def.h"
#include "hal_adc_v156_regs_op.h"

adc_regs_t *g_adc_regs = NULL;

errcode_t hal_adc_v156_regs_init(void)
{
    g_adc_regs = (adc_regs_t *)adc_porting_base_addr_get();
    return ERRCODE_SUCC;
}

#ifndef CONFIG_ADC_CHANNEL_TRIMMED
static void hal_adc_reg_ch_set(adc_channel_t channel, adc_trig_data_t *ch_data, uint32_t value)
{
    switch (channel) {
        case ADC_CHANNEL_0:
            ch_data->b.ch4 = value;
            break;
        case ADC_CHANNEL_1:
            ch_data->b.ch5 = value;
            break;
        case ADC_CHANNEL_2:
            ch_data->b.ch6 = value;
            break;
        case ADC_CHANNEL_3:
            ch_data->b.ch7 = value;
            break;
        case ADC_CHANNEL_4:
            ch_data->b.ch8 = value;
            break;
        case ADC_CHANNEL_5:
            ch_data->b.ch9 = value;
            break;
        case ADC_CHANNEL_6:
            ch_data->b.ch10 = value;
            break;
        case ADC_CHANNEL_7:
            ch_data->b.ch11 = value;
            break;
        default:
            break;
    }
}

static void hal_adc_reg_scan_ch_set(adc_channel_t channel, adc_scan_en_data_t *ch_data, uint32_t value)
{
    switch (channel) {
        case ADC_CHANNEL_0:
            ch_data->b.ch4 = value;
            break;
        case ADC_CHANNEL_1:
            ch_data->b.ch5 = value;
            break;
        case ADC_CHANNEL_2:
            ch_data->b.ch6 = value;
            break;
        case ADC_CHANNEL_3:
            ch_data->b.ch7 = value;
            break;
        case ADC_CHANNEL_4:
            ch_data->b.ch8 = value;
            break;
        case ADC_CHANNEL_5:
            ch_data->b.ch9 = value;
            break;
        case ADC_CHANNEL_6:
            ch_data->b.ch10 = value;
            break;
        case ADC_CHANNEL_7:
            ch_data->b.ch11 = value;
            break;
        default:
            break;
    }
}

static uint32_t hal_adc_reg_ch_get(adc_channel_t channel, adc_trig_data_t ch_data)
{
    switch (channel) {
        case ADC_CHANNEL_0:
            return ch_data.b.ch4;
        case ADC_CHANNEL_1:
            return ch_data.b.ch5;
        case ADC_CHANNEL_2:
            return ch_data.b.ch6;
        case ADC_CHANNEL_3:
            return ch_data.b.ch7;
        case ADC_CHANNEL_4:
            return ch_data.b.ch8;
        case ADC_CHANNEL_5:
            return ch_data.b.ch9;
        case ADC_CHANNEL_6:
            return ch_data.b.ch10;
        case ADC_CHANNEL_7:
            return ch_data.b.ch11;
        default:
            return 0;
    }
}

void hal_adc_reg_scan_mode_set(hal_adc_scan_mode_t mode, adc_channel_t channel, uint32_t value)
{
    adc_trig_data_t mode_data;
    mode_data.d32 = g_adc_regs->aux_adc_scan_mode[mode];
    hal_adc_reg_ch_set(channel, &mode_data, value);
    g_adc_regs->aux_adc_scan_mode[mode] = mode_data.d32;
}

uint32_t hal_adc_reg_scan_mode_get(hal_adc_scan_mode_t mode, adc_channel_t channel)
{
    adc_trig_data_t mode_data;
    mode_data.d32 = g_adc_regs->aux_adc_scan_mode[mode];
    return hal_adc_reg_ch_get(channel, mode_data);
}

void hal_adc_reg_scan_en_set_ch(adc_channel_t channel, uint32_t value)
{
    adc_scan_en_data_t en_data;
    en_data.d32 = g_adc_regs->aux_adc_scan_en;
    hal_adc_reg_scan_ch_set(channel, &en_data, value);
    g_adc_regs->aux_adc_scan_en = en_data.d32;
}

uint32_t hal_adc_reg_scan_start_get(hal_adc_scan_en_start_t reg, adc_channel_t channel)
{
    adc_trig_data_t start_data;
    start_data.d32 = g_adc_regs->aux_adc_scan_reg[reg];
    return hal_adc_reg_ch_get(channel, start_data);
}

void hal_adc_reg_scan_start_set(hal_adc_scan_en_start_t reg, adc_channel_t channel, uint32_t value)
{
    adc_trig_data_t start_data;
    start_data.d32 = g_adc_regs->aux_adc_scan_reg[reg];
    hal_adc_reg_ch_set(channel, &start_data, value);
    g_adc_regs->aux_adc_scan_reg[reg] = start_data.d32;
}
#endif /* CONFIG_ADC_CHANNEL_TRIMMED */

uint32_t hal_adc_reg_scan_ptr_get(hal_adc_scan_ptr_t ptr, bool on)
{
    adc_ptr_data_t ptr_data;
    ptr_data.d32 = g_adc_regs->aux_adc_scan_ptr[ptr];
    if (on) {
        return ptr_data.b.th23;
    } else {
        return ptr_data.b.th01;
    }
}

void hal_adc_reg_cali_en_set(uint32_t value)
{
    adc_cail_en_data_t en_data;
    en_data.d32 = g_adc_regs->aux_adc_cali_en;
    en_data.b.cali_en = value;
    g_adc_regs->aux_adc_cali_en = en_data.d32;
}
