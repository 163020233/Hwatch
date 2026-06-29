/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provide fsm manager state \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-03-08， Create file. \n
 */
#ifndef FSM_MASTER_H
#define FSM_MASTER_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum {
    /**
     *  整机在上电后，尚未进入业务状态时的状态。
     */
    MASTER_POWEROFF = 1,
    /**
     *  整机进入音频模式
     */
    MASTER_SOUND,
    /**
     *  整机进入助听模式
     */
    MASTER_HAID,
    /**
     *  整机进入恢厂模式
     */
    MASTER_RECOVER,
    /**
     *  整机进入升级模式
     */
    MASTER_OTA,
    /**
     *  整机进入工厂模式
     */
    MASTER_DUT,
    MASTER_STATE_BUTT
} fsm_master_state_t;

fsm_master_state_t get_fsm_master_state(void);
fsm_master_state_t get_pre_fsm_master_state(void);

void set_fsm_master_state(fsm_master_state_t state);

void fsm_master_init(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
