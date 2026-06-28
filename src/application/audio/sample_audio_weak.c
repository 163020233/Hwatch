/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2021. All rights reserved.
 * Description: source file for module operation
 */

#include "td_type.h"

#define define_sample_audio_weak_function(function)                      \
    td_void __attribute__((weak)) function(td_s32 argc, td_char *argv[]) \
    {                                                                    \
        (td_void)(argc);                                                 \
        (td_void)(argv);                                                 \
    }

define_sample_audio_weak_function(sample_audio_w4)
define_sample_audio_weak_function(sample_audio_mem32)
define_sample_audio_weak_function(sample_set_config)
define_sample_audio_weak_function(sample_ai)
define_sample_audio_weak_function(sample_asr)
define_sample_audio_weak_function(sample_vad_feed)
define_sample_audio_weak_function(sample_ao)
define_sample_audio_weak_function(sample_proc)
define_sample_audio_weak_function(sample_encode)
define_sample_audio_weak_function(sample_cast_aenc_play)
define_sample_audio_weak_function(sample_encode_play)
define_sample_audio_weak_function(sample_adec)
define_sample_audio_weak_function(sample_decode)
define_sample_audio_weak_function(sample_ai_aenc)
define_sample_audio_weak_function(sample_ai_sea_aenc)
define_sample_audio_weak_function(sample_ai_ao)
define_sample_audio_weak_function(sample_ai_sea_ao)
define_sample_audio_weak_function(sample_sea)
define_sample_audio_weak_function(sample_sea_only)
define_sample_audio_weak_function(sample_gui_aef)
define_sample_audio_weak_function(sample_lpwk)
define_sample_audio_weak_function(sample_dump)
define_sample_audio_weak_function(sample_phone_application)
define_sample_audio_weak_function(sample_aef)
define_sample_audio_weak_function(sample_effect)
define_sample_audio_weak_function(sample_test_int)
define_sample_audio_weak_function(sample_haid)
define_sample_audio_weak_function(sample_bmic)
define_sample_audio_weak_function(sample_dpm)
define_sample_audio_weak_function(sample_aha)
define_sample_audio_weak_function(sample_sle_music)
define_sample_audio_weak_function(sample_tts)
define_sample_audio_weak_function(sample_rms)
define_sample_audio_weak_function(sample_gui_aef_add_handle)
define_sample_audio_weak_function(sample_gui_aef_delete_handle)