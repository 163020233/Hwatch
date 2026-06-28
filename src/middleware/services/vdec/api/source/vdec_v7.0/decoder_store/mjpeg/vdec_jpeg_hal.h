/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2019-04-22
 */

#ifndef __VDEC_JPEG_HAL_H__
#define __VDEC_JPEG_HAL_H__

#include "drv_vdec_struct.h"
#include "soc_jpeg_api.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define VDEC_JPEG_HAL_DECODE_TIMEOUT           (td_u32)60  // ms

typedef struct {
    td_u32 width;
    td_u32 height;
    td_u32 luma_stride;
    td_u32 luma_size;
    td_u32 chrom_stride;
    td_u32 chrom_size;
    vdec_yuv_fmt fmt;
} vdec_jpeg_info;

jpeg_decompress_ptr vdec_jpeg_hal_init(td_s32 chan_id, const ext_vdec_stream *stream, vdec_jpeg_info *info);
td_void vdec_jpeg_hal_exit(jpeg_decompress_ptr dinfo_ptr);
td_s32 vdec_jpeg_hal_decode(jpeg_decompress_ptr dinfo_ptr, const vdec_jpeg_info *info, vdec_usrdec_frame *frame);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
