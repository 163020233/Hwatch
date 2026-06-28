/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Register at commands \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-04-21， Create file. \n
 */
#include <string.h>
#include <stdlib.h>
#include "at.h"
#include "dut.h"
#include "ux_at_manager.h"
#include "ux_at_sle.h"
#include "ux_at_bs.h"
#include "ux_at_bt.h"
#include "ux_at_tws.h"
#include "ux_at_haid.h"
#include "ux_at_box.h"
#include "ux_at_test.h"
#include "ux_at_recovery.h"
#include "ux_at_player.h"

#include "ux_at_audio.h"
#include "ux_at_dut.h"
#include "ux_at_table.h"
#ifndef DEMO_WATCH
#include "feature_config.h"
#endif
#include "ux_at_system.h"
#ifdef CONFIG_SUPPORT_PBAP
#include "ux_at_pbap.h"
#endif
#ifdef FT_SUPPORT_SN
#include "ux_at_sn.h"
#endif

const at_para_parse_syntax_t g_ux_para1_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = UX_AT_PARA_MAX_LENGTH,
        .offset = offsetof(ux_at_para1_args_t, para1)
    },
};

const at_para_parse_syntax_t g_ux_para2_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = false,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = UX_AT_PARA_MAX_LENGTH,
        .offset = offsetof(ux_at_para2_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = UX_AT_PARA_MAX_LENGTH,
        .offset = offsetof(ux_at_para2_args_t, para2)
    },
};

const at_para_parse_syntax_t g_ux_para3_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = false,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = UX_AT_PARA_MAX_LENGTH,
        .offset = offsetof(ux_at_para3_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = false,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = UX_AT_PARA_MAX_LENGTH,
        .offset = offsetof(ux_at_para3_args_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = UX_AT_PARA_MAX_LENGTH,
        .offset = offsetof(ux_at_para3_args_t, para3)
    }
};

const at_para_parse_syntax_t g_ux_para4_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = false,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = UX_AT_PARA_MAX_LENGTH,
        .offset = offsetof(ux_at_para4_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = false,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = UX_AT_PARA_MAX_LENGTH,
        .offset = offsetof(ux_at_para4_args_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = false,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = UX_AT_PARA_MAX_LENGTH,
        .offset = offsetof(ux_at_para4_args_t, para3)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = UX_AT_PARA_MAX_LENGTH,
        .offset = offsetof(ux_at_para4_args_t, para4)
    }
};

const at_para_parse_syntax_t g_ux_para5_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = false,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = UX_AT_PARA_MAX_LENGTH,
        .offset = offsetof(ux_at_para5_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = false,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = UX_AT_PARA_MAX_LENGTH,
        .offset = offsetof(ux_at_para5_args_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = false,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = UX_AT_PARA_MAX_LENGTH,
        .offset = offsetof(ux_at_para5_args_t, para3)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = false,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = UX_AT_PARA_MAX_LENGTH,
        .offset = offsetof(ux_at_para5_args_t, para4)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = UX_AT_PARA_MAX_LENGTH,
        .offset = offsetof(ux_at_para5_args_t, para5)
    }
};

/*
 * ux at cmd table
*/
static const at_cmd_entry_t g_ux_atcmd_table[] = {
#ifdef CONFIG_SUPPORT_PBAP
    {
        "PBAP",
        UX_ATCMD_NUM_PBAP,
        DUT_CMD_LEVEL_WHITELIST,
        g_ux_para5_syntax,
        NULL,
        (at_set_func_t)at_ux_pbap_set_process,
        at_ux_pbap_read_process,
        NULL,
#ifdef CONFIG_AT_SUPPORT_QUERY
        NULL
#endif
    },
#endif
#if defined(CONFIG_SUPPORT_UX_TEST_AT) || !defined(DEMO_WATCH)
    {
        "UXTEST",
        UX_ATCMD_NUM_UXTEST,
        DUT_CMD_LEVEL_WHITELIST,
        g_ux_para5_syntax,
        NULL,
        (at_set_func_t)at_ux_test_set_process,
        at_ux_test_read_process,
        NULL,
#ifdef CONFIG_AT_SUPPORT_QUERY
        (at_query_func_t)at_ux_test_query_process
#endif
    },
#endif
    {
        "BTCFG",
        UX_ATCMD_NUM_BTCFG,
        DUT_CMD_LEVEL_WHITELIST,
#ifdef DEMO_WATCH
        g_ux_para5_syntax,
#else
        g_ux_para2_syntax,
#endif
        NULL,
        (at_set_func_t)at_btcfg_set_process,
#if defined(MELODY_DEMO_EARPHONE) || defined(MELODY_DEMO_SOUND_BOX) || \
defined(MELODY_DEMO_HA) || defined(DEMO_WATCH)
        at_btcfg_read_process,
#else
        NULL,
#endif
        NULL,
#ifdef CONFIG_AT_SUPPORT_QUERY
        (at_query_func_t)at_btcfg_query_process
#endif
    },
#if defined(MELODY_DEMO_EARPHONE) || defined(MELODY_DEMO_SOUND_BOX) || \
defined(MELODY_DEMO_HA) || defined(CONFIG_SUPPORT_HFP_HF)
    {
        "CALL",
        UX_ATCMD_NUM_CALL,
        DUT_CMD_LEVEL_WHITELIST,
        g_ux_para2_syntax,
        NULL,
        (at_set_func_t)at_call_set_process,
        at_call_read_process,
        NULL,
#ifdef CONFIG_AT_SUPPORT_QUERY
        NULL
#endif
    },
#endif
#if defined(MELODY_DEMO_EARPHONE) || defined(MELODY_DEMO_SOUND_BOX) || \
defined(MELODY_DEMO_HA) || defined(CONFIG_SUPPORT_AVRCP)
    {
        "MUSIC",
        UX_ATCMD_NUM_MUSIC,
        DUT_CMD_LEVEL_WHITELIST,
        g_ux_para1_syntax,
        NULL,
        (at_set_func_t)at_music_set_process,
        at_music_read_process,
        NULL,
#ifdef CONFIG_AT_SUPPORT_QUERY
        NULL
#endif
    },
#endif
    {
        "POWER",
        UX_ATCMD_NUM_POWER,
        DUT_CMD_LEVEL_WHITELIST,
        g_ux_para4_syntax,
        NULL,
        (at_set_func_t)at_power_set_process,
#ifdef DEMO_WATCH
        at_power_read_process,
#else
        NULL,
#endif
        NULL,
#ifdef CONFIG_AT_SUPPORT_QUERY
        (at_query_func_t)at_power_query_process
#endif
    },
#ifndef DEMO_WATCH
#ifdef CONFIG_SUPPORT_BUTTON
    {
        "BUTTON",
        UX_ATCMD_NUM_BUTTON,
        DUT_CMD_LEVEL_WHITELIST,
        g_ux_para3_syntax,
        NULL,
        (at_set_func_t)at_button_set_process,
        at_button_read_process,
        NULL,
        (at_query_func_t)at_button_query_process
    },
#endif
    {
        "BOX",
        UX_ATCMD_NUM_BOX,
        DUT_CMD_LEVEL_WHITELIST,
        g_ux_para2_syntax,
        NULL,
        (at_set_func_t)at_box_set_process,
        at_box_read_process,
        NULL,
        (at_query_func_t)at_box_query_process
    },
    {
        "PAIR",
        UX_ATCMD_NUM_PAIR,
        DUT_CMD_LEVEL_WHITELIST,
        g_ux_para2_syntax,
        NULL,
        (at_set_func_t)at_pair_set_process,
        at_pair_read_process,
        NULL,
        (at_query_func_t)at_pair_query_process
    },
    {
        "SLECFG",
        UX_ATCMD_NUM_SLECFG,
        DUT_CMD_LEVEL_WHITELIST,
        g_ux_para3_syntax,
        NULL,
        (at_set_func_t)at_sle_set_process,
        NULL,
        NULL,
        (at_query_func_t)at_sle_query_process
    },
    {
        "BSCFG",
        UX_ATCMD_NUM_BSCFG,
        DUT_CMD_LEVEL_WHITELIST,
        g_ux_para3_syntax,
        NULL,
        (at_set_func_t)at_bs_set_process,
#if defined(MELODY_DEMO_EARPHONE) || defined(MELODY_DEMO_SOUND_BOX) || defined(MELODY_DEMO_HA)
        at_bs_read_process,
#else
        NULL,
#endif
        NULL,
        (at_query_func_t)at_bs_query_process
    },
    {
        "AUDIOCFG",
        UX_ATCMD_NUM_AUDIOCFG,
        DUT_CMD_LEVEL_WHITELIST,
        g_ux_para4_syntax,
        NULL,
        (at_set_func_t)ux_at_audio_set_process,
        NULL,
        NULL,
        (at_query_func_t)ux_at_audio_query_process,
    },
    {
        "RECOVERY",
        UX_ATCMD_NUM_RECOVERY,
        DUT_CMD_LEVEL_WHITELIST,
        g_ux_para4_syntax,
        NULL,
        (at_set_func_t)at_recovery_set_process,
        NULL,
        NULL,
        NULL,
    },
    {
        "LED",
        UX_ATCMD_NUM_LED,
        DUT_CMD_LEVEL_WHITELIST,
        g_ux_para3_syntax,
        NULL,
        (at_set_func_t)at_ux_led_set_process,
        NULL,
        NULL,
        NULL
    },
    {
        "SYSTEM",
        UX_ATCMD_NUM_SYS,
        DUT_CMD_LEVEL_WHITELIST,
        g_ux_para2_syntax,
        NULL,
        (at_set_func_t)at_ux_sys_set_process,
        NULL,
        NULL,
        (at_query_func_t)at_ux_sys_get_process
    },
#if defined(MELODY_DEMO_EARPHONE) || defined(MELODY_DEMO_SOUND_BOX) || defined(MELODY_DEMO_HA)
    {
        "EARPHONE",
        UX_ATCMD_NUM_EARPHONE,
        DUT_CMD_LEVEL_WHITELIST,
        g_ux_para2_syntax,
        NULL,
        (at_set_func_t)at_earphone_set_process,
        at_earphone_read_process,
        NULL,
        (at_query_func_t)at_earphone_query_process
    },
    {
        "TONE",
        UX_ATCMD_NUM_TONE,
        DUT_CMD_LEVEL_WHITELIST,
        g_ux_para2_syntax,
        NULL,
        (at_set_func_t)at_tone_set_process,
        NULL,
        NULL,
        (at_query_func_t)at_tone_query_process
    },
    {
        "PLAYTONE",
        UX_ATCMD_NUM_PLAYTONE,
        DUT_CMD_LEVEL_WHITELIST,
        g_ux_para1_syntax,
        NULL,
        (at_set_func_t)at_ux_tone_set_process,
        NULL,
        NULL,
        NULL
    },
#if defined(MELODY_DEMO_SOUND_BOX)
    {
        "PLAYBACK",
        UX_ATCMD_NUM_PLAYBACK,
        DUT_CMD_LEVEL_WHITELIST,
        g_ux_para2_syntax,
        NULL,
        (at_set_func_t)at_playback_set_process,
        NULL,
        NULL,
        (at_query_func_t)at_playback_query_process
    },
#endif
    {
        "SOUND",
        UX_ATCMD_NUM_SOUND,
        DUT_CMD_LEVEL_WHITELIST,
        g_ux_para1_syntax,
        NULL,
        NULL,
        (at_read_func_t)at_sound_read_process,
        NULL,
        NULL
    },
    {
        "PROFILE",
        UX_ATCMD_NUM_PROFILE,
        DUT_CMD_LEVEL_WHITELIST,
        g_ux_para1_syntax,
        NULL,
        NULL,
        NULL,
        NULL,
        (at_query_func_t)at_profile_query_process
    },
    {
        "MUTE",
        UX_ATCMD_NUM_MUTE,
        DUT_CMD_LEVEL_WHITELIST,
        g_ux_para2_syntax,
        NULL,
        (at_set_func_t)at_mute_set_process,
        NULL,
        NULL,
        (at_query_func_t)at_mute_query_process
    },
#if defined(MELODY_DEMO_HA)
    {
        "HAID",
        UX_ATCMD_NUM_HAID,
        DUT_CMD_LEVEL_WHITELIST,
        g_ux_para2_syntax,
        NULL,
        (at_set_func_t)at_haid_set_process,
        at_haid_read_process,
        NULL,
        (at_query_func_t)at_haid_query_process
    },
#endif
#endif
    {
        "BTEN",
        UX_ATCMD_NUM_FTM_BT_ENABLE,
        DUT_CMD_LEVEL_WHITELIST,
        NULL,
        at_ftm_bt_enable_process,
        NULL,
        at_ftm_bt_read_process,
        NULL,
        NULL,
    },
#endif
    {
        "FTM",
        UX_ATCMD_NUM_FTM,
        DUT_CMD_LEVEL_WHITELIST,
        g_ux_para1_syntax,
        NULL,
        (at_set_func_t)at_dut_set_process,
        at_dut_read_process,
        NULL,
#ifdef CONFIG_AT_SUPPORT_QUERY
        NULL
#endif
    },
#if defined(MELODY_DEMO_EARPHONE)
    {
        "BTSIGNALENABLE",
        UX_ATCMD_NUM_BT_SIGNAL_ENABLE,
        DUT_CMD_LEVEL_WHITELIST,
        g_ux_para1_syntax,
        NULL,
        (at_set_func_t)at_dut_bt_signal_set_process,
        NULL,
        NULL,
#ifdef CONFIG_AT_SUPPORT_QUERY
        NULL
#endif
    },
#endif
#ifdef DEMO_WATCH
    {
        "SDKVERSION",
        UX_ATCMD_NUM_VERSION,
        DUT_CMD_LEVEL_WHITELIST,
        NULL,
        at_ux_get_sdk_version,
        NULL,
        NULL,
        NULL,
#ifdef CONFIG_AT_SUPPORT_QUERY
        NULL,
#endif
    },
    {
        "ALLVERSION",
        UX_ATCMD_NUM_VERSION,
        DUT_CMD_LEVEL_WHITELIST,
        NULL,
        at_ux_get_all_version,
        NULL,
        NULL,
        NULL,
#ifdef CONFIG_AT_SUPPORT_QUERY
        NULL,
#endif
    },
#endif
#ifdef FT_SUPPORT_SN
    {
        "SN",
        UX_ATCMD_NUM_SN,
        DUT_CMD_LEVEL_WHITELIST,
        g_ux_para1_syntax,
        NULL,
        (at_set_func_t)at_sn_set_process,
        at_sn_read_process,
        NULL,
#ifdef CONFIG_AT_SUPPORT_QUERY
        NULL
#endif
    },
#endif
};

errcode_t ux_at_cmd_init(void)
{
    return uapi_at_cmd_table_register(g_ux_atcmd_table,
        sizeof(g_ux_atcmd_table) / sizeof(at_cmd_entry_t), UX_AT_CMD_MAX);
}
