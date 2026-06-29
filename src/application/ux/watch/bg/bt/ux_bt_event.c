/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * File          ux_bt_event.c
 * Description:  ux bt state report event callback impletement
 */

#include <stdint.h>
#include <securec.h>
#include "errcode.h"
#include "ux_bt_hal_hfp.h"
#include "ux_bt_hal_sco.h"
#include "securec.h"
#include "ux_api_fwk.h"
#include "ux_common_log.h"
#ifdef DEMO_WATCH
#include "ux_event_def.h"
#endif
#include "ux_bt_hfp_hf_basic_event.h"
#include "ux_bt_hfp_hf_event.h"
/* 拟挪至ux_bt_hal_sco.c内 */
void ux_bt_hfp_hf_sco_state_cbk(const ux_api_bt_addr_t *addr, ux_bt_hal_hfp_role_t hfp_role,
    ux_api_bt_stream_state_t sco_state)
{
    if (addr == NULL) {
        return;
    }
    ux_bt_call_sco_state_cb_packet_t sco_state_info = {
        .sco_state = sco_state,
        .role = hfp_role,
    };

    errno_t err = memcpy_s(&sco_state_info.addr, sizeof(sco_state_info.addr), addr, sizeof(ux_api_bt_addr_t));
    if (err != EOK) {
        ux_log_err("[ux][bt][ux_bt_hfp_hf_sco_state_cbk]Fail to memcpy addr 0x%x.", err);
        return;
    }
    ux_api_fwk_push_event(UX_EVENT_TYPE_BG, UX_BG_SCO_STATE_CHANGE, (uint8_t*)&sco_state_info, sizeof(sco_state_info));
}

/* 拟发送至单独线程处理，避免用锁和爆栈 */
/* 拟挪至ux_bt_hal_sco.c内 */
void ux_bt_hfp_hf_sco_codec_cbk(const ux_api_bt_addr_t *addr, ux_bt_hal_hfp_role_t hfp_role,
    const ux_audio_sco_codec *codec_cfg)
{
    if (addr == NULL) {
        return;
    }
    ux_bt_call_sco_codec_cb_packet_t sco_codec_info = {
        .role = hfp_role,
    };
    errno_t err = memcpy_s(&sco_codec_info.addr, sizeof(sco_codec_info.addr), addr, sizeof(ux_api_bt_addr_t));
    if (err != EOK) {
        ux_log_err("[ux_bt_hfp_hf_sco_codec_cbk]Fail to memcpy addr 0x%x.", err);
        return;
    }
    err = memcpy_s(&sco_codec_info.sco_codec, sizeof(sco_codec_info.sco_codec), codec_cfg, sizeof(ux_audio_sco_codec));
    if (err != EOK) {
        ux_log_err("[ux_bt_hfp_hf_sco_codec_cbk]Fail to save codec 0x%x.", err);
        return;
    }
    ux_api_fwk_push_event(UX_EVENT_TYPE_BG, UX_BG_SCO_CODEC_CHANGE, (uint8_t*)&sco_codec_info, sizeof(sco_codec_info));
}

/* 拟挪至ux_bt_hal_hfp_hf.c内 */
void ux_bt_hfp_hf_cat1_state_cbk(const ux_api_bt_addr_t *addr, const ux_bt_hal_call_cat1_info_t *cat1_info)
{
    ux_bt_call_cat1_state_packet_t cat1_packet = {0};
    errno_t err = EOK;
    err = memcpy_s(&cat1_packet.addr, sizeof(cat1_packet.addr), addr, sizeof(ux_api_bt_addr_t));
    if (err != EOK) {
        ux_log_err("[ux_bt_hfp_hf_cat1_state_cbk] Fail to memcpy addr 0x%x.", err);
        return;
    }
    size_t cat1_info_len = sizeof(cat1_packet.cat1_info);
    err = memcpy_s(&cat1_packet.cat1_info, cat1_info_len, cat1_info, cat1_info_len);
    if (err != EOK) {
        ux_log_err("[ux_bt_hfp_hf_cat1_state_cbk] Fail to memcpy addr 0x%x.", err);
        return;
    }
    ux_api_fwk_push_event(UX_EVENT_TYPE_BG, UX_BG_CALL_CAT1_CHANGE, (uint8_t*)&cat1_packet, sizeof(cat1_packet));
}

void ux_bt_hfp_hf_set_volume_request_cbk(const ux_api_bt_addr_t *addr, const ux_bt_hal_volume_info_t *volume_info)
{
    ux_bt_call_volume_packet_t volume_packet = {0};
    errno_t err = EOK;
    err = memcpy_s(&volume_packet.addr, sizeof(volume_packet.addr), addr, sizeof(ux_api_bt_addr_t));
    if (err != EOK) {
        ux_log_err("[ux_bt_hfp_hf_set_volume_request_cbk] Fail to memcpy addr 0x%x.", err);
        return;
    }
    size_t volume_info_len = sizeof(volume_packet.volume_info);
    err = memcpy_s(&volume_packet.volume_info, volume_info_len, volume_info, volume_info_len);
    if (err != EOK) {
        ux_log_err("[ux_bt_hfp_hf_set_volume_request_cbk] Fail to memcpy addr 0x%x.", err);
        return;
    }
    ux_api_fwk_push_event(UX_EVENT_TYPE_BG, UX_BG_CALL_VOLUME_CHANGE,
        (uint8_t*)&volume_packet, sizeof(ux_bt_call_volume_packet_t));
}