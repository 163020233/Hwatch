#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
#===============================================================================

add_subdirectory_if_exist(intelligent_voice_service/asr_engine asr)
add_subdirectory_if_exist(intelligent_voice_service/tts_engine tts)
add_subdirectory_if_exist(intelligent_voice_service/common common)
add_subdirectory_if_exist(a2dp_sink_service as_ser)
add_subdirectory_if_exist(audio_manger_service am_ser)
add_subdirectory_if_exist(audio_player_service ap_ser)
add_subdirectory_if_exist(video_player_service vp_ser)
add_subdirectory_if_exist(stream_source_player_service stream_source)