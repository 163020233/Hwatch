/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: diag temperature metrics
 * This file should be changed only infrequently and with great care.
 */

#include "diag_metrics_common.h"
#include "dfx_adapt_layer.h"
#include "soc_diag_cmd_id.h"
#include "adc.h"
#include "hal_adc.h"
#include "pinctrl.h"
#include "tcxo.h"

#define ADC_MAX_RANGE_MV 380                            // ADC最大电压范围0.38V
#define ADC_MIN_RANGE_MV 0                              // ADC最小电压范围0V
#define PERCENTAGE_MAX 100                              // 百分比最大值100%
#define PERCENTAGE_MIN 0                                // 百分比最小值0%
#define ADC_REFERENCE_RATIO_NUMERATOR 450U              // ADC参考电压比例分子450
#define ADC_REFERENCE_RATIO_DENOMINATOR (1024U / 2U)    // ADC参考电压比例分母512

typedef struct {
    bool is_running;
    bool init;
} sensor_diag_priv_data_t;

static sensor_diag_priv_data_t g_priv_data;

// 获取芯片温度信息
static errcode_t diag_get_tempe_info(void *data_ptr, size_t *len_ptr, void *ctx)
{
    unused(ctx);
    if (*len_ptr < sizeof(float)) {
        return ERRCODE_FAIL;
    }

    uint32_t irq_sts = (uint32_t)osal_irq_lock();
    float real_temp = 0;
    uint32_t tmp = 0;

    writel(0x570040BC, 0x50);
    uapi_tcxo_delay_us(120); // wait 120us
    writel(0x570040BC, 0x70);
    uapi_tcxo_delay_us(30); // wait 30us
    writel(0x570040BC, 0x30);
    writel(0x570000B8, 0x0);
    writel(0x57004020, 0x101);
    uapi_tcxo_delay_us(30); // wait 30us
    writel(0x57004020, 0x1);
    writel(0x57004020, 0x11);
    writel(0x570000B8, 0x7);
    writel(0x57004244, 0x10);
    writel(0x59000820, 0x1A0);
    reg32_setbit(0x57000068, 0x0);
    writel(0x59000A34, 0x0);
    writel(0x59000A04, 0x3);
    writel(0x59000A10, 0x0);
    writel(0x59000A00, 0x1);
    uapi_tcxo_delay_ms(10); // wait 10ms
    tmp = (uint32_t)readl(0x59000A0C);
    real_temp = (((float)tmp - 0x706) / 0x14) + 26.4; // base tempe 26.4
    osal_irq_restore(irq_sts);
    printf("chip temperature=%f(tmp=%u)\r\n", real_temp, tmp);
    writel(0x57004244, 0x0); // close Tsensor switch
    writel(0x57004020, 0x100);
    uapi_tcxo_delay_us(30); // wait 30us
    writel(0x570000B8, 0x0);
    writel(0x570040BC, 0xc0);
    
    *((float *)data_ptr) = real_temp;
    *len_ptr = sizeof(float);
    return ERRCODE_SUCC;
}

// 获取电池电压值
static uint32_t get_voltage(void)
{
    uint32_t stick = 0;
    uint32_t stick_count = 0;
    uint32_t samples_to_keep = 10;          // 总采样次数
    uint32_t samples_to_discard = 5;        // 丢弃的初始采样数
    // 丢弃初始帧数据
    for (uint32_t i = 0; i < samples_to_discard; i++) {
        uapi_adc_manual_sample(ADC_CHANNEL_NONE);
        uapi_tcxo_delay_ms((uint64_t)3);  // 延迟3ms
    }
    // 采集样本数
    for (uint32_t i = 0; i < samples_to_keep; i++) {
        uint32_t irq_sts = osal_irq_lock();
        uint32_t sample_result =
            (uint32_t)uapi_adc_manual_sample(ADC_CHANNEL_NONE);  // 每次读取CHANNEL_NONE? testsuit中是这样写的
        if (sample_result != 0) {
            stick += sample_result;
            stick_count++;
        }
        osal_irq_restore(irq_sts);
    }
    if (stick_count == 0) {
        return 0;
    }
    // 整数运算公式：(sytick * numerator + denominator/2) / denominator
    return ((stick / stick_count) * ADC_REFERENCE_RATIO_NUMERATOR + ADC_REFERENCE_RATIO_DENOMINATOR / 2) /
               ADC_REFERENCE_RATIO_DENOMINATOR;
}

// 获取电池健康信息
static errcode_t diag_get_battery_info(void *data_ptr, size_t *len_ptr, void *ctx)
{
    unused(ctx);
    if (*len_ptr < sizeof(uint8_t)) {
        return ERRCODE_FAIL;
    }
    errcode_t ret = ERRCODE_SUCC;
    uint8_t battery_percent = 0;             // 电池健康度
    uint32_t voltage = 0;                   // 电压值
    adc_channel_t channel = ADC_CHANNEL_3;  // ADC信道

    ret = uapi_adc_init(ADC_CLOCK_500KHZ);  // 初始化adc
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    uapi_adc_power_en(AFE_SCAN_MODE_MAX_NUM, true);
    ret = uapi_adc_open_channel(channel);
    if (ret != ERRCODE_SUCC) {
        uapi_adc_power_en(AFE_SCAN_MODE_MAX_NUM, false);  // 关闭电源
        uapi_adc_deinit();
        return ret;
    }

    voltage = get_voltage();  // 获取电压值

    // 关闭ADC通道
    uapi_adc_close_channel(channel);
    uapi_adc_power_en(AFE_SCAN_MODE_MAX_NUM, false);
    uapi_adc_deinit();

    // 根据电压值计算电量百分比
    if (voltage == 0) {
        battery_percent = 0;
    } else if (voltage >= ADC_MAX_RANGE_MV) {
        battery_percent = PERCENTAGE_MAX;
    } else if (voltage <= ADC_MIN_RANGE_MV) {
        battery_percent = PERCENTAGE_MIN;
    } else {
        uint32_t voltage_range = ADC_MAX_RANGE_MV - ADC_MIN_RANGE_MV;
        uint32_t rounding_term = voltage_range / 2;
        uint32_t voltage_offset = voltage - ADC_MIN_RANGE_MV;
        battery_percent = (voltage_offset * PERCENTAGE_MAX + rounding_term) / voltage_range;
    }

    *((uint8_t *)data_ptr) = battery_percent;
    *len_ptr = sizeof(uint8_t);
    return ret;
}

// 初始化，注册传感器metrics
errcode_t diag_sensor_metrics_init(void)
{
    errcode_t ret;

    if (g_priv_data.init) {
        return ERRCODE_SUCC; // Or an error indicating it's already running
    }

    // Create periodic task
    int err = memset_s(&g_priv_data, sizeof(g_priv_data), 0, sizeof(g_priv_data));
    if (err != EOK) {
        return ERRCODE_MEMSET;
    }

    // Register temperature metric
    ret = uapi_diag_metrics_period_report(DIAG_CMD_DIAG_REPORT_TEMPE_INFO, diag_get_tempe_info,
                                          0, sizeof(float), NULL);
    if (ret != ERRCODE_SUCC) {
        dfx_log_debug("register temperature metric failed: 0x%x", ret);
        return ret;
    }

    // Register battery metric
    ret = uapi_diag_metrics_period_report(DIAG_CMD_DIAG_REPORT_BATTERY_INFO, diag_get_battery_info,
                                          0, sizeof(uint8_t), NULL);
    if (ret != ERRCODE_SUCC) {
        dfx_log_debug("register battery metric failed: 0x%x", ret);
        return ret;
    }

    g_priv_data.is_running = true;
    g_priv_data.init = true;
    return ERRCODE_SUCC;
}

// 去初始化sensor metrics
errcode_t diag_sensor_metrics_deinit(void)
{
    errcode_t ret;
    if (!g_priv_data.init) {
        return ERRCODE_SUCC; // Or an error indicating it's not running
    }
    g_priv_data.is_running = false;
    uapi_diag_metrics_stop_report(DIAG_CMD_DIAG_REPORT_MEM_INFO);
    uapi_diag_metrics_stop_report(DIAG_CMD_DIAG_REPORT_BATTERY_INFO);
    uapi_diag_metrics_stop_report(DIAG_CMD_DIAG_REPORT_TEMPE_INFO);  // 新增：停止温度指标，释放对应节点
    g_priv_data.init = false;
    ret = (errcode_t)memset_s(&g_priv_data, sizeof(g_priv_data), 0, sizeof(g_priv_data));
    if (ret != ERRCODE_SUCC) {
        dfx_log_debug("memset fail : 0x%x", ret);
        return ret;
    }
    return ERRCODE_SUCC;
}