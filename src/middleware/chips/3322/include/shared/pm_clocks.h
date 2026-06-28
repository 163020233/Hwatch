/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: pm clocks manage header
 */

#ifndef SRC_LIB_PM_CLOCKS_H
#define SRC_LIB_PM_CLOCKS_H

#include "stdint.h"
#include "errcode.h"
#include "hal_clocks_app.h"
#include "pm_veto_porting.h"

typedef struct {
    clocks_src_t dsp_src;
    clocks_src_t codec_src;
    uint8_t dsp_div;
    uint8_t codec_div;
    uint64_t dsp_freq_val;
    uint64_t codec_freq_val;
} pm_hifi_freq_t;

typedef enum {
    PM_MODE_NONE = 0x0,
    PM_MODE_LP = 0x1,
    PM_MODE_NORMAL = 0x2,
    PM_MODE_MIDDLE = 0x3,
    PM_MODE_PERFORMACE = 0x4,
    PM_MODE_MAX,
} pm_mode_t;

typedef enum {
    HIFI_LPM_CLK_LOW_LEVEL,  // dsp:64MHz(asic)/32M(fpga), codec:32M
    HIFI_LPM_CLK_LEVEL1,     // dsp:133Mhz, codec:24.576Mhz
    HIFI_LPM_CLK_LEVEL2,     // dsp:200Mhz, codec:24.576Mhz
    HIFI_LPM_CLK_HIGH_LEVEL, // dsp:266Mhz, codec:24.576Mhz
    HIFI_LPM_CLK_NONE,
    HIFI_LPM_CLK_MAX,
} pm_hifi_mode_t;

typedef enum {
    PM_MODE_DPU_NONE,
    PM_MODE_DPU_AOD,
    PM_MODE_DPU_HIGH_SPEED,
    PM_MODE_DPU_MAX,
} pm_dpu_mode_t;

typedef enum {
    PM_MODE_NPU_NONE,
    PM_MODE_NPU_LP,
    PM_MODE_NPU_MIDDLE,
    PM_MODE_NPU_HIGH_SPEED,
    PM_MODE_NPU_MAX,
} pm_npu_mode_t;

typedef enum {
    CLOCKS_ID_M_SEC_PKE    = HAL_CLOCKS_APP_SEC_PKE_CLK,
    CLOCKS_ID_M_SEC_KM     = HAL_CLOCKS_APP_SEC_KM_CLK,
    CLOCKS_ID_M_SEC_BUS    = HAL_CLOCKS_APP_SEC_BUS_CLK,
    CLOCKS_ID_M_PWM        = HAL_CLOCKS_APP_PWM_CLK,
    CLOCKS_ID_M_SPI0       = HAL_CLOCKS_APP_SPI0_MS_CLK,
    CLOCKS_ID_M_SPI1       = HAL_CLOCKS_APP_SPI1_M_CLK,
    CLOCKS_ID_M_UART_H0    = HAL_CLOCKS_APP_UART_H0_CLK,
    CLOCKS_ID_M_UART_L0    = HAL_CLOCKS_APP_UART_L0_CLK,
    CLOCKS_ID_M_UART_H1    = HAL_CLOCKS_APP_UART_H1_CLK,
    CLOCKS_ID_M_I2C0       = HAL_CLOCKS_APP_I2C0_CLK,
    CLOCKS_ID_M_I2C1       = HAL_CLOCKS_APP_I2C1_CLK,
    CLOCKS_ID_M_I2C2       = HAL_CLOCKS_APP_I2C2_CLK,
    CLOCKS_ID_M_I2C3       = HAL_CLOCKS_APP_I2C3_CLK,
    CLOCKS_ID_M_AUXADC     = HAL_CLOCKS_APP_AUX_ADC_CLK,
    CLOCKS_ID_M_CRC        = HAL_CLOCKS_APP_M_CLK_CRC_CLK,
    CLOCKS_ID_M_QSPI_DISPLAY = HAL_CLOCKS_APP_QSPI_DISPLAY_CLK,

    CLOCKS_ID_MAX,
} clocks_id_t;

typedef void (*hifi_inform_handler)(void);

/**
 * @if Eng
 * @brief  Initializing the clocks Driver.
 * @else
 * @brief  初始化时钟驱动
 * @endif
 */
void uapi_clocks_init(void);

/**
 * @if Eng
 * @brief  CLOCKS switch configuration.
 * @param  id Configurable clocks module ID, see clocks_id_t.
 * @param  config Switch Status.
 * @else
 * @brief  配置时钟开关
 * @param  id 模块 id.
 * @param  config 时钟开关状态
 * @endif
 */
void uapi_clocks_switch(uint8_t id, switch_type_t config);

/**
 * @if Eng
 * @brief  CLOCKS mode configuration.
 * @param  uid Configurable clocks user module ID, see pm_veto_id_t.
 * @param  mode clock Status.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  配置时钟状态
 * @param  uid 用户模块 id，参考pm_veto_id_t。
 * @param  mode 时钟状态
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_clocks_config_mode(pm_veto_id_t uid, pm_mode_t mode);

/**
 * @if Eng
 * @brief  CLOCKS set hifi configuration.
 * @param  mode HIFI clock Status， see pm_hifi_mode_t.
 * @else
 * @brief  配置HIFI时钟状态
 * @param  mode HIFI时钟状态， 参考pm_hifi_mode_t。
 * @endif
 */
void uapi_set_hifi_mode(pm_hifi_mode_t mode);

/**
 * @if Eng
 * @brief  CLOCKS get hifi configuration.
 * @return  HIFI clock Status, see pm_hifi_mode_t.
 * @else
 * @brief  获取HIFI时钟状态
 * @return  HIFI时钟状态， 参考pm_hifi_mode_t。
 * @endif
 */
pm_hifi_mode_t uapi_pm_get_hifi_mode(void);

/**
 * @if Eng
 * @brief  CLOCKS get hifi clocks frequency based on hifi clocks mode.
 * @param  mode HIFI clock Status， see pm_hifi_mode_t.
 * @return  HIFI clock frequency, see pm_hifi_freq_t.
 * @else
 * @brief  配置HIFI时钟状态
 * @param  mode HIFI时钟状态， 参考pm_hifi_mode_t。
 * @return  HIFI时钟状态， 参考pm_hifi_freq_t。
 * @endif
 */
pm_hifi_freq_t uapi_pm_get_hifi_freq(pm_hifi_mode_t mode);

/**
 * @if Eng
 * @brief  CLOCKS register hifi clock swtich callback.
 * @param  hifi_inform_handler HIFI clock switch handler.
 * @else
 * @brief  配置HIFI时钟切换回调函数
 * @param  hifi_inform_handler HIF时钟切换函数。
 * @endif
 */
void uapi_register_hifi_inform_callback(hifi_inform_handler callback);

/**
 * @if Eng
 * @brief  CLOCKS set dpu configuration.
 * @param  mode DPU clock Status， see pm_dpu_mode_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  配置DPU时钟状态
 * @param  mode DPU时钟状态， 参考pm_dpu_mode_t。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pm_set_dpu_freq(pm_dpu_mode_t mode);

/**
 * @if Eng
 * @brief  CLOCKS get current clocks mode information.
 * @else
 * @brief  获取当前时钟模式信息
 * @endif
 */
void uapi_clocks_dfx_get_mode_info(void);

/**
 * @if Eng
 * @brief  CLOCKS set npu configuration.
 * @param  mode NPU clock Status， see pm_npu_mode_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  配置NPU时钟状态
 * @param  mode NPU时钟状态， 参考pm_npu_mode_t。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pm_set_npu_freq(pm_npu_mode_t mode);

#endif