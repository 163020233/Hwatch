/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provide fsm manager state \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-03-08， Create file. \n
 */
#include "fsm_master.h"

fsm_master_state_t g_fsm_master_state;
fsm_master_state_t g_pre_fsm_master_state;

fsm_master_state_t get_fsm_master_state(void)
{
    return g_fsm_master_state;
}

fsm_master_state_t get_pre_fsm_master_state(void)
{
    return g_pre_fsm_master_state;
}

void set_fsm_master_state(fsm_master_state_t state)
{
    g_pre_fsm_master_state = g_fsm_master_state;
    g_fsm_master_state = state;
}

void fsm_master_init(void)
{
    g_fsm_master_state = MASTER_POWEROFF;
    g_pre_fsm_master_state = g_fsm_master_state;
}
