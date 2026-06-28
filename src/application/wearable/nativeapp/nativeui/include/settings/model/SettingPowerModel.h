/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingPowerModel
 * Create: 2025-06-01
 */
#ifndef SETTING_POWER_MODEL_H
#define SETTING_POWER_MODEL_H

#include <cstdint>
#if !defined(_WIN32)
#include "adc.h"
#include "tcxo.h"
#include "product_pin.h"
#include "pinctrl_porting.h"
#include "hal_adc.h"
#endif
#include "settings/common/SettingCommon.h"
#include "settings/common/SettingRefSingleton.h"

namespace OHOS {
#if !defined(_WIN32)
class SettingPowerModel : public SettingRefSingleton<SettingPowerModel> {
public:
    SettingPowerModel();
    ~SettingPowerModel();
    uint8_t GetBatteryPowerLevel();
    const char* GetPowerModelNotify();
private:
    adc_channel_t channel_;             // ADC信道
    uint32_t samples_to_keep_ = 0;      // 总采样次数
    uint32_t samples_to_discard_ = 0;   // 丢弃的初始样本数
    uint32_t max_range_mv_ = 0;         // ADC原始值上限(单位:毫伏)
    uint32_t min_range_mv_ = 0;         // ADC原始值下限(单位:毫伏)
    float voltage_ = 0;                 // 采样换算电压
    void BatteryAdcManual(uint8_t channel, uint32_t sample_num, uint32_t sample_discard);
    void GetBatteryByAdcManual();
};
#else
class SettingPowerModel : public SettingRefSingleton<SettingPowerModel> {
public:
    uint8_t GetBatteryPowerLevel();
    const char* GetPowerModelNotify();
};
#endif
} // OHOS
#endif