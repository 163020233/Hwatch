/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2021. All rights reserved.
 * Description: Delay Header file.
 */

#ifndef __SOC_TIMER__
#define __SOC_TIMER__
#include <td_base.h>


void uapi_mdelay(uint32_t us);

void uapi_set_cpu_cycles_in_us(uint32_t cycle);

#endif
