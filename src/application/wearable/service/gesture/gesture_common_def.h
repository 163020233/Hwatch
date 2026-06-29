/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 */

#ifndef COMMON_DEF_H
#define COMMON_DEF_H

typedef enum { ACTION_STATUS_NORMAL = 0, ACTION_STATUS_OVER_ROTATE = 1, ACTION_STATUS_ERROR = -1 } ActionStatus;

extern short *g_npu_input1;
extern short *g_npu_input2;
extern short *g_npu_output;

#endif  // COMMON_DEF_H
