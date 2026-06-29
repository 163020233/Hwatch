/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: ux bg sle grant callback src code.\n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-12-08, Create file. \n
 */
#ifndef UX_BS_SLE_GRANT_ADAPT_H
#define UX_BS_SLE_GRANT_ADAPT_H

#include "errcode.h"
#include "stddef.h"
#include "stdbool.h"
#include "audio_manager_service.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define UX_BS_BGH_CBK_COUNT 7

typedef struct {
    AudioChannelSelect  channel_info[CONFIG_MAX_CONNECT_DEVICE_NUM];
} ux_channel_param;

bool ux_bs_grant_get_sle_enable(void);
void ux_bs_grant_set_sle_enable(bool is_enable);
ux_pair_mode_t ux_bs_grant_get_pair_mode(void);
void ux_bs_grant_set_pair_mode(ux_pair_mode_t mode);
uint32_t ux_bs_sle_grant_get_autorate_rate(void);

errcode_t ux_bs_sle_change_to_reconnect_mode(void);

errcode_t bs_sle_grant_register_cbk(void);
void bs_sle_grant_unregister_cbk(void);
void ux_bs_sle_grant_clear_channel_occupancy(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif