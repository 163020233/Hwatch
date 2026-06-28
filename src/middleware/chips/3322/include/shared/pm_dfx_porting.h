/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved. \n
 *
 * Description: Provides pm dfx port \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2024-08-09， Create file. \n
 */
#ifndef PM_DFX_PORTING_H
#define PM_DFX_PORTING_H

#include "stddef.h"
#include "stdint.h"
#include "platform_core.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup middleware_chips_pm_dfx_port PM dfx port
 * @ingroup  middleware_chips_pm
 * @{
 */

#if CORE == APPS
typedef enum {
    ULP_PWRON_DIS_INTR_WKUP  = 0,
    ULP_GPIO_INTR_WKUP       = 1,
    PWM_CFG_INTR_WKUP        = 2,
    NPU_INTR_WKUP            = 3,
    M_SSI_WKUP               = 4,
    M_RTC_INTR_WKUP          = 5,
    M_L0_UART_RXD_IN_WKUP    = 6,
    M_IPC_INTR_WKUP          = 7,
    M_H0_UART_RXD_IN_WKUP    = 8,
    M_GPIO_INTR_WKUP         = 9,
    M_DAP_WKUP               = 10,
    M_DAP_LEVEL_WKUP_UART_RXD_IN_WKUP = 11,
    DSP_WDT_INTR_WKUP        = 12,
    CWDT_INTR_WKUP           = 13,
    BT_WDT_INTR_WKUP         = 14,
    AUDIO_INTR_WKUP_EN_MASK  = 15,
    USB_VAILD_INTR_WKUP      = 16,
    ULP_VBUS_SENSE_INTR_WKUP = 17,
    ULP_VBUS_SENSE_DIS_INTR_WKUP = 18,
    ULP_SYS_TICK_ALARM_INTR_WKUP = 19,
    ULP_PWRON_INTR_WKUP_EN_MASK  = 20,
    DISPLAY_INTR_WKUP        = 21,
    TE_INTR_WKUP             = 22,
    WAKEUP_SOURCE_MAX,
} wakeup_source_t;

#define WAKEUP_SOURCE_COUNT     ULP_PWRON_INTR_WKUP_EN_MASK
#endif

#if CORE == BT
typedef enum {
    B_OSC_EN_WKUP           = 0,
    B_SSI_WKUP              = 1,
    B_RTC_INTR_WKUP         = 2,
    B_IPC_INTR_WKUP         = 3,
    B_GPIO_INTR_WKUP        = 4,
    B_DAP_WKUP              = 5,
    B_DAP_LEVEL_WKUP_UART_RXD_IN_WKUP = 6,
    B_COEX_UART_RXD_IN_WKUP = 7,
    WAKEUP_SOURCE_MAX,
} wakeup_source_t;

#define WAKEUP_SOURCE_COUNT     B_COEX_UART_RXD_IN_WKUP
#endif

/**
 * @if Eng
 * @brief  PM wakeup source statistics.
 * @param  [in]  interrupt_id wakup interrupt id.
 * @else
 * @brief  PM唤醒源统计接口。
 * @param  [in]  interrupt_id 唤醒中断号。
 * @endif
 */
void pm_dfx_wakeup_src_statistics(uint32_t interrupt_id);

/**
 * @if Eng
 * @brief  PM ext wakeup source statistics.
 * @param  [in]  wkup_src For details, see @ref wakeup_source_t.
 * @else
 * @brief  PM 额外唤醒源统计接口。
 * @param  [in]  wkup_src 参考 wakeup_source_t。
 * @endif
 */
void pm_dfx_ext_wakeup_src_statistics(wakeup_source_t wkup_src);

/**
 * @if Eng
 * @brief  PM dump wakeup source statistics.
 * @else
 * @brief  PM 打印唤醒源统计接口。
 * @endif
 */
void pm_dfx_dump_wakeup_src_statistics(void);

/**
 * @if Eng
 * @brief  PM clear wakeup source statistics.
 * @else
 * @brief  PM 唤醒源统计清零接口。
 * @endif
 */
void pm_dfx_clear_wakeup_src_statistics(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
