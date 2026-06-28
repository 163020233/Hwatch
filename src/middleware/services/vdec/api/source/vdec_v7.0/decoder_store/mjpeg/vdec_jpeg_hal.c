/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2019-04-22
 */

#include "mpi_vdec_sdk.h"
#include "mpi_vdec_osal.h"
#include "securec.h"
#include "soc_jpeg_api.h"
#include "vdec_jpeg_hal.h"
#ifdef VDEC_LOW_POWER_SUPPORT
#include "pm_veto.h"
#endif

static pthread_mutex_t g_jpg_lock = PTHREAD_MUTEX_INITIALIZER;
static struct jpeg_decompress_struct g_dinfo[VDEC_MAX_CHAN_NUM];

jpeg_decompress_ptr vdec_jpeg_hal_init(td_s32 chan_id, const ext_vdec_stream *stream, vdec_jpeg_info *info)
{
    td_s32 ret;
    struct jpeg_source_struct src = { 0 };
    jpeg_scale_multiple scale = JPEG_SCALE_1;
    td_bool output_yuv420sp   = TD_TRUE;
    jpeg_decompress_ptr dinfo_ptr = TD_NULL;

    vdec_check_ret_val(chan_id >= 0 && chan_id < VDEC_MAX_CHAN_NUM, TD_NULL);
    vdec_check_ret_val(stream != TD_NULL && info != TD_NULL, TD_NULL);

    dinfo_ptr = &g_dinfo[chan_id];
    (td_void)memset_s(info, sizeof(vdec_jpeg_info), 0, sizeof(vdec_jpeg_info));
    (td_void)memset_s(dinfo_ptr, sizeof(struct jpeg_decompress_struct), 0, sizeof(struct jpeg_decompress_struct));

    dinfo_ptr->scale = scale;
    dinfo_ptr->output_yuv420sp = output_yuv420sp;

    src.infile = TD_NULL;
    src.phy_buf = stream->start_addr;
    src.vir_buf = (td_char *)stream->vir_addr;
    src.size = stream->length;

    pthread_mutex_lock(&g_jpg_lock);
    ret = uapi_jpeg_decomress_create(dinfo_ptr, &src, TD_TRUE);
    if (ret != EXT_SUCCESS) {
        pthread_mutex_unlock(&g_jpg_lock);
        vdec_print_err("%s, %d, uapi_jpeg_decomress_create error \r\n", __func__, __LINE__);
        return TD_NULL;
    }
    pthread_mutex_unlock(&g_jpg_lock);

    info->fmt = VDEC_YUV_SP420;
    info->width = dinfo_ptr->output.width[0];
    info->height = dinfo_ptr->output.height[0];
    info->luma_stride = dinfo_ptr->output.stride[0];
    info->luma_size = dinfo_ptr->output.size[0];
    info->chrom_stride = dinfo_ptr->output.stride[1];
    info->chrom_size = dinfo_ptr->output.size[1];

    vdec_print_dbg("Jpeg info: out fmt %d in fmt %d| Y %dx%d stride %d size %d | C %dx%d stride %d size %d \r\n",
        info->fmt, dinfo_ptr->image_color_space, info->width, info->height, info->luma_stride, info->luma_size,
        dinfo_ptr->output.width[1], dinfo_ptr->output.height[1], dinfo_ptr->output.stride[1],
        dinfo_ptr->output.size[1]);

    return dinfo_ptr;
}

td_void vdec_jpeg_hal_exit(jpeg_decompress_ptr dinfo_ptr)
{
    pthread_mutex_lock(&g_jpg_lock);
    uapi_jpeg_decompress_destroy(dinfo_ptr);
    pthread_mutex_unlock(&g_jpg_lock);

#ifdef VDEC_LOW_POWER_SUPPORT
    uapi_pm_remove_sleep_veto_with_symmetry(PM_ID_JPEG);
#endif
}

td_s32 vdec_jpeg_hal_decode(jpeg_decompress_ptr dinfo_ptr, const vdec_jpeg_info *info, vdec_usrdec_frame *frame)
{
    td_s32 ret;
    td_u32 start_time = 0;
    td_u32 end_time = 0;
    struct jpeg_dst_struct dst = { 0 };

    vdec_check_ret(frame != TD_NULL);
    vdec_check_ret(info != TD_NULL);

    dst.stride    = dinfo_ptr->output.stride[0];
    dst.uv_stride = dinfo_ptr->output.stride[1];
    dst.size      = dinfo_ptr->output.size[0] + dinfo_ptr->output.size[1];
    dst.uv_offset = dinfo_ptr->output.size[0];
    dst.phy_buf = frame->phy_addr;
    dst.vir_buf = (td_char *)vdec_mmap(dst.phy_buf, dst.size);

    if (frame->mem_size < info->luma_size + info->chrom_size) {
        vdec_print_err("frame size %u < luma size %u + chrom size %u\n",
            (td_u32)frame->mem_size, info->luma_size, info->chrom_size);
        return EXT_FAILURE;
    }

#ifdef VDEC_LOW_POWER_SUPPORT
    uapi_pm_add_sleep_veto_with_symmetry(PM_ID_JPEG);
#endif

    vdec_get_time(&start_time);

    pthread_mutex_lock(&g_jpg_lock);
    ret = uapi_jpeg_decompress_start(dinfo_ptr, &dst);
    if (ret != EXT_SUCCESS) {
        vdec_print_err("%s, %d, uapi_jpeg_decompress_start error \r\n", __func__, __LINE__);
        pthread_mutex_unlock(&g_jpg_lock);
    }
    pthread_mutex_unlock(&g_jpg_lock);

    frame->bit_depth = 8;  // 8: only support 8 bit depth

    vdec_get_time(&end_time);

    if (end_time > start_time) {
        if ((end_time - start_time) >= VDEC_JPEG_HAL_DECODE_TIMEOUT) {
            vdec_print_err("mjpeg decode timeout %u ms.\n",  (end_time - start_time));
        }
    }

    return ret;
}

