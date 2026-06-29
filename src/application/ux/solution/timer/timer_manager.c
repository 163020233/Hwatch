/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provide timer set service \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-03-25， Create file. \n
 */
#include "cmsis_os2.h"
#include "ux_common_log.h"
#include "timer_manager.h"

osTimerId_t g_timer_set[UX_TIMER_SET_MAX] = {0};

errcode_t ux_timer_register(uint8_t index, timer_callback_t func)
{
    if (index >= UX_TIMER_SET_MAX) {
        ux_log_err("[ux][timer][ux_timer_register]invalid index:[%u]", index);
        return ERRCODE_FAIL;
    }

    if (g_timer_set[index] != NULL) {
        return ERRCODE_FAIL;
    }

    g_timer_set[index] = osTimerNew((osTimerFunc_t)func, osTimerOnce, NULL, NULL);
    if (g_timer_set[index] == NULL) {
        ux_log_err("[ux][timer][ux_timer_register] osTimerNew fail!timer idx:[%u]", index);
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

errcode_t ux_timer_start(uint8_t index, uint32_t time)
{
    uint32_t set_time = time;

    if (set_time == 0) {
        // 若定时器设置为0，则代表不需要等待，但是我们需要触发callback函数，所以此处将定时器设置为1ms
        set_time = 1;
    }

    if (index >= UX_TIMER_SET_MAX) {
        ux_log_err("[ux][timer][ux_timer_start]invalid index:[%u]", index);
        return ERRCODE_FAIL;
    }

    if (g_timer_set[index] == NULL) {
        ux_log_err("[ux][timer][ux_timer_start] timer null!timer idx:[%u]", index);
        return ERRCODE_FAIL;
    }

    if (osTimerIsRunning(g_timer_set[index]) != 0) {
        osStatus_t timer_ret = osTimerStop(g_timer_set[index]);
        if (timer_ret != osOK) {
            ux_log_info("[ux][timer][ux_timer_start]osTimerStop fail!timer idx:[%u] ret:[%d]", index, timer_ret);
            return ERRCODE_FAIL;
        }
    }

    osStatus_t timer_ret = osTimerStart(g_timer_set[index], set_time);
    if (timer_ret != osOK) {
        ux_log_err("[ux][timer][ux_timer_start]osTimerStart fail!timer idx:[%u] ret:[%d]",
            index, timer_ret);
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

errcode_t ux_timer_stop(uint8_t index)
{
    if (index >= UX_TIMER_SET_MAX) {
        ux_log_err("[ux][timer][ux_timer_stop]invalid index:[%u]", index);
        return ERRCODE_FAIL;
    }

    if (g_timer_set[index] == NULL) {
        return ERRCODE_FAIL;
    }

    if (osTimerIsRunning(g_timer_set[index]) == 0) {
        return ERRCODE_SUCC;
    }

    osStatus_t timer_ret = osTimerStop(g_timer_set[index]);
    if (timer_ret != osOK) {
        ux_log_err("[ux][timer][ux_timer_stop]osTimerStop fail!timer idx:[%u] ret:[%d]",
            index, timer_ret);
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

uint8_t get_timer_state(uint8_t index)
{
    if (index >= UX_TIMER_SET_MAX) {
        ux_log_err("[ux][timer][get_timer_state]invalid index:[%u]", index);
        return UX_TIMER_STATE_INVALID;
    }

    if (g_timer_set[index] == NULL) {
        return UX_TIMER_STATE_UNREGISTERED;
    }

    if (osTimerIsRunning(g_timer_set[index]) != 0) {
        return UX_TIMER_STATE_RUNNING;
    }

    return UX_TIMER_STATE_STOP;
}

void ux_timer_list_stop(uint8_t *list, uint8_t size)
{
    if ((list == NULL) || (size == 0)) {
        ux_log_err("[ux][timer][ux_timer_list_stop] para error!");
        return;
    }

    for (uint8_t i = 0; i < size; i++) {
        ux_timer_stop(list[i]);
    }
}

errcode_t ux_timer_deregister(uint8_t index)
{
    if (index >= UX_TIMER_SET_MAX) {
        ux_log_err("[ux][timer][ux_timer_deregister]invalid index:[%u]", index);
        return ERRCODE_FAIL;
    }

    if (g_timer_set[index] == NULL) {
        return ERRCODE_SUCC;
    }

    osStatus_t os_op = osTimerDelete(g_timer_set[index]);
    g_timer_set[index] = NULL;
    if (os_op != osOK) {
        ux_log_err(
            "[ux][timer][ux_timer_deregister]osTimerDelete fail!timer idx:[%u] ret:[%d]",
            index, os_op);
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}
