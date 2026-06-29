/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * File          ux_bs_database.h
 * Description:  provide ux bs database header file.
 */
#ifndef UX_BS_DATABASE_H
#define UX_BS_DATABASE_H

#include "errcode.h"
#include "stddef.h"
#include "stdbool.h"
#include "ux_bg_config.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void ux_bs_db_save_single_pdl_info(void *param);
void ux_bs_db_save_pdl_order(void *param);

void ux_bs_db_save_protocol_pdl_info(ux_work_protocol_t protocol, void *param);
void ux_bs_db_save_protocol_pdl_order(ux_work_protocol_t protocol, void *param);
void ux_bs_db_save_protocol_multi_pdl_info(ux_work_protocol_t protocol, void *param);
void ux_bs_db_save_protocol_remainder_multi_pdl_info(ux_work_protocol_t protocol, void *param);

void ux_bs_db_grant_pdl_data_print(void *param);
void ux_bs_db_read_all_pdl_info(void *pdl_info_list, uint8_t *pdl_num, uint8_t max_pdl_num);
void ux_bs_db_read_pdl_order(void *param);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  // end UX_BS_DATABASE_H