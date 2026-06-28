/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: dfx task
 * This file should be changed only infrequently and with great care.
 */
#include "dfx_task.h"
#include "securec.h"
#include "errcode.h"
#include "los_task.h"
#include "soc_osal.h"
#include "osal_adapt.h"

extern unsigned int g_taskMaxNum;
const CHAR *OsTskStatusConvertStr(UINT16 taskStatus);

uint32_t dfx_os_get_task_cnt(void)
{
    return g_taskMaxNum;
}

errcode_t dfx_os_get_all_task_info(task_info_t *inf, uint32_t info_cnt)
{
    if (inf == NULL) {
        return ERRCODE_FAIL;
    }

    for (unsigned i = 0; i < info_cnt; i++) {
        task_info_t *info = &inf[i];
        if (dfx_os_get_task_info(i, info) != 0) {
            continue;
        }
    }

    return ERRCODE_SUCC;
}

errcode_t dfx_os_get_task_info(uint32_t taskid, task_info_t *info)
{
    uint32_t ret;
    osal_tsk_info_s task_info;

    if (info == NULL) {
        return ERRCODE_FAIL;
    }

    ret = osal_taskinfo_get(taskid, &task_info);
    if (ret != OSAL_SUCCESS) {
        info->valid = false;
        return ERRCODE_FAIL;
    }

    (void)memset_s(info->name, sizeof(info->name), 0, sizeof(info->name));
    (void)memcpy_s(info->name, sizeof(info->name), task_info.acName, sizeof(info->name) - 1);
    info->id = task_info.uwTaskID;
    info->valid = true;
    info->status = task_info.usTaskStatus;
    info->priority = task_info.usTaskPrio;
    info->stack_size = task_info.uwStackSize;
    info->top_of_stack = (UINTPTR)task_info.uwTopOfStack;
    info->bottom_of_stack = (UINTPTR)task_info.uwBottomOfStack;
    info->sp = (UINTPTR)task_info.uwSP;
    info->curr_used = task_info.uwCurrUsed;
    info->peak_used = task_info.uwPeakUsed;
    info->overflow_flag = task_info.bOvf;
    return ERRCODE_SUCC;
}

const char *dfx_os_task_status_convert_str(uint16_t task_status)
{
    return OsTskStatusConvertStr(task_status);
}
