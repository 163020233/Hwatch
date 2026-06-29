/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description : dpu sre head file
 */

#ifndef DPU_SRE_H
#define DPU_SRE_H

#include "td_base.h"
#include "common_def.h"
#ifdef GPU_SRE_SUPPORT
td_void dpu_sre_process(td_void);
#else
STATIC INLINE td_void dpu_sre_process(td_void) {};
#endif

#endif
