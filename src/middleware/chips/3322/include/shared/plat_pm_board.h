/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: plat pm board source header file
 * Author: CompanyName
 */

#ifndef __PLAT_PM_BOARD_H__
#define __PLAT_PM_BOARD_H__

#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

errcode_t ws53_board_power_on(void);
errcode_t ws53_board_power_off(void);
errcode_t ws53_board_power_reset(void);
errcode_t pm_board_gpio_init(void);
void pm_board_gpio_deinit(void);
errcode_t pm_board_init(void);
void pm_board_exit(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __PLAT_PM_BOARD_H__ */
