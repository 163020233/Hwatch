/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: dfx cpup
 * This file should be changed only infrequently and with great care.
 */

#include "dfx_cpup.h"
#include "errcode.h"
#include "dfx_os_st.h"
#include "common_def.h"
#include "los_cpup.h"
#ifdef LOSCFG_KERNEL_CPUP
#include "osal_adapt.h"
#endif

uint32_t dfx_cpup_get_all_usage(uint16_t max_num, dfx_cpup_item_usage_info_t *cpup_info, uint32_t mode, uint16_t flag)
{
#ifdef LOSCFG_KERNEL_CPUP
    return osal_allcpu_usage(max_num, (osal_cpup_info_t *)cpup_info, mode, flag);
#else
    unused(max_num);
    unused(cpup_info);
    unused(mode);
    unused(flag);
    return (uint32_t)-1;
#endif
}

void dfx_cpup_reset(void)
{
#ifdef LOSCFG_KERNEL_CPUP
    osal_cpup_reset();
#endif
}

void dfx_cpup_stop(void)
{
#ifdef LOSCFG_KERNEL_CPUP
    osal_cpup_stop();
#endif
}

void dfx_cpup_start(void)
{
#ifdef LOSCFG_KERNEL_CPUP
    osal_cpup_start();
#endif
}