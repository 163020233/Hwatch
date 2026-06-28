/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: gnss board port config \n
 *
 * History: \n
 * 2023-11-15, Create file. \n
 */
#ifndef GNSS_BOARD_PORT_CONFIG_H
#define GNSS_BOARD_PORT_CONFIG_H

#include "gnss_board_port.h"
#include "gpio.h"
#include "uart.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#ifdef CFG_DRIVERS_SPINOR
#define BIN_FILE_PATH  "/user/gnss/firmware/gnss_config.bin"
#else
#define BIN_FILE_PATH  "/boot/gnss/gnss_config.bin"
#endif

static const gnss_board_hw_info g_gnss_board_hw_info = { UART_BUS_0, { S_AGPIO5, S_AGPIO3, S_AGPIO4 } };

static const gnss_board_info g_gnss_board_info = {
#ifdef CONFIG_FILE_BY_ARRAY
    .cfg_path = NULL,
#else
    .cfg_path = BIN_FILE_PATH,
#endif
    .hw_infos = (gnss_board_hw_info*)&g_gnss_board_hw_info
};

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
