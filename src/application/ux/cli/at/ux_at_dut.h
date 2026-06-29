/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 *
 * Description: Provide ux dut at service \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-09-13， Create file. \n
 */
#ifndef UX_AT_DUT_H
#define UX_AT_DUT_H

#include "stdint.h"
#include "errcode.h"
#include "at.h"
#include "ux_at_manager.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

at_ret_t at_dut_set_process(const ux_at_para1_args_t *args);
at_ret_t at_dut_read_process(void);

at_ret_t ftm_bt_set_addr(const char* para);
at_ret_t ftm_bt_set_name(const char* para);
at_ret_t ftm_bt_get_addr(void);
at_ret_t ftm_bt_get_name(void);

at_ret_t at_ftm_bt_enable_process(void);
at_ret_t at_ftm_bt_read_process(void);

#ifdef MELODY_DEMO_EARPHONE
at_ret_t ftm_bt_signal_enable_process(const char* para);
at_ret_t ftm_bt_signal_disable_process(const char* para);
uint8_t ftm_bt_signal_read_process(void);

at_ret_t at_dut_bt_signal_set_process(const ux_at_para1_args_t *args);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
