/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: header file for drv adp msg
 * Author: audio
 */

#ifndef __DRV_MSG_ANC_H__
#define __DRV_MSG_ANC_H__

#include "anc_struct.h"
#include "anc_coef.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

td_s32 drv_msg_anc_get_default_attr(anc_attr *attr);

td_s32 drv_msg_anc_open(const anc_attr *attr);

td_s32 drv_msg_anc_close(td_void);

td_s32 drv_msg_anc_get_param(td_u32 cmd, td_u8 *param, td_u32 param_size);

td_s32 drv_msg_anc_set_param(td_u32 cmd, const td_u8 *param, td_u32 param_size);

td_s32 drv_msg_anc_set_mode(td_u8 mode);

td_s32 drv_msg_anc_get_mode(td_u8 *mode);

td_s32 drv_msg_anc_set_attr(const anc_attr *attr);

td_s32 drv_msg_anc_get_attr(anc_attr *attr);

td_s32 drv_msg_anc_set_mic_sel(const anc_mic_sel *mic_sel);

td_s32 drv_msg_anc_get_mic_sel(anc_mic_sel *mic_sel);

td_s32 drv_msg_anc_dout_set_mute(td_bool mute);

td_s32 drv_msg_anc_mute_all_pga(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __DRV_MSG_ANC_H__ */
