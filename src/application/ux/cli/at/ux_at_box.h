/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 *
 * Description: Provide ux box service \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-08-26， Create file. \n
 */
#ifndef UX_AT_BOX_H
#define UX_AT_BOX_H
#include "stdint.h"
#include "errcode.h"
#include "at.h"
#include "ux_at_manager.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

at_ret_t parse_pos_para_process(const char *para);
at_ret_t query_position_state(void);

at_ret_t at_box_set_process(const ux_at_para2_args_t *args);
at_ret_t at_box_read_process(void);
at_ret_t at_box_query_process(const ux_at_para2_args_t *args);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
