/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: dfx res
 * This file should be changed only infrequently and with great care.
 */
#include "dfx_res.h"
#include "osal_inner.h"
#include "soc_osal.h"
#include "osal_adapt.h"

#if defined(ENABLE_LOS_CUSTOM_DEBUG) && ENABLE_LOS_CUSTOM_DEBUG != 0
#include "los_custom_debug.h"
#else
#include "los_swtmr_pri.h"
#include "los_task_pri.h"
#include "los_sem_pri.h"
#include "los_queue_pri.h"
#include "los_mux_pri.h"
#endif

uint8_t dfx_os_get_timer_usage(void)
{
    uint8_t num = 0;

#if defined(ENABLE_LOS_CUSTOM_DEBUG) && ENABLE_LOS_CUSTOM_DEBUG != 0
    num = 0;
#else
    for (uint32_t i = 0; i < LOSCFG_BASE_CORE_SWTMR_LIMIT; i++) {
        const LosSwtmrCB *swtmrCB = OS_SWT_FROM_SWTID(i);
        if (swtmrCB->state != 0) {
            num++;
        }
    }
#endif

    return num;
}

uint8_t dfx_os_get_task_usage(void)
{
    uint8_t num = 0;

#if defined(ENABLE_LOS_CUSTOM_DEBUG) && ENABLE_LOS_CUSTOM_DEBUG != 0
    num = 0;
#else
    for (uint32_t i = 0; i < g_taskMaxNum; i++) {
        const LosTaskCB *taskCB = OS_TCB_FROM_TID(i);
        if ((taskCB->taskStatus & OS_TASK_STATUS_UNUSED) != 0) {
            continue;
        }
        num++;
    }
#endif

    return num;
}

uint8_t dfx_os_get_sem_usage(void)
{
    uint8_t num = 0;

#if defined(ENABLE_LOS_CUSTOM_DEBUG) && ENABLE_LOS_CUSTOM_DEBUG != 0
    num = 0;
#else
    for (uint32_t i = 0; i < LOSCFG_BASE_IPC_SEM_LIMIT; i++) {
        const LosSemCB *semNode = GET_SEM(i);
        if (semNode->semStat == 1) {
            num++;
        }
    }
#endif

    return num;
}

uint8_t dfx_os_get_queue_usage(void)
{
    uint8_t num = 0;

#if defined(ENABLE_LOS_CUSTOM_DEBUG) && ENABLE_LOS_CUSTOM_DEBUG != 0
    num = 0;
#else
    for (uint32_t i = 0; i < LOSCFG_BASE_IPC_QUEUE_LIMIT; i++) {
        const LosQueueCB *queueCB = GET_QUEUE_HANDLE(i);
        if (queueCB->queueState == 1) {
            num++;
        }
    }
#endif

    return num;
}

uint8_t dfx_os_get_mux_usage(void)
{
    uint8_t num = 0;

#if defined(ENABLE_LOS_CUSTOM_DEBUG) && ENABLE_LOS_CUSTOM_DEBUG != 0
    num = 0;
#else
    for (uint32_t i = 0; i < LOSCFG_BASE_IPC_MUX_LIMIT; i++) {
        const LosMuxCB *muxCB = GET_MUX(i);
        if (muxCB->muxStat == 1) {
            num++;
        }
    }
#endif

    return num;
}

errcode_t dfx_os_get_resource_status(osal_os_resource_use_stat_t *os_resource_stat)
{
    if (os_resource_stat == NULL) {
        return ERRCODE_FAIL;
    }

    os_resource_stat->timer_usage = dfx_os_get_timer_usage();
    os_resource_stat->task_usage = dfx_os_get_task_usage();
    os_resource_stat->sem_usage = dfx_os_get_sem_usage();
    os_resource_stat->queue_usage = dfx_os_get_queue_usage();
    os_resource_stat->mux_usage = dfx_os_get_mux_usage();

    return ERRCODE_SUCC;
}