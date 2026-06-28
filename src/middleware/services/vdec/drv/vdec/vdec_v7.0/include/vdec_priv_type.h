/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2019-04-22
 */

#ifndef __VDEC_PRIV_TYPE_H__
#define __VDEC_PRIV_TYPE_H__

#include "drv_ioctl_vdec.h"
#include "drv_vdec_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define VDEC_VERSION "2025051500"

#define VDEC_MAX_FRAME_BUF_NUM 20
#define VDEC_LINSTEN_NUM   64

#define VDEC_MODULE_NAME   "VDEC"

#undef  LOG_MODULE_ID
#define LOG_MODULE_ID     SOC_ID_VDEC

#define INTF

#define VDEC_INVALID_PTS   ((td_s64)(-1))

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
