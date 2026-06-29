/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef __DEC_BASE_H__
#define __DEC_BASE_H__

#include "vfmw.h"
#include "vfmw_osal.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define DEC_OK  0
#define DEC_ERR 1
#define DEC_STOP 2
#define DEC_DESTROY 3
#define DEC_START   4

#define DEC_CHECK 0x1234abcd

#define DEC_DEBUG_CNT 8

typedef enum {
    DHAL_CID_NULL,
    DHAL_CID_REFRESH_BIN,
    DHAL_CID_WAIT_DEC,
    DHAL_CID_GET_VDH,
    DHAL_CID_PUT_VDH,
    DHAL_CID_IS_VDH_INT,
    DHAL_CID_GET_PIX_SUM,
    DHAL_CID_GET_CYCLE,
    DHAL_CID_MAX,
} dec_hal_cid;

typedef struct {
    td_s32 err_frame_num;
    td_u32 task_state;
    td_bool is_task_run;
    td_u32 dec_dps_num;
    td_u32 load;
} dec_info;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
