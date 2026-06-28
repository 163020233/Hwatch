/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides v330 HAL CAN-FD \n
 *
 * History: \n
 * 2023-02-23, Create file. \n
 */
#ifndef HAL_CAN_FD_V330_H
#define HAL_CAN_FD_V330_H

#include "hal_can.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_can_fd_v330 CAN-FD v330
 * @ingroup  drivers_hal_can
 * @{
 */

typedef enum can_int_source {
    RF0N,
    RF0W,
    RF0F,
    RF0L,
    RF1N,
    RF1W,
    RF1F,
    RF1L,
    HPM,
    TC,
    TCF,
    TFE,
    TEFN,
    TEFW,
    TEFF,
    TEFL,
    TSW,
    MRAF,
    TOO,
    DRX,
    BEC,
    BEU,
    ELO,
    EP,
    EW,
    BO,
    WDI,
    PEA,
    PED,
    ARA,

    CAN_V330_MAX_IRQ_NUM
} can_int_source_t;

/**
 * @if Eng
 * @brief  CAN-FD v330 interrupt handler
 * @else
 * @brief  CAN-FD v330中断控制器。
 * @endif
 */
void hal_can_fd_v330_irq_handler(uint32_t int_line);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif