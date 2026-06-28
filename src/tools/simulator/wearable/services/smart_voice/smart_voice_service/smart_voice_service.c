/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021. All rights reserved.
 * Description: smart voice service.
 */
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include "securec.h"
#include "soc_errno.h"
#include "audio_base_type.h"
#include "voice_engine_hal.h"
#include "wearable_log.h"
#include "smart_voice_service.h"

static AsrWorkMode g_work_mode = ALWAYS_ON;

static td_void dump_config(td_void)
{
}

static td_void dump_statis(td_void)
{
}

static td_void dump(td_void)
{
}

static td_void reset_dump_statis(td_void)
{
}

static SmartVoiceCmdId find_vcmd_id(td_uchar *cmd, td_u32 len)
{
    return SMART_VOICE_INVALID_CMD_ID;
}

static td_void update_state_machine(SmartVoiceEventType event)
{
}

static td_void v_callback(voice_engine_event_type type, td_void *data, td_u32 size)
{
}

static td_void audio_interrupt_callback(td_s32 type, td_s32 hint)
{
}

static td_s32 release_audio_stream()
{
    return EXT_SUCCESS;
}

static td_s32 get_audio_stream_channel_id(td_u32 *chan_id)
{
    return 0;
}

static td_s32 create_asr_inner(td_void)
{
    return 0;
}

static td_s32 start_asr_inner(td_void)
{
    return 0;
}

static td_s32 stop_asr_inner(td_void)
{
    return EXT_SUCCESS;
}

static td_s32 destroy_asr_inner(td_void)
{
    return EXT_SUCCESS;
}

static td_s32 state_vad_to_asr(td_void)
{
    return 0;
}

static td_s32 state_asr_to_vad(td_void)
{
    return EXT_SUCCESS;
}

static td_void state_machine_task(td_void *data)
{
}

static td_s32 start_state_machine(td_void)
{
    return EXT_SUCCESS;
}

static td_s32 stop_state_machine(td_void)
{
    return 0;
}

td_s32 LocalAsrInit(LocalAsrInitParams *initParams)
{
    return 0;
}

td_s32 LocalAsrDeInit(td_void)
{
    return EXT_SUCCESS;
}

td_s32 LocalAsrSetMode(AsrWorkMode workMode)
{
    return 0;
}

AsrWorkMode LocalAsrGetMode(td_void)
{
    return g_work_mode;
}

td_s32 LocalAsrStart(td_void)
{
    return 0;
}

td_s32 LocalAsrStop(td_void)
{
    return EXT_SUCCESS;
}

td_u32 LocalAsrReadData(td_uchar *voiceData, td_u32 length)
{
    return 0;
}

td_u32 LocalAsrReadRawData(td_uchar *voiceData, td_u32 length)
{
    return 0;
}

td_u32 LocalAsrWriteData(td_uchar *voiceData, td_u32 length)
{
    return 0;
}

td_s32 LocalAsrSetCallback(LocalAsrCb callback)
{
    return EXT_SUCCESS;
}

td_float LocalAsrCalcRms(td_uchar *voiceData, td_u32 length)
{
    return 0.0;
}

td_void LocalAsrDump(td_void)
{
}

td_void LocalAsrSetHifiMode(td_void)
{
}