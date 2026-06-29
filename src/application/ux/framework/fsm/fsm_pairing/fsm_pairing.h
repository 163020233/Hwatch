/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provide fsm pairing state \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-03-08， Create file. \n
 */
#ifndef FSM_PAIRING_H
#define FSM_PAIRING_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum {
    /**
     *  整机刚上电时的初始状态
     */
    PAIRING_INIT = 1,
    /**
     *  对耳配对状态
     */
    PAIRING_EAR,
    /**
     *  对耳回连状态
     */
    PAIRING_EAR_RECONNECTION,
    /**
     *  设备配对状态
     */
    PAIRING_DEVICE,
    /**
     *  设备回连状态
     */
    PAIRING_DEVICE_RECONNECTION,
    /**
     *  待机模式
     */
    PAIRING_STANDBY,

    PAIRING_STATE_BUTT
} fsm_pairing_state_t;

fsm_pairing_state_t get_fsm_pairing_state(void);
void set_fsm_pairing_state(fsm_pairing_state_t state);
void fsm_pairing_init(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif