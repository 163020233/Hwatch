/*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
* Description: decoder
* Author: AuthorNameMagicTag
* Create: 2019-04-22
 */

#include "vpp.h"
#include "dbg.h"
#include "soc_vdec_type.h"

/* -------------------------------LOCAL MACRO---------------------------------- */
#define VPP_TASK_NAME            "SOCT_VFMW_VPP"
#define PP_MAX_FRAME_RATE        (120 * 1000)
#define PP_DEFAULT_FRAME_RATE    (25 * 1000)
#define PP_MAX_FAILED_TIME       5
#define USERDATA_IDENTIFIER_DVB1 0x34394147
#define USERDATA_IDENTIFIER_AFD  0x31475444
#define USERDATA_TYPE_DVB1_CC    0x03
#define USERDATA_TYPE_DVB1_BAR   0x06
#define USERDATA_TYPE_INDEX      4
#define MAX_HDR_MTDT_SIZE       (4 * 1024)

#define vpp_report_new_frame(chan, chan_id, frame, size) do { \
    if ((chan)->event_report != TD_NULL && ((((chan)->evt_map) & EVNT_NEW_YUV) != 0)) { \
        (chan)->event_report ((chan_id), EVNT_NEW_YUV, (td_void *)(frame), size); \
    }                                                                  \
} while (0)

#define vpp_report_receive_failed(chan, chan_id) do { \
    if ((chan)->event_report != TD_NULL && ((((chan)->evt_map) & EVNT_RECEIVE_FAILED) != 0)) { \
        (chan)->event_report((chan_id), EVNT_RECEIVE_FAILED, TD_NULL, 0); \
    }                                                                 \
} while (0)

#define vpp_report_norm_chg(chan, chan_id, norm, size) do { \
    if ((chan)->event_report != TD_NULL && ((((chan)->evt_map) & EVNT_NORM_CHG) != 0)) { \
        (chan)->event_report ((chan_id), EVNT_NORM_CHG, (td_void *)(norm), size); \
    } \
} while (0)

#define vpp_report_frm_pck_type_chg(chan, chan_id, frm_pck_type, size) do { \
    if ((chan)->event_report != TD_NULL && ((((chan)->evt_map) & EVNT_FRM_PCK_CHG) != 0)) { \
        (chan)->event_report ((chan_id), EVNT_FRM_PCK_CHG, (td_void *)(frm_pck_type), size); \
    } \
} while (0)


/* -----------------------------LOCAL DEFINITION------------------------------- */
static vpp_entry g_vpp_entry = {0};
static const td_u32 g_type_user_table[][2] = { /* 2 is array size */
    { SPY_CB_CR_420,      EXT_VDEC_FMT_YUV420SP },
    { SPY_CB_CR_400,      EXT_VDEC_FMT_YUV400SP },

    { VFMW_YUV_FMT_MAX, EXT_DRV_PIXEL_MAX },  /* terminal element n */
};

static const td_u32 g_codec_type_table[][2] = { /* 2 is array size */
    { VFMW_H264,      EXT_VDEC_STD_H264 },
    { VFMW_HEVC,      EXT_VDEC_STD_H265 },
    { VFMW_MJPEG,     EXT_VDEC_STD_MJPEG },
    { VFMW_STD_MAX,   EXT_VDEC_STD_MAX },
};

/* ----------------------------INTERNAL FUNCTION------------------------------- */
vpp_entry *vpp_get_entry(td_void)
{
    return &g_vpp_entry;
}

vpp_chan *vpp_get_chan(td_s32 chan_id)
{
    vpp_entry *entry = TD_NULL;
    vpp_chan *chan = TD_NULL;

    entry = vpp_get_entry();
    chan = entry->chan[chan_id];

    return chan;
}

td_void vpp_sema_down(td_void)
{
    vpp_entry *entry = vpp_get_entry();
    os_sema_down(entry->sema);
}

td_void vpp_sema_up(td_void)
{
    vpp_entry *entry = vpp_get_entry();
    os_sema_up(entry->sema);
}

STATIC td_void vpp_print_frame_info_part1(const ext_vdec_frame *frame)
{
    (td_void)(frame); /* avoid to build err when printing is not supported. */
    dprint(PRN_ALWS, "index : %-10d\r\n", frame->frame_index);
    dprint(PRN_ALWS, "start_addr: 0x%x, offset_y 0x%x, offset_c 0x%x, stride_y 0x%x, stride_c 0x%x\r\n",
        frame->frame_addr.start_addr, frame->frame_addr.offset_y,
        frame->frame_addr.offset_c, frame->frame_addr.stride_y, frame->frame_addr.stride_c);

    dprint(PRN_ALWS, "tunnel addr:0x%-10x \n", frame->frame_tunnel.start_addr);
    dprint(PRN_ALWS, "width :%-10d, height :%-10d aspect_width :%-10d aspect_height :%-10d\n",
           frame->width, frame->height, frame->aspect_width, frame->aspect_height);

    dprint(PRN_ALWS, "src_pts :%-10d, pts :%-10d, src_frame_rate :%-10d, frame_rate :%-10d\n",
           frame->src_pts, frame->pts, frame->src_frame_rate, frame->frame_rate);

    dprint(PRN_ALWS, "standard :%-10d, bit_depth :%-10d, color_space :%-10d, sampling_type :%-10d\n",
           frame->standard, frame->bit_depth, frame->color_space, frame->sampling_type);

    dprint(PRN_ALWS, "rect_x :%-10u, rect_y :%-10u, rect_w :%-10u, rect_h :%-10u\n",
           frame->frame_rect.rect_x, frame->frame_rect.rect_y, frame->frame_rect.rect_w, frame->frame_rect.rect_h);

    dprint(PRN_ALWS, "frame_format :%-10d, frame_compress :%-10d\n", (td_s32)frame->frame_format,
        (td_s32)frame->frame_compress.cmp_mode);

    return;
}

STATIC td_void vpp_print_frame_info(const ext_vdec_frame *frame)
{
    dprint(PRN_ALWS, "\r\n------------------------- print vdec info --------------------------\r\n");

    vpp_print_frame_info_part1(frame);

    dprint(PRN_ALWS, "----------------------------------------------------------------------\r\n");
    os_msleep(15); /* sleep 15 ms */

    return;
}

STATIC td_u32 vpp_format_convert(td_u32 code)
{
    td_u32 index = 0;
    td_u32 converted_code = EXT_VDEC_FMT_YUV420SP;

    while (1) {
        if (g_type_user_table[index][0] == VFMW_YUV_FMT_MAX || g_type_user_table[index][1] == EXT_VDEC_FMT_MAX) {
            break;
        }

        if (code == g_type_user_table[index][0]) {
            converted_code = g_type_user_table[index][1];
            break;
        }

        index++;
    }

    return converted_code;
}

STATIC td_void vpp_get_pix_format(vfmw_image *image, ext_vdec_frame *frame)
{
    frame->frame_format = vpp_format_convert(image->yuv_format);
}

STATIC td_void vpp_get_cmp_info(const vfmw_image *image, ext_vdec_frame *frame)
{
    if (image->disp_info.compress_en == 0 || image->disp_info.linear_en == 1) {
        frame->frame_compress.cmp_mode = EXT_VDEC_COMPRESS_MODE_OFF;
    } else {
        frame->frame_compress.cmp_mode = EXT_VDEC_COMPRESS_MODE_MAX;
    }

    return;
}

STATIC td_void vpp_get_sample_type(const vfmw_image *image, ext_vdec_frame *frame)
{
    switch (image->format.source_format) {
        case 0: /* 0 is PROGRESSIVE in vfmw */
            frame->sampling_type = EXT_VDEC_PROGRESSIVE;
            break;
        case 1: /* 1 is INTERLACE in vfmw */
            frame->sampling_type = EXT_VDEC_INTERLACED;
            break;
        default:
            frame->sampling_type = EXT_VDEC_SAMPLE_MAX;
            break;
    }

    return;
}

STATIC td_void vpp_get_bit_width(const vfmw_image *image, ext_vdec_frame *frame)
{
    if (image->bit_depth == 8) { /* 8 :a number */
        frame->bit_depth = EXT_VDEC_BIT_DEPTH_8;
    } else if (image->bit_depth == 10) { /* 10 :a number */
        frame->bit_depth = EXT_VDEC_BIT_DEPTH_10;
    } else {
        frame->bit_depth = EXT_VDEC_BIT_DEPTH_AUTO;
    }

    return;
}

STATIC td_u32 vpp_get_buf_handle(vpp_chan *chan, const image_disp *disp_info, td_bool right)
{
    td_u32 start_addr = 0;

    vfmw_unused(chan);
    start_addr = (right == TD_TRUE) ?  disp_info->luma_phy_addr_1 : disp_info->luma_phy_addr;

    return start_addr;
}

STATIC td_void vpp_get_buf_addr(vfmw_image *image, ext_vdec_frame *frame, vpp_chan *chan)
{
    frame->frame_addr.start_addr = vpp_get_buf_handle(chan, &image->disp_info, TD_FALSE);
    frame->frame_addr.offset_y = 0;
    frame->frame_addr.stride_y = image->disp_info.y_stride;
    frame->frame_addr.offset_c = image->disp_info.chrom_phy_addr - image->disp_info.luma_phy_addr;
    frame->frame_addr.stride_c = frame->frame_addr.stride_y;
    frame->frame_addr.vir_addr_y = (td_u8 *)(uintptr_t)image->disp_info.luma_phy_addr;
    frame->frame_addr.vir_addr_c = (td_u8 *)(uintptr_t)image->disp_info.chrom_phy_addr;

    return;
}

STATIC td_void vpp_convert_codec_type(vfmw_vid_std vid_std, ext_vdec_frame *frame)
{
    td_u32 index = 0;
    frame->standard = EXT_VDEC_STD_UNKNOWN;
    while (1) {
        if (g_codec_type_table[index][0] == VFMW_STD_MAX) {
            break;
        }

        if (vid_std == g_codec_type_table[index][0]) {
            frame->standard = g_codec_type_table[index][1];
            break;
        }

        index++;
    }

    return;
}

STATIC td_s32 vpp_check_frame(const ext_vdec_frame *frame)
{
    if (frame->width == 0 || frame->height == 0) {
        dprint(PRN_ERROR, "%s err, width:%d height:%d\n", __func__, frame->width, frame->height);
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

STATIC td_void vpp_get_private_frame_info(vfmw_image *image, vfmw_private_frm_info *private_info)
{
    private_info->image_id = image->image_id;
    private_info->image_id_1 = (td_u32)image->image_id_1;
    private_info->gop_num = (td_s32)image->gop_num;
    private_info->frm_type = image->format.frame_type;
    private_info->top_field_frame_format = image->top_fld_type & 0x3;
    private_info->bot_field_frame_format = image->bottom_fld_type & 0x3;
    private_info->field_flag = (td_s32)image->is_fld_flag;
    private_info->shr_index = image->shr_index;
    private_info->user_tag = (td_u32)image->user_tag;
    private_info->pix_sum_luma = image->pix_info.pix_sum_luma;
    private_info->pix_sum_chroma = image->pix_info.pix_sum_chroma;
    private_info->cycle = image->cycle;
}

STATIC vfmw_norm_type vpp_get_disp_norm(td_u32 video_fmt)
{
    switch (video_fmt) {
        case 1: /* 1 is PAL in vfmw */
            return VFMW_NORM_PAL;
        case 2: /* 2 is NTSC in vfmw */
            return VFMW_NORM_NTSC;
        default:
            return VFMW_NORM_MAX;
    }
}

STATIC td_void vpp_report_norm_info(const vpp_chan *chan, const vfmw_image *image)
{
    vfmw_norm_chg_param norm_param = {0};
    norm_param.norm_type = vpp_get_disp_norm(image->format.video_format);
    norm_param.width = image->disp_info.disp_width;
    norm_param.height = image->disp_info.disp_height;
    norm_param.progressive =
        (image->format.source_format == 0 || image->format.source_format == 2) ? /* 0 2 is progressive */
        TD_TRUE : TD_FALSE;
    if (norm_param.height <= 288) { /* 288 is image height */
        norm_param.progressive = TD_TRUE;
    }
    norm_param.frm_rate = 25000; /* 25000 is default frame rate */
    vpp_report_norm_chg(chan, chan->chan_id, &norm_param, sizeof(vfmw_norm_chg_param));
}

STATIC vfmw_private_frm_info *vpp_get_private_node(vpp_chan *chan)
{
    td_s32 i;
    vpp_private_block *block = TD_NULL;

    for (i = 0; i < VPP_MAX_PRIVATE_NODE_NUM; i++) {
        block = chan->vpp_private_node[i];
        if (block == TD_NULL) {
            break;
        }

        if (block->is_use == TD_FALSE) {
            block->is_use = TD_TRUE;
            return &block->vdec_private;
        }
    }

    if (i < VPP_MAX_PRIVATE_NODE_NUM) {
        vfmw_check_ret(os_alloc_vir != TD_NULL, TD_NULL);
        block = os_alloc_vir(chan->chan_id, "vpp_private_info", sizeof(vpp_private_block));
        if (block == TD_NULL) {
            dprint(PRN_ERROR, "%s:%d alloc vir mem fail!\n", __func__, __LINE__);
            return TD_NULL;
        }

        chan->vpp_private_node[i] = block;
        block->is_use = TD_TRUE;
        return &block->vdec_private;
    }

    dprint(PRN_ERROR, "%s:%d get priv fail!\n", __func__, __LINE__);

    return TD_NULL;
}

STATIC td_void vpp_put_private_node(vpp_chan *chan, vfmw_private_frm_info *vdec_private, td_s32 index)
{
    vpp_private_block *block = TD_NULL;

    vfmw_check((index >= 0) && (index < VPP_MAX_PRIVATE_NODE_NUM));

    block = chan->vpp_private_node[index];
    if (block != TD_NULL && (&block->vdec_private) == vdec_private) {
        block->is_use = TD_FALSE;
    }
}

STATIC td_void vpp_free_all_private_node(vpp_chan *chan)
{
    td_s32 i;
    vpp_private_block *block = TD_NULL;

    for (i = 0; i < VPP_MAX_PRIVATE_NODE_NUM; i++) {
        block = chan->vpp_private_node[i];
        if (block != TD_NULL) {
            os_free_vir(block);
            chan->vpp_private_node[i] = TD_NULL;
        }
    }
}

STATIC td_s32 vpp_get_private_node_index(vpp_chan *chan, vfmw_private_frm_info *vdec_private)
{
    td_s32 i;
    vpp_private_block *block = TD_NULL;

    for (i = 0; i < VPP_MAX_PRIVATE_NODE_NUM; i++) {
        block = chan->vpp_private_node[i];
        if (block != TD_NULL && (&block->vdec_private) == vdec_private) {
            return i;
        }
    }

    return -1;
}

STATIC td_void vpp_get_frame_common_info(vfmw_image *image, ext_vdec_frame *frame)
{
    frame->width = image->disp_info.disp_width;
    frame->height = image->disp_info.disp_height;
    frame->frame_rect.rect_x = 0;
    frame->frame_rect.rect_y = 0;
    frame->frame_rect.rect_w = image->disp_info.disp_width;
    frame->frame_rect.rect_h = image->disp_info.disp_height;
    frame->error_ratio = image->error_level;
    frame->src_pts = image->src_pts;
    frame->pts = image->pts;
    frame->frame_index = image->seq_img_cnt;
    frame->aspect_width = image->aspect_width;
    frame->aspect_height = image->aspect_height;
    frame->frame_rate = image->frame_rate <= PP_MAX_FRAME_RATE ? image->frame_rate : PP_MAX_FRAME_RATE;
    frame->splice_info.type = (image->splice_info.type == VFMW_SPLICE_AYUV_TAB) ?
        EXT_VDEC_SPLICE_AYUV_TAB : EXT_VDEC_SPLICE_NONE;
    frame->splice_info.ori_width = image->splice_info.ori_width;
    frame->splice_info.ori_height = image->splice_info.ori_height;
    if (image->line_num_phy_addr == image->disp_info.luma_phy_addr) {
        frame->frame_tunnel.start_addr = 0;
    } else {
        frame->frame_tunnel.start_addr = image->line_num_phy_addr;
    }
}

STATIC td_s32 vpp_image_2_frame(vpp_chan *chan, vfmw_image *image, ext_vdec_frame *frame)
{
    ext_vdec_frame_private *private_vdec_frame_info = TD_NULL;
    vfmw_private_frm_info *vpp_private = TD_NULL;

    vfmw_check_ret((frame != TD_NULL) && (image != TD_NULL), EXT_FAILURE);
    private_vdec_frame_info = &(frame->frame_private);
    vpp_private = vpp_get_private_node(chan);
    if (vpp_private == TD_NULL) {
        return EXT_FAILURE;
    }

    vpp_get_bit_width(image, frame);
    vpp_get_pix_format(image, frame);
    vpp_convert_codec_type(image->vid_std, frame);
    vpp_get_buf_addr(image, frame, chan);
    vpp_get_sample_type(image, frame);
    vpp_get_cmp_info(image, frame);
    vpp_get_frame_common_info(image, frame);
    vpp_get_private_frame_info(image, vpp_private);
    private_vdec_frame_info->type = EXT_DRV_MODULE_VDEC;
    private_vdec_frame_info->data = ptr_uint64(vpp_private);
    private_vdec_frame_info->size = (td_u32)sizeof(vfmw_private_frm_info);

    if (image->disp_info.disp_width != chan->last_image.disp_info.disp_width ||
        image->disp_info.disp_height != chan->last_image.disp_info.disp_height) {
        vpp_report_norm_info(chan, image);
    }

    return EXT_SUCCESS;
}

STATIC td_s32 vpp_frame_2_image(vpp_chan *chan, ext_vdec_frame *frame, vfmw_image *image)
{
    ext_vdec_frame_private *module_private = &(frame->frame_private);
    vfmw_private_frm_info *vpp_private = (vfmw_private_frm_info *)uint64_ptr(module_private->data);
    td_s32 priv_index = vpp_get_private_node_index(chan, vpp_private);

    vfmw_check_ret(module_private->type == EXT_DRV_MODULE_VDEC, EXT_FAILURE);
    vfmw_check_ret(uint64_ptr(module_private->data) != TD_NULL, EXT_FAILURE);
    vfmw_check_ret(module_private->size == sizeof(vfmw_private_frm_info), EXT_FAILURE);

    vfmw_check_ret(priv_index >= 0, EXT_FAILURE);
    vfmw_check_ret(vpp_private->image_id != 0, EXT_FAILURE);
    vfmw_check_ret(vpp_private->image_id_1 != 0, EXT_FAILURE);

    image->image_id = vpp_private->image_id;
    image->image_id_1 = (td_s32)vpp_private->image_id_1;
    image->disp_info.disp_width = frame->width;
    image->disp_info.disp_height = frame->height;

    if (image->image_id_1 != -1) {
        image->is_3d = 1;
    }

    vpp_put_private_node(chan, vpp_private, priv_index);
    module_private->data = 0;
    module_private->size = 0;

    return EXT_SUCCESS;
}

STATIC td_s32 vpp_recieve_normal_mode_chan_judge(const vpp_chan *chan, td_s32 chan_id)
{
    (td_void)(chan_id); /* avoid to build err when printing is not supported. */
    if (chan == TD_NULL || chan->created != TD_TRUE) {
        dprint(PRN_ERROR, "%s chan:%d not Create\n", __func__, chan_id);
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

STATIC td_s32 vpp_recieve_normal_mode(td_s32 chan_id, td_void *frame)
{
    vpp_chan *chan = TD_NULL;
    vfmw_image tmp_image = {0};
    vpp_entry *entry = vpp_get_entry();
    ext_vdec_frame *tmp_frame = (ext_vdec_frame *)frame;

    chan = vpp_get_chan(chan_id);
    if (vpp_recieve_normal_mode_chan_judge(chan, chan_id) == (td_s32)EXT_FAILURE) {
        return EXT_FAILURE;
    }

    vfmw_check_ret(entry->get_image != TD_NULL, EXT_FAILURE);

    vfmw_check_ret_no_print(entry->get_image((td_s32)chan_id, &tmp_image) == EXT_SUCCESS, EXT_FAILURE);

    if (vpp_image_2_frame(chan, &tmp_image, tmp_frame) != EXT_SUCCESS) {
        dprint(PRN_ERROR, "call vpp_image_2_frame failed\n");
        goto exit;
    }

    if (vpp_check_frame(tmp_frame) != EXT_SUCCESS) {
        dprint(PRN_ERROR, "call vir_pp_check_frame failed\n");
        goto exit;
    }

    if (entry->print_frm) {
        vpp_print_frame_info(tmp_frame);
    }

    if (tmp_frame->frame_rate == 0 || entry->bypass_chan == chan_id) {
        vpp_frame_2_image(chan, tmp_frame, &tmp_image);
        goto exit;
    }

    vfmw_check_sec_func(memcpy_s(&chan->last_image, sizeof(chan->last_image), &tmp_image, sizeof(vfmw_image)));

    if (entry->print_buf) {
        dprint(PRN_ALWS, "get frame addr:0x%x\r\n", tmp_frame->frame_addr.start_addr);
    }

    return EXT_SUCCESS;

exit:
    if (entry->rls_image(chan_id, &tmp_image) != EXT_SUCCESS) {
        dprint(PRN_ERROR, "%s %d release image fail.\n", __func__, __LINE__);
    }

    return EXT_FAILURE;
}

td_void vpp_thread_wakeup(td_s32 chan_id)
{
    vpp_entry *entry = vpp_get_entry();
    vpp_chan *chan = TD_NULL;
    if (vpp_get_entry()->thread_state != THREA_SLEEP) {
        return;
    }

    chan = vpp_get_chan(chan_id);
    if (chan->chan_lowly_enable != 0) {
        os_event_give(entry->event);
    }

    return;
}

STATIC td_void vpp_config_souce(vpp_chan *chan, td_u32 source)
{
    switch (source) {
        case VFMW_SOURCE_MEDIA:
            chan->source = EXT_DRV_SOURCE_MEDIA;
            break;
        case VFMW_SOURCE_DTV:
            chan->source = EXT_DRV_SOURCE_DTV;
            break;
        case VFMW_SOURCE_NETWORK:
            chan->source = EXT_DRV_SOURCE_NETWORK;
            break;
        case VFMW_SOURCE_CAST:
            chan->source = EXT_DRV_SOURCE_CAST;
            break;
        default:
            chan->source = EXT_DRV_SOURCE_MEDIA;
            break;
    }
}

td_s32 vpp_init(vpp_init_param *param)
{
    vpp_entry *entry = vpp_get_entry();

    vfmw_check_ret(param != TD_NULL, EXT_FAILURE);
    vfmw_check_ret(os_event_init != TD_NULL, EXT_FAILURE);
    vfmw_check_ret(os_create_thread != TD_NULL, EXT_FAILURE);
    os_sema_init(&entry->sema);
    os_event_init(&entry->event, 1);

    entry->get_image = param->get_image;
    entry->rls_image = param->rls_image;
    entry->chk_image = param->chk_image;
    entry->chk_eos = param->chk_eos;
    entry->bypass_chan = -1;

    entry->inited = TD_TRUE;

    return EXT_SUCCESS;
}

td_s32 vpp_deinit(td_void)
{
    vpp_entry *entry = vpp_get_entry();

    vfmw_check_ret(entry->inited == TD_TRUE, EXT_FAILURE);

    os_event_exit(entry->event);
    os_sema_exit(entry->sema);

    entry->get_image = TD_NULL;
    entry->rls_image = TD_NULL;

    entry->inited = TD_FALSE;

    return EXT_SUCCESS;
}

td_s32 vpp_create(td_s32 chan_id, vpp_create_param *param)
{
    vpp_chan *chan = TD_NULL;
    vpp_entry *entry = vpp_get_entry();

    vfmw_check_ret(chan_id < VFMW_CHAN_NUM, EXT_FAILURE);
    vfmw_check_ret(param != TD_NULL, EXT_FAILURE);
    vfmw_check_ret(param->event_report != TD_NULL, EXT_FAILURE);
    vfmw_check_ret(entry->inited == TD_TRUE, EXT_FAILURE);

    vpp_sema_down();
    chan = vpp_get_chan(chan_id);
    if (chan != TD_NULL) {
        vpp_sema_up();
        return EXT_FAILURE;
    }
    vpp_sema_up();
    vfmw_check_ret(os_alloc_vir != TD_NULL, EXT_FAILURE);
    chan = os_alloc_vir(chan_id, "vpp_inst", sizeof(vpp_chan));
    if (chan == TD_NULL) {
        return EXT_FAILURE;
    }
    vfmw_check_sec_func(memset_s(chan, sizeof(vpp_chan), 0, sizeof(vpp_chan)));
    chan->created = TD_TRUE;
    chan->chan_id = chan_id;
    chan->state = STOP_STATE;
    chan->work_mode = param->work_mode;
    chan->event_report = param->event_report;
    chan->frm_fence_enable = TD_TRUE;

    vpp_sema_down();
    entry->chan[chan_id] = chan;
    vpp_sema_up();

    return EXT_SUCCESS;
}

td_s32 vpp_destroy(td_s32 chan_id)
{
    vpp_chan *chan = TD_NULL;
    vpp_entry *entry = vpp_get_entry();

    vfmw_check_ret(chan_id < VFMW_CHAN_NUM, EXT_FAILURE);
    vfmw_check_ret(entry->inited == TD_TRUE, EXT_FAILURE);

    vpp_sema_down();

    chan = vpp_get_chan(chan_id);
    if (chan == TD_NULL || chan->created == TD_FALSE) {
        dprint(PRN_FATAL, "%s chan %d is not inited.\n", __func__, chan_id);
        vpp_sema_up();
        return EXT_FAILURE;
    }

    if (chan->state == START_STATE) {
        dprint(PRN_FATAL, "%s chan:%d not stop before destroy\n", __func__, chan_id);
        vpp_sema_up();
        return EXT_FAILURE;
    }

    vpp_free_all_private_node(chan);

    entry->chan[chan_id] = TD_NULL;
    os_free_vir(chan);

    vpp_sema_up();

    return EXT_SUCCESS;
}

td_s32 vpp_start(td_s32 chan_id)
{
    vpp_chan *chan = TD_NULL;
    vpp_entry *entry = vpp_get_entry();

    vfmw_check_ret(chan_id < VFMW_CHAN_NUM, EXT_FAILURE);
    vfmw_check_ret(entry->inited == TD_TRUE, EXT_FAILURE);

    vpp_sema_down();
    chan = vpp_get_chan(chan_id);
    if (chan == TD_NULL || chan->created == TD_FALSE) {
        dprint(PRN_FATAL, "%s chan %d is not inited.\n", __func__, chan_id);
        vpp_sema_up();
        return EXT_FAILURE;
    }

    chan->state = START_STATE;
    vpp_sema_up();

    return EXT_SUCCESS;
}

td_s32 vpp_stop(td_s32 chan_id)
{
    vpp_chan *chan = TD_NULL;
    vpp_entry *entry = vpp_get_entry();

    vfmw_check_ret(chan_id < VFMW_CHAN_NUM, EXT_FAILURE);
    vfmw_check_ret(entry->inited == TD_TRUE, EXT_FAILURE);

    vpp_sema_down();

    chan = vpp_get_chan(chan_id);
    if (chan == TD_NULL || chan->created == TD_FALSE) {
        dprint(PRN_FATAL, "%s chan %d is not inited.\n", __func__, chan_id);
        vpp_sema_up();

        return EXT_FAILURE;
    }

    chan->state = STOP_STATE;
    vpp_sema_up();

    return EXT_SUCCESS;
}

td_s32 vpp_reset(td_s32 chan_id)
{
    vpp_chan *chan = TD_NULL;
    vpp_entry *entry = vpp_get_entry();

    vfmw_check_ret(chan_id < VFMW_CHAN_NUM, EXT_FAILURE);
    vfmw_check_ret(entry->inited == TD_TRUE, EXT_FAILURE);

    vpp_sema_down();

    chan = vpp_get_chan(chan_id);
    if (chan == TD_NULL || chan->created == TD_FALSE) {
        dprint(PRN_FATAL, "%s chan %d is not inited.\n", __func__, chan_id);
        vpp_sema_up();
        return EXT_FAILURE;
    }

    vpp_sema_up();

    return EXT_SUCCESS;
}

td_s32 vpp_config(td_s32 chan_id, vfmw_chan_cfg *cfg)
{
    vpp_chan *chan = TD_NULL;
    vpp_entry *entry = vpp_get_entry();

    vfmw_check_ret(entry->inited == TD_TRUE, EXT_FAILURE);
    vfmw_check_ret(chan_id < VFMW_CHAN_NUM, EXT_FAILURE);
    vfmw_check_ret(cfg != TD_NULL, EXT_FAILURE);

    vpp_sema_down();

    chan = vpp_get_chan(chan_id);
    if (chan == TD_NULL || chan->created == TD_FALSE) {
        dprint(PRN_FATAL, "%s chan %d is not inited.\n", __func__, chan_id);
        vpp_sema_up();
        return EXT_FAILURE;
    }

    vpp_config_souce(chan, cfg->source);

    chan->std_ext = cfg->std_ext;
    chan->err_thr = cfg->chan_err_thr;
    chan->dec_mode = cfg->dec_mode;
    chan->single_i_frame = cfg->single_i_frame;
    chan->alloc_type = cfg->alloc_type;
    chan->frm_fence_enable = cfg->frm_fence_enable;
    chan->chan_lowly_enable = cfg->chan_lowly_enable;

    vpp_sema_up();

    return EXT_SUCCESS;
}

td_s32 vpp_get_chan_status(td_s32 chan_id, vfmw_chan_info *status)
{
    vpp_chan *chan = TD_NULL;
    vpp_entry *entry = vpp_get_entry();

    vfmw_check_ret(entry->inited == TD_TRUE, EXT_FAILURE);
    vfmw_check_ret(chan_id < VFMW_CHAN_NUM, EXT_FAILURE);
    vfmw_check_ret(status != TD_NULL, EXT_FAILURE);

    vpp_sema_down();

    chan = vpp_get_chan(chan_id);
    if (chan == TD_NULL || chan->created != TD_TRUE) {
        vpp_sema_up();
        return EXT_FAILURE;
    }

    status->std = chan->last_image.vid_std;
    status->video_format = chan->last_image.format.video_format;
    status->scan_type = chan->last_image.format.source_format;
    status->aspect_width = chan->last_image.aspect_width;
    status->aspect_height = chan->last_image.aspect_height;
    status->bit_depth = chan->last_image.bit_depth;
    status->frame_rate = chan->last_image.frame_rate;
    status->disp_height = chan->last_image.disp_info.disp_height;
    status->disp_width = chan->last_image.disp_info.disp_width;
    status->disp_center_x = status->disp_width / 2; /* 2 caculate center by divide 2 */
    status->disp_center_y = status->disp_height / 2; /* 2 caculate center by divide 2 */
    status->image_width = chan->last_image.image_width;
    status->image_height = chan->last_image.image_height;
    status->is_field_flg = chan->last_image.is_fld_flag;
    /* overwrites the previous total_frame_num to avoid sw decode use voq info always 0 */
    status->total_frame_num = chan->receive_ok;
    status->vpp_receive_try = chan->receive_try;
    status->vpp_receive_ok = chan->receive_ok;
    status->vpp_release_try = chan->release_try;
    status->vpp_release_ok = chan->release_ok;
    status->dec_mode_attr = chan->dec_mode;

    vpp_sema_up();

    return EXT_SUCCESS;
}

td_s32 vpp_receive_frame(td_s32 chan_id, td_void *frame)
{
    td_s32 ret;
    vpp_chan *chan = TD_NULL;
    vpp_entry *entry = TD_NULL;

    vfmw_check_ret(chan_id < VFMW_CHAN_NUM, EXT_FAILURE);
    vfmw_check_ret(frame != TD_NULL, EXT_FAILURE);
    chan = vpp_get_chan(chan_id);
    if (chan == TD_NULL) {
        dprint(PRN_ERROR, "chan %d is TD_NULL!\n", chan_id);
        return EXT_FAILURE;
    }

    entry = vpp_get_entry();
    if (entry->no_recv_frm != 0) {
        return EXT_FAILURE;
    }

    chan->receive_try++;

    ret = vpp_recieve_normal_mode(chan_id, frame);
    if (ret == EXT_SUCCESS) {
        chan->receive_ok++;
    }

    return ret;
}

td_s32 vpp_release_frame(td_s32 chan_id, const td_void *frame)
{
    td_s32 ret;
    vpp_chan *chan = TD_NULL;
    vfmw_image tmp_image = {0};
    ext_vdec_frame *tmp_frame = TD_NULL;
    vpp_entry *entry = TD_NULL;

    vfmw_check_ret(chan_id < VFMW_CHAN_NUM, EXT_FAILURE);
    vfmw_check_ret(frame != TD_NULL, EXT_FAILURE);

    chan = vpp_get_chan(chan_id);
    if (chan == TD_NULL || chan->created != TD_TRUE) {
        dprint(PRN_ERROR, "%s chan:%d not Create\n", __func__, chan_id);
        return EXT_FAILURE;
    }

    entry = vpp_get_entry();
    tmp_frame = (ext_vdec_frame *)frame;

    chan->release_try++;

    ret = vpp_frame_2_image(chan, tmp_frame, &tmp_image);
    vfmw_check_ret_print(ret == EXT_SUCCESS, EXT_FAILURE, "call vir_pp_convert_image failed\n");

    if (entry->rls_image == TD_NULL) {
        return EXT_FAILURE;
    }

    if (entry->print_buf) {
        dprint(PRN_ALWS, "rls frame addr:%x \n", tmp_frame->frame_addr.start_addr);
    }

    ret = entry->rls_image(chan_id, &tmp_image);
    if (ret != EXT_SUCCESS) {
        dprint(PRN_ERROR, "call vir_pp_release_image failed\n");
        return EXT_FAILURE;
    }

    chan->release_ok++;

    return EXT_SUCCESS;
}

td_s32 vpp_set_evt_map(td_s32 chan_id, td_u64 evt_map)
{
    vpp_chan *chan = TD_NULL;

    vfmw_check_ret(chan_id < VFMW_CHAN_NUM, EXT_FAILURE);

    chan = vpp_get_chan(chan_id);
    if (chan == TD_NULL || chan->created != TD_TRUE) {
        dprint(PRN_ERROR, "%s chan:%d not Create\n", __func__, chan_id);
        return EXT_FAILURE;
    }

    chan->evt_map = evt_map;

    return EXT_SUCCESS;
}

td_s32 vpp_check_frame_state(td_s32 chan_id, const td_void *frame, td_u32 size)
{
    const ext_vdec_frame *tmp_frame = TD_NULL;
    const vfmw_private_frm_info *priv_info = TD_NULL;
    vfmw_image img;
    vpp_entry *entry = TD_NULL;

    vfmw_check_ret(frame != TD_NULL, EXT_FAILURE);
    vfmw_check_ret(size == sizeof(ext_vdec_frame), EXT_FAILURE);

    entry = vpp_get_entry();
    if (entry->chk_image == TD_NULL) {
        dprint(PRN_FATAL, "check image interface is TD_NULL!\n");
        return EXT_FAILURE;
    }

    tmp_frame = (ext_vdec_frame*)frame;
    priv_info = (vfmw_private_frm_info*)uint64_ptr(tmp_frame->frame_private.data);
    if (priv_info == TD_NULL) {
        dprint(PRN_ERROR, "frame private info is TD_NULL!\n");
        return EXT_FAILURE;
    }

    img.image_id = priv_info->image_id;
    img.image_id_1 = (td_s32)priv_info->image_id_1;
    img.shr_index = priv_info->shr_index;

    return entry->chk_image(chan_id, &img);
}
