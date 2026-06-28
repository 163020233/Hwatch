/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides adc driver source \n
 *
 * History: \n
 * 2022-09-16， Create file. \n
 */

#include "common_def.h"
#include "soc_osal.h"
#include "hal_adc.h"
#include "adc_porting.h"
#include "adc.h"

#if defined(CONFIG_ADC_SUPPORT_AFE) && defined(CONFIG_ADC_SUPPORT_HAFE)
bool adc_is_power_on[AFE_SCAN_MODE_MAX_NUM] = {false};        /* ADC is power on or not. */
#ifdef CONFIG_ADC_SUPPORT_MUTEX
static osal_mutex g_adc_mutex[AFE_SCAN_MODE_MAX_NUM] = { NULL };
#endif
#else
bool adc_is_power_on = false;        /* ADC is power on or not. */
#ifdef CONFIG_ADC_SUPPORT_MUTEX
static osal_mutex g_adc_mutex;
#endif
#endif /* CONFIG_ADC_SUPPORT_AFE */

bool adc_is_initialised = false;     /* ADC is initialised or not. */

#if defined(CONFIG_ADC_SUPPORT_DIFFERENTIAL)
typedef enum adc_channel_status {
    ADC_SINGLE_WORKING,
    ADC_DIFFERENTIAL_WORKING,
    ADC_IDLE
} adc_channel_status_t;
uint8_t adc_working_channel[2] = {0}; /* ADC differential channel be choosed. */
adc_channel_status_t adc_channel_working_status = ADC_IDLE;
#else
uint8_t adc_working_channel = 0; /* ADC channel be choosed. */
#endif /* CONFIG_ADC_SUPPORT_DIFFERENTIAL */

#if defined(CONFIG_ADC_SUPPORT_AFE) && defined(CONFIG_ADC_SUPPORT_HAFE)
afe_scan_mode_t adc_working_afe = AFE_SCAN_MODE_MAX_NUM;
#endif /* CONFIG_ADC_SUPPORT_AFE */


static hal_adc_funcs_t *g_hal_funcs = NULL;

errcode_t uapi_adc_init(adc_clock_t clock)
{
    if (adc_is_initialised) {
        return ERRCODE_SUCC;
    }

    adc_port_init_clock(clock);
    adc_port_clock_enable(true);
    adc_port_register_hal_funcs();
    g_hal_funcs = hal_adc_get_funcs();
    errcode_t ret = hal_adc_init();
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
#if defined(CONFIG_ADC_SUPPORT_AUTO_SCAN)
    adc_port_register_irq();
#endif /* CONFIG_ADC_SUPPORT_AUTO_SCAN */
    adc_is_initialised = true;

    return ret;
}

errcode_t uapi_adc_deinit(void)
{
    if (!adc_is_initialised) {
        return ERRCODE_SUCC;
    }

    errcode_t ret = hal_adc_deinit();

    adc_port_unregister_hal_funcs();

    adc_port_clock_enable(false);
#if defined(CONFIG_ADC_SUPPORT_AUTO_SCAN)
    adc_port_unregister_irq();
#endif /* CONFIG_ADC_SUPPORT_AUTO_SCAN */
    adc_is_initialised = false;

    return ret;
}

void uapi_adc_power_en(afe_scan_mode_t afe_scan_mode, bool en)
{
#if defined(CONFIG_ADC_SUPPORT_AFE) && defined(CONFIG_ADC_SUPPORT_HAFE)
    if (afe_scan_mode >= AFE_SCAN_MODE_MAX_NUM || adc_is_power_on[afe_scan_mode] == en) {
        return;
    }
#else
    if (adc_is_power_on == en) {
        return;
    }
#endif /* CONFIG_ADC_SUPPORT_AFE */

    adc_port_power_on(en);
#if defined(CONFIG_ADC_SUPPORT_AFE) && defined(CONFIG_ADC_SUPPORT_HAFE)
#if (AFE_ENABLE_INTERRUPT)
    if (en) {
        adc_working_afe = afe_scan_mode;
        adc_port_register_irq(afe_scan_mode);
    } else {
        adc_port_unregister_irq(afe_scan_mode);
    }
#endif /* AFE_ENABLE_INTERRUPT */
#endif /* CONFIG_ADC_SUPPORT_AFE */

    hal_adc_power_en(afe_scan_mode, en);

#if defined(CONFIG_ADC_SUPPORT_AFE) && defined(CONFIG_ADC_SUPPORT_HAFE)
    adc_is_power_on[afe_scan_mode] = en;
#ifdef CONFIG_ADC_SUPPORT_MUTEX
    if (en) {
        (void)osal_mutex_init(&g_adc_mutex[afe_scan_mode]);
    } else {
        osal_mutex_destroy(&g_adc_mutex[afe_scan_mode]);
    }
#endif
#else
    adc_is_power_on = en;
#ifdef CONFIG_ADC_SUPPORT_MUTEX
    if (en) {
        (void)osal_mutex_init(&g_adc_mutex);
    } else {
        osal_mutex_destroy(&g_adc_mutex);
    }
#endif
#endif /* CONFIG_ADC_SUPPORT_AFE */
}

bool uapi_adc_is_using(void)
{
#if defined(CONFIG_ADC_SUPPORT_AFE) && defined(CONFIG_ADC_SUPPORT_HAFE)
    return adc_is_power_on[AFE_GADC_MODE] || adc_is_power_on[AFE_HADC_MODE];
#else
    return adc_is_power_on;
#endif /* CONFIG_ADC_SUPPORT_AFE */
}

errcode_t uapi_adc_open_channel(uint8_t channel)
{
    if (unlikely(channel >= ADC_CHANNEL_MAX_NUM)) {
        return ERRCODE_ADC_INVALID_PARAMETER;
    }
#if defined(CONFIG_ADC_SUPPORT_DIFFERENTIAL)
    if (adc_channel_working_status != ADC_SINGLE_WORKING && adc_channel_working_status != ADC_IDLE) {
        return ERRCODE_ADC_INVALID_PARAMETER;
    }
#endif
#if defined(CONFIG_ADC_SUPPORT_AUTO_SCAN)
    if (uapi_adc_auto_scan_is_enabled()) {
        return ERRCODE_ADC_SCAN_NOT_DISABLE;
    }
#endif /* CONFIG_ADC_SUPPORT_AUTO_SCAN */
    uint32_t irq_sts = adc_irq_lock();
#if defined(CONFIG_ADC_SUPPORT_AFE)
    errcode_t ret = hal_adc_ch_set(channel, true);
#else
    errcode_t ret = hal_adc_ch_set((adc_channel_t)channel, true);
#endif /* CONFIG_ADC_SUPPORT_AFE */
    if (ret == ERRCODE_SUCC) {
#if defined(CONFIG_ADC_SUPPORT_DIFFERENTIAL)
        adc_working_channel[0] = channel;
        adc_channel_working_status = ADC_SINGLE_WORKING;
#else
        adc_working_channel = channel;
#endif
    }
    adc_irq_unlock(irq_sts);
    return ret;
}

errcode_t uapi_adc_close_channel(uint8_t channel)
{
#if defined(CONFIG_ADC_SUPPORT_DIFFERENTIAL)
    if (adc_channel_working_status != ADC_SINGLE_WORKING) {
        return ERRCODE_ADC_INVALID_PARAMETER;
    }
    if (unlikely(channel != adc_working_channel[0])) {
        return ERRCODE_ADC_INVALID_PARAMETER;
    }
#else
    if (unlikely(channel != adc_working_channel)) {
        return ERRCODE_ADC_INVALID_PARAMETER;
    }
#endif

#if defined(CONFIG_ADC_SUPPORT_AUTO_SCAN)
    if (uapi_adc_auto_scan_is_enabled()) {
        return ERRCODE_ADC_SCAN_NOT_DISABLE;
    }
#endif /* CONFIG_ADC_SUPPORT_AUTO_SCAN */

    uint32_t irq_sts = adc_irq_lock();
    errcode_t ret = hal_adc_ch_set((adc_channel_t)channel, false);
    if (ret == ERRCODE_SUCC) {
#if defined(CONFIG_ADC_SUPPORT_DIFFERENTIAL)
        adc_channel_working_status = ADC_IDLE;
        adc_working_channel[0] = ADC_CHANNEL_NONE;
#else
        adc_working_channel = ADC_CHANNEL_NONE;
#endif
    }
    adc_irq_unlock(irq_sts);
    return ret;
}

#if defined(CONFIG_ADC_SUPPORT_DIFFERENTIAL)
errcode_t uapi_adc_open_differential_channel(uint8_t postive_ch, uint8_t negative_ch)
{
    if (unlikely(postive_ch >= ADC_CHANNEL_MAX_NUM) || unlikely(negative_ch >= ADC_CHANNEL_MAX_NUM)) {
        return ERRCODE_ADC_INVALID_PARAMETER;
    }
    if (adc_channel_working_status != ADC_DIFFERENTIAL_WORKING && adc_channel_working_status != ADC_IDLE) {
        return ERRCODE_ADC_INVALID_PARAMETER;
    }
    uint32_t irq_sts = adc_irq_lock();
    errcode_t ret = hal_adc_diff_ch_set((adc_channel_t)postive_ch, (adc_channel_t)negative_ch, true);
    if (ret == ERRCODE_SUCC) {
        adc_working_channel[0] = postive_ch;
        adc_working_channel[1] = negative_ch;
        adc_channel_working_status = ADC_DIFFERENTIAL_WORKING;
    }
    adc_irq_unlock(irq_sts);
    return ret;
}

errcode_t uapi_adc_close_differential_channel(uint8_t postive_ch, uint8_t negative_ch)
{
    if (unlikely(postive_ch != adc_working_channel[0]) ||
        unlikely(negative_ch != adc_working_channel[1])) {
        return ERRCODE_ADC_INVALID_PARAMETER;
    }
    if (adc_channel_working_status != ADC_DIFFERENTIAL_WORKING) {
        return ERRCODE_ADC_INVALID_PARAMETER;
    }
#if defined(CONFIG_ADC_SUPPORT_AUTO_SCAN)
    if (uapi_adc_auto_scan_is_enabled()) {
        return ERRCODE_ADC_SCAN_NOT_DISABLE;
    }
#endif /* CONFIG_ADC_SUPPORT_AUTO_SCAN */

    uint32_t irq_sts = adc_irq_lock();
    errcode_t ret = hal_adc_diff_ch_set((adc_channel_t)postive_ch, (adc_channel_t)negative_ch, false);
    if (ret == ERRCODE_SUCC) {
        adc_working_channel[0] = ADC_CHANNEL_NONE;
        adc_working_channel[1] = ADC_CHANNEL_NONE;
        adc_channel_working_status = ADC_IDLE;
    }
    adc_irq_unlock(irq_sts);
    return ret;
}
#endif

#if defined(CONFIG_ADC_SUPPORT_AUTO_SCAN)
static bool adc_auto_scan_ch_param_check(uint8_t channel, adc_scan_config_t *config, adc_callback_t callback)
{
    if (unlikely(channel >= ADC_CHANNEL_MAX_NUM)) {
        return false;
    }

    if (unlikely(callback == NULL)) {
        return false;
    }

    if (unlikely(config->type > HAL_ADC_SCAN_TYPE_THRESHOLD)) {
        return false;
    }

    if (unlikely(config->freq >= HAL_ADC_SCAN_FREQ_MAX)) {
        return false;
    }

    return true;
}

errcode_t uapi_adc_auto_scan_ch_enable(uint8_t channel, adc_scan_config_t config, adc_callback_t callback)
{
    hal_adc_scan_config_t adc_config;

    if (!adc_auto_scan_ch_param_check(channel, &config, callback)) {
        return ERRCODE_ADC_INVALID_PARAMETER;
    }
    if (unlikely(!adc_is_power_on)) {
        return ERRCODE_PWM_NOT_POWER_ON;
    }

    adc_config.type = (hal_adc_scan_type_t)config.type;
    adc_config.freq = (hal_adc_scan_freq_t)config.freq;
#if defined(CONFIG_ADC_SUPPORT_LONG_SAMPLE)
    adc_config.long_sample_time = config.long_sample_time;
#endif
    adc_config.threshold_l = config.threshold_l;
    adc_config.threshold_h = config.threshold_h;

    adc_port_init_clock(ADC_CLOCK_500KHZ);

    errcode_t ret = g_hal_funcs->ch_config((adc_channel_t)channel, &adc_config, (hal_adc_callback_t)callback);
    if (ret != ERRCODE_SUCC) {
        adc_port_init_clock(ADC_CLOCK_015KHZ);
    }

    return ret;
}

errcode_t uapi_adc_auto_scan_ch_disable(uint8_t channel)
{
    if (unlikely(channel >= ADC_CHANNEL_MAX_NUM)) {
        return ERRCODE_ADC_INVALID_PARAMETER;
    }

    return g_hal_funcs->ch_enable((adc_channel_t)channel, false);
}

void uapi_adc_auto_scan_disable(void)
{
    g_hal_funcs->enable(false);
    uapi_adc_open_channel(ADC_CHANNEL_NONE);
#if defined(CONFIG_ADC_SUPPORT_AFE) && defined(CONFIG_ADC_SUPPORT_HAFE)
    uapi_adc_power_en(adc_working_afe, false);
#else
    uapi_adc_power_en(AFE_SCAN_MODE_MAX_NUM, false);
#endif /* CONFIG_ADC_SUPPORT_AFE */
}

bool uapi_adc_auto_scan_is_enabled(void)
{
    return g_hal_funcs->isenable();
}
#endif /* CONFIG_ADC_SUPPORT_AUTO_SCAN */

int32_t uapi_adc_manual_sample(uint8_t channel)
{
    if (channel >= ADC_CHANNEL_MAX_NUM) { return 0; }

    return hal_adc_manual((adc_channel_t)channel);
}

#if defined(CONFIG_ADC_SUPPORT_AFE)
int32_t uapi_adc_auto_sample(uint8_t channel)
{
    int32_t sample_value = 0;
#if defined(CONFIG_ADC_SUPPORT_AFE) && defined(CONFIG_ADC_SUPPORT_HAFE)
    afe_scan_mode_t afe_scan_mode = (channel < HADC_CHANNEL_0) ? AFE_GADC_MODE : AFE_HADC_MODE;
    if ((channel >= ADC_CHANNEL_MAX_NUM) || (!adc_is_power_on[afe_scan_mode])) { return 0; }
#ifdef CONFIG_ADC_SUPPORT_MUTEX
    osal_mutex_lock(&g_adc_mutex[afe_scan_mode]);
#endif
    sample_value = hal_adc_auto_sample((adc_channel_t)channel);
#ifdef CONFIG_ADC_SUPPORT_MUTEX
    osal_mutex_unlock(&g_adc_mutex[afe_scan_mode]);
#endif
#else
#ifdef CONFIG_ADC_SUPPORT_DIFFERENTIAL
    if (adc_channel_working_status != ADC_SINGLE_WORKING) { return 0; } /* Mutually exclusive with AMIC */
#endif
    if ((channel >= ADC_CHANNEL_MAX_NUM) || (!adc_is_power_on)) { return 0; }
#ifdef CONFIG_ADC_SUPPORT_MUTEX
    osal_mutex_lock(&g_adc_mutex);
#endif
    sample_value = hal_adc_auto_sample((adc_channel_t)channel);
#ifdef CONFIG_ADC_SUPPORT_MUTEX
    osal_mutex_unlock(&g_adc_mutex);
#endif
#endif
    return sample_value;
}

#ifdef CONFIG_ADC_SUPPORT_DIFFERENTIAL_SAMPLE
int32_t uapi_adc_auto_sample_differential(uint8_t pchannel, uint8_t nchannel)
{
    /* 差分未使能 */
    if (adc_channel_working_status != ADC_DIFFERENTIAL_WORKING) { return 0; } /* Mutually exclusive with AMIC */
    int32_t sample_value = 0;
    if (((pchannel >= ADC_CHANNEL_MAX_NUM) || (nchannel >= ADC_CHANNEL_MAX_NUM)) || (!adc_is_power_on)) { return 0; }
#ifdef CONFIG_ADC_SUPPORT_MUTEX
    osal_mutex_lock(&g_adc_mutex);
#endif
    sample_value = hal_adc_auto_sample_differential((adc_channel_t)pchannel, (adc_channel_t)nchannel);
#ifdef CONFIG_ADC_SUPPORT_MUTEX
    osal_mutex_unlock(&g_adc_mutex);
#endif
    return sample_value;
}
#endif /* CONFIG_ADC_SUPPORT_DIFFERENTIAL_SAMPLE */

#endif /* CONFIG_ADC_SUPPORT_AFE */
