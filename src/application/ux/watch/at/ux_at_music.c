/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-10-10， Create file. \n
 */
#include "at.h"
#include "errcode.h"
#include "ux_api_fwk.h"
#include "debug_print.h"
#include "ux_api_bt.h"
#include "ux_bt_a2dp_src.h"
#include "ux_bt_a2dp_sink.h"
#include "ux_bt_avrcp.h"
#include "ux_event_define.h"
#ifdef DEMO_WATCH
#include "ux_event_def.h"
#endif
#include "ux_at_manager.h"

#define at_uart_printf PRINT /* 仅在此文件中重定向，后续3322引入at_dut后切换 */
typedef struct {
    const char *cmd_name;
    errcode_t (*func)(uint16_t device_id, uint8_t *data, uint16_t data_len);
} ux_at_music_cmd_t;

errcode_t ux_at_bt_play_music(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);
    unused(data);
    unused(data_len);
    return ux_api_fwk_push_event(UX_EVENT_TYPE_BG, UX_BG_MEDIA_PLAY, NULL, 0);
}

errcode_t ux_at_bt_pause_music(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);
    unused(data);
    unused(data_len);
    return ux_api_fwk_push_event(UX_EVENT_TYPE_BG, UX_BG_MEDIA_PAUSE, NULL, 0);
}

errcode_t ux_at_bt_music_prev(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);
    unused(data);
    unused(data_len);
    return ux_api_fwk_push_event(UX_EVENT_TYPE_BG, UX_BG_MEDIA_PLAY_PRE, NULL, 0);
}

errcode_t ux_at_bt_music_next(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);
    unused(data);
    unused(data_len);
    return ux_api_fwk_push_event(UX_EVENT_TYPE_BG, UX_BG_MEDIA_PLAY_NEXT, NULL, 0);
}

errcode_t ux_at_bt_music_volumeup(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);
    unused(data);
    unused(data_len);
    return ux_api_fwk_push_event(UX_EVENT_TYPE_BG, UX_BG_MEDIA_VOLUMEUP, NULL, 0);
}

errcode_t ux_at_bt_music_volumedown(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);
    unused(data);
    unused(data_len);
    return ux_api_fwk_push_event(UX_EVENT_TYPE_BG, UX_BG_MEDIA_VOLUMEDOWN, NULL, 0);
}

errcode_t ux_at_bt_music_mute(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);
    unused(data);
    unused(data_len);
    return ux_api_fwk_push_event(UX_EVENT_TYPE_BG, UX_BG_MEDIA_MUTE, NULL, 0);
}

errcode_t ux_at_bt_music_fast_forward(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);
    unused(data);
    unused(data_len);
    return ux_api_fwk_push_event(UX_EVENT_TYPE_BG, UX_BG_MEDIA_FAST_FORWARD, NULL, 0);
}

errcode_t ux_at_bt_start_src_stream(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);
    unused(data);
    unused(data_len);
    return ux_api_fwk_push_event(UX_EVENT_TYPE_BG, UX_BG_A2DP_SRC_START_STREAM, NULL, 0);
}

errcode_t ux_at_bt_stop_src_stream(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);
    unused(data);
    unused(data_len);
    return ux_api_fwk_push_event(UX_EVENT_TYPE_BG, UX_BG_A2DP_SRC_STOP_STREAM, NULL, 0);
}

at_ret_t at_music_set_process(const ux_at_para1_args_t *args)
{
    if (args == NULL) {
        return AT_RET_SYNTAX_ERROR;
    }

    ux_at_music_cmd_t cmd[] = {
        { "play",         ux_at_bt_play_music },
        { "pause",        ux_at_bt_pause_music },
        { "play-",        ux_at_bt_music_prev },
        { "play+",        ux_at_bt_music_next },
        { "volume+",      ux_at_bt_music_volumeup },
        { "volume-",      ux_at_bt_music_volumedown },
        { "mute",         ux_at_bt_music_mute },
        { "fast_forward", ux_at_bt_music_fast_forward },
        { "src_start",    ux_at_bt_start_src_stream },
        { "src_stop",     ux_at_bt_stop_src_stream },
    };

    for (uint32_t i = 0; i < (sizeof(cmd) / sizeof(ux_at_music_cmd_t)); i++) {
        if (strcasecmp(cmd[i].cmd_name, args->para1) == 0) {
            return cmd[i].func(0, NULL, 0);
        }
    }
    return AT_RET_SYNTAX_ERROR;
}

static void print_profile_state(ux_api_bt_profile_state_t profile_state)
{
    if (profile_state == UX_API_BT_PROFILE_STATE_CONNECTING) {
        at_uart_printf(" profile: connecting\n");
    } else if (profile_state == UX_API_BT_PROFILE_STATE_CONNECTED) {
        at_uart_printf(" profile: connected\n");
    } else if (profile_state == UX_API_BT_PROFILE_STATE_DISCONNECTING) {
        at_uart_printf(" profile: disconnecting\n");
    } else if (profile_state == UX_API_BT_PROFILE_STATE_DISCONNECTED) {
        at_uart_printf(" profile: disconnected\n");
    } else {
        at_uart_printf(" profile: unknow\n");
    }
}

static void print_stream_state(ux_api_bt_stream_state_t stream_state)
{
    if (stream_state == UX_API_BT_STREAM_STATE_STARTING) {
        at_uart_printf(" stream: starting\n");
    } else if (stream_state == UX_API_BT_STREAM_STATE_STREAMING) {
        at_uart_printf(" stream: working\n");
    } else if (stream_state == UX_API_BT_STREAM_STATE_STOPPED) {
        at_uart_printf(" stream: stop\n");
    } else if (stream_state == UX_API_BT_STREAM_STATE_STOPPING) {
        at_uart_printf(" stream: stopping\n");
    } else {
        at_uart_printf(" stream: unknow\n");
    }
}

static void print_play_state(ux_bt_media_play_state_t play_state)
{
    if (play_state == UX_BT_MEDIA_PLAY_STATE_STOPPED) {
        at_uart_printf(" player: stop\n");
    } else if (play_state == UX_BT_MEDIA_PLAY_STATE_PLAYING) {
        at_uart_printf(" player: playing\n");
    } else if (play_state == UX_BT_MEDIA_PLAY_STATE_PAUSED) {
        at_uart_printf(" player: pause\n");
    } else if (play_state == UX_BT_MEDIA_PLAY_STATE_SEEK_NEXT) {
        at_uart_printf(" player: seeking next\n");
    } else if (play_state == UX_BT_MEDIA_PLAY_STATE_SEEK_PRE) {
        at_uart_printf(" player: seeking pre\n");
    } else {
        at_uart_printf(" player: unknow\n");
    }
}

at_ret_t at_music_read_process(void)
{
    at_uart_printf(" ========= a2dp src ========= \r\n");
    ux_bt_a2dp_info_t *src_info = ux_bt_a2dp_get_src_info();
    at_uart_printf(" device: %hu\n", src_info->device_id);
    print_profile_state(src_info->profile_state);
    print_stream_state(src_info->stream_state);

    at_uart_printf(" ========= a2dp snk ========= \r\n");
    ux_bt_a2dp_info_t *sink_info = ux_bt_a2dp_get_sink_info();
    at_uart_printf(" device: %hu\n", sink_info->device_id);
    print_profile_state(sink_info->profile_state);
    print_stream_state(sink_info->stream_state);

    at_uart_printf(" ========== avrcp =========== \r\n");

    ux_bt_avrcp_info_t *avrcp_info = ux_bt_avrcp_get_info();
    for (uint8_t i = 0; i < BS_CONNECTION_UPPER_LIMIT; i++) {
        at_uart_printf(" device: %hu\n", avrcp_info[i].device_id);
        print_profile_state(avrcp_info[i].profile_state);
        print_play_state(avrcp_info[i].media_play_state);
        at_uart_printf(" volume: %hhu\n", avrcp_info[i].volume);
        if (avrcp_info[i].is_mute) {
            at_uart_printf(" mute: yes\n");
        } else {
            at_uart_printf(" mute: no\n");
        }
        at_uart_printf(" ---------------------------- \r\n");
    }
    at_uart_printf(" =========== end ============ \r\n");
    return AT_RET_OK;
}