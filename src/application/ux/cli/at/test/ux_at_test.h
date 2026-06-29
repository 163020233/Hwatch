/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provide api functions for at \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-04-21， Create file. \n
 */
#ifndef UX_AT_TEST_H
#define UX_AT_TEST_H
#ifndef DEMO_WATCH
#include "feature_config.h"
#endif
#include "stdint.h"
#include "errcode.h"
#include "at.h"
#include "ux_at_manager.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef errcode_t (*at_ux_test_process_t)(const char *func_name, const char *device, const char *param,
    const char *sub_param);
typedef errcode_t (*at_ux_test_operator_t)(const char *device, const char *param, const char *sub_param);
typedef struct {
    const char *cmd;
    at_ux_test_process_t func;
} ux_at_test_cmd_map_t;

typedef struct {
    const char *cmd;
    at_ux_test_operator_t operator_func;
} ux_at_test_opts_map_t;
errcode_t ux_at_test_char_to_num_16(uint8_t ch, uint8_t *num);
errcode_t ux_at_test_fixed_str_to_data(uint16_t str_len, uint8_t *str, uint16_t data_len, uint8_t *data);
uint32_t ux_at_test_get_digit(const char *param);
errcode_t test_commu_sync_get_process(const char *device, const char *param, const char *sub_param);
errcode_t ux_at_test_commu_query_process(const char *func_name, const char *device, const char *param,
    const char *sub_param);
at_ret_t at_ux_test_set_process(const ux_at_para5_args_t *args);
at_ret_t at_ux_test_read_process(void);
at_ret_t at_ux_test_query_process(const ux_at_para5_args_t *args);
at_ret_t at_ux_test_set_process(const ux_at_para5_args_t *args);
at_ret_t at_ux_test_read_process(void);
uint32_t ux_at_test_get_digit(const char *param);
errcode_t ux_at_test_str_to_data(const char *str, uint16_t data_len, uint8_t *data);
errcode_t ux_at_test_fill_data(void *dst, size_t dst_len, void *src, size_t src_len);
errcode_t ux_at_test_sensor_set_process(const char *func_name, const char *device,
    const char *param, const char *sub_param);
#ifdef CONFIG_SUPPORT_JACK
errcode_t ux_at_test_jack_query_process(const char *func_name, const char *device,
    const char *param, const char *sub_param);
#endif

#ifdef CONFIG_SUPPORT_SOURCE_MANAGER_FOCUS_MODE
errcode_t ux_at_test_play_mode_query_process(const char *func_name, const char *device,
    const char *param, const char *sub_param);
#endif

errcode_t ux_at_test_sle_query_process(const char *func_name, const char *param1,
    const char *param2, const char *param3);
errcode_t ux_at_test_sle_set_process(const char *func_name, const char *param1,
    const char *param2, const char *param3);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
