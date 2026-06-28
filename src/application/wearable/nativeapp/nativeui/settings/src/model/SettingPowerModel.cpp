/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingPowerModel
 * Create: 2025-06-01
 */
#if !defined(_WIN32)
#include "osal_interrupt.h"
#include "pinctrl.h"
#include "product_pin.h"
#endif
#include "settings/common/SettingCommon.h"
#include "settings/model/SettingPowerModel.h"

namespace OHOS {
static constexpr const uint32_t samples_to_keep = 10;
static constexpr const uint32_t samples_to_discard = 5;
static constexpr const uint32_t max_range_mv = 1800;
static constexpr const uint32_t min_range_mv = 0;
static constexpr const uint8_t battery_adc_discard_nums_delay = 3;
static constexpr const uint32_t voltage_v_to_mv = 1000;
static constexpr const uint8_t voltage_scale = 100;
#define battery_adc_trans_sytick_to_voltage_without_auto_scan(x) ((float)(((float)(x) * 18) / 40960))

#if !defined(_WIN32)
SettingPowerModel::SettingPowerModel()
{
    channel_ = ADC_CHANNEL_3;
    samples_to_keep_ = samples_to_keep;
    samples_to_discard_ = samples_to_discard;
    // 毫伏更容易计算电量百分比
    max_range_mv_ = max_range_mv;
    min_range_mv_ = min_range_mv;
}

SettingPowerModel::~SettingPowerModel()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "~SettingPowerModel.\r\n");
}

const char* SettingPowerModel::GetPowerModelNotify()
{
    return FontGlobalManager::GetInstance()->GetText(POWER_PROMPT_LANGUAGE);
}

void SettingPowerModel::BatteryAdcManual(uint8_t channel, uint32_t sample_num, uint32_t sample_discard)
{
    for (uint32_t i = 0; i < sample_discard; i++) {
        (void)uapi_adc_manual_sample(channel);
        uapi_tcxo_delay_ms(battery_adc_discard_nums_delay);
    }

    uint16_t *sample_result = (uint16_t *)malloc(sizeof(uint16_t) * sample_num);
    if (sample_result == NULL) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "sample_result malloc failed\r\n");
        return;
    }

    uint64_t time_start = uapi_tcxo_get_us();
    for (uint32_t i = 0; i < sample_num; i++) {
        sample_result[i] = (uint16_t)uapi_adc_manual_sample(channel);
    }
    uint64_t time_end = uapi_tcxo_get_us();

    uint32_t sample_zero_count = 0;
    uint32_t stick = 0;
    for (uint32_t i = 0; i < sample_num; i++) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "sample_result = %d.\r\n", sample_result[i]);
        if (sample_result[i] == 0) {
            sample_zero_count++;
        }
        stick += sample_result[i];
    }

    sample_num -= sample_zero_count;
    if (sample_num == 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "sample_num = 0, sample_zero_count = %d\r\n", sample_zero_count);
        free(sample_result);
        sample_result = NULL;
        return;
    }

    stick = (stick / sample_num);
    voltage_ = battery_adc_trans_sytick_to_voltage_without_auto_scan((uint16_t)stick);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "simulate voltage: %.4f.\r\n", voltage_);
    free(sample_result);
    sample_result = NULL;
    return;
}

void SettingPowerModel::GetBatteryByAdcManual()
{
    uapi_adc_init(ADC_CLOCK_500KHZ);
    uapi_adc_power_en(AFE_SCAN_MODE_MAX_NUM, true);
    errcode_t ret = uapi_adc_open_channel(channel_);
    if (ret == ERRCODE_SUCC) {
        BatteryAdcManual(channel_, samples_to_keep_, samples_to_discard_);
    } else {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "uapi_adc_open_channel failed = %d\r\n", ret);
    }

    uapi_adc_close_channel(channel_);
    uapi_adc_power_en(AFE_SCAN_MODE_MAX_NUM, false);
    uapi_adc_deinit();
}

uint8_t SettingPowerModel::GetBatteryPowerLevel()
{
    GetBatteryByAdcManual();
    uint8_t battery_level = (uint8_t)((voltage_ * voltage_v_to_mv / (max_range_mv_ - min_range_mv_)) *voltage_scale);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "GetBatteryPowerLevel = %d\r\n", battery_level);
    return battery_level;
}
#else
static constexpr const uint8_t percentage_max = 100;
uint8_t SettingPowerModel::GetBatteryPowerLevel()
{
    return percentage_max;
}

const char* SettingPowerModel::GetPowerModelNotify()
{
    return FontGlobalManager::GetInstance()->GetText(POWER_PROMPT_LANGUAGE);
}
#endif
}