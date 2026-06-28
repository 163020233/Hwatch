/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provide fsm box state \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-03-08， Create file. \n
 */
#include "fsm_box.h"

fsm_box_state_t g_fsm_box_state;

fsm_box_state_t get_fsm_box_state(void)
{
    return g_fsm_box_state;
}

void set_fsm_box_state(fsm_box_state_t state)
{
    g_fsm_box_state = state;
}

void fsm_box_init(void)
{
    g_fsm_box_state = BOX_STATE_INBOX;
}