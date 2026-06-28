/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-10-10， Create file. \n
 */
#include "common_def.h"
#include "ux_device_manager.h"
#include "ux_bg_config.h"
#ifdef CONFIG_SUPPORT_A2DP_SRC
#include "ux_bt_a2dp_src.h"
#endif
#ifdef CONFIG_SUPPORT_A2DP_SINK
#include "ux_bt_a2dp_sink.h"
#endif
#include "ux_common_log.h"
#include "bs_br_media_stream.h"
#include "bs_br_call.h"
#include "ux_api_bt.h"
#include "ux_bt.h"
#include "ux_event_define.h"
#include "ux_bt_audio.h"
#include "ux_bt_a2dp.h"

#define BTH_IDX 0
#define UX_IDX  1

static uint8_t g_mapping_ux_a2dp_profile_state_to_bth[UX_API_BT_PROFILE_STATE_MAX] = {
    [UX_API_BT_PROFILE_STATE_CONNECTING]    = BR_PROFILE_STATE_CONNECTING,
    [UX_API_BT_PROFILE_STATE_CONNECTED]     = BR_PROFILE_STATE_CONNECTED,
    [UX_API_BT_PROFILE_STATE_DISCONNECTING] = BR_PROFILE_STATE_DISCONNECTING,
    [UX_API_BT_PROFILE_STATE_DISCONNECTED]  = BR_PROFILE_STATE_DISCONNECTED,
};

static ux_api_bt_stream_state_t stream_state_from_bth_to_ux(bs_br_media_stream_state_t state)
{
    switch (state) {
        case BR_MEDIA_STREAM_STARTING:
            return UX_API_BT_STREAM_STATE_STARTING;
        case BR_MEDIA_STREAM_STREAMING:
            return UX_API_BT_STREAM_STATE_STREAMING;
        case BR_MEDIA_STREAM_STOPED:
            return UX_API_BT_STREAM_STATE_STOPPED;
        case BR_MEDIA_STREAM_CLOSED:
            return UX_API_BT_STREAM_STATE_STOPPED;
        default:
            return UX_API_BT_STREAM_STATE_MAX;
            break;
    }
}

static void process_a2dp_connect_state_cb(uint16_t dev_id, uint8_t a2dp_role, uint8_t conn_state)
{
    ux_log_info("[ux][bt][process_a2dp_connect_state_cb] dev_id[%d] a2dp_role{%d], conn_state[%d]}.\n",
        dev_id, a2dp_role, conn_state);
    if (a2dp_role > BR_MEDIA_A2DP_SRC || a2dp_role < BR_MEDIA_A2DP_SNK) {
        ux_log_err("[process_a2dp_connect_state_cb] Role err.\n");
        return;
    }
    if (conn_state > BR_PROFILE_STATE_DISCONNECTED || conn_state < BR_PROFILE_STATE_CONNECTING) {
        ux_log_err("[process_a2dp_connect_state_cb] Ivalid state.\n");
        return;
    }

    ux_bt_a2dp_info_t a2dp_profile_package = {
        .device_id = dev_id,
        .a2dp_role = a2dp_role == BR_MEDIA_A2DP_SRC ? UX_BT_A2DP_SRC : UX_BT_A2DP_SNK,
    };
    for (uint8_t ux_state = 0; ux_state < arry_num(g_mapping_ux_a2dp_profile_state_to_bth); ux_state++) {
        if (g_mapping_ux_a2dp_profile_state_to_bth[ux_state] == conn_state) {
            a2dp_profile_package.profile_state = ux_state;
            break;
        }
    }
#ifdef CONFIG_SUPPORT_A2DP_SRC
    if (a2dp_profile_package.a2dp_role == UX_BT_A2DP_SRC) {
        ux_bt_a2dp_src_connect_state_cbk(&a2dp_profile_package);
    }
#endif

#ifdef CONFIG_SUPPORT_A2DP_SINK
    if (a2dp_profile_package.a2dp_role == UX_BT_A2DP_SNK) {
        ux_bt_a2dp_sink_connect_state_cbk(&a2dp_profile_package);
    }
#endif
}

static void process_a2dp_codec_cb(uint16_t dev_id, uint8_t a2dp_role, bs_br_media_codec_info_t *codec)
{
    if (a2dp_role > BR_MEDIA_A2DP_SRC || a2dp_role < BR_MEDIA_A2DP_SNK) {
        ux_log_err("[process_a2dp_codec_cb] Role err.\n");
        return;
    }
    if (codec == NULL) {
        ux_log_err("[process_a2dp_codec_cb] Nullptr.\n");
        return;
    }
    if (codec->codec_type < BR_MEDIA_STREAM_CODEC_SBC || codec->codec_type > BR_MEDIA_STREAM_CODEC_AAC) {
        ux_log_err("[process_a2dp_codec_cb] Codec err.\n");
        return;
    }

#ifdef CONFIG_SUPPORT_A2DP_SRC
    if (a2dp_role == BR_MEDIA_A2DP_SRC) {
        ux_bt_a2dp_src_codec_cbk(dev_id, a2dp_role, codec);
    }
#endif

#ifdef CONFIG_SUPPORT_A2DP_SINK
    if (a2dp_role == BR_MEDIA_A2DP_SNK) {
        ux_bt_a2dp_sink_codec_cbk(dev_id, a2dp_role, codec);
    }
#endif
}

static void process_a2dp_stream_state_change_cb(uint16_t dev_id, uint8_t a2dp_role, uint8_t stream_state)
{
    ux_log_info("[ux][bt][process_a2dp_stream_state_change_cb] a2dp_role{%d], stream_state[%d]}.\n",
        a2dp_role, stream_state);
    if (a2dp_role > BR_MEDIA_A2DP_SRC || a2dp_role < BR_MEDIA_A2DP_SNK) {
        ux_log_err("[process_a2dp_stream_state_change_cb] Role err.\n");
        return;
    }

    if (stream_state > BR_MEDIA_STREAM_CLOSED || stream_state < BR_MEDIA_STREAM_STARTING) {
        ux_log_err("[process_a2dp_stream_state_change_cb] Invalid state.\n");
        return;
    }

    ux_bt_a2dp_info_t a2dp_stream_state_package = {0};
    a2dp_stream_state_package.device_id = dev_id;
    a2dp_stream_state_package.a2dp_role = a2dp_role == BR_MEDIA_A2DP_SRC ? UX_BT_A2DP_SRC : UX_BT_A2DP_SNK;

    a2dp_stream_state_package.stream_state = stream_state_from_bth_to_ux(stream_state);
#ifdef CONFIG_SUPPORT_A2DP_SRC
    if (a2dp_stream_state_package.a2dp_role == UX_BT_A2DP_SRC) {
        ux_bt_a2dp_src_stream_state_cbk(&a2dp_stream_state_package);
    }
#endif

#ifdef CONFIG_SUPPORT_A2DP_SINK
    if (a2dp_stream_state_package.a2dp_role == UX_BT_A2DP_SNK) {
        ux_bt_a2dp_sink_stream_state_cbk(&a2dp_stream_state_package);
    }
#endif
}

bs_br_media_stream_callbacks_t g_a2dp_cb = {
    .connect_state_cbk = process_a2dp_connect_state_cb,
    .stream_codec_cbk  = process_a2dp_codec_cb,
    .stream_state_cbk  = process_a2dp_stream_state_change_cb,
};

errcode_t ux_bt_a2dp_init(void)
{
    errcode_t err = UX_ERR_SUCCESS;
    err = bs_br_media_stream_register_callbacks(0, &g_a2dp_cb);
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[ux][bt][ux_bt_a2dp_init] Fail to register 0x%x.", err);
        return UX_ERR_FAIL_TO_REGISTER;
    }
    return UX_ERR_SUCCESS;
}

errcode_t ux_bt_a2dp_deinit(void)
{
    errcode_t err = UX_ERR_SUCCESS;
    err = bs_br_media_stream_unregister_callbacks(0);
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[ux][bt][ux_bt_a2dp_deinit] Fail to register 0x%x.", err);
    }
#ifdef CONFIG_SUPPORT_A2DP_SRC
    ux_bt_a2dp_info_t *src_info = ux_bt_a2dp_get_src_info();
    if (src_info->profile_state == UX_API_BT_PROFILE_STATE_CONNECTED) {
        process_a2dp_connect_state_cb(src_info->device_id, BR_MEDIA_A2DP_SRC, BR_PROFILE_STATE_DISCONNECTED);
    }
#endif
#ifdef CONFIG_SUPPORT_A2DP_SRC
    ux_bt_a2dp_info_t *snk_info = ux_bt_a2dp_get_sink_info();
    if (snk_info->profile_state == UX_API_BT_PROFILE_STATE_CONNECTED) {
        process_a2dp_connect_state_cb(snk_info->device_id, BR_MEDIA_A2DP_SNK, BR_PROFILE_STATE_DISCONNECTED);
    }
#endif
    return UX_ERR_SUCCESS;
}

errcode_t ux_bt_a2dp_connect(ux_api_bt_addr_t *addr, uint8_t len)
{
    if (addr == NULL || len != sizeof(ux_api_bt_addr_t)) {
        ux_log_err("[ux][bt][ux_bt_a2dp_connect] invalid param.\n");
        return UX_ERR_INVALID_PARAMETER;
    }
    ux_bg_device_info_t *find_result = NULL;
    errcode_t err = UX_ERR_SUCCESS;
    ux_bt_a2dp_info_t *a2dp_info = NULL;
    find_result = ux_bg_find_device_info_by_addr(ux_bg_get_paired_device_list(), addr, UX_API_BT_PROTOCOL_BR);
    if (find_result == NULL) {
        return UX_ERR_BG_DEVICE_DOES_NOT_EXIST;
    }
    if (find_result->connect_state != UX_API_BT_PROFILE_STATE_CONNECTED) {
        return UX_ERR_BG_ACL_DISCONNECT;
    }
    /**
     * 若设备为外围设备，一般为sink端
     * 若设备为中心设备，一般为src端
     * 未见特例
     */
    if (find_result->role == UX_API_BT_AS_PERIPHERAL) {
        a2dp_info = ux_bt_a2dp_get_src_info();
        a2dp_info->a2dp_role = UX_BT_A2DP_SRC;
    } else if (find_result->role == UX_API_BT_AS_CENTRAL) {
        a2dp_info = ux_bt_a2dp_get_sink_info();
        a2dp_info->a2dp_role = UX_BT_A2DP_SNK;
    }

    if (a2dp_info->profile_state == UX_API_BT_PROFILE_STATE_CONNECTING ||
        a2dp_info->profile_state == UX_API_BT_PROFILE_STATE_CONNECTED) {
        return UX_ERR_SUCCESS;
    }
    a2dp_info->device_id = find_result->id;
    a2dp_info->profile_state = UX_API_BT_PROFILE_STATE_CONNECTING;
    a2dp_info->stream_state = UX_API_BT_STREAM_STATE_STOPPED;
    if (a2dp_info->a2dp_role == UX_BT_A2DP_SNK) {
        err = bs_br_media_a2dp_connect(a2dp_info->device_id, BR_MEDIA_A2DP_SNK);
    } else if (a2dp_info->a2dp_role == UX_BT_A2DP_SRC) {
        err = bs_br_media_a2dp_connect(a2dp_info->device_id, BR_MEDIA_A2DP_SRC);
    }
    // 失败，状态回滚
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[ux][bt][ux_bt_a2dp_connect] Fail to connect a2dp 0x%x.", err);
        a2dp_info->profile_state = UX_API_BT_PROFILE_STATE_DISCONNECTED;
        return err;
    }
    ux_log_info("[ux][bt][ux_bt_a2dp_connect] a2dp conn success addr= ****%02x%02x%02x%02x.",
        addr->addr[BG_ADDR_INDEX_3], addr->addr[BG_ADDR_INDEX_2],
        addr->addr[BG_ADDR_INDEX_1], addr->addr[BG_ADDR_INDEX_0]);
    return err;
}

errcode_t ux_bt_a2dp_disconnect(ux_api_bt_addr_t *addr, uint8_t len)
{
    ux_bg_device_info_t *find_result = NULL;
    errcode_t err = UX_ERR_SUCCESS;
    ux_bt_a2dp_info_t *a2dp_info = NULL;
    if (addr == NULL || len != sizeof(ux_api_bt_addr_t)) {
        ux_log_err("[ux][bt][ux_bt_a2dp_disconnect] invalid param.\n");
        return UX_ERR_INVALID_PARAMETER;
    }
    find_result = ux_bg_find_device_info_by_addr(ux_bg_get_paired_device_list(), addr, UX_API_BT_PROTOCOL_BR);
    if (find_result == NULL) {
        ux_log_err("[ux][bt][ux_bt_a2dp_disconnect] ux_bg_find_device_info_by_addr null.\n");
        return UX_ERR_BG_DEVICE_DOES_NOT_EXIST;
    }
    /**
     * 若设备为外围设备，一般为sink端
     * 若设备为中心设备，一般为src端
     * 未见特例
     */
    if (find_result->role == UX_API_BT_AS_PERIPHERAL) {
        a2dp_info = ux_bt_a2dp_get_src_info();
    } else if (find_result->role == UX_API_BT_AS_CENTRAL) {
        a2dp_info = ux_bt_a2dp_get_sink_info();
    }

    if (a2dp_info->profile_state == UX_API_BT_PROFILE_STATE_DISCONNECTING ||
        a2dp_info->profile_state == UX_API_BT_PROFILE_STATE_DISCONNECTED) {
        return UX_ERR_SUCCESS;
    }
    a2dp_info->profile_state = UX_API_BT_PROFILE_STATE_DISCONNECTING;
    a2dp_info->stream_state = UX_API_BT_STREAM_STATE_STOPPED;
    if (a2dp_info->a2dp_role == UX_BT_A2DP_SNK) {
        err = bs_br_media_a2dp_disconnect(a2dp_info->device_id, BR_MEDIA_A2DP_SNK);
    } else if (a2dp_info->a2dp_role == UX_BT_A2DP_SRC) {
        err = bs_br_media_a2dp_disconnect(a2dp_info->device_id, BR_MEDIA_A2DP_SRC);
    }
    // 失败，状态回滚
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[ux][bt][ux_bt_a2dp_disconnect] Fail to connect a2dp 0x%x.", err);
        a2dp_info->profile_state = UX_API_BT_PROFILE_STATE_CONNECTED;
        return UX_ERR_BG_FAIL_TO_CONNECT;
    }
    ux_log_info("[ux][bt][ux_bt_a2dp_disconnect] a2dp disconn success addr= ****%02x%02x%02x%02x.",
        addr->addr[BG_ADDR_INDEX_3], addr->addr[BG_ADDR_INDEX_2],
        addr->addr[BG_ADDR_INDEX_1], addr->addr[BG_ADDR_INDEX_0]);
    return err;
}

errcode_t ux_bt_hal_a2dp_req_reply(bool is_accept)
{
    errcode_t err = UX_ERR_SUCCESS;
    ux_bt_a2dp_info_t *a2dp_info = ux_bt_a2dp_get_sink_info();
    err = bs_br_media_stream_operate_result_reply(a2dp_info->device_id, BR_MEDIA_STREAM_OPT_START, is_accept);
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[ux_bt_hal_a2dp_req_reply] Fail to reply media stream operate 0x%x.\n", err);
        return UX_ERR_BG_FAIL_TO_REPLY;
    }
    return UX_ERR_SUCCESS;
}

errcode_t ux_bt_a2dp_start_stream(const ux_api_bt_addr_t *addr)
{
    if (addr == NULL) {
        ux_log_err("[ux][bt][ux_bt_a2dp_start_stream] nullptr.\n");
        return UX_ERR_NULL_PTR;
    }
    ux_log_info("[ux][bt][ux_bt_a2dp_start_stream] start stream, addr= ****%02x%02x%02x%02x.",
        addr->addr[BG_ADDR_INDEX_3], addr->addr[BG_ADDR_INDEX_2],
        addr->addr[BG_ADDR_INDEX_1], addr->addr[BG_ADDR_INDEX_0]);

    uint16_t device_id = bs_find_device_id_by_addr(BS_PROTOCOL_BREDR, 0, addr->addr);
    ux_bt_a2dp_info_t *a2dp_info = ux_bt_a2dp_get_sink_info();
    if (a2dp_info->device_id != device_id) {
        a2dp_info = ux_bt_a2dp_get_src_info();
        if (a2dp_info->device_id != device_id) {
            return UX_ERR_BG_DEVICE_DOES_NOT_EXIST;
        }
    }
    errcode_t err = ux_bt_stream_fsm_excute(UX_API_BT_PROFILE_A2DP, a2dp_info->device_id,
        UX_BT_STREAM_FSM_INPUT_START, &(a2dp_info->stream_state));
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[ux][bt][ux_bt_a2dp_start_stream] Fail to start media stream 0x%x.\n", err);
        return err;
    }

    return UX_ERR_SUCCESS;
}

errcode_t ux_bt_a2dp_stop_stream(const ux_api_bt_addr_t *addr)
{
    if (addr == NULL) {
        ux_log_err("[ux][bt][ux_bt_a2dp_stop_stream] nullptr.\n");
        return UX_ERR_NULL_PTR;
    }
    ux_log_info("[ux][bt][ux_bt_a2dp_stop_stream] stop stream, addr= ****%02x%02x%02x%02x.",
        addr->addr[BG_ADDR_INDEX_3], addr->addr[BG_ADDR_INDEX_2],
        addr->addr[BG_ADDR_INDEX_1], addr->addr[BG_ADDR_INDEX_0]);
    uint16_t device_id = bs_find_device_id_by_addr(BS_PROTOCOL_BREDR, 0, addr->addr);
    ux_bt_a2dp_info_t *a2dp_info = ux_bt_a2dp_get_sink_info();
    if (a2dp_info->device_id != device_id) {
        a2dp_info = ux_bt_a2dp_get_src_info();
        if (a2dp_info->device_id != device_id) {
            return UX_ERR_BG_DEVICE_DOES_NOT_EXIST;
        }
    }
    errcode_t err = ux_bt_stream_fsm_excute(UX_API_BT_PROFILE_A2DP, a2dp_info->device_id,
        UX_BT_STREAM_FSM_INPUT_STOP, &(a2dp_info->stream_state));
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[ux][bt][ux_bt_a2dp_stop_stream] Fail to start media stream 0x%x.\n", err);
        return err;
    }
    return UX_ERR_SUCCESS;
}

ux_api_bt_audio_info_t* ux_bt_a2dp_get_audio_info(const ux_api_bt_addr_t *addr)
{
    typedef ux_bt_a2dp_info_t* (*ux_bt_get_a2dp_info_handle)(void);
    static ux_api_bt_audio_info_t s_ux_a2dp_audio_info[UX_BT_A2DP_MAX] = {0};
    static ux_bt_get_a2dp_info_handle s_ux_bt_get_a2dp_info_handle_set[UX_BT_A2DP_MAX] = {
#ifdef CONFIG_SUPPORT_A2DP_SINK
        [UX_BT_A2DP_SNK] = ux_bt_a2dp_get_sink_info,
#endif
#ifdef CONFIG_SUPPORT_A2DP_SRC
        [UX_BT_A2DP_SRC] = ux_bt_a2dp_get_src_info,
#endif
    };
    errcode_t err = UX_ERR_SUCCESS;
    ux_bt_a2dp_info_t *a2dp_info = NULL;
    ux_api_bt_addr_t a2dp_addr = {0};
    for (uint8_t i = UX_BT_A2DP_SNK; i < UX_BT_A2DP_MAX; i++) {
        if (s_ux_bt_get_a2dp_info_handle_set[i] == NULL) {
            continue;
        }
        a2dp_info = s_ux_bt_get_a2dp_info_handle_set[i]();
        err = ux_bt_get_addr_by_device_id(a2dp_info->device_id, &a2dp_addr);
        if (err != UX_ERR_SUCCESS) {
            /* 不存在此设备, 查找下一个 */
            continue;
        }
        if (memcmp(&a2dp_addr, addr, sizeof(a2dp_addr)) != 0) {
            /* 地址不匹配, 查找下一个 */
            continue;
        }
        if (a2dp_info->a2dp_role >= UX_BT_A2DP_MAX) {
            /* role不合法, 查找下一个 */
            continue;
        }

        s_ux_a2dp_audio_info[a2dp_info->a2dp_role].profile_state = a2dp_info->profile_state;
        s_ux_a2dp_audio_info[a2dp_info->a2dp_role].stream_state = a2dp_info->stream_state;
        return &s_ux_a2dp_audio_info[a2dp_info->a2dp_role];
    }
    return NULL;
}
