/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provide fsm pairing state \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-03-08， Create file. \n
 */
#include "fsm_pairing.h"

fsm_pairing_state_t g_fsm_pairing_state;

fsm_pairing_state_t get_fsm_pairing_state(void)
{
    return g_fsm_pairing_state;
}

void set_fsm_pairing_state(fsm_pairing_state_t state)
{
    g_fsm_pairing_state = state;
}

void fsm_pairing_init(void)
{
    g_fsm_pairing_state = PAIRING_INIT;
}
