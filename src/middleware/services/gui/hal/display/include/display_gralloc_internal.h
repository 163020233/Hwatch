/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: display gralloc internal
 * Author: Hisi Graphic Team
 * Created: 2025-10
 */

#ifndef DISPLAY_GRALLOC_INTERNAL_H
#define DISPLAY_GRALLOC_INTERNAL_H
#include "display_type.h"

/* max allowed malloc memory:256M */
#define MAX_MALLOC_SIZE  0x10000000L

#define DIV_ROUND_UP(n, d) (((n) + (d)-1) / (d))
#define HEIGHT_ALIGN 2U
#define WIDTH_ALIGN 8U
#define MAX_PLANES 3

#define DEFAULT_READ_WRITE_PERMISSIONS 0666
#define SHM_MAX_KEY 10000
#define SHM_START_KEY 1
#define BITS_PER_BYTE 8

typedef struct {
    BufferHandle hdl;
    int32_t shmid;
} PriBufferHandle;
#endif /* DISPLAY_GRALLOC_INTERNAL_H */
