/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 *
 * Description:
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-09-11， Create file. \n
 */
#include "bts_def.h"
#include "ux_errcode.h"
#include "ux_event_define.h"
#include "bs_device_mgr.h"
#include "bs_br_call_sco.h"
#include "adapter_bt_br.h"
#include "ux_common_log.h"
#include "ux_bt_call_hf.h"
#include "ux_bt_audio.h"
#include "ux_bt_hal_sco.h"
static ux_api_bt_stream_state_t ux_bt_convert_call_sco_state_from_bth_to_ux(bs_br_sco_state_t sco_state)
{
    switch (sco_state) {
        case BR_SCO_STATE_CONNECTING:
            return UX_API_BT_STREAM_STATE_STARTING;
        case BR_SCO_STATE_CONNECTED:
            return UX_API_BT_STREAM_STATE_STREAMING;
        case BR_SCO_STATE_DISCONNECTED:
            return UX_API_BT_STREAM_STATE_STOPPED;
        default:
            return UX_API_BT_STREAM_STATE_MAX;
    }
}
ux_bt_hal_sco_callbacks_t *g_sco_notice = NULL;

static void normalize_sco_state_cb(uint16_t dev_id, uint8_t hfp_role, uint8_t sco_state)
{
    ux_log_info("[ux][normalize_sco_state_cb] dev_id=%d, role=%d, state=%d.\n", dev_id, hfp_role, sco_state);
    if (g_sco_notice == NULL) {
        return;
    }

    ux_api_bt_stream_state_t hal_sco_state = ux_bt_convert_call_sco_state_from_bth_to_ux(sco_state);
    ux_bt_hal_hfp_role_t hal_hfp_role = hfp_role == BR_CALL_HFP_HF ? UX_BT_HAL_HFP_ROLE_HF : UX_BT_HAL_HFP_ROLE_AG;
    ux_api_bt_addr_t *addr = (ux_api_bt_addr_t *)bs_get_addr_by_device_id((uint8_t)dev_id);

    if (addr == NULL) {
        ux_log_err("[normalize_sco_state_cb] Fail to get addr");
        return;
    }
    if (g_sco_notice == NULL || g_sco_notice->ux_bt_hal_sco_state_cbk == NULL) {
        return;
    }
    /* 根据不同的sco状态 ：BR_SCO_STATE_CONNECTING， BR_SCO_STATE_CONNECTED， BR_SCO_STATE_DISCONNECTED，进行不同处理 */
    g_sco_notice->ux_bt_hal_sco_state_cbk(addr, hal_hfp_role, hal_sco_state);
    /* sco链路状态变化进一步通知上层app */
    ux_adapter_bt_sco_state_process((bd_addr_t *)addr, (uint8_t)sco_state);
}

static void normalize_sco_codec_cb(uint16_t dev_id, uint8_t hfp_role, uint8_t codec_type)
{
    ux_log_info("[ux][normalize_sco_codec_cb] dev_id=%d, role=%d, codec=%d.\n", dev_id, hfp_role, codec_type);
    if (g_sco_notice == NULL) {
        return;
    }
    if (hfp_role >= UX_BT_HAL_HFP_ROLE_MAX || hfp_role < UX_BT_HAL_HFP_ROLE_HF) {
        return;
    }

    ux_audio_sco_codec hal_sco_codec = {0};
    switch ((bs_br_sco_codec_id_t)codec_type) {
        case BR_SCO_CODEC_ID_CVSD:
            hal_sco_codec.codec = PCM;
            hal_sco_codec.sample_rate = SAMPLE_RATE_8K;
            break;
        case BR_SCO_CODEC_ID_MSBC:
            hal_sco_codec.codec = mSBC;
            hal_sco_codec.sample_rate = SAMPLE_RATE_16K;
            break;
        default:
            return;
    }
    hal_sco_codec.bit_width = BIT_WIDTH_16;
    ux_bt_hal_hfp_role_t hal_hfp_role = hfp_role == BR_CALL_HFP_HF ? UX_BT_HAL_HFP_ROLE_HF : UX_BT_HAL_HFP_ROLE_AG;
    bs_addr_t *addr = bs_get_addr_by_device_id((uint8_t)dev_id);
    if (addr == NULL) {
        ux_log_err("[normalize_sco_codec_cb] Fail to get addr");
        return;
    }
    if (g_sco_notice == NULL || g_sco_notice->ux_bt_hal_sco_codec_cbk == NULL) {
        return;
    }
    g_sco_notice->ux_bt_hal_sco_codec_cbk((ux_api_bt_addr_t *)addr, hal_hfp_role, &hal_sco_codec);
}

errcode_t ux_bt_hal_sco_register_callbacks(uint8_t app_id, const ux_bt_hal_sco_callbacks_t *func)
{
    if (func == NULL) {
        return UX_ERR_NULL_PTR;
    }
    g_sco_notice = (ux_bt_hal_sco_callbacks_t *)func;

    /* 不管22还是72都可以用如下额回调注册，具体在normalize_sco_state_cb中在体现差异 */
    static bs_br_sco_callbacks_t sco_cb = {
        .sco_state_cbk = normalize_sco_state_cb,
        .sco_codec_cbk = normalize_sco_codec_cb,
    };
    return bs_br_sco_register_callbacks(app_id, &sco_cb);
}

errcode_t ux_bt_hal_sco_unregister_callbacks(uint8_t app_id)
{
    g_sco_notice = NULL;
    return bs_br_sco_unregister_callbacks(app_id);
}

errcode_t ux_bt_hal_sco_create_connection(const ux_api_bt_addr_t *addr)
{
    if (addr == NULL) {
        return UX_ERR_NULL_PTR;
    }
    uint16_t dev_id = bs_find_device_id_by_addr(BS_PROTOCOL_BREDR, (uint8_t)addr->type, addr->addr);
    ux_bt_call_info_t *call_info = ux_bt_call_get_info();
    return ux_bt_stream_fsm_excute(UX_API_BT_PROFILE_SCO, dev_id,
        UX_BT_STREAM_FSM_INPUT_START, &(call_info->sco_state));
}

errcode_t ux_bt_hal_sco_release_connection(const ux_api_bt_addr_t *addr)
{
    if (addr == NULL) {
        return UX_ERR_NULL_PTR;
    }
    uint16_t dev_id = bs_find_device_id_by_addr(BS_PROTOCOL_BREDR, (uint8_t)addr->type, addr->addr);
    ux_bt_call_info_t *call_info = ux_bt_call_get_info();
    return ux_bt_stream_fsm_excute(UX_API_BT_PROFILE_SCO, dev_id,
        UX_BT_STREAM_FSM_INPUT_STOP, &(call_info->sco_state));
}

errcode_t ux_bt_hal_sco_reply_request(const ux_api_bt_addr_t *addr, bool is_accept)
{
    if (addr == NULL) {
        return UX_ERR_NULL_PTR;
    }
    uint16_t dev_id = bs_find_device_id_by_addr(BS_PROTOCOL_BREDR, (uint8_t)addr->type, addr->addr);
    return bs_br_create_sco_result_reply(dev_id, is_accept);
}
