/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Header file of lcd driver configuration.
 *
 * Create: 2022-03-05
 */

#ifndef __LCD_CONFIG_H__
#define __LCD_CONFIG_H__

#include "soc_lcd_api.h"

#define DEFAULT_BRIGHTNESS 255

typedef enum {
    VIDEO_LCD = 1,
    COMMAND_LCD = 2,
} lcd_type;

lcd_drv_cfg *lcd_get_driver_ops(void);
uint32_t lcd_get_ops_num(void);
combo_dev_cfg_t *lcd_get_bus_escape_config(void);

#endif
