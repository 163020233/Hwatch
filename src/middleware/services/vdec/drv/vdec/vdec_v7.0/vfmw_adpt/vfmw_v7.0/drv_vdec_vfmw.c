/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2019-04-22
 */

#include "drv_vdec_vfmw.h"
#include "vdec_priv_type.h"
#include "drv_vdec_proc.h"
#include "drv_vdec_sdk.h"
#include "drv_vdec_convert.h"
#include "securec.h"
#include "vdec_chr.h"

#define VDEC_UNSUPPORT_TIME   (6 * 1000 * 1000)

typedef struct {
    td_s64 frame_handle;
    td_u64 frame_offset;
    td_u64 frame_dma_buf;
    td_u32 frame_phy_addr;

    td_s64 meta_handle;
    td_u64 meta_offset;
    td_u64 meta_dma_buf;
    td_u64 meta_phy_addr;
    td_u64 meta_vir_addr;
    td_u32 meta_size;
} vdec_vfmw_frm_desc;

typedef struct {
    td_s32 vdec_chan_id;
    vfmw_chan_option option;
    ext_vdec_output_order out_order;
    ext_vdec_frame_buf_mode frm_buf_mode;
    td_bool is_sec;
    td_bool is_used;
    td_bool is_eos;
    vdec_vfmw_frm_desc frm_buf[VDEC_MAX_FRAME_BUF_NUM];
    td_u32 frm_buf_num;
    td_handle es_buf_fd;
    fn_vdec_vfmw_callback call_back;
    td_u32 last_fail_time; /* used for unsupport event */
    td_bool first_frame_done; /* used for unsupport event can not clear after create */
    td_bool first_stream_done; /* used for unsupport event can not clear after create */
} vdec_vfmw_chan;

typedef struct {
    vfmw_export_fun vfmw_func;
    vdec_vfmw_chan *chan[VDEC_MAX_CHAN_NUM];
    vdec_osal_atomic init_cnt;
    vfmw_cap cap;
} vdec_vfmw_entry;

typedef td_s32 (*fun_command)(td_s32 chan_id, td_u8 *param, td_u32 param_size);

typedef struct {
    ext_vdec_cmd_id id;
    fun_command cmd_fun;
} vdec_vfmw_cmd_item;

vdec_vfmw_entry g_vfmw_entry = {
    .chan = { TD_NULL },
};

STATIC vdec_vfmw_entry* vdec_vfmw_get_entry(td_void)
{
    return &g_vfmw_entry;
}

STATIC vdec_vfmw_chan* vdec_vfmw_get_chan(td_s32 vdec_vfmw_id)
{
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();

    if (vdec_vfmw_id < 0 || vdec_vfmw_id >= VDEC_MAX_CHAN_NUM) {
        return TD_NULL;
    }

    return entry->chan[vdec_vfmw_id];
}

STATIC td_void vdec_vfmw_set_chan_option(vfmw_chan_option *option, const vdec_vfmw_create_param *param)
{
    option->adpt_type = ADPT_TYPE_VDEC;
    option->vdec_handle = param->vdec_handle;
    option->is_sec_mode = 0;
    option->vid_std = vdec_cnvt_std_2_vfmw(param->attr->standard);

    if (param->attr->standard == EXT_VDEC_STD_HEIF) {
        option->seg_buf_size = (td_s32)param->attr->max_stream_size;
    }
}

STATIC td_s32 vdec_vfmw_event_call_back(td_s32 chan_id, td_s32 type, const td_void *args, td_s32 len)
{
    td_s32 ret;
    ext_vdec_event out = {0};
    vdec_vfmw_chan *chan = TD_NULL;

    ret = vdec_cnvt_event_2_vdec(chan_id, type, args, len, &out);
    if (ret != EXT_SUCCESS) {
        vdec_print_err("chan %d convert event %d error!\n", chan_id, type);
        return EXT_FAILURE;
    }

    chan = vdec_vfmw_get_chan(chan_id);
    vdec_check_ret(chan != TD_NULL);
    if (chan->call_back != TD_NULL) {
        ret = chan->call_back(chan->vdec_chan_id, out.event_id, &out.event_param, sizeof(out.event_param));
        if (ret != EXT_SUCCESS) {
            vdec_print_dbg("chan %d event %d call event callback func fail!\n", chan_id, out.event_id);
            return EXT_FAILURE;
        }
    }

    vdec_print_dbg("chan %d report event %d\n", chan_id, out.event_id);

    return EXT_SUCCESS;
}

td_s32 vdec_vfmw_is_std_support(ext_vdec_opt_is_std_support *support)
{
    td_u32 i;
    vfmw_vid_std standard;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();

    support->support = TD_FALSE;
    standard = vdec_cnvt_std_2_vfmw(support->standard);
    if (support->user_dec == TD_FALSE && standard == VFMW_MJPEG) {
        return EXT_SUCCESS;
    }

    for (i = 0; i < VFMW_STD_MAX; i++) {
        if (entry->cap.supported_std[i] != VFMW_STD_MAX &&
            entry->cap.supported_std[i] == standard) {
            support->support = TD_TRUE;
            break;
        }
    }

    return EXT_SUCCESS;
}

STATIC td_s32 vdec_vfmw_get_usrdec_stream(td_s32 chan_id, td_u8 *param, td_u32 size)
{
    td_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    vfmw_stream_buf stream_data = {0};
    ext_vdec_stream *stream = (ext_vdec_stream*)param;

    vdec_check_ret(param != TD_NULL);
    vdec_check_ret(size == sizeof(ext_vdec_stream));

    ret = entry->vfmw_func.fn_vfmw_control(chan_id, VFMW_CID_GET_USRDEC_STREAM, &stream_data, sizeof(stream_data));
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

    (td_void)memset_s(stream, sizeof(ext_vdec_stream), 0, sizeof(ext_vdec_stream));
    stream->start_addr = stream_data.phy_addr;
    stream->pts = stream_data.pts;
    stream->length = stream_data.length;
    stream->vir_addr = (td_u8 *)(uintptr_t)(stream_data.phy_addr);
    stream->is_end_of_frame = stream_data.is_end_of_frm;
    return EXT_SUCCESS;
}

STATIC td_s32 vdec_vfmw_put_usrdec_stream(td_s32 chan_id, td_u8 *param, td_u32 size)
{
    td_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    vfmw_stream_buf stream_data = {0};
    ext_vdec_stream *stream = (ext_vdec_stream*)param;

    vdec_check_ret(param != TD_NULL);
    vdec_check_ret(size == sizeof(ext_vdec_stream));

    stream_data.phy_addr = stream->start_addr;
    stream_data.pts = stream->pts;
    stream_data.length = stream->length;
    stream_data.is_end_of_frm = stream->is_end_of_frame;

    ret = entry->vfmw_func.fn_vfmw_control(chan_id, VFMW_CID_PUT_USRDEC_STREAM, &stream_data, sizeof(stream_data));
    if (ret != EXT_SUCCESS) {
        vdec_print_dbg("vfmw chan %d rel stream fail!\n", chan_id);
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

STATIC td_s32 vdec_vfmw_get_usrdec_frame(td_s32 chan_id, td_u8 *param, td_u32 size)
{
    td_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    vfmw_usrdec_frame_desc usr_frame = {0};
    vdec_usrdec_frame *frame = (vdec_usrdec_frame*)param;

    vdec_check_ret(param != TD_NULL);
    vdec_check_ret(size == sizeof(vdec_usrdec_frame));

    usr_frame.width = frame->width;
    usr_frame.height = frame->height;
    usr_frame.mem_size = frame->mem_size;
    usr_frame.luma_stride = frame->luma_stride;
    usr_frame.bit_depth = frame->bit_depth;
    ret = entry->vfmw_func.fn_vfmw_control(chan_id, VFMW_CID_GET_USRDEC_FRAME, &usr_frame, sizeof(usr_frame));
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

    frame->phy_addr = usr_frame.phy_addr;
    frame->line_num_offset = usr_frame.line_num_offset;
    frame->line_num_size = usr_frame.line_num_size;
    frame->mem_handle = usr_frame.mem_handle;
    frame->mem_size = usr_frame.mem_size;

    return EXT_SUCCESS;
}

STATIC td_s32 vdec_vfmw_put_usrdec_frame(td_s32 chan_id, td_u8 *param, td_u32 size)
{
    td_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    vfmw_usrdec_frame_desc usr_frame = {0};
    vdec_usrdec_frame *frame = (vdec_usrdec_frame*)param;

    vdec_check_ret(param != TD_NULL);
    vdec_check_ret(size == sizeof(vdec_usrdec_frame));

    usr_frame.is_frame_valid = frame->is_frame_valid;
    usr_frame.fmt = vdec_cnvt_yuv_fmt_2_vfmw(frame->fmt);
    usr_frame.pts = frame->pts;
    usr_frame.disp_time = frame->disp_time;
    usr_frame.bit_depth = frame->bit_depth;
    usr_frame.width = frame->width;
    usr_frame.height = frame->height;
    usr_frame.luma_stride = frame->luma_stride;
    usr_frame.chrom_stride = frame->chrom_stride;
    usr_frame.phy_addr = frame->phy_addr;
    usr_frame.chrom_offset = frame->chrom_offset;
    usr_frame.line_num_offset = frame->line_num_offset;
    usr_frame.mem_handle = frame->mem_handle;
    usr_frame.mem_size = frame->mem_size;

    ret = entry->vfmw_func.fn_vfmw_control(chan_id, VFMW_CID_PUT_USRDEC_FRAME, &usr_frame, sizeof(usr_frame));
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

td_s32 vdec_vfmw_send_eos(td_s32 chan_id)
{
    td_s32 ret;
    td_bool eos = TD_TRUE;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    vdec_vfmw_chan *chan = vdec_vfmw_get_chan(chan_id);

    vdec_check_ret(chan != TD_NULL);

    chan->is_eos = eos;
    ret = entry->vfmw_func.fn_vfmw_control(chan_id, VFMW_CID_SEND_EOS, &eos, sizeof(td_bool));
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

    vdec_print_info("chan:%d eos:%d\n", chan_id, chan->is_eos);

    return EXT_SUCCESS;
}

td_s32 vdec_vfmw_bind_extern_frame_buffer(td_s32 chan_id, ext_vdec_frame_buf *buf)
{
    td_s32 ret;
    vfmw_ext_buf ext_buf = { 0 };
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    vdec_vfmw_chan *chan = vdec_vfmw_get_chan(chan_id);

    vdec_check_ret(chan != TD_NULL);
    vdec_check_ret(buf != TD_NULL);
    vdec_check_ret(chan->frm_buf_num < VDEC_MAX_FRAME_BUF_NUM);

    ext_buf.ext_frm_buf.size = buf->frame_size;
    ext_buf.ext_frm_buf.phy_addr = buf->phy_addr;
    ext_buf.ext_frm_buf.vir_addr = buf->phy_addr;

    chan->frm_buf[chan->frm_buf_num].frame_phy_addr = buf->phy_addr;
    chan->frm_buf[chan->frm_buf_num].meta_dma_buf = 0;
    chan->frm_buf[chan->frm_buf_num].meta_phy_addr = 0;

    ret = entry->vfmw_func.fn_vfmw_control(chan_id, VFMW_CID_BIND_EXTERN_BUFFER, &ext_buf,
        sizeof(vfmw_ext_buf));
    if (ret != EXT_SUCCESS) {
        vdec_print_alert("chan %d bind extern buffer 0x%x fail!\n", chan_id, ext_buf.ext_frm_buf.phy_addr);
        return EXT_FAILURE;
    }

    chan->frm_buf_num++;

    return EXT_SUCCESS;
}

td_s32 vdec_vfmw_init_input_buf(td_s32 chan_id, td_u32 len)
{
    td_s32 ret;
    vfmw_stream_buf buf = {0};
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();

    vdec_check_ret(len > 0 && len <= MAX_VID_INPUT_BUFFER_SIZE);
    ret = entry->vfmw_func.fn_vfmw_control(chan_id, VFMW_CID_INIT_ES_BUF, &len, sizeof(td_u32));
    if (ret != EXT_SUCCESS) {
        vdec_print_alert("init input buf error!\n");
        return EXT_FAILURE;
    }

    ret = entry->vfmw_func.fn_vfmw_control(chan_id, VFMW_CID_BIND_ES_BUF, &buf, sizeof(buf));
    if (ret != EXT_SUCCESS) {
        entry->vfmw_func.fn_vfmw_control(chan_id, VFMW_CID_DEINIT_ES_BUF, TD_NULL, 0);
        vdec_print_alert("vfmw chan %d bind es buffer fail!\n", chan_id);
        return EXT_FAILURE;
    }

    vdec_print_info("chan:%d\n", chan_id);

    return EXT_SUCCESS;
}

td_s32 vdec_vfmw_deinit_input_buf(td_s32 chan_id)
{
    td_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();

    ret = entry->vfmw_func.fn_vfmw_control(chan_id, VFMW_CID_UNBIND_ES_BUF, TD_NULL, 0);
    if (ret != EXT_SUCCESS) {
        vdec_print_alert("vfmw chan %d unbind es buffer fail!\n", chan_id);
        return EXT_FAILURE;
    }

    ret = entry->vfmw_func.fn_vfmw_control(chan_id, VFMW_CID_DEINIT_ES_BUF, TD_NULL, 0);
    if (ret != EXT_SUCCESS) {
        vdec_print_alert("chan %d deinit input buf fail!\n", chan_id);
        return EXT_FAILURE;
    }
    vdec_record_chr_event(chan_id, MT_VDEC_DESTROY, MT_VDEC_STORE_CLOSE, MT_VDEC_VFMW_DEINIT_INPUT_BUF_OK);

    vdec_print_info("chan:%d\n", chan_id);

    return EXT_SUCCESS;
}

td_s32 vdec_vfmw_get_cap(ext_vdec_capability *cap)
{
    td_u32 cnt;
    vfmw_vid_std vfmw_std;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();

    vdec_check_ret(cap != TD_NULL);

    vfmw_std = vdec_cnvt_std_2_vfmw(cap->standard);

    for (cnt = 0; cnt < VFMW_STD_MAX; cnt++) {
        if (vfmw_std == entry->cap.supported_std[cnt]) {
            break;
        }
    }

    if (cnt >= VFMW_STD_MAX ||
        vfmw_std == VFMW_STD_MAX) {
        cap->max_width = 0;
        cap->max_height = 0;
        cap->bit_depth_num = 0;
        cap->max_frame_rate = 0;
        cap->frame_layout_num = 0;
        return EXT_FAILURE;
    }

    cap->max_width = entry->cap.max_width[cnt];
    cap->max_height = entry->cap.max_height[cnt];
    vdec_cnvt_frm_layout_cap_2_vdec(entry->cap.frm_layout_cap, cap);
    vdec_cnvt_bitdepth_cap_2_vdec(entry->cap.bit_depth_cap, cap);
    cap->max_frame_rate = entry->cap.max_frm_rate[cnt];
    return EXT_SUCCESS;
}

STATIC td_s32 vdec_vfmw_check_frame_state(td_s32 chan_id, td_u8 *param, td_u32 size)
{
    td_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();

    ret = entry->vfmw_func.fn_vfmw_control(chan_id, VFMW_CID_CHECK_FRM_STATE, param, size);
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

td_s32 vdec_vfmw_set_frame_rate(td_s32 chan_id, ext_vdec_frame_rate *frame_rate)
{
    td_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    vfmw_pts_frmrate frm_rate;

    frm_rate.en_frm_rate_type = vdec_cnvt_frmrate_type_2_vfmw(frame_rate->type);
    frm_rate.st_set_frm_rate.fps_integer = frame_rate->frame_rate / 1000; /* 1000 :a number */
    frm_rate.st_set_frm_rate.fps_decimal = frame_rate->frame_rate % 1000; /* 1000 :a number */

    ret = entry->vfmw_func.fn_vfmw_control(chan_id, VFMW_CID_SET_FRAME_RATE_TYPE, &frm_rate, sizeof(frm_rate));
    if (ret != EXT_SUCCESS) {
        vdec_print_alert("vfmw chan:%d set frame rate type fail!\n", chan_id);
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

td_s32 vdec_vfmw_get_frame_rate(td_s32 chan_id, ext_vdec_frame_rate *frame_rate)
{
    td_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    vfmw_pts_frmrate frm_rate;
    td_u64 frame_rate_total;

    ret = entry->vfmw_func.fn_vfmw_control(chan_id, VFMW_CID_GET_FRAME_RATE_TYPE, &frm_rate, sizeof(frm_rate));
    if (ret != EXT_SUCCESS) {
        vdec_print_alert("get vfmw frame rate type error!\n");
        return EXT_FAILURE;
    }

    frame_rate->type = vdec_cnvt_frmrate_type_2_vdec(frm_rate.en_frm_rate_type);
    frame_rate_total = (td_u64)frm_rate.st_set_frm_rate.fps_integer * 1000 + /* 1000 :a number */
        frm_rate.st_set_frm_rate.fps_decimal;
    if (frame_rate_total <= 0xFFFFFFFF) { /* 1000 :a number, 0xFFFFFFFF: uint limit */
        frame_rate->frame_rate = (td_u32)(frame_rate_total);
    } else {
        vdec_print_err("%s %d overflow!\n", __func__, __LINE__);
    }

    return EXT_SUCCESS;
}

td_s32 vdec_vfmw_get_low_delay(td_s32 chan_id, td_bool *low_delay)
{
    td_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    vfmw_chan_cfg cfg = {0};

    ret = entry->vfmw_func.fn_vfmw_control(chan_id, VFMW_CID_GET_CHAN_CFG, &cfg, sizeof(vfmw_chan_cfg));
    if (ret != EXT_SUCCESS) {
        vdec_print_alert("get chan %d vfmw cfg fail!\n", chan_id);
        return EXT_FAILURE;
    }

    *low_delay = cfg.module_lowly_enable;

    return EXT_SUCCESS;
}

td_s32 vdec_vfmw_set_low_delay(td_s32 chan_id, td_bool low_delay)
{
    td_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    vfmw_chan_cfg cfg = {0};

    ret = entry->vfmw_func.fn_vfmw_control(chan_id, VFMW_CID_GET_CHAN_CFG, &cfg, sizeof(vfmw_chan_cfg));
    if (ret != EXT_SUCCESS) {
        vdec_print_alert("get chan %d vfmw cfg fail!\n", chan_id);
        return EXT_FAILURE;
    }

    cfg.module_lowly_enable = low_delay;

    ret = entry->vfmw_func.fn_vfmw_control(chan_id, VFMW_CID_CFG_CHAN, &cfg, sizeof(vfmw_chan_cfg));
    if (ret != EXT_SUCCESS) {
        vdec_print_alert("set chan %d vfmw cfg fail!\n", chan_id);
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

STATIC td_void vdec_vfmw_set_hdr_attr(const ext_vdec_hdr_attr *hdr_attr, vfmw_chan_cfg *cfg)
{
    if (hdr_attr->chan_type != EXT_VDEC_HDR_CHAN_NONE) {
        cfg->be_hdr_chan = 1;

        if (hdr_attr->chan_type == EXT_VDEC_HDR_CHAN_BL) {
            cfg->dv_capacity = DV_BL_DECODING_SUPPORT;
        } else if (hdr_attr->chan_type == EXT_VDEC_HDR_CHAN_EL) {
            cfg->dv_capacity = DV_EL_DECODING_SUPPORT;
        } else {
            vdec_print_alert("hdr info error!");
        }

        if (hdr_attr->stm_type == EXT_VDEC_HDR_SS_DL_VES || hdr_attr->stm_type == EXT_VDEC_HDR_SS_SL_VES) {
            cfg->ves_imp = DV_SINGLE_VES_IMP;
        } else if (hdr_attr->stm_type == EXT_VDEC_HDR_DS_DL_VES) {
            cfg->ves_imp = DV_DUAL_VES_IMP;
        } else {
            cfg->ves_imp = DV_VES_IMP_MAX;
        }

        if (hdr_attr->stm_type == EXT_VDEC_HDR_SS_SL_VES) {
            cfg->layer_imp = DV_SINGLE_LAYER_IMP;
        } else if (hdr_attr->stm_type == EXT_VDEC_HDR_SS_DL_VES ||
            hdr_attr->stm_type == EXT_VDEC_HDR_DS_DL_VES) {
            cfg->layer_imp = DV_DUAL_LAYER_IMP;
        } else {
            cfg->layer_imp = DV_LAYER_IMP_MAX;
        }

        cfg->dv_compatible = hdr_attr->is_dv_frm_compatible;
    } else {
        cfg->be_hdr_chan = 0;
    }

    return;
}

td_s32 vdec_vfmw_get_feature(td_s32 chan_id, ext_vdec_feature *feature)
{
    td_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    vfmw_chan_info vfmw_status = {0};

    ret = entry->vfmw_func.fn_vfmw_control(chan_id, VFMW_CID_GET_CHAN_STATE, &vfmw_status, sizeof(vfmw_chan_info));
    if (ret != EXT_SUCCESS) {
        vdec_print_alert("get vfmw chan status fail!\n");
        return EXT_FAILURE;
    }

    feature->sub_version = EXT_VDEC_SUB_STD_UNKNOWN;
    feature->sub_version = 0;
    feature->profile = vfmw_status.profile;
    feature->level = vfmw_status.level;
    feature->sampling_type = vdec_cnvt_sampling_type_2_vdec(vfmw_status.scan_type);
    feature->aspect_height = vfmw_status.aspect_height;
    feature->aspect_width = vfmw_status.aspect_width;
    feature->bit_rate = vfmw_status.bit_rate;
    feature->frame_rate = vfmw_status.frame_rate;
    feature->dec_width = vfmw_status.image_width;
    feature->dec_height = vfmw_status.image_height;
    feature->disp_width = vfmw_status.disp_width;
    feature->disp_height = vfmw_status.disp_height;
    feature->disp_center_x = vfmw_status.disp_center_x;
    feature->disp_center_y = vfmw_status.disp_center_y;
    feature->bit_depth = vdec_cnvt_bit_depth_2_vdec(vfmw_status.bit_depth);
    feature->color_space = vdec_cnvt_color_space_2_vdec(vfmw_status.std, &vfmw_status.color_info,
        vfmw_status.image_width, vfmw_status.image_height);

    vdec_print_dbg("std:%x profile:%x level:%x sample_type:%d aspect_height:%d aspect_width:%d bit_rate:%d "
        "frame_rate:%d dec_width:%d dec_height:%d disp_height:%d disp_width:%d center_x:%d center_y:%d "
        "bit_depth:%d color_space:%d\n", feature->standard, feature->profile, feature->level,
        feature->sampling_type, feature->aspect_height, feature->aspect_width, feature->bit_rate, feature->frame_rate,
        feature->dec_width, feature->dec_height, feature->disp_height, feature->disp_width,
        feature->disp_center_x, feature->disp_center_y, feature->bit_depth, feature->color_space);

    return EXT_SUCCESS;
}

td_s32 vdec_vfmw_get_status(td_s32 chan_id, ext_vdec_status *status)
{
    td_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    vfmw_chan_info vfmw_status = {0};

    ret = entry->vfmw_func.fn_vfmw_control(chan_id, VFMW_CID_GET_CHAN_STATE, &vfmw_status, sizeof(vfmw_chan_info));
    if (ret != EXT_SUCCESS) {
        vdec_print_alert("get vfmw chan status fail!\n");
        return EXT_FAILURE;
    }

    status->stream_state.bit_rate_in_bps = vfmw_status.bit_rate;
    status->stream_state.free_stream_buffer_size = vfmw_status.free_stm_buf_size;
    status->stream_state.total_stream_buffer_size = vfmw_status.total_stm_buf_size;
    status->stream_state.undecode_stream_num = vfmw_status.undecode_stm_num;
    status->stream_state.undecode_stream_buffer_size = vfmw_status.undecode_stm_size +
        (td_u32)vfmw_status.buffered_stream_size;
    status->stream_state.used_stream_buffer_size = vfmw_status.used_stm_buf_size;

    status->frame_state.error_frame_num = (td_u32)vfmw_status.error_frame_num;
    status->frame_state.total_output_frame_num = (td_u32)vfmw_status.total_frame_num;
    status->frame_state.wait_output_frame_num = vfmw_status.wait_disp_frame_num;
    status->frame_state.display_frame_rate = vfmw_status.frame_rate;
    status->frame_state.decode_frame_rate = vfmw_status.dec_frm_rate;

    vdec_print_dbg("bitrate:%d free_stm_buf %x eos:%d total_stm_buf:%x undecoded_num:%d undecoded size:%x "
        "used_stm_size:%x err_frm_num:%d total_out:%d wait_out:%d fps:%d field_flag:%d pvr_buf_num: %d "
        "dec_fps:%d \n",
        vfmw_status.bit_rate, vfmw_status.free_stm_buf_size, vfmw_status.is_end_of_stm, vfmw_status.total_stm_buf_size,
        vfmw_status.undecode_stm_num, vfmw_status.undecode_stm_size, vfmw_status.used_stm_buf_size,
        vfmw_status.error_frame_num, vfmw_status.total_frame_num, vfmw_status.wait_disp_frame_num,
        vfmw_status.frame_rate, vfmw_status.is_field_flg, vfmw_status.pvr_buf_num, vfmw_status.dec_frm_rate);

    return EXT_SUCCESS;
}

td_s32 vdec_vfmw_open(td_void)
{
    td_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();

    ret = vdec_osal_atomic_init(&entry->init_cnt);
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

    ret = vdec_drv_get_vfmw_func(&entry->vfmw_func);
    if (ret != EXT_SUCCESS) {
        vdec_osal_atomic_exit(&entry->init_cnt);
        vdec_print_alert("get vfmw ext func fail!\n");
        return EXT_FAILURE;
    }

    ret = entry->vfmw_func.fn_vfmw_control(VDEC_INVALID_HANDLE, VFMW_CID_GET_CAPABILITY,
        &entry->cap, sizeof(entry->cap));
    if (ret != EXT_SUCCESS) {
        vdec_osal_atomic_exit(&entry->init_cnt);
        vdec_print_alert("get vfmw cap fail!\n");
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

td_s32 vdec_vfmw_close(td_void)
{
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();

    vdec_osal_atomic_exit(&entry->init_cnt);

    return EXT_SUCCESS;
}

td_s32 vdec_vfmw_init(td_void)
{
    td_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    vfmw_init_param operation = {0};

    if (vdec_osal_atomic_inc_return(&entry->init_cnt) == 1) {
        operation.adpt_type = ADPT_TYPE_VDEC;
        operation.fn_callback = vdec_vfmw_event_call_back;
        ret = entry->vfmw_func.fn_vfmw_init(&operation);
        if (ret != EXT_SUCCESS) {
            vdec_osal_atomic_dec(&entry->init_cnt);
            vdec_print_alert("vfmw init error!\n");
            return EXT_FAILURE;
        }
    }

    return EXT_SUCCESS;
}

td_s32 vdec_vfmw_exit(td_void)
{
    td_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    td_bool is_sec = TD_FALSE;

    if (vdec_osal_atomic_dec_return(&entry->init_cnt) == 0) {
        ret = entry->vfmw_func.fn_vfmw_exit(&is_sec);
        if (ret != EXT_SUCCESS) {
            vdec_osal_atomic_inc(&entry->init_cnt);
            vdec_print_alert("vfmw exit error!\n");
            return EXT_FAILURE;
        }
    }
    vdec_record_chr_event(-1, MT_VDEC_DESTROY, MT_VDEC_STORE_CLOSE, MT_VDEC_VFMW_EXIT_OK);

    return EXT_SUCCESS;
}

td_s32 vdec_vfmw_suspend(td_void)
{
    td_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();

    ret = vdec_drv_get_vfmw_func(&entry->vfmw_func);
    if (ret != EXT_SUCCESS) {
        vdec_print_alert("get vfmw ext func fail!\n");
        return EXT_FAILURE;
    }

    ret = entry->vfmw_func.fn_vfmw_suspend(TD_NULL);
    if (ret != EXT_SUCCESS) {
        vdec_print_alert("vfmw suspend fail!\n");
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

td_s32 vdec_vfmw_resume(td_void)
{
    td_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();

    ret = vdec_drv_get_vfmw_func(&entry->vfmw_func);
    if (ret != EXT_SUCCESS) {
        vdec_print_alert("get vfmw ext func fail!\n");
        return EXT_FAILURE;
    }

    ret = entry->vfmw_func.fn_vfmw_resume(TD_NULL);
    if (ret != EXT_SUCCESS) {
        vdec_print_alert("vfmw resume fail!\n");
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

td_s32 vdec_vfmw_freeze(td_void)
{
    td_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    td_u32 s4_flag = 1;

    ret = vdec_drv_get_vfmw_func(&entry->vfmw_func);
    if (ret != EXT_SUCCESS) {
        vdec_print_alert("get vfmw ext func fail!\n");
        return EXT_FAILURE;
    }

    ret = entry->vfmw_func.fn_vfmw_suspend(&s4_flag);
    if (ret != EXT_SUCCESS) {
        vdec_print_alert("vfmw suspend fail!\n");
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

td_s32 vdec_vfmw_restore(td_void)
{
    td_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    td_u32 s4_flag = 1;

    ret = vdec_drv_get_vfmw_func(&entry->vfmw_func);
    if (ret != EXT_SUCCESS) {
        vdec_print_alert("get vfmw ext func fail!\n");
        return EXT_FAILURE;
    }

    ret = entry->vfmw_func.fn_vfmw_resume(&s4_flag);
    if (ret != EXT_SUCCESS) {
        vdec_print_alert("vfmw resume fail!\n");
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

STATIC td_void vdec_vfmw_init_chan(vdec_vfmw_chan *chan, const vdec_vfmw_create_param *param)
{
    (td_void)memset_s(chan, sizeof(vdec_vfmw_chan), 0, sizeof(vdec_vfmw_chan));

    chan->is_used = TD_TRUE;
    chan->is_sec = 0;
    chan->call_back = param->callback;
    chan->vdec_chan_id = param->vdec_id;
}

td_s32 vdec_vfmw_create(td_s32 *chan_id, const vdec_vfmw_create_param *param)
{
    td_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    vdec_vfmw_chan *chan = TD_NULL;
    vfmw_chan_option option = {0};

    vdec_check_ret(chan_id != TD_NULL);
    vdec_check_ret(param != TD_NULL);
    vdec_check_ret(param->attr != TD_NULL);
    vdec_vfmw_set_chan_option(&option, param);

    ret = entry->vfmw_func.fn_vfmw_control(-1, VFMW_CID_CREATE_CHAN, &option, sizeof(option));
    if (ret != EXT_SUCCESS) {
        vdec_print_alert("vfmw create chan fail!\n");
        return EXT_FAILURE;
    }

    *chan_id = option.chan_id;

    chan = (vdec_vfmw_chan *)osal_vmalloc(sizeof(vdec_vfmw_chan));
    if (chan == TD_NULL) {
        vdec_print_alert("alloc chan fail!\n");
        entry->vfmw_func.fn_vfmw_control(*chan_id, VFMW_CID_DESTROY_CHAN, TD_NULL, 0);
        return EXT_FAILURE;
    }
    (td_void)memset_s(chan, sizeof(vdec_vfmw_chan), 0, sizeof(vdec_vfmw_chan));

    entry->chan[*chan_id] = chan;

    vdec_record_chr_event(*chan_id, MT_VDEC_SET_ATTR, MT_VDEC_SET_HW_ATTR, MT_VFMW_CREATE_CHAN);

    vdec_vfmw_init_chan(chan, param);

    return EXT_SUCCESS;
}

td_s32 vdec_vfmw_destroy(td_s32 chan_id)
{
    td_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    vdec_vfmw_chan *chan = TD_NULL;

    chan = entry->chan[chan_id];
    if (chan == TD_NULL) {
        vdec_print_alert("chan %d is already destroyed!\n", chan_id);
        return EXT_FAILURE;
    }

    ret = entry->vfmw_func.fn_vfmw_control(chan_id, VFMW_CID_DESTROY_CHAN, TD_NULL, 0);
    if (ret != EXT_SUCCESS) {
        vdec_print_alert("vfmw destroy chan %d fail!\n", chan_id);
        return EXT_FAILURE;
    }

    entry->chan[chan_id] = TD_NULL;
    osal_vfree(chan);

    return EXT_SUCCESS;
}

td_s32 vdec_vfmw_start(td_s32 chan_id)
{
    td_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();

    ret = entry->vfmw_func.fn_vfmw_control(chan_id, VFMW_CID_START_CHAN, TD_NULL, 0);
    if (ret != EXT_SUCCESS) {
        vdec_print_alert("vfmw start chan %d fail!\n", chan_id);
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

td_s32 vdec_vfmw_stop(td_s32 chan_id)
{
    td_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();

    ret = entry->vfmw_func.fn_vfmw_control(chan_id, VFMW_CID_STOP_CHAN, TD_NULL, 0);
    if (ret != EXT_SUCCESS) {
        vdec_print_alert("vfmw stop chan %d fail!\n", chan_id);
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

td_s32 vdec_vfmw_reset(td_s32 chan_id)
{
    td_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();

    ret = entry->vfmw_func.fn_vfmw_control(chan_id, VFMW_CID_RESET_CHAN, TD_NULL, 0);
    if (ret != EXT_SUCCESS) {
        vdec_print_alert("vfmw reset chan %d fail!\n", chan_id);
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

td_s32 vdec_vfmw_flush(td_s32 chan_id, const ext_vdec_opt_flush *param)
{
    td_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    vfmw_flush_type flush_type;

    vdec_check_ret(param != TD_NULL);

    flush_type = vdec_cnvt_flush_type_2_vfmw(param->type);
    ret = entry->vfmw_func.fn_vfmw_control(chan_id, VFMW_CID_FLUSH_CHAN, &flush_type, sizeof(flush_type));
    if (ret != EXT_SUCCESS) {
        vdec_print_alert("vfmw flush chan %d fail!\n", chan_id);
        return EXT_FAILURE;
    }
    vdec_record_chr_event(chan_id, MT_VDEC_FLUSH, MT_VDEC_VFMW_FLUSH, MT_VDEC_INFO_3_DEFAULT);

    return EXT_SUCCESS;
}

STATIC td_void vdec_vfmw_get_out_order(vdec_vfmw_chan *chan,
    const ext_vdec_attr *param,  const vdec_vfmw_param *ext_attr, vfmw_chan_cfg *vfmw_cfg)
{
    if (ext_attr->dpb_mode == EXT_VDEC_DPB_MODE_SKYPLAY) {
        vfmw_cfg->dec_order_output = VFMW_DEC_ORDER_SKYPLAY;
    } else if (ext_attr->dpb_mode == EXT_VDEC_DPB_MODE_SIMPLE) {
        vfmw_cfg->dec_order_output = VFMW_DEC_ORDER_SIMPLE_DPB;
    } else {
        vfmw_cfg->dec_order_output = (param->output_order == EXT_VDEC_OUT_ORDER_DISPLAY) ? 0 : 1;
    }
    chan->out_order = (ext_attr->enable_low_delay == TD_TRUE) ? EXT_VDEC_OUT_ORDER_DECODE : param->output_order;
}

td_s32 vdec_vfmw_set_attr(td_s32 chan_id, const ext_vdec_attr *param, const vdec_vfmw_param *ext_attr)
{
    td_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    vdec_vfmw_chan *chan = vdec_vfmw_get_chan(chan_id);
    vfmw_chan_cfg vfmw_cfg = {0};

    vdec_check_ret((param != TD_NULL) && (ext_attr != TD_NULL) && (chan != TD_NULL));

    ret = entry->vfmw_func.fn_vfmw_control(chan_id, VFMW_CID_GET_CHAN_CFG, &vfmw_cfg, sizeof(vfmw_cfg));
    vdec_check_ret(ret == EXT_SUCCESS);

    vdec_record_chr_event(chan_id, MT_VDEC_SET_ATTR, MT_VDEC_SET_HW_ATTR, MT_VFMW_GET_VFMW_CFG_OK);

    vfmw_cfg.is_miracast_chan = (ext_attr->dpb_mode == EXT_VDEC_DPB_MODE_SIMPLE) ? 1 : 0;
    vfmw_cfg.module_lowly_enable = ext_attr->enable_low_delay;
    vfmw_cfg.chan_lowly_enable = 0;
    vfmw_cfg.dec_lowly_enable = 1;
    vdec_check_ret(param->max_stream_num <= VDEC_DFT_MAX_RAW_NUM);
    vfmw_cfg.max_raw_packet_num = (td_s32)param->max_stream_num;

    vdec_vfmw_get_out_order(chan, param, ext_attr, &vfmw_cfg);

    vfmw_cfg.vid_std = vdec_cnvt_std_2_vfmw(param->standard);
    vfmw_cfg.extra_frame_store_num = param->extra_frame_num;
    vfmw_cfg.chan_err_thr = (td_s8)param->error_threshold;
    vfmw_cfg.frame_layout = vdec_cnvt_frm_layout_2_vfmw(param->frame_layout);
    vfmw_cfg.en_bit_depth_cfg = VFMW_BIT_DEPTH_NORMAL;

    vfmw_cfg.dec_mode = vdec_cnvt_decmode_2_vfmw(param->decode_mode);
    vfmw_cfg.dec_mode = (ext_attr->enable_low_delay == TD_TRUE) ? VFMW_DEC_MODE_IP : vfmw_cfg.dec_mode;
    vfmw_cfg.dec_order_output = (ext_attr->enable_low_delay == TD_TRUE) ? VFMW_DEC_ORDER : vfmw_cfg.dec_order_output;
    vfmw_cfg.chan_lowly_enable = (ext_attr->enable_low_delay == TD_TRUE) ? 1 : 0;
    vfmw_cfg.v_cmp_mode = VFMW_CMP_ADJUST;

    vfmw_cfg.is_tunneled_path = 0;
    vfmw_cfg.uvmos_enable = ext_attr->enable_uvmos;

    vfmw_cfg.vdec_handle = chan->vdec_chan_id;
    vfmw_cfg.source = ext_attr->source_tag.tag_value;
    vfmw_cfg.poc_threshold = ext_attr->poc_thrshold;
    vfmw_cfg.usd_pool_size = 0;
    vfmw_cfg.alloc_type = (param->frame_buf_mode == EXT_VDEC_FRM_EXTERNAL) ? VFMW_FRM_ALLOC_OUTER : VFMW_FRM_ALLOC_INNER;
    chan->frm_buf_mode = param->frame_buf_mode;

    vdec_vfmw_set_hdr_attr(&ext_attr->hdr_attr, &vfmw_cfg);
    vfmw_cfg.outer_frm_mode = VFMW_OUTER_FRM_NO_WAIT;

    if (param->max_dec_w_h.is_max_dec_w_h_en == TD_TRUE && param->max_dec_w_h.max_dec_width != 0 &&
        param->max_dec_w_h.max_dec_height != 0) {
        vfmw_cfg.is_max_buf_size_en = TD_TRUE;
        vfmw_cfg.max_dec_width = param->max_dec_w_h.max_dec_width;
        vfmw_cfg.max_dec_height = param->max_dec_w_h.max_dec_height;
    }

    ret = entry->vfmw_func.fn_vfmw_control(chan_id, VFMW_CID_CFG_CHAN, &vfmw_cfg, sizeof(vfmw_cfg));
    if (ret != EXT_SUCCESS) {
        vdec_print_alert("vfmw set chan %d cfg fail!\n", chan_id);
    }

    vdec_record_chr_event(chan_id, MT_VDEC_SET_ATTR, MT_VDEC_SET_HW_ATTR, MT_VFMW_CFG_CHAN_OK);

    return ret;
}

td_s32 vdec_vfmw_set_event_map(td_s32 chan_id, td_u64 event_map)
{
    td_s32 ret;
    td_u64 vfmw_event_map;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();

    vfmw_event_map = vdec_cnvt_event_map_2_vfmw(event_map);

    vdec_print_dbg("vdec_event_map %llx vfmw_event_map %llx\n", event_map, vfmw_event_map);

    ret = entry->vfmw_func.fn_vfmw_control(chan_id, VFMW_CID_SET_EVENT_MAP, &vfmw_event_map, sizeof(td_u64));
    if (ret != EXT_SUCCESS) {
        vdec_print_alert("vfmw set chan %d event map fail!\n", chan_id);
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

td_s32 vdec_vfmw_put_stream(td_s32 chan_id, const ext_vdec_stream *stream)
{
    td_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    vfmw_stream_buf stream_data = {0};
    vdec_vfmw_chan *chan = vdec_vfmw_get_chan(chan_id);

    vdec_check_ret(stream != TD_NULL);
    vdec_check_ret(chan != TD_NULL);

    stream_data.phy_addr = stream->start_addr;
    stream_data.pts = stream->pts;
    stream_data.length = stream->length;
    stream_data.is_end_of_frm = stream->is_end_of_frame;
    stream_data.disp_time = (td_u64)stream->pts;

    ret = entry->vfmw_func.fn_vfmw_control(chan_id, VFMW_CID_QUEUE_STREAM, &stream_data, sizeof(stream_data));
    if (ret != EXT_SUCCESS) {
        vdec_print_alert("vfmw chan %d queue stream fail!\n", chan_id);
        return EXT_FAILURE;
    }

    chan->first_stream_done = TD_TRUE;

    return EXT_SUCCESS;
}

td_s32 vdec_vfmw_get_stream(td_s32 chan_id, const ext_vdec_opt_get_stream *option, ext_vdec_stream *stream)
{
    td_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    vfmw_stream_buf stream_data = {0};

    vdec_check_ret(option != TD_NULL);
    vdec_check_ret(stream != TD_NULL);

    stream_data.length = option->expect_size;
    ret = entry->vfmw_func.fn_vfmw_control(chan_id, VFMW_CID_DEQUEUE_STREAM, &stream_data, sizeof(stream_data));
    if (ret != EXT_SUCCESS) {
        vdec_print_dbg("vfmw chan %d dequeue stream fail!\n", chan_id);
        return EXT_FAILURE;
    }

    stream->start_addr = stream_data.phy_addr;
    stream->length = stream_data.length;
    stream->vir_addr = (td_u8 *)(td_uintptr_t)(stream->start_addr);
    stream->pts = stream_data.pts;
    stream->is_end_of_frame = TD_TRUE;

    return EXT_SUCCESS;
}

td_s32 vdec_vfmw_acquire_frame(td_s32 chan_id, const ext_vdec_opt_acq_frame *option, ext_vdec_frame *frame, ext_vdec_ext_frm_info *ext_frm_info)
{
    td_s32 ret;
    vfmw_ext_frm_info vfmw_ext_frm = {{0}, 0};
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    vdec_vfmw_chan *chan = vdec_vfmw_get_chan(chan_id);
    td_u32 time;

    vdec_unused(option);
    vdec_check_ret(chan != TD_NULL);

    ret = entry->vfmw_func.fn_vfmw_get_frame(chan_id, frame, &vfmw_ext_frm);
    if (ret != EXT_SUCCESS) {
        if (chan->first_frame_done == TD_FALSE && chan->first_stream_done == TD_TRUE) {
            time = vdec_osal_get_time();
            chan->last_fail_time = (chan->last_fail_time == 0) ? time : chan->last_fail_time;
            if (time - chan->last_fail_time > VDEC_UNSUPPORT_TIME) {
                chan->last_fail_time = 0;
            }
        }
        return EXT_FAILURE;
    }

    chan->last_fail_time = 0;
    chan->first_frame_done = TD_TRUE;

    vdec_cnvt_ext_frm_2_vdec(&vfmw_ext_frm, ext_frm_info);

    return EXT_SUCCESS;
}

td_s32 vdec_vfmw_release_frame(td_s32 chan_id, const ext_vdec_frame *frame)
{
    td_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();

    ret = entry->vfmw_func.fn_vfmw_rels_frame(chan_id, frame);
    if (ret != EXT_SUCCESS) {
        vdec_print_dbg("vfmw chan %d release frame fail!\n", chan_id);
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

td_s32 vdec_vfmw_command(td_s32 chan_id, ext_vdec_cmd_id cmd, td_u8 *param, td_u32 size)
{
    td_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();
    td_u32 i;
    fun_command cmd_fun = TD_NULL;
    vdec_vfmw_cmd_item g_vdec_vfmw_command[] = {
        { (ext_vdec_cmd_id)VDEC_PRIVATE_CMD_GET_STREAM,       vdec_vfmw_get_usrdec_stream },
        { (ext_vdec_cmd_id)VDEC_PRIVATE_CMD_PUT_STREAM,       vdec_vfmw_put_usrdec_stream },
        { (ext_vdec_cmd_id)VDEC_PRIVATE_CMD_GET_FRAME,        vdec_vfmw_get_usrdec_frame },
        { (ext_vdec_cmd_id)VDEC_PRIVATE_CMD_PUT_FRAME,        vdec_vfmw_put_usrdec_frame },
        { (ext_vdec_cmd_id)VDEC_PRIVATE_CMD_CHK_FRM_STATE,    vdec_vfmw_check_frame_state },
    };

    for (i = 0; i < sizeof(g_vdec_vfmw_command) / sizeof(vdec_vfmw_cmd_item); i++) {
        if (g_vdec_vfmw_command[i].id == cmd) {
            cmd_fun = g_vdec_vfmw_command[i].cmd_fun;
            break;
        }
    }

    if (cmd_fun != TD_NULL) {
        ret = cmd_fun(chan_id, param, size);
    } else {
        ret = entry->vfmw_func.fn_vfmw_control(chan_id, (td_s32)vdec_cnvt_cmd_id_2_vfmw(cmd), param, size);
    }

    return ret;
}

td_s32 vdec_vfmw_wakeup(td_s32 chan_id, td_u8 *param, td_u32 size)
{
    td_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();

    ret = entry->vfmw_func.fn_vfmw_control(chan_id, VFMW_CID_EXTRA_WAKEUP_THREAD, param, size);
    if (ret != EXT_SUCCESS) {
        vdec_print_dbg("vfmw chan %d wakeup fail!\n", chan_id);
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

td_s32 vdec_vfmw_read_proc(td_void *buf, td_u32 len, td_u32 *used, td_s32 chan_id)
{
    vdec_vfmw_chan *chan = TD_NULL;
    ext_vdec_output_order chan_out_order;

    td_char *out_order[] = {"display", "dec", "na"};

    vdec_check_ret(chan_id < VDEC_MAX_CHAN_NUM);

    chan = vdec_vfmw_get_chan(chan_id);
    vdec_check_ret(chan != TD_NULL);

    vdec_drv_proc_dump(buf, len, used, VP_VFMW_HEAD);
    vdec_drv_proc_dump(buf, len, used, VP_D_D, "vdec_id", chan->vdec_chan_id, "vfmw_id", chan_id);
    chan_out_order = (chan->out_order >= EXT_VDEC_OUT_ORDER_DISPLAY && chan->out_order <= EXT_VDEC_OUT_ORDER_MAX) ?
        chan->out_order : EXT_VDEC_OUT_ORDER_MAX;
    vdec_drv_proc_dump(buf, len, used, VP_S_D, "out_order", out_order[chan_out_order], "is_sec", chan->is_sec);
    vdec_drv_proc_dump(buf, len, used, VP_D_D, "is_used", chan->is_used, "is_eos", chan->is_eos);

    return EXT_SUCCESS;
}

td_s32 vdec_vfmw_mod_init(td_void)
{
    td_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();

    ret = vdec_drv_get_vfmw_func(&entry->vfmw_func);
    if (ret != EXT_SUCCESS) {
        vdec_osal_atomic_exit(&entry->init_cnt);
        vdec_print_alert("get vfmw ext func fail!\n");
        return EXT_FAILURE;
    }

    ret = entry->vfmw_func.fn_vfmw_mod_init();
    if (ret != EXT_SUCCESS) {
        vdec_print_fatal("vfmw chan mod init fail!\r\n");
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

td_void vdec_vfmw_mod_exit(td_void)
{
    td_s32 ret;
    vdec_vfmw_entry *entry = vdec_vfmw_get_entry();

    ret = vdec_drv_get_vfmw_func(&entry->vfmw_func);
    if (ret != EXT_SUCCESS) {
        vdec_osal_atomic_exit(&entry->init_cnt);
        vdec_print_alert("get vfmw ext func fail!\n");
        return;
    }

    entry->vfmw_func.fn_vfmw_mod_exit();

    return;
}
