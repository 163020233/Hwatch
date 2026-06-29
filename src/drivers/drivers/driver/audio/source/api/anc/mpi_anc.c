/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: source file for mpi anc
 * Author: audio
 */

#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>

#include "audio_type.h"
#include "audio_osal.h"
#include "audio_debug.h"
#include "drv_sap_ioctl.h"
#include "drv_msg_anc.h"
#include "mpi_anc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

static td_u32 g_mpi_anc_init_cnt = 0;
static pthread_mutex_t g_mpi_anc_mutex = PTHREAD_MUTEX_INITIALIZER;

#define ANC_OPEN_DELAY_MS 100
#define ANC_CLOSE_DELAY_MS 160

#define mpi_anc_check_inited()         \
    do {                               \
        if (g_mpi_anc_init_cnt == 0) { \
            return ERR_ANC_NOT_INIT;   \
        }                              \
    } while (0)

static td_void mpi_anc_lock(td_void)
{
    (td_void)pthread_mutex_lock(&g_mpi_anc_mutex);
}

static td_void mpi_anc_unlock(td_void)
{
    (td_void)pthread_mutex_unlock(&g_mpi_anc_mutex);
}

td_s32 mpi_anc_init(td_void)
{
    mpi_anc_lock();
    g_mpi_anc_init_cnt++;
    mpi_anc_unlock();
    return AUDIO_SUCCESS;
}

td_s32 mpi_anc_deinit(td_void)
{
    mpi_anc_lock();

    if (g_mpi_anc_init_cnt == 0) {
        mpi_anc_unlock();
        return AUDIO_SUCCESS;
    }

    g_mpi_anc_init_cnt--;
    mpi_anc_unlock();
    return AUDIO_SUCCESS;
}

td_s32 mpi_anc_get_default_attr(anc_attr *attr)
{
    td_s32 ret;
    mpi_anc_check_inited();
    audio_api_time_consum_stat();

    mpi_anc_lock();
    ret = drv_msg_anc_get_default_attr(attr);
    mpi_anc_unlock();
    return ret;
}

td_s32 mpi_anc_open(const anc_attr *attr)
{
    td_s32 ret;
    mpi_anc_check_inited();
    audio_api_time_consum_stat();

    mpi_anc_lock();
    ret = drv_msg_anc_open(attr);
    mpi_anc_unlock();
    audio_osal_msleep(ANC_OPEN_DELAY_MS);
    return ret;
}

td_s32 mpi_anc_close(td_void)
{
    td_s32 ret;
    mpi_anc_check_inited();
    audio_api_time_consum_stat();

    mpi_anc_lock();
    (td_void)drv_msg_anc_mute_all_pga();
    mpi_anc_unlock();
    audio_osal_msleep(ANC_CLOSE_DELAY_MS);

    mpi_anc_lock();
    ret = drv_msg_anc_close();
    mpi_anc_unlock();
    return ret;
}

td_s32 mpi_anc_get_param(td_u32 cmd, td_u8 *param, td_u32 param_size)
{
    td_s32 ret;
    mpi_anc_check_inited();
    audio_api_time_consum_stat();

    mpi_anc_lock();
    ret = drv_msg_anc_get_param(cmd, param, param_size);
    mpi_anc_unlock();
    return ret;
}

td_s32 mpi_anc_set_param(td_u32 cmd, const td_u8 *param, td_u32 param_size)
{
    td_s32 ret;
    mpi_anc_check_inited();
    audio_api_time_consum_stat();

    mpi_anc_lock();
    ret = drv_msg_anc_set_param(cmd, param, param_size);
    mpi_anc_unlock();
    return ret;
}

static td_s32 mapi_anc_set_mode(td_u8 mode)
{
    td_s32 ret;

    mpi_anc_lock();
    ret = drv_msg_anc_set_mode(mode);
    mpi_anc_unlock();
    return ret;
}

static td_s32 mapi_anc_dout_set_mute(td_bool mute)
{
    td_s32 ret;

    mpi_anc_lock();
    ret = drv_msg_anc_dout_set_mute(mute);
    mpi_anc_unlock();
    return ret;
}

static td_s32 mapi_anc_set_mode_anc_on(td_void)
{
    td_s32 ret;

    ret = mapi_anc_set_mode(AHA_MODE_ANC_ON);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(mapi_anc_set_mode, ret);
        return ret;
    }

    /* unmute anc to dout */
    ret = mapi_anc_dout_set_mute(TD_FALSE);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(mapi_anc_dout_set_mute, ret);
        return ret;
    }

    return AUDIO_SUCCESS;
}

static td_s32 mapi_anc_set_mode_anc_off(td_void)
{
    td_s32 ret;

    /* mute anc to dout */
    ret = mapi_anc_dout_set_mute(TD_TRUE);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(mapi_anc_dout_set_mute, ret);
    }

    ret = mapi_anc_set_mode(AHA_MODE_ANC_OFF);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(mapi_anc_set_mode, ret);
        return ret;
    }

    return AUDIO_SUCCESS;
}

td_s32 mpi_anc_set_mode(td_u8 mode)
{
    mpi_anc_check_inited();
    audio_api_time_consum_stat();

    if (mode == AHA_MODE_ANC_ON) {
        return mapi_anc_set_mode_anc_on();
    } else if (mode == AHA_MODE_ANC_OFF) {
        return mapi_anc_set_mode_anc_off();
    } else {
        return ERR_ANC_NOT_SUPPORTED;
    }
}

td_s32 mpi_anc_get_mode(td_u8 *mode)
{
    td_s32 ret;
    mpi_anc_check_inited();
    audio_api_time_consum_stat();

    mpi_anc_lock();
    ret = drv_msg_anc_get_mode(mode);
    mpi_anc_unlock();
    return ret;
}

td_s32 mpi_anc_set_attr(const anc_attr *attr)
{
    td_s32 ret;
    mpi_anc_check_inited();
    audio_api_time_consum_stat();

    mpi_anc_lock();
    ret = drv_msg_anc_set_attr(attr);
    mpi_anc_unlock();
    return ret;
}

td_s32 mpi_anc_get_attr(anc_attr *attr)
{
    td_s32 ret;
    mpi_anc_check_inited();
    audio_api_time_consum_stat();

    mpi_anc_lock();
    ret = drv_msg_anc_get_attr(attr);
    mpi_anc_unlock();
    return ret;
}

td_s32 mpi_anc_set_mic_sel(const anc_mic_sel *mic_sel)
{
    td_s32 ret;
    mpi_anc_check_inited();
    audio_api_time_consum_stat();

    mpi_anc_lock();
    ret = drv_msg_anc_set_mic_sel(mic_sel);
    mpi_anc_unlock();
    return ret;
}

td_s32 mpi_anc_get_mic_sel(anc_mic_sel *mic_sel)
{
    td_s32 ret;
    mpi_anc_check_inited();
    audio_api_time_consum_stat();

    mpi_anc_lock();
    ret = drv_msg_anc_get_mic_sel(mic_sel);
    mpi_anc_unlock();
    return ret;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
