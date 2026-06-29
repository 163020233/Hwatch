/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 *
 * Description: Provide ux bt at service \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-06-13， Create file. \n
 */
#ifndef UX_AT_TWS_H
#define UX_AT_TWS_H
#include "stdint.h"
#include "errcode.h"
#include "at.h"
#include "ux_at_manager.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


at_ret_t parse_wear_para_process(const char *para);
at_ret_t parse_recover_para_process(const char *para);
at_ret_t parse_voiceasst_para_process(const char *para);
at_ret_t parse_wear_control_process(const char *para);
at_ret_t parse_wear_detection_switch_process(const char *para);
at_ret_t parse_low_latency_switch_process(const char *para);
at_ret_t parse_dual_connection_switch_process(const char *para);
at_ret_t parse_language_process(const char *para);
at_ret_t parse_try_handover_process(const char *para);
at_ret_t query_wear_state(void);
at_ret_t query_work_state(void);
at_ret_t query_a2dp_state(void);
at_ret_t query_hfp_state(void);
/* 查询 language */
at_ret_t query_language(void);
at_ret_t query_wear_control_state(void);
at_ret_t query_wear_detection_switch_state(void);
at_ret_t query_low_latency_switch_state(void);
at_ret_t query_dual_connection_switch_state(void);

errcode_t get_wear_state_result(uint8_t *result, uint16_t length);
errcode_t get_pos_state_result(uint8_t *result, uint16_t length);
errcode_t get_work_state_result(uint8_t *result, uint16_t length);
errcode_t get_main_state_result(uint8_t *result, uint16_t length);
errcode_t get_call_state_result(uint8_t *result, uint16_t length);
errcode_t get_wear_control_result(uint8_t *result, uint16_t length);
errcode_t get_wear_detection_result(uint8_t *result, uint16_t length);
errcode_t get_low_latency_result(uint8_t *result, uint16_t length);
errcode_t get_dual_connection_result(uint8_t *result, uint16_t length);
errcode_t get_language_result(uint8_t *result, uint16_t length);

at_ret_t at_earphone_set_process(const ux_at_para2_args_t *args);
at_ret_t at_earphone_read_process(void);
at_ret_t at_earphone_query_process(const ux_at_para2_args_t *args);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
