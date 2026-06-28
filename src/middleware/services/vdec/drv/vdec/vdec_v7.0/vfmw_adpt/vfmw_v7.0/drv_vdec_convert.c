/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2019-04-22
 */

#include "drv_vdec_convert.h"
#include "drv_vdec_sdk.h"
#include "vdec_priv_type.h"

#define VDEC_EVENT_TABLE_ITEM_SIZE 2

typedef td_s32 (*fn_evt_process_type)(const td_void *args, td_s32 len, ext_vdec_event *out);

typedef struct {
    td_s32 evt_id;
    fn_evt_process_type evt_process;
} vdec_cnvt_evt_item;

STATIC td_s32 vdec_cnvt_new_image_2_vdec(const td_void *args, td_s32 len, ext_vdec_event *out)
{
    vdec_unused(args);
    vdec_unused(len);
    out->event_id = EXT_VDEC_EVT_NEW_FRAME;

    return EXT_SUCCESS;
}

STATIC td_s32 vdec_cnvt_stream_2_vdec(const td_void *args, td_s32 len, ext_vdec_event *out)
{
    vfmw_stream_buf *stream = (vfmw_stream_buf*)args;

    if (stream == TD_NULL || len != sizeof(vfmw_stream_buf)) {
        vdec_print_err("stream param invalid\n");
        return EXT_FAILURE;
    }

    out->event_id = EXT_VDEC_EVT_FREE_STREAM;

    return EXT_SUCCESS;
}

STATIC td_s32 vdec_cnvt_eos_2_vdec(const td_void *args, td_s32 len, ext_vdec_event *out)
{
    if (args == TD_NULL || len != sizeof(td_u32)) {
        vdec_print_err("eos param invalid\n");
        return EXT_FAILURE;
    }
    out->event_id = EXT_VDEC_EVT_EOS;
    out->event_param.eos_param.is_last_frame_taken = (((td_u32*)args)[0] == 0) ? 0 : 1;

    return EXT_SUCCESS;
}

STATIC td_s32 vdec_cnvt_err_frm_2_vdec(const td_void *args, td_s32 len, ext_vdec_event *out)
{
    td_s8 vfmw_ipb_type;

    vdec_unused(len);

    out->event_id = EXT_VDEC_EVT_ERR_FRAME;
    vfmw_ipb_type = ((td_s8*)args)[0];
    if (vfmw_ipb_type == 0) {
        out->event_param.error_frame.frame_type = EXT_VDEC_FRM_TYPE_I;
    } else if (vfmw_ipb_type == 1) {
        out->event_param.error_frame.frame_type = EXT_VDEC_FRM_TYPE_P;
    } else {
        out->event_param.error_frame.frame_type = EXT_VDEC_FRM_TYPE_B;
    }

    out->event_param.error_frame.error_ratio = ((td_u8*)args)[1];

    return EXT_SUCCESS;
}

STATIC td_s32 vdec_cnvt_unsupport_2_vdec(const td_void *args, td_s32 len, ext_vdec_event *out)
{
    vdec_unused(args);
    vdec_unused(len);

    out->event_id = EXT_VDEC_EVT_ERR_UNSUPPORT;

    return EXT_SUCCESS;
}

STATIC td_s32 vdec_cnvt_unsupport_spec_2_vdec(const td_void *args, td_s32 len, ext_vdec_event *out)
{
    vdec_unused(args);
    vdec_unused(len);

    out->event_id = EXT_VDEC_EVT_ERR_UNSUPPORT;

    return EXT_SUCCESS;
}

STATIC td_s32 vdec_cnvt_resolution_change_2_vdec(const td_void *args, td_s32 len, ext_vdec_event *out)
{
    td_u32 *param = (td_u32 *)args;

    if (args == TD_NULL || len != sizeof(td_u32) * VFMW_OUTER_PARA_SIZE) {
        vdec_print_err("resolution change invalid\n");
        return EXT_FAILURE;
    }

    out->event_id = EXT_VDEC_EVT_SIZE_CHG;
    out->event_param.size_change.new_frame_size = param[0]; /* 0 is the index of new_frm_size */
    out->event_param.size_change.new_frame_num = param[1]; /* 1 is the index of new_frm_num */
    out->event_param.size_change.new_width = param[6]; /* 6 is the index of new_width */
    out->event_param.size_change.new_height = param[7]; /* 7 is the index of new_height */
    out->event_param.size_change.new_stride = param[4]; /* 4 is the index of new_stride */
    out->event_param.size_change.new_bit_depth = param[5]; /* 5 is the index of new_bit_depth */
    out->event_param.size_change.new_splice_info.ori_width = param[9]; /* 9 is the index of ori_width */
    out->event_param.size_change.new_splice_info.ori_height = param[10]; /* 10 is the index of ori_width */
    out->event_param.size_change.new_splice_info.type = (ext_vdec_splice_type)param[11]; /* 11 is ori_width */

    vdec_print_dbg("%u x %u frm_size 0x%x num %u ori_width %u ori_height %u type %u\n",
        out->event_param.size_change.new_width, out->event_param.size_change.new_height, param[0], param[1],
        param[9], param[10], param[11]); /* 9 10 11 is the index */

    return EXT_SUCCESS;
}

STATIC td_s32 vdec_cnvt_out_of_memory_2_vdec(const td_void *args, td_s32 len, ext_vdec_event *out)
{
    vdec_unused(args);
    vdec_unused(len);

    out->event_id = EXT_VDEC_EVT_OUT_OF_MEMORY;
    return EXT_SUCCESS;
}

STATIC td_s32 vdec_cnvt_dec_err_2_vdec(const td_void *args, td_s32 len, ext_vdec_event *out)
{
    td_s64 *param = (td_s64*)args;

    vdec_unused(len);

    out->event_id = EXT_VDEC_EVT_ERR_DECODE;
    out->event_param.error_decode.pts = param[0];
    switch (param[1]) {
        case SYNTAX_ERR_VPS:
            out->event_param.error_decode.error_type = EXT_VDEC_ERROR_VPS;
            break;
        case SYNTAX_ERR_SPS:
            out->event_param.error_decode.error_type = EXT_VDEC_ERROR_SPS;
            break;
        case SYNTAX_ERR_PPS:
            out->event_param.error_decode.error_type = EXT_VDEC_ERROR_PPS;
            break;
        case SYNTAX_NO_REF:
            out->event_param.error_decode.error_type = EXT_VDEC_ERROR_REF_FRAME;
            break;
        case SYNTAX_ERR_SLICE_HDR:
            out->event_param.error_decode.error_type = EXT_VDEC_ERROR_SLICE_HDR;
            break;
        default:
            out->event_param.error_decode.error_type = EXT_VDEC_ERROR_UNDEFINED;
            break;
    }

    return EXT_SUCCESS;
}

STATIC ext_vdec_usd_pos vdec_cnvt_get_usd_pos(vfmw_usd_src_type from)
{
    ext_vdec_usd_pos pos;

    switch (from) {
        case VFMW_USD_INVALID:
            pos = EXT_VDEC_USD_POS_UNKNOWN;
            break;
        case VFMW_USD_MP2SEQ:
            pos = EXT_VDEC_USD_POS_MPEG2_SEQ;
            break;
        case VFMW_USD_MP2GOP:
            pos = EXT_VDEC_USD_POS_MPEG2_GOP;
            break;
        case VFMW_USD_MP2PIC:
            pos = EXT_VDEC_USD_POS_MPEG2_FRM;
            break;
        case VFMW_USD_MP4VSOS:
            pos = EXT_VDEC_USD_POS_MPEG4_VSOS;
            break;
        case VFMW_USD_MP4VSO:
            pos = EXT_VDEC_USD_POS_MPEG4_VSO;
            break;
        case VFMW_USD_MP4VOL:
            pos = EXT_VDEC_USD_POS_MPEG4_VOL;
            break;
        case VFMW_USD_MP4GOP:
            pos = EXT_VDEC_USD_POS_MPEG4_GOP;
            break;
        case VFMW_USD_H264_REG:
            pos = EXT_VDEC_USD_POS_H264_REG;
            break;
        case VFMW_USD_H264_UNREG:
            pos = EXT_VDEC_USD_POS_H264_UNREG;
            break;
        case VFMW_USD_AVSSEQ:
            pos = EXT_VDEC_USD_POS_AVS_SEQ;
            break;
        case VFMW_USD_AVSPIC:
            pos = EXT_VDEC_USD_POS_AVS_FRM;
            break;
        default:
            pos = EXT_VDEC_USD_POS_UNKNOWN;
            break;
    }

    return pos;
}

STATIC ext_vdec_usd_type vdec_cnvt_usrdat_type_2_vdec(vfmw_usd_type vfmw_type)
{
    switch (vfmw_type) {
        case VFMW_USD_TYPE_DVB1_CC:
            return EXT_VDEC_USD_TYPE_DVB1_CC;
        case VFMW_USD_TYPE_DVB1_BAR:
            return EXT_VDEC_USD_TYPE_DVB1_BAR;
        case VFMW_USD_TYPE_AFD:
            return EXT_VDEC_USD_TYPE_AFD;
        default:
            return EXT_VDEC_USD_TYPE_UNKNOWN;
    }
}

STATIC vfmw_usd_type vdec_cnvt_usrdat_type_2_vfmw(ext_vdec_usd_type vdec_type)
{
    switch (vdec_type) {
        case EXT_VDEC_USD_TYPE_DVB1_CC:
            return VFMW_USD_TYPE_DVB1_CC;
        case EXT_VDEC_USD_TYPE_DVB1_BAR:
            return VFMW_USD_TYPE_DVB1_BAR;
        case EXT_VDEC_USD_TYPE_AFD:
            return VFMW_USD_TYPE_AFD;
        default:
            return VFMW_USD_TYPE_UNKNOWN;
    }
}

ext_vdec_norm_type vdec_cnvt_disp_norm_2_vdec(td_u32 norm)
{
    switch (norm) {
        case 1: /* 1 is PAL in vfmw */
            return EXT_VDEC_NORM_PAL;
        case 2: /* 2 is NTSC in vfmw */
            return EXT_VDEC_NORM_NTSC;
        default:
            return EXT_VDEC_NORM_MAX;
    }
}

ext_vdec_frame_rate_type vdec_cnvt_frmrate_type_2_vdec(vfmw_pts_framrate_type type)
{
    switch (type) {
        case PTS_FRMRATE_TYPE_PTS:
            return EXT_VDEC_FR_TYPE_PTS;
        case PTS_FRMRATE_TYPE_STREAM:
            return EXT_VDEC_FR_TYPE_STREAM;
        case PTS_FRMRATE_TYPE_USER:
            return EXT_VDEC_FR_TYPE_USER;
        case PTS_FRMRATE_TYPE_USER_PTS:
            return EXT_VDEC_FR_TYPE_USER_PTS;
        default:
            return EXT_VDEC_FR_TYPE_PTS;
    }
}

ext_vdec_sampling_type vdec_cnvt_sampling_type_2_vdec(td_u32 scan_type)
{
    switch (scan_type) {
        case 0: /* 0 is progressive */
            return EXT_VDEC_PROGRESSIVE;
        case 1: /* 1 is interlaced */
            return EXT_VDEC_INTERLACED;
        default:
            return EXT_VDEC_SAMPLE_UNKNOWN;
    }
}

ext_vdec_bit_depth vdec_cnvt_bit_depth_2_vdec(td_u32 bit_depth)
{
    switch (bit_depth) {
        case 10: /* 10 is 10bit */
            return EXT_VDEC_BIT_DEPTH_10;
        case 8: /* 8 is 8bit */
            return EXT_VDEC_BIT_DEPTH_8;
        default:
            return EXT_VDEC_BIT_DEPTH_8;
    }
}

STATIC ext_vdec_color_space vdec_convert_color_primaries_1(const vfmw_color_desc *color_info)
{
    td_u8 color_primaries = color_info->colour_primaries;
    td_u8 matrix_coeffs = color_info->matrix_coeffs;
    td_u8 full_range_flag = color_info->full_rangeflag;

    if (color_primaries == 1 && matrix_coeffs == 1) {
        return (full_range_flag == 1) ? EXT_VDEC_CS_BT709_YUV_FULL : EXT_VDEC_CS_BT709_YUV_LIMITED;
    }

    return EXT_VDEC_CS_UNKNOWN;
}

STATIC ext_vdec_color_space vdec_convert_color_primaries_5(const vfmw_color_desc *color_info)
{
    td_u8 color_primaries = color_info->colour_primaries;
    td_u8 matrix_coeffs = color_info->matrix_coeffs;
    td_u8 full_range_flag = color_info->full_rangeflag;

    if (color_primaries == 5 && matrix_coeffs == 5) { /* 5 5 specific value in protrol */
        return (full_range_flag == 1) ? EXT_VDEC_CS_BT601_YUV_FULL : EXT_VDEC_CS_BT601_YUV_LIMITED;
    }

    return EXT_VDEC_CS_UNKNOWN;
}

STATIC ext_vdec_color_space vdec_convert_color_primaries_9(const vfmw_color_desc *color_info, vfmw_vid_std std)
{
    td_u8 color_primaries = color_info->colour_primaries;
    td_u8 matrix_coeffs = color_info->matrix_coeffs;
    td_u8 full_range_flag = color_info->full_rangeflag;

    if (color_primaries == 9) { /* 9 specific value in protrol */
        if (matrix_coeffs == 9) { /* 9 specific value in protrol */
            return (full_range_flag == 1) ? EXT_VDEC_CS_BT2020_YUV_FULL : EXT_VDEC_CS_BT2020_YUV_LIMITED;
        } else if (matrix_coeffs == 10) { /* 10 specific value in protrol */
            return (full_range_flag == 1) ? EXT_VDEC_CS_BT2020_YUV_FULL : EXT_VDEC_CS_BT2020_YUV_LIMITED;
        } else if ((matrix_coeffs == 8) && (std == VFMW_AVS2)) { /* 8 specific value in protrol */
            return (full_range_flag == 1) ? EXT_VDEC_CS_BT2020_YUV_FULL : EXT_VDEC_CS_BT2020_YUV_LIMITED;
        }
    }

    return EXT_VDEC_CS_UNKNOWN;
}

ext_vdec_color_space vdec_cnvt_color_space_2_vdec(vfmw_vid_std std, const vfmw_color_desc *color_info,
    td_u32 width, td_u32 height)
{
    ext_vdec_color_space ret;

    if (std == VFMW_H264 || std == VFMW_HEVC || std == VFMW_AVS2) {
        ret = vdec_convert_color_primaries_1(color_info);
        if (ret != EXT_VDEC_CS_UNKNOWN) {
            return ret;
        }

        ret = vdec_convert_color_primaries_5(color_info);
        if (ret != EXT_VDEC_CS_UNKNOWN) {
            return ret;
        }

        ret = vdec_convert_color_primaries_9(color_info, std);
        if (ret != EXT_VDEC_CS_UNKNOWN) {
            return ret;
        }
    }

    if (width >= 1280 || height >= 720) { /* 1280 720 is the threshold */
        return EXT_VDEC_CS_BT709_YUV_LIMITED;
    } else {
        return EXT_VDEC_CS_BT601_YUV_LIMITED;
    }
}

td_void vdec_cnvt_bitdepth_cap_2_vdec(td_u32 vfmw_cap, ext_vdec_capability *cap)
{
    vdec_check(cap != TD_NULL);

    cap->bit_depth_num = 0;

    if ((vfmw_cap & VFMW_BITDEPTH_CAP_DEFAULT) != 0) {
        cap->bit_depth[cap->bit_depth_num] = EXT_VDEC_BIT_DEPTH_AUTO;
        cap->bit_depth_num++;
    }

    if ((vfmw_cap & VFMW_BITDEPTH_CAP_8_BIT_ONLY) != 0) {
        cap->bit_depth[cap->bit_depth_num] = EXT_VDEC_BIT_DEPTH_8;
        cap->bit_depth_num++;
    }

    return;
}

td_void vdec_cnvt_frm_layout_cap_2_vdec(td_u32 vfmw_cap, ext_vdec_capability *cap)
{
    vdec_check(cap != TD_NULL);

    cap->frame_layout_num = 0;

    if ((vfmw_cap & VFMW_FRM_LAYOUT_CAP_LINEAR) != 0) {
        cap->frame_layout[cap->frame_layout_num] = EXT_VDEC_FRM_LAYOUT_LINEAR;
        cap->frame_layout_num++;
    }

    return;
}


td_s32 vdec_cnvt_event_2_vdec(td_s32 chan_id, td_s32 type, const td_void *args, td_s32 len,
    ext_vdec_event *out)
{
    td_s32 ret = EXT_FAILURE;
    td_u32 i;
    fn_evt_process_type evt_process = TD_NULL;
    vdec_cnvt_evt_item items[] = {
        { EVNT_NEW_IMAGE,            vdec_cnvt_new_image_2_vdec },
        { EVNT_RLS_STREAM,           vdec_cnvt_stream_2_vdec },
        { EVNT_LAST_FRAME,           vdec_cnvt_eos_2_vdec },
        { EVNT_ERR_FRAME,            vdec_cnvt_err_frm_2_vdec },
        { EVNT_UNSUPPORT,            vdec_cnvt_unsupport_2_vdec },
        { EVNT_UNSUPPORT_SPEC,       vdec_cnvt_unsupport_spec_2_vdec },
        { EVNT_REPORT_ERR,           vdec_cnvt_dec_err_2_vdec },
        { EVNT_RESOLUTION_CHANGE,    vdec_cnvt_resolution_change_2_vdec },
        { EVNT_OUT_OF_MEMORY_ERR,    vdec_cnvt_out_of_memory_2_vdec },
    };

    vdec_unused(chan_id);

    vdec_check_ret(out != TD_NULL);
    out->event_id = EXT_VDEC_EVT_UNKNOWN;

    for (i = 0; i < sizeof(items) / sizeof(vdec_cnvt_evt_item); i++) {
        if (items[i].evt_id == type) {
            evt_process = items[i].evt_process;
            break;
        }
    }

    if (evt_process != TD_NULL) {
        ret = evt_process(args, len, out);
    }

    return ret;
}

/* convert from vdec to vfmw */
vfmw_vid_std vdec_cnvt_std_2_vfmw(ext_vdec_standard std)
{
    td_u32 i;
    td_u32 row_size;

    td_s32 std_table[][2] = { /* 25 : std_table length. */
        {EXT_VDEC_STD_H264,  VFMW_H264},
        {EXT_VDEC_STD_H265,  VFMW_HEVC},
        {EXT_VDEC_STD_MJPEG, VFMW_MJPEG},
        {EXT_VDEC_STD_HEIF,  VFMW_HEVC},
    };

    row_size = (td_u32)sizeof(std_table) / (td_u32)sizeof(std_table[0]);

    for (i = 0; i < row_size; i++) {   /* 25 : std_table length. */
        if ((ext_vdec_standard)std_table[i][0] == std) {
            return std_table[i][1];
        }
    }

    return VFMW_STD_MAX;
}

vfmw_dec_mode vdec_cnvt_decmode_2_vfmw(ext_vdec_decode_mode mode)
{
    switch (mode) {
        case EXT_VDEC_DEC_MODE_IPB:
            return VFMW_DEC_MODE_NORMAL;
        case EXT_VDEC_DEC_MODE_IP:
            return VFMW_DEC_MODE_IP;
        case EXT_VDEC_DEC_MODE_I:
            return VFMW_DEC_MODE_I;

        default:
            return VFMW_DEC_MODE_NORMAL;
    }
}

vfmw_cmp vdec_cnvt_cmp_2_vfmw(ext_vdec_cmp_mode mode)
{
    switch (mode) {
        case EXT_VDEC_CMP_ADJUST:
            return VFMW_CMP_ADJUST;
        case EXT_VDEC_CMP_OFF:
            return VFMW_CMP_OFF;
        case EXT_VDEC_CMP_ON:
            return VFMW_CMP_ON;
        default:
            return VFMW_CMP_MAX;
    }
}

td_u32 vdec_cnvt_cmd_id_2_vfmw(td_u32 cmd)
{
    switch (cmd) {
        case VDEC_PRIVATE_CMD_GET_STREAM:
            return VFMW_CID_GET_USRDEC_STREAM;
        case VDEC_PRIVATE_CMD_PUT_STREAM:
            return VFMW_CID_PUT_USRDEC_STREAM;
        case VDEC_PRIVATE_CMD_GET_FRAME:
            return VFMW_CID_GET_USRDEC_FRAME;
        case VDEC_PRIVATE_CMD_PUT_FRAME:
            return VFMW_CID_PUT_USRDEC_FRAME;
        case VDEC_PRIVATE_CMD_BIND_ES_USR_ADDR:
            return VFMW_CID_BIND_ES_USR_ADDR;
        default:
            return VFMW_CID_MAX;
    }
}

vfmw_flush_type vdec_cnvt_flush_type_2_vfmw(ext_vdec_flush_type type)
{
    switch (type) {
        case EXT_VDEC_FLUSH_IN:
            return VFMW_FLUSH_IN;
        case EXT_VDEC_FLUSH_OUT:
            return VFMW_FLUSH_OUT;
        case EXT_VDEC_FLUSH_ALL:
            return VFMW_FLUSH_ALL;
        default:
            return VFMW_FLUSH_MAX;
    }
}

vfmw_pts_framrate_type vdec_cnvt_frmrate_type_2_vfmw(ext_vdec_frame_rate_type type)
{
    switch (type) {
        case EXT_VDEC_FR_TYPE_PTS:
            return PTS_FRMRATE_TYPE_PTS;
        case EXT_VDEC_FR_TYPE_STREAM:
            return PTS_FRMRATE_TYPE_STREAM;
        case EXT_VDEC_FR_TYPE_USER:
            return PTS_FRMRATE_TYPE_USER;
        case EXT_VDEC_FR_TYPE_USER_PTS:
            return PTS_FRMRATE_TYPE_USER_PTS;
        default:
            return PTS_FRMRATE_TYPE_MAX;
    }
}

td_u64 vdec_cnvt_event_map_2_vfmw(td_u64 vdec_event_map)
{
    td_s32 i;
    td_u32 vfmw_event_map = 0;
    td_u32 event_table[][VDEC_EVENT_TABLE_ITEM_SIZE] = {
        { EXT_VDEC_EVT_NEW_FRAME,       EVNT_NEW_IMAGE },
        { EXT_VDEC_EVT_FREE_STREAM,     EVNT_RLS_STREAM },
        { EXT_VDEC_EVT_EOS,             EVNT_LAST_FRAME },
        { EXT_VDEC_EVT_ERR_FRAME,       EVNT_ERR_FRAME },
        { EXT_VDEC_EVT_ERR_UNSUPPORT,   EVNT_UNSUPPORT | EVNT_UNSUPPORT_SPEC },
        { EXT_VDEC_EVT_ERR_DECODE,      EVNT_REPORT_ERR },
        { EXT_VDEC_EVT_SIZE_CHG,        EVNT_RESOLUTION_CHANGE },
        { EXT_VDEC_EVT_OUT_OF_MEMORY,   EVNT_OUT_OF_MEMORY_ERR },
        { EXT_VDEC_EVT_UNKNOWN,         EVNT_MAX },
    };

    for (i = 0; event_table[i][0] != EXT_VDEC_EVT_UNKNOWN; i++) {
        if ((vdec_event_map & event_table[i][0]) != 0) {
            vfmw_event_map |= event_table[i][1];
        }
    }

    return vfmw_event_map;
}

vfmw_yuv_fmt vdec_cnvt_yuv_fmt_2_vfmw(vdec_yuv_fmt vdec_fmt)
{
    switch (vdec_fmt) {
        case VDEC_YUV_SP400:
            return SPY_CB_CR_400;
        case VDEC_YUV_SP411:
            return SPY_CB_CR_411;
        case VDEC_YUV_SP420:
            return SPY_CB_CR_420;
        case VDEC_YUV_SP422_1X2:
            return SPY_CB_CR_422_1X2;
        case VDEC_YUV_SP422_2X1:
            return SPY_CB_CR_422_2X1;
        case VDEC_YUV_SP444:
            return SPY_CB_CR_444;
        case VDEC_YUV_P400:
            return PLNY_CB_CR_400;
        case VDEC_YUV_P411:
            return PLNY_CB_CR_411;
        case VDEC_YUV_P420:
            return PLNY_CB_CR_420;
        case VDEC_YUV_P422_1X2:
            return PLNY_CB_CR_422_1X2;
        case VDEC_YUV_P422_2X1:
            return PLNY_CB_CR_422_2X1;
        case VDEC_YUV_P444:
            return PLNY_CB_CR_444;
        case VDEC_YUV_P410:
            return PLNY_CB_CR_410;
        default:
            return VFMW_YUV_FMT_MAX;
    }
}

td_void vdec_cnvt_ext_frm_2_vdec(vfmw_ext_frm_info *vfmw_ext_frm, ext_vdec_ext_frm_info *vdec_ext_frm)
{
    vdec_check(vfmw_ext_frm != TD_NULL);
    vdec_check(vdec_ext_frm != TD_NULL);

    vdec_ext_frm->uvmos_info.avg_mv = vfmw_ext_frm->uvmos_info.avg_mv;
    vdec_ext_frm->uvmos_info.avg_qp = vfmw_ext_frm->uvmos_info.avg_qp;
    vdec_ext_frm->uvmos_info.frame_stream_size = vfmw_ext_frm->uvmos_info.frame_stream_size;
    vdec_ext_frm->uvmos_info.max_mv = vfmw_ext_frm->uvmos_info.max_mv;
    vdec_ext_frm->uvmos_info.min_mv = vfmw_ext_frm->uvmos_info.min_mv;
    vdec_ext_frm->uvmos_info.skip_ratio = vfmw_ext_frm->uvmos_info.skip_ratio;
    vdec_ext_frm->usd_num = vfmw_ext_frm->usd_num;

    switch (vfmw_ext_frm->uvmos_info.frm_type) {
        case VFMW_FRAME_TYPE_I:
            vdec_ext_frm->uvmos_info.frm_type = EXT_VDEC_FRM_TYPE_I;
            break;
        case VFMW_FRAME_TYPE_P:
            vdec_ext_frm->uvmos_info.frm_type = EXT_VDEC_FRM_TYPE_P;
            break;
        case VFMW_FRAME_TYPE_B:
            vdec_ext_frm->uvmos_info.frm_type = EXT_VDEC_FRM_TYPE_B;
            break;
        default:
            vdec_ext_frm->uvmos_info.frm_type = EXT_VDEC_FRM_TYPE_MAX;
            break;
    }
}

td_void vdec_cnvt_usrdat_2_vdec(vfmw_usrdat_ext *vfmw_usr, ext_vdec_user_data *vdec_usr)
{
    vdec_check(vfmw_usr != TD_NULL);
    vdec_check(vdec_usr != TD_NULL);

    vdec_usr->broadcast = EXT_VDEC_BROADCAST_DVB;
    vdec_usr->fd = vfmw_usr->fd;
    vdec_usr->is_over_flow = vfmw_usr->is_over_flow;
    vdec_usr->is_top_field_first = vfmw_usr->is_top_field_first;
    vdec_usr->position = vdec_cnvt_get_usd_pos(vfmw_usr->position);
    vdec_usr->pts = vfmw_usr->pts;
    vdec_usr->seq_cnt = vfmw_usr->seq_cnt;
    vdec_usr->seq_frm_cnt = vfmw_usr->seq_frm_cnt;
    vdec_usr->type = vdec_cnvt_usrdat_type_2_vdec(vfmw_usr->type);
    vdec_usr->usd_offset = vfmw_usr->usd_offset;
    vdec_usr->user_data_len = vfmw_usr->user_data_len;
}

td_void vdec_cnvt_usrdat_2_vfmw(ext_vdec_user_data *vdec_usr, vfmw_usrdat_ext *vfmw_usr)
{
    vdec_check(vdec_usr != TD_NULL);
    vdec_check(vfmw_usr != TD_NULL);

    vfmw_usr->fd = vdec_usr->fd;
    vfmw_usr->usd_offset = vdec_usr->usd_offset;
    vfmw_usr->user_data_len = vdec_usr->user_data_len;
    vfmw_usr->type = vdec_cnvt_usrdat_type_2_vfmw(vdec_usr->type);
}

vfmw_frm_layout vdec_cnvt_frm_layout_2_vfmw(ext_vdec_frame_layout frm_layout_mode)
{
    switch (frm_layout_mode) {
        case EXT_VDEC_FRM_LAYOUT_AUTO:
            return VFMW_FRM_LAYOUT_ADPT;
        case EXT_VDEC_FRM_LAYOUT_LINEAR:
            return VFMW_FRM_LAYOUT_LINEAR;
        default:
            return VFMW_FRM_LAYOUT_ADPT;
    }
}

vfmw_bit_depth_attr vdec_cnvt_bitdepth_mode_2_vfmw(ext_vdec_bit_depth bit_depth_mode)
{
    switch (bit_depth_mode) {
        case EXT_VDEC_BIT_DEPTH_AUTO:
            return VFMW_BIT_DEPTH_NORMAL;
        case EXT_VDEC_BIT_DEPTH_8:
            return VFMW_BIT_DEPTH_8BIT_ONLY;
        default:
            return VFMW_BIT_DEPTH_NORMAL;
    }
}
