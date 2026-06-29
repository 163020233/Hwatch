
/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: amap controller
 * Created: 2026-01-14
 */

#ifndef GRALLOC_C_API_H
#define GRALLOC_C_API_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void *GrallocAllocBufferBySize(size_t size, unsigned int flag);

void GrallocFreeBuffer(void *ptr);

#ifdef __cplusplus
}
#endif

#endif
