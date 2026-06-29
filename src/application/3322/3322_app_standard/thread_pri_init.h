/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description:  Application Core OS Priority Initialize Interface Header for Standard
 */

#ifndef THREAD_PRI_INIT_H
#define THREAD_PRI_INIT_H

#include <stdint.h>
#include "drv_gpu_intf.h"

void los_vfs_init(void);
uint32_t dpal_driver_base_init(void);

/**
 * @defgroup MAIN_ENTRY CHIP Main Entry
 * @{
 */

/**
 * @brief  Thread task creat.
 */
void thread_pri_init(void);

/**
 * @}
 */

#endif
