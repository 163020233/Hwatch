/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-10-10， Create file. \n
 */
 
#ifndef UX_BT_A2DP_SRC_H
#define UX_BT_A2DP_SRC_H
#include "ux_bt_a2dp.h"
#include "bs_br_media_stream.h"

#if defined(__cplusplus) && __cplusplus
extern "C" {
#endif /* __cplusplus */

ux_bt_a2dp_info_t* ux_bt_a2dp_get_src_info(void);
void ux_bt_a2dp_src_codec_cbk(uint16_t dev_id, uint8_t a2dp_role, bs_br_media_codec_info_t *codec);
void ux_bt_a2dp_src_connect_state_cbk(ux_bt_a2dp_info_t *profile_state_package);
void ux_bt_a2dp_src_stream_state_cbk(ux_bt_a2dp_info_t *a2dp_stream_state_package);

#if defined(__cplusplus) && __cplusplus
}
#endif /* __cplusplus */
#endif