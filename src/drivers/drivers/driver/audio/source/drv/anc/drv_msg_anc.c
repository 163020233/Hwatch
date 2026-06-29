/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: anc msg driver
 * Author: audio
 */

#include "securec.h"
#include "audio_osal.h"
#include "audio_debug.h"
#include "core_msg_struct.h"
#include "core_msg.h"
#include "drv_msg_anc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define ANC_PARAM_SIZE_MAX (4 << 10) /* 4k */

#define drv_anc_send_msg(msg)  core_msg_send(AUDIO_CORE_DSP0, MSG_CORE_ANC_MODULE, msg, MSG_SYNC_WAIT_TIME_MS)

td_s32 drv_msg_anc_get_default_attr(anc_attr *attr)
{
    core_msg_data msg = {
        .cmd = MSG_CORE_ANC_GET_DEFAULT_ATTR,
        .handle = 0,
        .input_data = TD_NULL,
        .input_data_size = 0,
        .output_data = attr,
        .output_data_size = sizeof(*attr),
    };

    anc_check_null_ptr_return(attr);
    return drv_anc_send_msg(&msg);
}

td_s32 drv_msg_anc_open(const anc_attr *attr)
{
    core_msg_data msg = {
        .cmd = MSG_CORE_ANC_OPEN,
        .handle = 0,
        .input_data = (td_void *)attr,
        .input_data_size = (td_u32)sizeof(*attr),
        .output_data = TD_NULL,
        .output_data_size = 0,
    };

    anc_check_null_ptr_return(attr);
    return drv_anc_send_msg(&msg);
}

td_s32 drv_msg_anc_close(td_void)
{
    core_msg_data msg = {
        .cmd = MSG_CORE_ANC_CLOSE,
        .handle = 0,
        .input_data = TD_NULL,
        .input_data_size = 0,
        .output_data = TD_NULL,
        .output_data_size = 0,
    };

    return drv_anc_send_msg(&msg);
}

td_s32 drv_msg_anc_get_param(td_u32 cmd, td_u8 *param, td_u32 param_size)
{
    td_s32 ret;
    core_msg_data msg;
    anc_common_rw_param cp;
    audio_buffer *mmz_buf = &cp.mmz_buf;

    if (param_size == 0 || param_size > ANC_PARAM_SIZE_MAX) {
        return ERR_ANC_INVALID_PARAM;
    }

    (td_void)memset_s(&cp, sizeof(cp), 0, sizeof(anc_common_rw_param));

    /* Use mmz to transfer param data to dsp */
    ret = audio_osal_mmz_alloc(mmz_buf, param_size);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(audio_osal_mmz_alloc, ret);
        return ERR_ANC_MEM_ALLOC;
    }

    cp.param = (td_u8 *)mmz_buf->virt_addr;
    cp.cmd = cmd;
    cp.param_size = param_size;
    ret = memcpy_s(cp.param, cp.param_size, param, param_size);
    if (ret != EOK) {
        audio_osal_mmz_release(mmz_buf);
        return ERR_ANC_MEM_COPY;
    }

    msg.cmd = MSG_CORE_ANC_GET_PARAM;
    msg.handle = 0;
    msg.input_data = &cp;
    msg.input_data_size = (td_u32)sizeof(cp);
    msg.output_data = TD_NULL;
    msg.output_data_size = 0;

    ret = drv_anc_send_msg(&msg);
    if (ret != AUDIO_SUCCESS) {
        audio_osal_mmz_release(mmz_buf);
        return ret;
    }

    ret = memcpy_s(param, param_size, cp.param, cp.param_size);
    if (ret != EOK) {
        audio_osal_mmz_release(mmz_buf);
        return ERR_ANC_MEM_COPY;
    }

    /* Free mmz used to transfer param data */
    audio_osal_mmz_release(mmz_buf);
    return AUDIO_SUCCESS;
}

td_s32 drv_msg_anc_set_param(td_u32 cmd, const td_u8 *param, td_u32 param_size)
{
    td_s32 ret;
    core_msg_data msg;
    anc_common_rw_param cp;
    audio_buffer *mmz_buf = &cp.mmz_buf;

    if (param_size == 0 || param_size > ANC_PARAM_SIZE_MAX) {
        return ERR_ANC_INVALID_PARAM;
    }

    (td_void)memset_s(&cp, sizeof(cp), 0, sizeof(anc_common_rw_param));

    /* Use mmz to transfer param data to dsp */
    ret = audio_osal_mmz_alloc(mmz_buf, param_size);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(audio_osal_mmz_alloc, ret);
        return ERR_ANC_MEM_ALLOC;
    }

    cp.param = (td_u8 *)mmz_buf->virt_addr;
    cp.cmd = cmd;
    cp.param_size = param_size;
    ret = memcpy_s(cp.param, cp.param_size, param, param_size);
    if (ret != EOK) {
        audio_osal_mmz_release(mmz_buf);
        return ERR_ANC_MEM_COPY;
    }

    msg.cmd = MSG_CORE_ANC_SET_PARAM;
    msg.handle = 0;
    msg.input_data = &cp;
    msg.input_data_size = (td_u32)sizeof(cp);
    msg.output_data = TD_NULL;
    msg.output_data_size = 0;

    ret = drv_anc_send_msg(&msg);
    if (ret != AUDIO_SUCCESS) {
        audio_osal_mmz_release(mmz_buf);
        return ret;
    }

    /* Free mmz used to transfer param data */
    audio_osal_mmz_release(mmz_buf);
    return AUDIO_SUCCESS;
}

td_s32 drv_msg_anc_set_mode(td_u8 mode)
{
    core_msg_data msg = {
        .cmd = MSG_CORE_ANC_SET_MODE,
        .handle = 0,
        .input_data = &mode,
        .input_data_size = (td_u32)sizeof(mode),
        .output_data = TD_NULL,
        .output_data_size = 0,
    };

    return drv_anc_send_msg(&msg);
}

td_s32 drv_msg_anc_get_mode(td_u8 *mode)
{
    core_msg_data msg = {
        .cmd = MSG_CORE_ANC_GET_MODE,
        .handle = 0,
        .input_data = TD_NULL,
        .input_data_size = 0,
        .output_data = TD_NULL,
        .output_data_size = 0,
    };

    anc_check_null_ptr_return(mode);

    msg.output_data = mode;
    msg.output_data_size = (td_u32)sizeof(*mode);

    return drv_anc_send_msg(&msg);
}

td_s32 drv_msg_anc_set_attr(const anc_attr *attr)
{
    core_msg_data msg = {
        .cmd = MSG_CORE_ANC_SET_ATTR,
        .handle = 0,
        .input_data = (td_void *)attr,
        .input_data_size = (td_u32)sizeof(*attr),
        .output_data = TD_NULL,
        .output_data_size = 0,
    };

    anc_check_null_ptr_return(attr);
    return drv_anc_send_msg(&msg);
}

td_s32 drv_msg_anc_get_attr(anc_attr *attr)
{
    core_msg_data msg = {
        .cmd = MSG_CORE_ANC_GET_ATTR,
        .handle = 0,
        .input_data = TD_NULL,
        .input_data_size = 0,
        .output_data = attr,
        .output_data_size = sizeof(*attr),
    };

    anc_check_null_ptr_return(attr);
    return drv_anc_send_msg(&msg);
}

td_s32 drv_msg_anc_set_mic_sel(const anc_mic_sel *mic_sel)
{
    core_msg_data msg = {
        .cmd = MSG_CORE_ANC_SET_MIC_SEL,
        .handle = 0,
        .input_data = (td_void *)mic_sel,
        .input_data_size = (td_u32)sizeof(*mic_sel),
        .output_data = TD_NULL,
        .output_data_size = 0,
    };

    anc_check_null_ptr_return(mic_sel);
    return drv_anc_send_msg(&msg);
}

td_s32 drv_msg_anc_get_mic_sel(anc_mic_sel *mic_sel)
{
    core_msg_data msg = {
        .cmd = MSG_CORE_ANC_GET_MIC_SEL,
        .handle = 0,
        .input_data = TD_NULL,
        .input_data_size = 0,
        .output_data = mic_sel,
        .output_data_size = (td_u32)sizeof(*mic_sel),
    };

    anc_check_null_ptr_return(mic_sel);
    return drv_anc_send_msg(&msg);
}

td_s32 drv_msg_anc_dout_set_mute(td_bool mute)
{
    td_u8 en = mute;
    core_msg_data msg = {
        .cmd = MSG_CORE_ANC_DOUT_SET_MUTE,
        .handle = 0,
        .input_data = &en,
        .input_data_size = (td_u32)sizeof(en),
        .output_data = TD_NULL,
        .output_data_size = 0,
    };

    return drv_anc_send_msg(&msg);
}

td_s32 drv_msg_anc_mute_all_pga(td_void)
{
    core_msg_data msg = {
        .cmd = MSG_CORE_ANC_MUTE_ALL_PGA,
        .handle = 0,
        .input_data = TD_NULL,
        .input_data_size = 0,
        .output_data = TD_NULL,
        .output_data_size = 0,
    };

    return drv_anc_send_msg(&msg);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
