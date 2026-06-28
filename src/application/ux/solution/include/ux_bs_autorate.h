/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * File          ux_bs_filter.h
 * Description:  provide ux bs autorate header file.
 */
#ifndef UX_BS_AUTORATE_H
#define UX_BS_AUTORATE_H

#include "errcode.h"
#include "stddef.h"
#include "stdbool.h"
#include "ux_common_define.h"
#include "ssf_autorate.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


typedef struct {
    uint32_t rate;                              // 码率，dongle和直播麦均使用该参数
    sle_iog_phy_param_t sle_iog_phy_param;      // dongle需要，直播麦不用
} ux_autorate_param_t;

errcode_t ux_sle_confirm_autorate_change(uint32_t rate, bool result);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  // end UX_BS_AUTORATE_H
