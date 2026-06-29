/*
 * Copyright (c) 2022 HiSilicon (Shanghai) Technologies Co., Ltd..
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "display_gralloc.h"
#include <errno.h>
#include <securec.h>
#include "display_common.h"
#include "display_gralloc_internal.h"
#include "graphic_hardware_config.h"
#include "mem_pool_types.h"

#define MAX_MALLOC_SIZE 0x10000000L
TryToFreeImgCallback g_tryToFreeImgCallback = NULL;
TryToFreeFontCacheCallback g_tryToFreeFontCacheCallback = NULL;

static uint32_t GetFormatInfo(uint32_t format)
{
    uint32_t bitsPerPixel = 0;
    switch (format) {
        case PIXEL_FMT_RGBX_8888:
        case PIXEL_FMT_RGBA_8888:
        case PIXEL_FMT_BGRX_8888:
        case PIXEL_FMT_BGRA_8888:
            bitsPerPixel = 32;              // 32 : 每个像素占32bit
            break;
        case PIXEL_FMT_RGB_888:
            bitsPerPixel = 24;              // 24 : 每个像素占24bit
            break;
        case PIXEL_FMT_BGR_565:
        case PIXEL_FMT_RGB_565:
        case PIXEL_FMT_RGBA_5551:
        case PIXEL_FMT_BGRX_5551:
        case PIXEL_FMT_BGRA_5551:
        case PIXEL_FMT_BGRA_4444:
        case PIXEL_FMT_BGRX_4444:
        case PIXEL_FMT_RGBX_4444:
        case PIXEL_FMT_RGBA_4444:
            bitsPerPixel = 16;              // 16 : 每个像素占16bit
            break;
        default:
            DISP_LOGE("the format can not support %d\n", format);
            break;
    }
    return bitsPerPixel;
}

static int32_t InitBufferHandle(BufferHandle *buffer, const AllocInfo *info)
{
    if ((info->usage & HBM_USE_ASSIGN_SIZE) == HBM_USE_ASSIGN_SIZE) {
        if (info->expectedSize <= 0) {
            DISP_LOGE("%s: alloc info is invalid, size is %d\n", __func__, info->expectedSize);
            return DISPLAY_FAILURE;
        }
        buffer->size = info->expectedSize;
    } else {
        buffer->stride = ALIGN_UP(info->width, WIDTH_ALIGN) * GetFormatInfo(info->format) / BITS_PER_BYTE;
        buffer->size = info->height * buffer->stride;
        buffer->width = info->width;
        buffer->height = info->height;
    }
    buffer->usage = info->usage;
    buffer->fd = -1;
    buffer->format = info->format;
    return DISPLAY_SUCCESS;
}

static int32_t AllocMem(const AllocInfo *info, BufferHandle **handle)
{
    CHECK_NULLPOINTER_RETURN_VALUE(info, DISPLAY_NULL_PTR);
    CHECK_NULLPOINTER_RETURN_VALUE(handle, DISPLAY_NULL_PTR);
    BufferHandle *buffer = (BufferHandle *)malloc(sizeof(BufferHandle));
    if (buffer == NULL) {
        return DISPLAY_NULL_PTR;
    }
    int32_t ret = InitBufferHandle(buffer, info);
    if (ret != DISPLAY_SUCCESS) {
        goto END;
    }

    if (buffer->size > MAX_MALLOC_SIZE || buffer->size == 0) {
        ret = DISPLAY_FAILURE;
        goto END;
    }

    uint8_t *addr = (uint8_t *)malloc(buffer->size);
    if (addr == NULL) {
        ret = DISPLAY_FAILURE;
        goto END;
    }
    buffer->virAddr = (void *)addr;
    buffer->phyAddr = (uint64_t)addr;

END:
    if ((ret != DISPLAY_SUCCESS) && (buffer != NULL)) {
        free(buffer);
        buffer = NULL;
    }
    *handle = buffer;
    return ret;
}

static void FreeMem(BufferHandle *handle)
{
    CHECK_NULLPOINTER_RETURN(handle);
    if (handle->virAddr != NULL) {
        free(handle->virAddr);
        handle->virAddr = NULL;
        handle->phyAddr = 0;
    }

    if (handle->phyAddr != 0) {
        free((void *)handle->phyAddr);
        handle->virAddr = NULL;
        handle->phyAddr = 0;
    }
    free(handle);
}

static void RegisterTryToFreeImgCallback(TryToFreeImgCallback callback)
{
    g_tryToFreeImgCallback = callback;
}

static void RegisterTryToFreeFontCacheCallback(TryToFreeFontCacheCallback callback)
{
    g_tryToFreeFontCacheCallback = callback;
}

int32_t GrallocInitialize(GrallocFuncs **funcs)
{
    CHECK_NULLPOINTER_RETURN_VALUE(funcs, DISPLAY_NULL_PTR);
    GrallocFuncs *gFuncs = (GrallocFuncs *)malloc(sizeof(GrallocFuncs));
    if (gFuncs == NULL) {
        return DISPLAY_NULL_PTR;
    }

    (void)memset_s(gFuncs, sizeof(GrallocFuncs), 0, sizeof(GrallocFuncs));
    gFuncs->AllocMem = AllocMem;
    gFuncs->FreeMem = FreeMem;
    gFuncs->RegisterTryToFreeImgCallback = RegisterTryToFreeImgCallback;
    gFuncs->RegisterTryToFreeFontCacheCallback = RegisterTryToFreeFontCacheCallback;
    *funcs = gFuncs;
    return DISPLAY_SUCCESS;
}

int32_t GrallocUninitialize(GrallocFuncs *funcs)
{
    CHECK_NULLPOINTER_RETURN_VALUE(funcs, DISPLAY_NULL_PTR);
    free(funcs);
    funcs = NULL;
    return DISPLAY_SUCCESS;
}
