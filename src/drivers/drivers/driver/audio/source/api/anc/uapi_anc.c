/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: unf anc
 * Author: Audio
 */

#include "soc_uapi_anc.h"
#include "mpi_anc.h"
#include "audio_type.h"
#include "anc_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

td_s32 uapi_anc_init(td_void)
{
    return mpi_anc_init();
}

td_s32 uapi_anc_deinit(td_void)
{
    return mpi_anc_deinit();
}

td_s32 uapi_anc_get_default_attr(uapi_anc_attr *attr)
{
    return mpi_anc_get_default_attr((anc_attr *)attr);
}

td_s32 uapi_anc_open(const uapi_anc_attr *attr)
{
    return mpi_anc_open((anc_attr *)attr);
}

td_s32 uapi_anc_close(td_void)
{
    return mpi_anc_close();
}

td_s32 uapi_anc_get_param(td_u32 cmd, td_u8 *param, td_u32 param_size)
{
    return mpi_anc_get_param(cmd, param, param_size);
}

td_s32 uapi_anc_set_param(td_u32 cmd, const td_u8 *param, td_u32 param_size)
{
    return mpi_anc_set_param(cmd, param, param_size);
}

td_s32 uapi_anc_set_mode(td_u8 mode)
{
    return mpi_anc_set_mode(mode);
}

td_s32 uapi_anc_get_mode(td_u8 *mode)
{
    return mpi_anc_get_mode(mode);
}

td_s32 uapi_anc_set_attr(const uapi_anc_attr *attr)
{
    return mpi_anc_set_attr((const anc_attr *)attr);
}

td_s32 uapi_anc_get_attr(uapi_anc_attr *attr)
{
    return mpi_anc_get_attr((anc_attr *)attr);
}

td_s32 uapi_anc_set_mic_sel(const uapi_anc_mic_sel *mic_sel)
{
    return mpi_anc_set_mic_sel((anc_mic_sel *)mic_sel);
}

td_s32 uapi_anc_get_mic_sel(uapi_anc_mic_sel *mic_sel)
{
    return mpi_anc_get_mic_sel((anc_mic_sel *)mic_sel);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
