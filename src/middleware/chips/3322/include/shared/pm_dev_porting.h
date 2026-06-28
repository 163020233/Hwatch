/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provides pm device port \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-01-13， Create file. \n
 */
#ifndef PM_DEV_PORTING_H
#define PM_DEV_PORTING_H
#include "chip_core_definition.h"

#include "chip_io.h"
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup middleware_chips_pm_dev_port PM dev port
 * @ingroup  middleware_chips_pm
 * @{
 */

#if defined(CONFIG_VIDEO_SUPPORT_LPM) && defined(SUPPORT_DISPLAY_TECH)
void uapi_pm_set_display_power_sts(switch_type_t type);
errcode_t uapi_video_suspend(uintptr_t arg);
errcode_t uapi_video_resume(uintptr_t arg);
#endif

typedef enum pm_dev_id {
    PM_DEV_NONE,
    PM_DEV_CPU_TRACE,
    PM_DEV_PINCTRL,
    PM_DEV_WATCHDOG,
    PM_DEV_UART,
    PM_DEV_SFC,
    PM_DEV_PSRAM,
    PM_DEV_DMA,
    PM_DEV_I2C,
    PM_DEV_SPI,
    PM_DEV_FMC,
    PM_DEV_EMMC,
    PM_DEV_SDIO_HOST,
    PM_DEV_VIDEO,
#if CORE == BT
#else
    PM_DEV_PWM,
#endif
#if defined(CONFIG_GPIO_SUPPORT_LPM)
    PM_DEV_GPIO,
#endif
    PM_DEV_MAX,
} pm_dev_id_t;

typedef enum {
    PM_VIDEO_SWITCH_SCREEN_STA = 0,
    PM_VIDEO_SWITCH_LOW_PWR_STA,
} pm_video_sta_change_type;

/**
 * @if Eng
 * @brief  Register PM dev resume interface.
 * @else
 * @brief  注册PM设备恢复接口。
 * @endif
 */
void pm_register_dev_resume_interface(void);

void pm_register_clock_interface(void);

#if defined(CONFIG_PM_DEV_SUSPEND_DFX)
void uapi_pm_dev_record_suspend(uint8_t id);

void uapi_pm_dev_record_resume(uint8_t id);

uint8_t uapi_pm_dev_get_suspend_info(uint8_t id);
#endif

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif

