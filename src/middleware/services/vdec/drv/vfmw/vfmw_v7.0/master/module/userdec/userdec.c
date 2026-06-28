/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#include "userdec.h"
#include "vfmw_osal.h"
#include "dbg.h"
#include "vfmw_define.h"
#include "stream.h"
#include "liteos_proc.h"
#include "vdec_chr.h"

#define USER_MAX_FS_NUM         3
#define USER_MAX_FS_SIZE        (20 * 1024 * 1024)  /* 20M */
#define USER_INDEX_OVERFLOW_NUM 1000000

#define USER_LOWDELAY_LINE_SIZE 32

#define USER_OUTER_PARA_SIZE    VFMW_OUTER_PARA_SIZE

typedef td_s32 (*user_handler)(td_s32, td_void *, td_u32);

typedef struct {
    vfmw_cid cmd_id;
    user_handler handler;
    td_u32 length;
} user_case;

typedef enum {
    FRM_NULL,
    FRM_IDLE,
    FRM_GET,
    FRM_PUT,
    FRM_ACQ,
    FRM_BUTT
} user_frm_state;

typedef struct {
    td_s32 order;
    user_frm_state state;
    vfmw_mem_desc st_mem;
    vfmw_image image;
} user_frame;

typedef struct {
    td_s32 chan_id;
    td_s32 unid;
    td_s32 index;
    td_s32 sec_flag;
    td_u32 img_slot_len;
    td_s32 frm_alloc_done_cnt;
    td_u32 last_image_id;
    td_u32 recv_eos;
    td_u32 report_eos;
    td_s32 new_frm;
    vfmw_adpt_type adpt_type;
    vfmw_chan_cfg cfg;
    user_frame frm[USER_MAX_FS_NUM];
    td_u32 seq_cnt;
} user_ctx;

typedef struct {
    os_sema sema;
    td_u32 chan_num;
    td_s32 dec_num;
    fun_vfmw_event_report fp_vdec_report;
    fun_vfmw_event_report fp_omx_report;
} user_global_ctx;

user_ctx *g_user_chan_ctx[VFMW_CHAN_NUM];
user_global_ctx g_user_ctx = {
    .dec_num = -1,
};

#ifdef VFMW_USER_SUPPORT
STATIC td_void user_read_chan(td_void *buf, td_s32 len, td_s32 *used, user_ctx *chan)
{
    td_s32 i;
    user_frame *frm = TD_NULL;
    td_char *state_str[FRM_BUTT + 1] = { "null", "idle", "get", "put", "acq", "na" };

    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "chan_id", chan->chan_id, "unid", chan->unid);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "adpt_type", chan->adpt_type, "frm_num",
                 chan->frm_alloc_done_cnt);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "sec_flag", chan->sec_flag, "frm_size", chan->img_slot_len);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "last_image_id", chan->last_image_id, "new_frm", chan->new_frm);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "recv_eos", chan->recv_eos, "report_eos", chan->report_eos);
    os_dump_proc(buf, len, used, 0, LOG_FMT_S_D, "standard", dbg_show_std(chan->cfg.vid_std, &chan->cfg.std_ext),
                 "lowdelay", chan->cfg.module_lowly_enable);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D, "dec_num", g_user_ctx.dec_num);

    os_dump_proc(buf, len, used, 0, LOG_USER_DEC_FRM_HEAD);

    os_dump_proc(buf, len, used, 0, "%-11s %-10s %-10s %-10s %-8s %-25s|\r\n",
        "frame_num", "state", "width", "height", "size", "phy");
    for (i = 0; i < USER_MAX_FS_NUM; i++) {
        frm = &(chan->frm[i]);
        if (frm->state == FRM_NULL) {
            continue;
        }
        os_dump_proc(buf, len, used, 0, "%-11d %-10s %-10d %-10d 0x%-8x 0x%-21x|\r\n",
            i, state_str[frm->state], frm->image.disp_info.disp_width, frm->image.disp_info.disp_height,
            frm->st_mem.length, log_get_mask_phy_addr(frm->st_mem.phy_addr));
    }
    os_dump_proc(buf, len, used, 0, LOG_FMT_TAIL);
}

STATIC td_s32 user_read_proc(td_void *buf)
{
    td_u32 chan_id;
    td_s32 used = 0;
    const td_s32 len = 0;
    user_ctx *chan = TD_NULL;

    vfmw_check_ret(buf != TD_NULL, EXT_FAILURE);
    os_dump_proc(buf, len, &used, 0, LOG_USER_DEC_FMT_HEAD);

    if (g_user_ctx.chan_num == 0) {
        os_dump_proc(buf, len, &used, 0, "user no chan exists.%-59s|\r\n", " ");
        return EXT_SUCCESS;
    }

    for (chan_id = 0; chan_id < VFMW_CHAN_NUM; chan_id++) {
        chan = g_user_chan_ctx[chan_id];
        if (chan != TD_NULL) {
            user_read_chan(buf, len, &used, chan);
        }
    }

    return EXT_SUCCESS;
}

STATIC td_void user_write_proc(cmd_str_ptr buf, td_u32 cmd_cnt)
{
    td_u32 dec_num = 0;
    td_char *cmd_id = ((*buf)[0]);

    vfmw_check_print(cmd_cnt >= 0x2, "cmd param cmd_cnt %d invalid.\n", cmd_cnt);

    vfmw_check(os_strncmp != TD_NULL);
    vfmw_check(os_strlen != TD_NULL);

    /* Parse command */
    if (os_strncmp(cmd_id, LOG_CMD_USR_DEC_NUM, os_strlen(cmd_id)) == 0) {
        str_to_val((*buf)[1], &dec_num); /* 1 means 2nd param */
        g_user_ctx.dec_num = (td_s32)dec_num;
        dprint(PRN_ALWS, "set usr dec_num %d\n", g_user_ctx.dec_num);
    } else {
        dprint(PRN_ALWS, "unsupport command '%s', cmd_cnt %d!\n", cmd_id, cmd_cnt);
    }
}
#endif

td_s32 user_set_callback(vfmw_adpt_type type, fun_vfmw_event_report fp_event_report)
{
    vfmw_check_ret(fp_event_report != TD_NULL, EXT_FAILURE);

    switch (type) {
        case ADPT_TYPE_VDEC:
            g_user_ctx.fp_vdec_report = fp_event_report;
            break;

        default:
            dprint(PRN_ERROR, "%s Unkown adpt type: %d\n", __func__, type);
            return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

STATIC td_void user_event_report(td_s32 chan_id, td_s32 type, td_void *args, td_s32 len)
{
    user_ctx *chan = g_user_chan_ctx[chan_id];

    switch (chan->adpt_type) {
        case ADPT_TYPE_VDEC:
            vfmw_check_print(g_user_ctx.fp_vdec_report != TD_NULL, "ERR: fp_vdec_report = TD_NULL\n");
            g_user_ctx.fp_vdec_report(chan_id, type, args, len);
            break;

        default:
            dprint(PRN_ERROR, "Unsupport adpt_type %d.\n", chan->adpt_type);
            break;
    }

    return;
}

STATIC td_bool user_is_frame_list_empty(const user_ctx *chan)
{
    td_s32 i;

    for (i = 0; i < USER_MAX_FS_NUM; i++) {
        if (chan->frm[i].state == FRM_GET ||
            chan->frm[i].state == FRM_PUT) {
            return TD_FALSE;
        }
    }

    return TD_TRUE;
}

STATIC td_s32 user_alloc_ext_frame_buffer(user_ctx *chan, const vfmw_usrdec_frame_desc *user_frm)
{
    td_u32 img_slot_len;
    td_u32 para[USER_OUTER_PARA_SIZE] = {0};

    vfmw_check_ret(chan != TD_NULL, EXT_FAILURE);
    vfmw_check_ret(user_frm != TD_NULL, EXT_FAILURE);

    if (user_frm->mem_size >= USER_MAX_FS_SIZE) {
        dprint(PRN_ERROR, "Invalid param: frame_size %llu >= USER_MAX_FS_SIZE %u.\n", user_frm->mem_size,
            USER_MAX_FS_SIZE);
        return EXT_FAILURE;
    }

    img_slot_len = (td_u32)user_frm->mem_size;
    if (chan->cfg.module_lowly_enable == 1) {
        img_slot_len += USER_LOWDELAY_LINE_SIZE;
    }

    para[0] = img_slot_len;
    para[1] = USER_MAX_FS_NUM;
    para[6] = user_frm->width;      /* 6 :a number */
    para[7] = user_frm->height;     /* 7 :a number */
    para[4] = user_frm->luma_stride; /* 4 :a number */
    para[5] = user_frm->bit_depth;   /* 5 :a number */

    user_event_report(chan->chan_id, EVNT_RESOLUTION_CHANGE, (td_void *)para, sizeof(para));
    chan->img_slot_len = img_slot_len;

    return EXT_SUCCESS;
}

STATIC td_s32 user_bind_req_mem(td_s32 chan_id, td_void *args, td_u32 len)
{
    user_ctx *chan = g_user_chan_ctx[chan_id];
    td_s32 index;
    const vfmw_mem_report *user_mem = TD_NULL;

    vfmw_unused(len);
    vfmw_check_ret(args != TD_NULL, EXT_FAILURE);
    vfmw_check_ret(chan != TD_NULL, EXT_FAILURE);

    user_mem = (vfmw_mem_report *)args;

    for (index = 0; index < USER_MAX_FS_NUM; index++) {
        if (chan->frm[index].st_mem.length == 0) {
            chan->frm[index].st_mem.length = user_mem->size;
            chan->frm[index].st_mem.phy_addr = user_mem->phy_addr;
            chan->frm[index].st_mem.vir_addr = user_mem->vir_addr;
            chan->frm[index].st_mem.dma_buf = user_mem->dma_buf;
            chan->frm[index].state = FRM_IDLE;
            chan->frm_alloc_done_cnt++;
            break;
        }
    }

    if (index == USER_MAX_FS_NUM) {
        dprint(PRN_ERROR, "user frm has something wrong!\n");
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

STATIC td_s32 user_alloc_psram_mem(td_s32 chan_id, td_u32 dec_size, vfmw_mem_report *mem)
{
    mem_record mem_tmp = { 0 };

    mem_tmp.mem_name = "user_dec";
    mem_tmp.length = dec_size;
    mem_tmp.is_cached = 0;
    mem_tmp.unid = (td_u32)chan_id;
    vfmw_check_ret(os_malloc != TD_NULL, EXT_FAILURE);
    if (os_malloc(&mem_tmp) != OSAL_OK) {
        dprint(PRN_ERROR, "%s, %d, os_malloc err\n", __func__, __LINE__);
        return EXT_FAILURE;
    }

    vfmw_check_sec_func(strcpy_s(mem->buf_name, VFMW_REPORT_MAX_NAME_LEN, mem_tmp.mem_name));
    mem->is_map = 1;
    mem->phy_addr = mem_tmp.phy_addr;
    mem->vir_addr = ptr_uint64(mem_tmp.vir_addr);
    mem->size = mem_tmp.length;

    return EXT_SUCCESS;
}

STATIC td_void user_free_psram_mem(UADDR phy_addr)
{
    mem_record mem = { 0 };

    if (phy_addr != 0) {
        mem.phy_addr = phy_addr;
        mem.vir_addr = (td_u8 *)uint64_ptr(phy_addr);
        mem.is_cached = 0;
        os_free(&mem);
    }
}

STATIC td_void user_free_frame_buffer(user_ctx *chan)
{
    td_s32 index;

    for (index = 0; index < USER_MAX_FS_NUM; index++) {
        if (chan->frm[index].st_mem.length == 0) {
            continue;
        }

        user_free_psram_mem(chan->frm[index].st_mem.phy_addr);

        if (chan->frm_alloc_done_cnt > 0) {
            chan->frm_alloc_done_cnt--;
        }
        chan->frm[index].state = FRM_NULL;
    }

    return;
}

STATIC td_s32 user_alloc_frame_buffer(user_ctx *chan, td_u32 frame_size)
{
    td_s32 index;
    td_s32 ret;
    td_u32 img_slot_len;
    vfmw_mem_report mem_rep;

    if (frame_size >= USER_MAX_FS_SIZE) {
        dprint(PRN_ERROR, "Invalid param: frame_size %u >= USER_MAX_FS_SIZE %u.\n", frame_size, USER_MAX_FS_SIZE);
        return EXT_FAILURE;
    }

    img_slot_len = frame_size;
    if (chan->cfg.module_lowly_enable == 1) {
        img_slot_len += USER_LOWDELAY_LINE_SIZE;
    }

    vfmw_check_sec_func(memset_s(&mem_rep, sizeof(vfmw_mem_report), 0, sizeof(vfmw_mem_report)));

    for (index = 0; index < USER_MAX_FS_NUM; index++) {
        ret = user_alloc_psram_mem(chan->chan_id, img_slot_len, &mem_rep);
        vfmw_check_goto(ret == EXT_SUCCESS, out);
        user_bind_req_mem(chan->chan_id, &mem_rep, sizeof(vfmw_mem_report));
        vfmw_check_goto(ret == EXT_SUCCESS, out);
    }

    chan->img_slot_len = img_slot_len;

    return EXT_SUCCESS;

out:
    user_free_frame_buffer(chan);
    return EXT_FAILURE;
}

STATIC user_frame *user_get_idle_frame(user_ctx *chan)
{
    td_s32 i;

    for (i = 0; i < USER_MAX_FS_NUM; i++) {
        if (chan->frm[i].state == FRM_IDLE &&
            chan->frm[i].st_mem.length != 0) {
            chan->frm[i].state = FRM_GET;
            break;
        }
    }

    if (i >= USER_MAX_FS_NUM) {
        return TD_NULL;
    }

    return &chan->frm[i];
}

STATIC user_frame *user_find_match_frame(user_ctx *chan, const vfmw_usrdec_frame_desc *user_frm)
{
    td_s32 index;
    user_frame *frame = TD_NULL;

    for (index = 0; index < USER_MAX_FS_NUM; index++) {
        frame = &chan->frm[index];
        if (frame->state == FRM_GET &&
            user_frm->phy_addr == frame->st_mem.phy_addr) {
            frame->state = FRM_PUT;
            frame->order = ++chan->index;
            break;
        }
    }

    if (index >= USER_MAX_FS_NUM) {
        dprint(PRN_ERROR, "no match user frame.\n");
        return TD_NULL;
    }

    if (user_frm->is_frame_valid == TD_FALSE) {
        frame->state = FRM_IDLE;
        frame->order = 0;
    } else {
        /* resolve overflow */
        if (chan->index > USER_INDEX_OVERFLOW_NUM) {
            frame->order %= USER_INDEX_OVERFLOW_NUM;
            chan->index %= USER_INDEX_OVERFLOW_NUM;
        }
    }

    return frame;
}

STATIC td_void user_record_eos(td_s32 chan_id)
{
    user_ctx *chan = g_user_chan_ctx[chan_id];

    vfmw_check(chan != TD_NULL);
    chan->recv_eos++;
}

STATIC td_void user_try_report_eos(td_s32 chan_id)
{
    user_ctx *chan = g_user_chan_ctx[chan_id];

    vfmw_check(chan != TD_NULL);
    if (chan->recv_eos <= chan->report_eos) {
        return;
    }

    if (user_is_frame_list_empty(chan) == TD_TRUE) {
        td_u32 last_id_plus2 = vfmw_last_img_id(chan->last_image_id);
        user_event_report(chan_id, EVNT_LAST_FRAME, &last_id_plus2, sizeof(td_u32));
        chan->new_frm++;
        chan->report_eos++;
    }
}

STATIC td_s32 user_get_dec_stream(td_s32 chan_id, td_void *args, td_u32 len)
{
    td_s32 ret;

    vfmw_unused(len);
    vfmw_check_ret(args != TD_NULL, EXT_FAILURE);

    ret = stream_read_buffer(chan_id, (vfmw_stream_buf *)args);
    if (ret == STREAM_EOS) {
        user_record_eos(chan_id);
        ret = EXT_FAILURE;
    }

#ifdef VFMW_PRSAM_FLUSH_ALL_CACHE_SUPPORT
    os_flush_all_cache();
#endif

    return ret;
}

STATIC td_s32 user_put_dec_stream(td_s32 chan_id, td_void *args, td_u32 len)
{
    vfmw_unused(len);
    vfmw_check_ret(args != TD_NULL, EXT_FAILURE);

    return stream_release_buffer(chan_id, (vfmw_stream_buf *)args);
}

STATIC td_s32 user_get_dec_frame(td_s32 chan_id, td_void *args, td_u32 len)
{
    td_s32 ret;
    td_u32 offset = 0;
    user_frame *frm = TD_NULL;
    user_ctx *chan = g_user_chan_ctx[chan_id];
    vfmw_usrdec_frame_desc *user_frm = (vfmw_usrdec_frame_desc *)args;

    vfmw_unused(len);

    vfmw_check_ret(args != TD_NULL, EXT_FAILURE);
    vfmw_check_ret(chan != TD_NULL, EXT_FAILURE);

    if (chan->img_slot_len == 0) {
        if (chan->cfg.alloc_type == VFMW_FRM_ALLOC_INNER) {
            ret = user_alloc_frame_buffer(chan, (td_u32)user_frm->mem_size);
        } else {
            ret = user_alloc_ext_frame_buffer(chan, user_frm);
        }
        if (ret != EXT_SUCCESS) {
            dprint(PRN_ERROR, "user alloc frame buffer size %d failed.\n", user_frm->mem_size);
            return EXT_FAILURE;
        }
    } else if (chan->img_slot_len < user_frm->mem_size) {
        dprint(PRN_ERROR, "user not support resolutian change, slot len %d < frame size %d.\n",
               chan->img_slot_len, user_frm->mem_size);
        return EXT_FAILURE;
    }

    if ((g_user_ctx.dec_num != -1) && (chan->new_frm >= g_user_ctx.dec_num)) {
        return EXT_FAILURE;
    }

    frm = user_get_idle_frame(chan);
    if (frm == TD_NULL) {
        return EXT_FAILURE;
    }

    user_frm->phy_addr = frm->st_mem.phy_addr;
    offset += (td_u32)user_frm->mem_size;
    if (chan->cfg.module_lowly_enable == 1) {
        user_frm->line_num_offset = offset;
        user_frm->line_num_size = USER_LOWDELAY_LINE_SIZE;
        offset += user_frm->line_num_size;
    } else {
        user_frm->line_num_offset = 0;
        user_frm->line_num_size = 0;
    }
    user_frm->mem_size = frm->st_mem.length;

    if (offset > frm->st_mem.length) {
        dprint(PRN_ERROR, "user get frame failed: need size %d > slot size %d\n", offset, frm->st_mem.length);
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

STATIC td_s32 user_put_dec_frame(td_s32 chan_id, td_void *args, td_u32 len)
{
    vfmw_image *img = TD_NULL;
    user_frame *frm = TD_NULL;
    user_ctx *chan = g_user_chan_ctx[chan_id];
    vfmw_usrdec_frame_desc *user_frm = (vfmw_usrdec_frame_desc *)args;

    vfmw_unused(len);

    vfmw_check_ret(args != TD_NULL, EXT_FAILURE);
    vfmw_check_ret(chan != TD_NULL, EXT_FAILURE);

    frm = user_find_match_frame(chan, user_frm);
    vfmw_check_ret(frm != TD_NULL, EXT_FAILURE);

    img = &(frm->image);

    vfmw_check_sec_func(memset_s(img, sizeof(vfmw_image), 0, sizeof(vfmw_image)));

    img->is_user_dec = TD_TRUE;
    img->vid_std = chan->cfg.vid_std;
    img->image_id = (td_u32)frm->order;
    img->image_id_1 = -1;
    img->src_pts = user_frm->pts;
    img->pts = user_frm->pts;
    img->user_tag = (td_u64)user_frm->pts;
    img->disp_time = user_frm->disp_time;
    img->disp_enable_flag = 1;
    img->image_width = user_frm->width;
    img->image_height = user_frm->height;
    img->bit_depth = user_frm->bit_depth;
    img->yuv_format = user_frm->fmt;

    img->disp_info.linear_en = 1;
    img->disp_info.disp_width = user_frm->width;
    img->disp_info.disp_height = user_frm->height;
    img->disp_info.y_stride = user_frm->luma_stride;
    img->disp_info.uv_stride = user_frm->chrom_stride;
    img->disp_info.disp_center_x = img->disp_info.disp_width / 2;   /* 2: disp center in middle */
    img->disp_info.disp_center_y = img->disp_info.disp_height / 2;  /* 2: disp center in middle */
    img->disp_info.luma_phy_addr = user_frm->phy_addr;
    img->disp_info.chrom_phy_addr = user_frm->phy_addr + user_frm->chrom_offset;
    img->disp_info.linear_en = 1;

    img->format.frame_type = 0;
    img->format.sample_type = 0;
    img->format.video_format = 1;
    img->format.source_format = 0;
    img->format.field_valid_flag = 3; /* 3: both fields valid */
    img->format.top_field_first = 1;
    img->seq_img_cnt = chan->seq_cnt++;
    img->line_num_phy_addr = user_frm->phy_addr + user_frm->line_num_offset;

    user_event_report(chan_id, EVNT_NEW_IMAGE, (td_void *)img, sizeof(vfmw_image));
    chan->new_frm++;

#ifdef VFMW_PRSAM_FLUSH_ALL_CACHE_SUPPORT
    os_flush_all_cache();
#endif

    return EXT_SUCCESS;
}

STATIC td_s32 user_acquire_frame(td_s32 chan_id, vfmw_image *image_info)
{
    td_s32 i;
    td_s32 index = USER_INDEX_OVERFLOW_NUM;
    td_s32 order = USER_INDEX_OVERFLOW_NUM;
    td_s32 order_than_half_flag = 0;
    td_s32 half_index = (USER_INDEX_OVERFLOW_NUM + 1) / 2; /* 1: add 1 num; 2: use half num */
    td_u64 out_frame_addr;
    user_ctx *chan = g_user_chan_ctx[chan_id];

    vfmw_check_ret(chan != TD_NULL, EXT_FAILURE);
    vfmw_check_ret(image_info != TD_NULL, EXT_FAILURE);

    user_try_report_eos(chan_id);

    /* Get vfmw_min order one, it is the first output image */
    for (i = 0; i < USER_MAX_FS_NUM; i++) {
        if (chan->frm[i].state == FRM_PUT &&
            chan->frm[i].order >= half_index) {
            order_than_half_flag = 1;
            break;
        }
    }

    if (order_than_half_flag != 0) {
        for (i = 0; i < USER_MAX_FS_NUM; i++) {
            if (chan->frm[i].state == FRM_PUT &&
                chan->frm[i].order <= order &&
                chan->frm[i].order >= half_index) {
                order = chan->frm[i].order;
                index = i;
            }
        }
    } else {
        for (i = 0; i < USER_MAX_FS_NUM; i++) {
            if (chan->frm[i].state == FRM_PUT &&
                chan->frm[i].order <= order) {
                order = chan->frm[i].order;
                index = i;
            }
        }
    }

    if (index >= USER_MAX_FS_NUM) {
        return EXT_FAILURE;
    }

    chan->frm[index].state = FRM_ACQ;
    out_frame_addr = image_info->frame_ptr;
    vfmw_check_sec_func(memcpy_s(image_info, sizeof(vfmw_image),
                                 &(chan->frm[index].image), sizeof(chan->frm[index].image)));
    image_info->frame_ptr = out_frame_addr;
    chan->last_image_id = image_info->image_id;

    return EXT_SUCCESS;
}

STATIC td_s32 user_release_frame(td_s32 chan_id, const vfmw_image *image_info)
{
    td_s32 i;
    user_ctx *chan = g_user_chan_ctx[chan_id];

    vfmw_check_ret(chan != TD_NULL, EXT_FAILURE);
    vfmw_check_ret(image_info != TD_NULL, EXT_FAILURE);

    for (i = 0; i < USER_MAX_FS_NUM; i++) {
        if (chan->frm[i].order == (td_s32)image_info->image_id &&
            chan->frm[i].state == FRM_ACQ) {
            chan->frm[i].order = 0;
            chan->frm[i].state = FRM_IDLE;
            break;
        }
    }

    if (i >= USER_MAX_FS_NUM) {
        dprint(PRN_ERROR, "%s not found image %d\n", __func__, image_info->image_id);
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

STATIC td_s32 user_create_chan(td_s32 chan_id, td_void *args, td_u32 len)
{
    user_ctx *chan = TD_NULL;
    vfmw_chan_option *user_opt = TD_NULL;

    vfmw_unused(len);

    vfmw_check_ret(args != TD_NULL, EXT_FAILURE);
    vfmw_check_ret(chan_id < VFMW_CHAN_NUM, EXT_FAILURE);

    user_opt = (vfmw_chan_option *)args;

    if (g_user_chan_ctx[chan_id] != TD_NULL) {
        dprint_warn("%s chan is not null\n", __func__);
        return EXT_FAILURE;
    }

    g_user_chan_ctx[chan_id] = os_alloc_vir(chan_id, "user_inst", sizeof(user_ctx));
    chan = g_user_chan_ctx[chan_id];
    if (chan == TD_NULL) {
        dprint(PRN_ERROR, "%s chan OS_ALLOC_VIR_MEM fail\n", __func__);
        return EXT_FAILURE;
    }
    vfmw_check_sec_func(memset_s(chan, sizeof(user_ctx), 0, sizeof(user_ctx)));

    chan->chan_id = chan_id;
    chan->adpt_type = user_opt->adpt_type;
    chan->unid = os_get_unid();

    vfmw_check_ret(2 * USER_MAX_FS_NUM <= USER_INDEX_OVERFLOW_NUM, EXT_FAILURE); /* 2: twice fs num */

    chan->sec_flag = user_opt->is_sec_mode;

    user_opt->chan_id = chan_id;

    g_user_ctx.chan_num++;
    vdec_record_chr_event(chan_id, MT_VDEC_SET_ATTR, MT_VDEC_SET_HW_ATTR, MT_VFMW_CREATE_USER_CHAN);

    return EXT_SUCCESS;
}

STATIC td_s32 user_get_config(td_s32 chan_id, td_void *args, td_u32 len)
{
    user_ctx *chan = g_user_chan_ctx[chan_id];
    vfmw_chan_cfg *cfg = (vfmw_chan_cfg *)args;

    vfmw_unused(len);

    vfmw_check_ret(chan != TD_NULL, EXT_FAILURE);
    vfmw_check_ret(cfg != TD_NULL, EXT_FAILURE);

    vfmw_check_sec_func(memcpy_s(cfg, sizeof(vfmw_chan_cfg), &chan->cfg, sizeof(chan->cfg)));

    return EXT_SUCCESS;
}

STATIC td_s32 user_set_config(td_s32 chan_id, td_void *args, td_u32 len)
{
    user_ctx *chan = g_user_chan_ctx[chan_id];
    vfmw_chan_cfg *cfg = (vfmw_chan_cfg *)args;

    vfmw_unused(len);

    vfmw_check_ret(chan != TD_NULL, EXT_FAILURE);
    vfmw_check_ret(cfg != TD_NULL, EXT_FAILURE);

    vfmw_check_sec_func(memcpy_s(&chan->cfg, sizeof(chan->cfg), cfg, sizeof(vfmw_chan_cfg)));
    vdec_record_chr_event(chan_id, MT_VDEC_SET_ATTR, MT_VDEC_SET_MJPEG_ATTR, MT_VFMW_SET_USER_DEC_ATTR_OK);

    return EXT_SUCCESS;
}

STATIC td_s32 user_destroy_chan(td_s32 chan_id, td_void *args, td_u32 len)
{
    user_ctx *chan = g_user_chan_ctx[chan_id];

    vfmw_unused(len);
    vfmw_unused(args);

    vfmw_check_ret(chan != TD_NULL, EXT_FAILURE);

    if (chan->cfg.alloc_type != VFMW_FRM_ALLOC_OUTER) {
        user_free_frame_buffer(chan);
    }

    os_free_vir(chan);
    g_user_chan_ctx[chan_id] = TD_NULL;

    g_user_ctx.chan_num--;
    vdec_record_chr_event(chan_id, MT_VDEC_DESTROY, MT_VDEC_MJPEG_DESTROY, MT_VDEC_USER_DESTROY_OK);

    return EXT_SUCCESS;
}

STATIC td_s32 user_bind_ext_req_mem(td_s32 chan_id, td_void *args, td_u32 len)
{
    user_ctx *chan = g_user_chan_ctx[chan_id];
    td_s32 index;
    const vfmw_ext_buf *user_mem = TD_NULL;
    const vfmw_mem_report *tmp_mem = TD_NULL;

    vfmw_unused(len);

    vfmw_check_ret(args != TD_NULL, EXT_FAILURE);
    vfmw_check_ret(chan != TD_NULL, EXT_FAILURE);

    user_mem = (vfmw_ext_buf *)args;
    tmp_mem = &user_mem->ext_frm_buf;

    for (index = 0; index < USER_MAX_FS_NUM; index++) {
        if (chan->frm[index].st_mem.length == 0) {
            chan->frm[index].st_mem.length = tmp_mem->size;
            chan->frm[index].st_mem.phy_addr = tmp_mem->phy_addr;
            chan->frm[index].st_mem.vir_addr = tmp_mem->vir_addr;
            chan->frm[index].st_mem.dma_buf = tmp_mem->dma_buf;
            chan->frm[index].st_mem.fd = tmp_mem->mem_fd;
            chan->frm[index].state = FRM_IDLE;
            chan->frm_alloc_done_cnt++;
            break;
        }
    }

    if (index == USER_MAX_FS_NUM) {
        dprint(PRN_ERROR, "user frm has something wrong!\n");
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

STATIC td_s32 user_unbind_ext_req_mem(td_s32 chan_id, td_void *args, td_u32 len)
{
    user_ctx *chan = g_user_chan_ctx[chan_id];
    td_s32 index;
    const vfmw_ext_buf *user_mem = TD_NULL;
    const vfmw_mem_report *tmp_mem = TD_NULL;

    vfmw_unused(len);

    vfmw_check_ret(args != TD_NULL, EXT_FAILURE);
    vfmw_check_ret(chan != TD_NULL, EXT_FAILURE);

    user_mem = (vfmw_ext_buf *)args;
    tmp_mem = &user_mem->ext_frm_buf;

    for (index = 0; index < USER_MAX_FS_NUM; index++) {
        if (chan->frm[index].st_mem.phy_addr == tmp_mem->phy_addr) {
            chan->frm[index].st_mem.length = 0;
            chan->frm[index].st_mem.phy_addr = 0;
            chan->frm[index].st_mem.vir_addr = 0;
            chan->frm[index].st_mem.dma_buf = 0;
            chan->frm[index].st_mem.fd = 0;
            chan->frm[index].state = FRM_NULL;
            chan->frm_alloc_done_cnt = (chan->frm_alloc_done_cnt > 0) ? chan->frm_alloc_done_cnt - 1 : 0;
            break;
        }
    }

    if (index == USER_MAX_FS_NUM) {
        dprint(PRN_ERROR, "user frm has something wrong!\n");
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

STATIC td_s32 user_flush_chan(td_s32 chan_id, td_void *args, td_u32 len)
{
    td_s32 i;
    vfmw_flush_type flush_type;
    user_ctx *chan = g_user_chan_ctx[chan_id];
    td_u32 cnt = 0;

    (td_void)(cnt); /* avoid to build err when printing is not supported. */
    vfmw_unused(len);

    vfmw_check_ret(args != TD_NULL, EXT_FAILURE);
    vfmw_check_ret(chan != TD_NULL, EXT_FAILURE);

    flush_type = *((vfmw_flush_type *)args);

    if (flush_type == VFMW_FLUSH_OUT || flush_type == VFMW_FLUSH_ALL) {
        for (i = 0; i < USER_MAX_FS_NUM; i++) {
            if (chan->frm[i].state == FRM_GET || chan->frm[i].state == FRM_PUT) {
                chan->frm[i].state = FRM_IDLE;
                chan->frm[i].order = 0;
                cnt++;
            }
        }
    }

    dprint(PRN_ERROR, "user flush chan %d\n clear frame cnt %d", chan_id, cnt);

    return EXT_SUCCESS;
}

STATIC user_case g_user_case_table[] = {
    {VFMW_CID_CREATE_CHAN,           user_create_chan,        VFMW_CREATE_OPT_SIZE},
    {VFMW_CID_GET_CHAN_CFG,          user_get_config,         sizeof(vfmw_chan_cfg)},
    {VFMW_CID_CFG_CHAN,              user_set_config,         sizeof(vfmw_chan_cfg)},
    {VFMW_CID_DESTROY_CHAN,          user_destroy_chan,       0},
    {VFMW_CID_BIND_MEM,              user_bind_req_mem,       sizeof(vfmw_mem_report)},
    {VFMW_CID_GET_USRDEC_STREAM,     user_get_dec_stream,     sizeof(vfmw_stream_buf)},
    {VFMW_CID_PUT_USRDEC_STREAM,     user_put_dec_stream,     sizeof(vfmw_stream_buf)},
    {VFMW_CID_GET_USRDEC_FRAME,      user_get_dec_frame,      sizeof(vfmw_usrdec_frame_desc)},
    {VFMW_CID_PUT_USRDEC_FRAME,      user_put_dec_frame,      sizeof(vfmw_usrdec_frame_desc)},
    {VFMW_CID_BIND_EXTERN_BUFFER,    user_bind_ext_req_mem,   sizeof(vfmw_ext_buf)},
    {VFMW_CID_UNBIND_EXTERN_BUFFER,  user_unbind_ext_req_mem, sizeof(vfmw_ext_buf)},
    {VFMW_CID_FLUSH_CHAN,            user_flush_chan,         sizeof(vfmw_flush_type)},
    {VFMW_CID_MAX,                   TD_NULL,               0},
};

td_s32 user_open(td_void)
{
    td_s32 dec_num;
    user_global_ctx *ctx = &g_user_ctx;

    dec_num = ctx->dec_num;
    vfmw_check_sec_func(memset_s(ctx, sizeof(user_global_ctx), 0, sizeof(user_global_ctx)));
    ctx->dec_num = dec_num;
    vfmw_check_sec_func(memset_s(g_user_chan_ctx, sizeof(user_ctx*) * VFMW_CHAN_NUM, 0,
        sizeof(user_ctx*) * VFMW_CHAN_NUM));
    os_sema_init(&ctx->sema);

    return EXT_SUCCESS;
}

td_s32 user_close(td_void)
{
    user_global_ctx *ctx = &g_user_ctx;

    os_sema_exit(ctx->sema);

    return EXT_SUCCESS;
}

td_s32 user_init(td_void *args, td_u32 len)
{
    td_s32 ret;
    vfmw_init_param *param = (vfmw_init_param *)args;

    vfmw_unused(len);

    vfmw_check_ret(param != TD_NULL, EXT_FAILURE);
    os_sema_down(g_user_ctx.sema);

    ret = user_set_callback(param->adpt_type, param->fn_callback);

    os_sema_up(g_user_ctx.sema);

    return ret;
}

td_s32 user_control(td_s32 chan_id, td_s32 cmd, td_void *args, td_u32 len)
{
    td_s32 ret;
    td_u32 index;
    td_u32 case_num;
    user_case *ctr_case = TD_NULL;
    user_handler user_target_handler = TD_NULL;

    case_num = (td_u32)sizeof(g_user_case_table) / (td_u32)sizeof(user_case);
    for (index = 0; index < case_num; index++) {
        ctr_case = &g_user_case_table[index];
        if (cmd == (td_s32)ctr_case->cmd_id) {
            if (ctr_case->length > len) {
                dprint(PRN_ERROR, "%s cmd 0x%x param len invalid %d > %d\n", __func__, cmd, ctr_case->length, len);
                return EXT_FAILURE;
            }
            user_target_handler = ctr_case->handler;
            break;
        }
    }

    if (user_target_handler == TD_NULL) {
        return EXT_SUCCESS;
    }

    os_sema_down(g_user_ctx.sema);
    ret = user_target_handler(chan_id, args, len);
    os_sema_up(g_user_ctx.sema);

    return ret;
}

td_s32 user_get_image(td_s32 chan_id, td_void *user_image)
{
    td_s32 ret;

    os_sema_down(g_user_ctx.sema);

    ret = user_acquire_frame(chan_id, (vfmw_image *)user_image);

    os_sema_up(g_user_ctx.sema);

    return ret;
}

td_s32 user_release_image(td_s32 chan_id, const td_void *user_image)
{
    td_s32 ret;

    os_sema_down(g_user_ctx.sema);

    ret = user_release_frame(chan_id, (vfmw_image *)user_image);

    os_sema_up(g_user_ctx.sema);

    return ret;
}

td_s32 user_info_read(td_void *buf)
{
    td_s32 ret;

    os_sema_down(g_user_ctx.sema);
    ret = user_read_proc(buf);
    os_sema_up(g_user_ctx.sema);

    return ret;
}

td_s32 user_info_write(cmd_str_ptr buffer, td_u32 count)
{
    os_sema_down(g_user_ctx.sema);
    user_write_proc(buffer, count);
    os_sema_up(g_user_ctx.sema);

    return EXT_SUCCESS;
}

