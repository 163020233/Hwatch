/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides pwm port \n
 *
 * History: \n
 * 2022-09-16， Create file. \n
 */
#include "common_def.h"
#include "chip_io.h"
#include "platform_core.h"
#include "reboot_porting.h"


void reboot_port_reboot_chip(void)
{
    reg16_clrbit(GLB_CTL_M_RB_SYS_SOFT_WDT_RST_REG, GLB_CTL_M_RB_CFG_GLBM_SOFT_RST_SYS_N_OFFSET);
}

