/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-10-10， Create file. \n
 */
#include "ux_audio_a2dp_src.h"
#include "audio_base_type.h"
#include "ux_common_log.h"

errcode_t ux_audio_a2dp_src_set_param(AudioA2dpCodecAttr *codec)
{
    int32_t err = AudioManagerSetA2dpAttr(codec, sizeof(AudioA2dpCodecAttr));
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[ux_audio_a2dp_src_set_param] Fail to set src codec 0x%x.", err);
    }
    return UX_ERR_SUCCESS;
}