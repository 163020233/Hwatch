/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef __EVENT_H__
#define __EVENT_H__

#include "vfmw.h"
#include "vdec_chr.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* CNcomment:   有一帧图像进入队列 */
#define report_imgrdy(chan_id, image_info) \
    do { \
        event_report(chan_id, EVNT_NEW_IMAGE, (vfmw_image *)(image_info), sizeof(vfmw_image)); \
    } while (0)

/* CNcomment:   发现user data */
#define report_usrdat(chan_id, usr_data) \
    do { \
        td_u64 para[1]; \
        para[0] = ptr_uint64(usr_data); \
        event_report(chan_id, EVNT_USRDAT, (td_u8 *)para, sizeof(para)); \
    } while (0)

/* CNcomment:   发现一个I帧，准备解码此I帧 */
#define report_find_iframe(chan_id, stream_size) \
    do { \
        td_u32 para[1]; \
        para[0] = stream_size; \
        event_report(chan_id, EVNT_FIND_IFRAME, (td_u8 *)para, sizeof(para)); \
    } while (0)

/* CNcomment:   VDM不响应 */
#define report_vdmerr(chan_id) \
    do { \
        event_report(chan_id, EVNT_VDM_ERR, TD_NULL, 0); \
    } while (0)

/* CNcomment:   发现不支持的规格 无 */
#define report_unsupport(chan_id) \
    do { \
        event_report(chan_id, EVNT_UNSUPPORT, TD_NULL, 0); \
        vdec_record_chr_event(chan_id, MT_VDEC_LISTEN_EVENT, MT_VDEC_LISTEN_ERR_UNSUPPORT_OK, MT_VDEC_INFO_3_DEFAULT); \
    } while (0)

/* CNcomment:   码流的语法错误    无 */
#define report_se_err(chan_id) \
    do { \
        event_report(chan_id, EVNT_SE_ERR, TD_NULL, 0); \
    } while (0)

#define report_dec_err(chan_id, pts, err_type) \
    do { \
        td_s64 para[2]; \
        para[0] = pts; \
        para[1] = (td_s64)(err_type); \
        event_report(chan_id, EVNT_REPORT_ERR, (td_u8 *)para, sizeof(para)); \
        vdec_record_chr_event(chan_id, MT_VDEC_LISTEN_EVENT, MT_VDEC_LISTEN_ERR_DECODE_OK, MT_VDEC_INFO_3_DEFAULT); \
    } while (0)

/* CNcomment:   发现新的帧结束符    无 */
#define report_meet_newfrm(chan_id) \
    do { \
        event_report(chan_id, EVNT_MEET_NEWFRM, TD_NULL, 0); \
    } while (0)

/* CNcomment:   码流解析错误 */
#define report_decsyntax_err(chan_id) \
    do { \
        event_report(chan_id, EVNT_DECSYNTAX_ERR, TD_NULL, 0); \
    } while (0)

/* CNcomment:    最后一帧输出结果上报 */
#define report_last_frame(chan_id, flag) \
    do { \
        td_u32 para[1]; \
        para[0] = (td_u32)(flag); \
        event_report(chan_id, EVNT_LAST_FRAME, (td_u8 *)para, sizeof(para)); \
        vdec_record_chr_event(chan_id, MT_VDEC_LISTEN_EVENT, MT_VDEC_LISTEN_EOS_OK, MT_VDEC_INFO_3_DEFAULT); \
    } while (0)

/* CNcomment:   码流信息上报划分帧存 */
#define report_need_arrange(chan_id, frm_num, frm_size, width, height) \
    do { \
        td_u32 para[4]; \
        para[0] = (td_u32)(frm_num); \
        para[1] = (td_u32)(frm_size); \
        para[2] = (td_u32)(width); \
        para[3] = (td_u32)(height); \
        event_report(chan_id, EVNT_NEED_ARRANGE, (td_u8 *)para, sizeof(para)); \
    } while (0)

td_bool event_check_typical_resolution(td_u32 width);
/* CNcomment:   发现不支持的规格 上报类型及数值 */
#define report_unsupport_spec(chan_id, vfmw_unsupport_spec, data) \
    do { \
        td_u32 para[2]; \
        para[0] = vfmw_unsupport_spec; \
        para[1] = data; \
        event_report(chan_id, EVNT_UNSUPPORT_SPEC, (td_u8 *)para, sizeof(para)); \
        vdec_record_chr_event(chan_id, MT_VDEC_LISTEN_EVENT, MT_VDEC_LISTEN_ERR_UNSUPPORT_OK, MT_VDEC_INFO_3_DEFAULT); \
    } while (0)

/* CNcomment:   变分辨率时输出假帧让后级还帧 */
#define report_fake_frame(chan_id) \
    do { \
        event_report(chan_id, EVNT_FAKE_FRAME, TD_NULL, 0); \
    } while (0)

#define report_err_frame(chan_id, frm_type, err_ratio) \
    do { \
        td_s8 para[2]; \
        para[0] = frm_type; \
        para[1] = err_ratio; \
        event_report(chan_id, EVNT_ERR_FRAME, (td_s8*)para, sizeof(para)); \
        vdec_record_chr_event(chan_id, MT_VDEC_LISTEN_EVENT, MT_VDEC_LISTEN_ERR_FRAME_OK, MT_VDEC_INFO_3_DEFAULT); \
    } while (0)

#define report_require_alloc_mem(chan_id, memory) \
    do { \
        event_report (chan_id, EVNT_ALLOC_MEM, (vfmw_mem_report *)(memory), sizeof(vfmw_mem_report)); \
    } while (0)

#define report_require_release_mem(chan_id, memory) \
    do { \
        event_report (chan_id, EVNT_RELEASE_MEM, (vfmw_mem_report *)(memory), sizeof(vfmw_mem_report)); \
    } while (0)

#define report_require_release_burst_mem(chan_id, private_id, private_mask) \
    do {  \
        td_s64 para[2]; \
        para[0] = private_id; \
        para[1] = private_mask; \
        event_report(chan_id, EVNT_RELEASE_BURST_MEM, (td_u8 *)para, sizeof(para)); \
    } while (0)

#define report_require_cancel_alloc(chan_id, private_id, private_mask) \
    do { \
        td_s64 para[2]; \
        para[0] = private_id; \
        para[1] = private_mask; \
        event_report(chan_id, EVNT_CANCEL_ALLOC, (td_u8 *)para, sizeof(para)); \
    } while (0)

#define report_set_address_in(chan_id, physical_addr) \
    do { \
        td_s64 para[1];  \
        para[0] = physical_addr; \
        event_report(chan_id, EVNT_SET_ADDRESS_IN, (td_u8 *)para, sizeof(para)); \
    } while (0)

#define report_first_pts(chan_id, pts) \
    do { \
        td_s64 para[1]; \
        para[0] = pts; \
        event_report(chan_id, EVNT_FIRST_PTS, (td_u8 *)para, sizeof(para)); \
    } while (0)

#define report_second_pts(chan_id, pts) \
    do { \
        td_s64 para[1]; \
        para[0] = pts; \
        event_report(chan_id, EVNT_SECOND_PTS, (td_u8 *)para, sizeof(para)); \
    } while (0)

#define report_err_std(chan_id, std) \
    do { \
        td_u32 para[1]; \
        para[0] = std; \
        event_report(chan_id, EVNT_VIDSTD_ERROR, (td_u8 *)para, sizeof(para)); \
    } while (0)

/* CNcomment: 变分辨率重新分割帧存上报 */
/* args: chan_id, size, num, width, height, stride, bitdepth, corp_w, crop_h, compress */
#define report_resolution_change(chan_id, para) \
    do { \
        event_report(chan_id, EVNT_RESOLUTION_CHANGE, (td_u8 *)(para), sizeof(para)); \
        vdec_record_chr_event(chan_id, MT_VDEC_LISTEN_EVENT, MT_VDEC_LISTEN_SIZE_CHG_OK, MT_VDEC_INFO_3_DEFAULT); \
    } while (0)

#define report_hdr_info(chan_id, full_range_flag, colour_primaries, matrix_coeffs, transfer_characteristics) \
    do { \
        td_u32  para[4]; \
        para[0] = full_range_flag; \
        para[1] = colour_primaries; \
        para[2] = matrix_coeffs; \
        para[3] = transfer_characteristics; \
        event_report(chan_id, EVNT_HDR_INFO, (td_u8*)para, sizeof(para)); \
    } while (0)

/* CNcomment: 内存不足上报 */
#define report_out_of_memory_err(chan_id) \
    do { \
        event_report(chan_id, EVNT_OUT_OF_MEMORY_ERR, TD_NULL, 0); \
    } while (0)

td_void event_set_callback(fun_vfmw_event_report report);
td_void event_report(const td_s32 inst_id, td_s32 type, td_void *args, td_s32 len);
td_s32 event_set_map(td_s32 chan_id, td_u64 event_map);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __PUBLIC_H__ */
