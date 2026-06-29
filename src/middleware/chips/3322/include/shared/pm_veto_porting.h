/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provides pm veto port \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-01-13， Create file. \n
 */
#ifndef PM_VETO_PORTING_H
#define PM_VETO_PORTING_H

#include <stdint.h>
#include <stdbool.h>
#include "systick.h"
#include "systick_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup middleware_chips_pm_veto_port PM veto port
 * @ingroup  middleware_chips_pm
 * @{
 */

#define PM_GET_LR               ((uintptr_t)__builtin_return_address(0))
#define PM_GET_CURRENT_MS       uapi_systick_get_ms()
#define PM_GET_CURRENT_COUNT    uapi_systick_get_count()
#define pm_convert_ms_2_count   convert_ms_2_count

#ifndef CONFIG_PM_VETO_TIMEOUT_COUNT
#define CONFIG_PM_VETO_TIMEOUT_COUNT 1
#endif

#if CORE == BT // btrom调用，枚举值无法修改
typedef enum pm_veto_id {
    PM_ID_BSP       = 0,
    PM_ID_SYS       = 1,
    PM_ID_BTH       = 2,
    PM_ID_BTC       = 3,
    PM_ID_AUDIO     = 4,
    PM_ID_VIDEO     = 5,
    PM_ID_DMA       = 6,
    PM_ID_DPU       = 7,
    PM_ID_VAU       = 8,
    PM_ID_BUTTON    = 9,
    PM_ID_DEBUG     = 10,
    PM_ID_ADC       = 11,
    PM_ID_DFX       = 12,
    PM_ID_SCREEN    = 13,
    PM_ID_STORAGE   = 14,
    PM_ID_TOUCH     = 15,
    PM_ID_JPEG      = 16,
    PM_ID_VDEC      = 17,
    PM_ID_MAX       = 18,
    PM_VETO_ID_MAX = PM_ID_MAX,
} pm_veto_id_t;
#else
typedef enum pm_veto_id {
    PM_ID_BSP               = 0,
    PM_ID_MMC               = 1, // FIXED VALUE.
    PM_ID_SYS               = 2,
    PM_ID_BTH               = 3,
    PM_ID_UIKIT             = 4,
    PM_ID_EFUSE             = 5,
    PM_ID_AUDIO             = 6,
    PM_ID_VIDEO             = 7,
    PM_ID_DMA               = 8,
    PM_ID_SPI               = 9,
    PM_ID_I2C               = 10,
    PM_ID_DPU               = 11,
    PM_ID_VAU               = 12,
    PM_ID_BUTTON            = 13,
    PM_ID_DEBUG             = 14,
    PM_ID_ADC               = 15,
    PM_ID_DFX               = 16,
    PM_ID_SCREEN            = 17,
    PM_ID_STORAGE           = 18,
    PM_ID_TOUCH             = 19,
    PM_ID_JPEG              = 20,
    PM_ID_USB               = 21,
    PM_ID_VDEC              = 22,
    PM_ID_NPU               = 23,
    PM_WLAN_VETO_ID         = 24,
    PM_IPERF_VETO_ID        = 25,
    PM_SECURITY_KM_VETO_ID  = 26,
    PM_SECURITY_PKE_VETO_ID = 27,
    PM_SECURITY_SYMC_VETO_ID = 28,
    PM_SECURITY_HASH_VETO_ID = 29,
    PM_SECURITY_TRNG_VETO_ID = 30,
    PM_ID_JS                = 31,
    PM_ID_MSGCENTER         = 32,
    PM_ID_CAN               = 33,
    PM_ID_VICAP             = 34,
    PM_ID_SENSOR            = 35,
    PM_ID_NAND              = 36,
    PM_ID_TIOT              = 37,
    PM_ID_AUDIO_DEBUG       = 38,
    PM_ID_GNSS              = 39,
    PM_ID_MAX               = 40,
    PM_VETO_ID_MAX = PM_ID_MAX,
} pm_veto_id_t;
#endif

/**
 * @if Eng
 * @brief  Get customized sleep veto.
 * @retval true  Veto deep-sleep.
 * @retval false Allow deep-sleep.
 * @else
 * @brief  获取客制化的睡眠否决票。
 * @retval true  不允许睡眠。
 * @retval false 允许睡眠。
 * @endif
 */
bool pm_port_get_customized_sleep_veto(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
