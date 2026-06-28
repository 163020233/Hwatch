/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provide fsm box state \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-03-08， Create file. \n
 */
#ifndef FSM_BOX_H
#define FSM_BOX_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum {
    /**
     *  耳机在盒子里的状态。
     */
    BOX_STATE_INBOX = 1,
    /**
     *  耳机在盒子外的状态。
     */
    BOX_STATE_OUTBOX,
    BOX_STATE_BUTT
} fsm_box_state_t;

fsm_box_state_t get_fsm_box_state(void);

void set_fsm_box_state(fsm_box_state_t state);

void fsm_box_init(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif