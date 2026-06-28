/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#include "voque.h"
#include "fsp.h"
#include "event.h"
#include "dbg.h"
#include "vfmw_define.h"
#include "vdec_chr.h"
#ifdef VFMW_MVC_SUPPORT
#include "mvc_img.h"
#endif

static voq_entry g_voq_entry_info = {
    .reserve1 = 0x1234abcd,
    .ref_cnt = 0,
    .state = VOQ_STATE_NULL,
    .ctx_size = sizeof(voq_entry),
    .reserve2 = 0x1234abcd,
};

#define voq_assert_mod_goto(to)                     \
    if (g_voq_entry_info.state == VOQ_STATE_NULL) { \
        goto to;                     \
    }

#define voq_assert_chan_goto(chan_id, to) do {            \
    if (((chan_id) < 0) || ((chan_id) >= VOQ_CHAN_NUM)) { \
        goto to;                            \
    }                                                     \
    if (g_voq_entry_info.chan[chan_id] == TD_NULL) {      \
        goto to;                           \
    }                                                     \
} while (0)

#define voq_inc(x) (((x) + 1) % VOQ_IMAGE_NUM)
#define voq_dec(x) (((x) == 0) ? VOQ_IMAGE_NUM - 1 : (x) - 1)

/* inner func */
voq_entry *voq_get_entry(td_void)
{
    return &g_voq_entry_info;
}

voq_chan *voq_get_chan(td_s32 chan_id)
{
    if (chan_id < 0 || chan_id >= VOQ_CHAN_NUM) {
        return TD_NULL;
    }

    return voq_get_entry()->chan[chan_id];
}

td_void voq_sema_down(td_void)
{
    os_sema_down(voq_get_entry()->sema);
}

td_void voq_sema_up(td_void)
{
    os_sema_up(voq_get_entry()->sema);
}

STATIC td_void voq_lock(td_ulong *lock_flag)
{
    os_spin_lock(voq_get_entry()->lock, lock_flag);
}

STATIC td_void voq_unlock(td_ulong *lock_flag)
{
    os_spin_unlock(voq_get_entry()->lock, lock_flag);
}

STATIC td_s32 voq_is_full(td_s32 chan_id)
{
    td_s32 ret = VOQ_ERR;
    voq_chan *chan = TD_NULL;

    voq_assert_mod_goto(out);

    chan = voq_get_chan(chan_id);
    vfmw_check_goto(chan != TD_NULL, out);

    if (voq_inc(chan->tail) == chan->head ||
        voq_inc(chan->tail) == chan->history) {
        ret = VOQ_OK;
    }

out:

    return ret;
}

STATIC td_bool voq_is_empty(const voq_chan *chan)
{
    return (chan->tail == chan->head) ? TD_TRUE : TD_FALSE;
}

STATIC td_void voq_init_side_info(voq_side *side_info)
{
    side_info->prev_pts = VFMW_INVALID_PTS;
    side_info->old_aspect_ratio = 0xffffffff;
    side_info->old_disp_center = 0xffffffff;
    side_info->old_disp_size = 0xffffffff;
    side_info->old_scan_type = 0xffffffff;
    side_info->old_seq_cnt = 0xffffffff;
}

STATIC td_void voq_fill_pts(td_s32 chan_id, vfmw_image *image_info)
{
    voq_chan *chan = TD_NULL;
    voq_side *side_info = TD_NULL;
    td_s64 this_frame_pts;
    td_ulong lock_flag;

    chan = voq_get_chan(chan_id);
    if (chan == TD_NULL) {
        dprint(PRN_ERROR, "Get chan %d failed!\n", chan_id);
        return;
    }

    side_info = &chan->side_info;

    image_info->src_pts = image_info->pts;

    this_frame_pts = image_info->pts;

    fsp_img_format_lock(&lock_flag);
    if (this_frame_pts - side_info->prev_pts <= 5 && side_info->prev_pts != VFMW_INVALID_PTS) { /* 5: min pts gap */
        this_frame_pts = VFMW_INVALID_PTS;
        image_info->src_pts = VFMW_INVALID_PTS;
    }
    fsp_img_format_unlock(&lock_flag);

    image_info->pts = this_frame_pts;

    return;
}

STATIC td_void voq_fill_side_info(td_s32 chan_id, vfmw_image *image_info)
{
    td_u32 disp_size, disp_center;
    td_u32 aspect_ratio, scan_type;
    voq_chan *chan = TD_NULL;
    voq_side *side_info = TD_NULL;
    td_ulong lock_flag;

    chan = voq_get_chan(chan_id);
    if (chan == TD_NULL) {
        dprint(PRN_ERROR, "Get chan %d failed!\n", chan_id);
        return;
    }

    side_info = &chan->side_info;

    if (image_info->disp_info.disp_width == 0 || image_info->disp_info.disp_height == 0) {
        image_info->disp_info.disp_center_x = image_info->image_width / 2; /* 2 :a number */
        image_info->disp_info.disp_center_y = image_info->image_height / 2; /* 2 :a number */
        image_info->disp_info.disp_width = image_info->image_width;
        image_info->disp_info.disp_height = image_info->image_height;
    }

    fsp_img_format_lock(&lock_flag);
    aspect_ratio = image_info->format.aspect_ratio;
    fsp_img_format_unlock(&lock_flag);
    scan_type = image_info->format.source_format;
	/* 16 :a number */
    disp_size = (image_info->disp_info.disp_height << 16) |
		(image_info->disp_info.disp_width & 0xffff);
	/* 16 :a number */
    disp_center = (image_info->disp_info.disp_center_x << 16) |
		(image_info->disp_info.disp_center_y & 0xffff);

    image_info->seq_img_cnt = side_info->seq_img_cnt++;
    image_info->yuv_format = SPY_CB_CR_420;

    side_info->last_image_id_plus2 = vfmw_last_img_id(image_info->image_id);
    if (side_info->old_aspect_ratio != aspect_ratio) {
        side_info->old_aspect_ratio = aspect_ratio;
    }

    if (side_info->old_scan_type != scan_type) {
        side_info->old_scan_type = scan_type;
    }

    if (side_info->old_disp_size != disp_size || side_info->old_disp_center != disp_center) {
        side_info->old_disp_size = disp_size;
        side_info->old_disp_center = disp_center;
    }
}

STATIC td_void voq_fill_info(td_s32 chan_id, vfmw_image *image_info)
{
    voq_fill_pts(chan_id, image_info);
    voq_fill_side_info(chan_id, image_info);
}

STATIC td_void voq_clear_un_disp(voq_chan *chan)
{
    vfmw_image *image_info = TD_NULL;

    while (chan->tail != chan->head) {
        chan->tail = voq_dec(chan->tail);
        image_info = chan->img[chan->tail];
        if (image_info != TD_NULL) {
            fsp_set_disp(chan->id, (td_s32)image_info->image_id, 0);
            chan->img[chan->tail] = TD_NULL;
        }
    }
}

STATIC td_s32 voq_count(td_s32 chan_id)
{
    td_s32 ret;
    td_u32 cur_time_ms;
    td_u32 time_period;
    voq_chan *chan = TD_NULL;

    ret = VOQ_ERR;
    voq_assert_mod_goto(out);

    chan = voq_get_chan(chan_id);
    vfmw_check_goto(chan != TD_NULL, out);
    vfmw_check_ret(os_get_time_ms != TD_NULL, VOQ_ERR);

    chan->frm_cnt++;
    chan->frm_total++;

    cur_time_ms = os_get_time_ms();
    if (chan->last_time_ms == 0) {
        chan->last_time_ms = cur_time_ms;
    } else {
        time_period = cur_time_ms - chan->last_time_ms;
        if (time_period > 1000) { /* 1000 :a number */
            chan->frm_rate = chan->frm_cnt * 1000 / time_period; /* 1000 :a number */
            chan->frm_cnt = 0;
            chan->last_time_ms = cur_time_ms;
        }
    }

    ret = VOQ_OK;

out:

    return ret;
}

STATIC td_void voq_report_last_frame(voq_chan *chan, td_s32 flag)
{
    if (chan->speed < 0) {
        return;
    }

    report_last_frame(chan->id, flag);

    chan->rpt_eos_cnt++;
}

STATIC vfmw_image *voq_get_first_image(voq_chan *chan)
{
    td_s32 idx;
    td_s32 pre;
    td_s32 state;
    vfmw_image *que_img = TD_NULL;

    que_img = chan->img[chan->head];

    /* when VOQ exist the frame but it not send dps to DEC, the out_put_flag is 2 */
    do {
        state = fsp_check_img_dec_state(que_img, FSP_DEC_STATE_ERR);
        if (state != FSP_OK) {
            break;
        }
        fsp_clear_dec(chan->id, (td_s32)que_img->image_id);
        fsp_set_disp(chan->id, (td_s32)que_img->image_id, 0);
        if (chan->head < VOQ_IMAGE_NUM) {
            chan->img[chan->head] = TD_NULL;
        } else {
            dprint(PRN_ERROR, "%s %d chan->head overlimit!\n", __func__, __LINE__);
        }

        if (que_img->last_frame == 1) {
            chan->eos.type = VOQ_EOS_TYPE_FAIL;
            voq_report_last_frame(chan, 1);
            chan->rpt_eos_cnt++;
        }

        for (idx = chan->head; idx != chan->history; idx = voq_dec(idx)) {
            pre = (idx == 0) ? (VOQ_IMAGE_NUM - 1) : (idx - 1);
            if (idx < VOQ_IMAGE_NUM && pre < VOQ_IMAGE_NUM) {
                chan->img[idx] = chan->img[pre];
            } else {
                dprint(PRN_ERROR, "%s %d idx %d or pre %d overlimit!\n", __func__, __LINE__, idx, pre);
            }
        }
        chan->img[chan->history] = TD_NULL;
        chan->head = voq_inc(chan->head);
        chan->history = voq_inc(chan->history);
        que_img = chan->img[chan->head];
        if (voq_is_empty(chan) == TD_TRUE) {
            return TD_NULL;
        }
    } while (state == FSP_OK);

    return que_img;
}

STATIC td_s32 voq_get_view(voq_chan *chan, vfmw_image **param_img)
{
    vfmw_image *que_img = TD_NULL;

    if (voq_is_empty(chan) == TD_TRUE) {
        return VOQ_ERR;
    }

    que_img = voq_get_first_image(chan);
    if (que_img == TD_NULL) {
        return VOQ_ERR;
    }

    if (fsp_check_img_dec_state(que_img, FSP_DEC_STATE_OVER) == FSP_ERR) {
        return VOQ_ERR;
    }

    chan->head = voq_inc(chan->head);
    *param_img = que_img;

    return VOQ_OK;
}

STATIC td_bool voq_is_last_frm(const vfmw_image *image_info, const voq_eos *eos)
{
    if (image_info->last_frame == 1) {
        return TD_TRUE;
    }

    if (eos->last_img_id_plus2 == (td_s32)vfmw_last_img_id(image_info->image_id)) {
        return TD_TRUE;
    }

    return TD_FALSE;
}

STATIC td_bool voq_is_image_ready(voq_chan *chan, vfmw_image *img, td_bool *try_next)
{
    td_s32  state;
    td_bool is_3d;
    td_bool view_ok;
    td_bool fence_on;

    vfmw_unused(chan);
    vfmw_check_ret(img != TD_NULL, TD_FALSE);
    vfmw_check_ret(try_next != TD_NULL, TD_FALSE);
    *try_next = TD_TRUE;
    is_3d = (img->is_3d == 1) ? TD_TRUE : TD_FALSE;
    fence_on = TD_FALSE;

    /* 1: not 3d, fence on -> report */
    if (is_3d == TD_FALSE && fence_on == TD_TRUE) {
        return TD_TRUE;
    }

    state = fsp_check_img_dec_state(img, FSP_DEC_STATE_OVER);
    /* 2: not 3d, fence off, state ok -> report */
    if (is_3d == TD_FALSE && fence_on == TD_FALSE && state == FSP_OK) {
        return TD_TRUE;
    }

    view_ok = (img->view_id == 1 || (img->view_id == 0 && img->last_frame == 1)) ? TD_TRUE : TD_FALSE;
    /* 3: is 3d, fence off, state ok, view ok -> report; , view nok -> try next */
    if (is_3d == TD_TRUE && fence_on == TD_FALSE && state == FSP_OK) {
        return (view_ok == TD_TRUE) ? TD_TRUE : TD_FALSE;
    }

    *try_next = TD_FALSE;

    return TD_FALSE;
}

STATIC td_s32 voq_release_by_img_id(voq_chan *chan, td_u32 img_id)
{
    td_s32 ret = VOQ_ERR;
    td_s32 idx;
    td_s32 pre;
    td_s32 find_flag = 0;

    /* find the image */
    for (idx = chan->history; idx != chan->head; idx = voq_inc(idx)) {
        if (chan->img[idx] != TD_NULL) {
            if (img_id == chan->img[idx]->image_id) {
                find_flag = 1;
                break;
            }
        }
    }

    if (find_flag == 1) {
        for (; idx != chan->history; idx = voq_dec(idx)) {
            pre = (idx == 0) ? (VOQ_IMAGE_NUM - 1) : (idx - 1);
            if (idx >= 0 && idx < VOQ_IMAGE_NUM && pre >= 0 && pre < VOQ_IMAGE_NUM) {
                chan->img[idx] = chan->img[pre];
            }
        }

        chan->img[chan->history] = TD_NULL;
        chan->history = voq_inc(chan->history);

        (td_void)fsp_clear_dec(chan->id, (td_s32)img_id);
        (td_void)fsp_set_disp(chan->id, (td_s32)img_id, 0);
        ret = VOQ_OK;
    }

    return ret;
}

#ifdef VFMW_MVC_SUPPORT
STATIC td_s32 voq_reque_last_image(voq_chan *chan, vfmw_image *img)
{
    td_u16 last_idx;

    last_idx = voq_dec(chan->head);
    if (chan->img[last_idx]->image_id != img->image_id) {
        dprint(PRN_ERROR, "last image unmatch, reque failed %d %d.\n", chan->img[last_idx]->image_id, img->image_id);
        return VOQ_ERR;
    }
    chan->head = last_idx;

    return VOQ_OK;
}

STATIC td_s32 voq_get_view1(voq_chan *chan, vfmw_image *img0)
{
    td_s32 ret;
    vfmw_image *img1 = TD_NULL;
    mvc_img_state img1_state = MVC_IMG_NULL;

    if (img0->last_frame == 1) {
        mvc_img_set_view0_last_frame(img0);
        return VOQ_OK;
    }

    ret = voq_get_view(chan, &img1);
    if (ret != VOQ_OK) {
        (td_void)voq_reque_last_image(chan, img0);  /* requeue image0 */
        return VOQ_ERR;
    }

    ret = mvc_img_patch_two_view(img0, img1, &img1_state);
    if (ret != EXT_SUCCESS) {
        (td_void)voq_release_by_img_id(chan, img0->image_id);      /* discard image0 */
        if (img1_state == MVC_IMG_REQUE) {
            (td_void)voq_reque_last_image(chan, img1);             /* requeue image1 */
        } else if (img1_state == MVC_IMG_DISCARD) {
            (td_void)voq_release_by_img_id(chan, img1->image_id);  /* discard image1 */
        }
        return VOQ_ERR;
    }

    chan->view_num = 2; /* 2: stand for 2 views invalid */

    return VOQ_OK;
}
#endif

STATIC void voq_report_eos_normal(voq_chan *chan)
{
    if (chan->eos.state == VOQ_EOS_STATE_WAIT) {
        chan->eos.state = VOQ_EOS_STATE_OUT;
        voq_report_last_frame(chan, 1);
    }
}

STATIC void voq_report_eos_error(voq_chan *chan, td_s32 rcv_ok)
{
    if (rcv_ok != VOQ_OK &&
        voq_is_empty(chan) == TD_TRUE &&
        chan->eos.state == VOQ_EOS_STATE_WAIT) {
        chan->eos.state = VOQ_EOS_STATE_OUT;
        voq_report_last_frame(chan, 1);
    }
}

/* extern func */
td_s32 voq_get_mem_need_size(vfmw_svr_mem_need *mem)
{
    vfmw_check_ret(mem != TD_NULL, VOQ_ERR);
    mem->chan_cache_size += vfmw_align_up((td_u32)sizeof(voq_chan), VFMW_CTX_ALIGN);

    return VOQ_OK;
}

td_s32 voq_open(td_void)
{
    voq_entry *entry = TD_NULL;

    entry = voq_get_entry();
    if (entry->state != VOQ_STATE_NULL) {
        dprint(PRN_ERROR, "%s State Error \n", __func__);
        return VOQ_ERR;
    }

    os_sema_init(&entry->sema);
    os_spin_lock_init(&entry->lock);

    entry->state = VOQ_STATE_OPEN;
    vfmw_check_sec_func(memset_s(entry->chan, sizeof(entry->chan), 0, sizeof(entry->chan)));

    return VOQ_OK;
}

td_s32 voq_close(td_void)
{
    voq_entry *entry = TD_NULL;

    entry = voq_get_entry();
    if (entry->state == VOQ_STATE_NULL ||
        entry->state == VOQ_STATE_INIT) {
        dprint(PRN_ERROR, "%s State Error \n", __func__);
        return VOQ_ERR;
    }

    entry->state = VOQ_STATE_NULL;
    vfmw_check_sec_func(memset_s(entry->chan, sizeof(entry->chan), 0, sizeof(entry->chan)));
    os_sema_exit(entry->sema);
    os_spin_lock_exit(entry->lock);

    return VOQ_OK;
}

td_s32 voq_init(td_void)
{
    td_s32 ret = VOQ_ERR;
    voq_entry *entry = TD_NULL;

    voq_sema_down();

    entry = voq_get_entry();
    if (entry->state == VOQ_STATE_NULL ||
        entry->state == VOQ_STATE_INIT) {
        dprint(PRN_ERROR, "%s State Error \n", __func__);
        goto out;
    }

    entry->ref_cnt++;
    entry->state = VOQ_STATE_INIT;
    vfmw_check_sec_func(memset_s(entry->chan, sizeof(entry->chan), 0, sizeof(entry->chan)));
    ret = VOQ_OK;

out:

    voq_sema_up();

    return ret;
}

td_s32 voq_exit(td_void)
{
    td_s32 ret = VOQ_ERR;
    voq_entry *entry = TD_NULL;

    voq_sema_down();

    entry = voq_get_entry();
    if (entry->state != VOQ_STATE_INIT) {
        dprint(PRN_ERROR, "%s State Error \n", __func__);
        goto out;
    }

    entry->ref_cnt--;
    entry->state = VOQ_STATE_EXIT;
    vfmw_check_sec_func(memset_s(entry->chan, sizeof(entry->chan), 0, sizeof(entry->chan)));
    ret = VOQ_OK;

out:

    voq_sema_up();

    return ret;
}

td_s32 voq_create(td_s32 chan_id)
{
    td_s32 ret = VOQ_ERR;
    td_u32 size;
    td_u8 *vir = TD_NULL;
    voq_chan *chan = TD_NULL;
    td_ulong lock_flag = 0;
    mem_record mem = { 0 };

    vfmw_check_ret(os_get_unid != TD_NULL, VOQ_ERR);
    size = (td_u32)sizeof(voq_chan);
    mem.mem_name = "voq_chan";
    mem.length = (td_u32)(vfmw_align_up(size, VFMW_CTX_ALIGN));
    mem.is_cached = 0;
    mem.type = CACHE_MEM_TYPE;
    mem.unid = (td_u32)chan_id;
    vfmw_check_ret(os_malloc != TD_NULL, VOQ_ERR);
    os_malloc(&mem); /* VOQ_Inst */
    vir = mem.vir_addr;

    if (vir == TD_NULL) {
        dprint(PRN_ERROR, "VOQ AllocMem(%d) Error\n", size);
        return VOQ_ERR;
    }

    vfmw_check_sec_func(memset_s(vir, size, 0, size));

    voq_sema_down();
    voq_lock(&lock_flag);

    voq_assert_mod_goto(out);

    chan = voq_get_chan(chan_id);
    if (chan == TD_NULL) {
        chan = (voq_chan *)vir;
        chan->id = chan_id;
        chan->un_id = os_get_unid();
        chan->handle = chan;
        chan->magic_word = 100; /* 100 :a number */
        chan->state = 1;
        voq_init_side_info(&chan->side_info);

        voq_get_entry()->chan[chan_id] = chan;
        voq_get_entry()->ctx_size += size;
        ret = VOQ_OK;
    }

out:
    voq_unlock(&lock_flag);
    voq_sema_up();

    if (ret != VOQ_OK) {
        os_free(&mem);
    }
    vdec_record_chr_event(chan_id, MT_VDEC_SET_ATTR, MT_VDEC_SET_HW_ATTR, MT_VFMW_CREATE_VOQ_OK);

    return ret;
}

td_s32 voq_reset(td_s32 chan_id)
{
    td_s32 ret;
    voq_chan *chan = TD_NULL;
    td_ulong lock_flag;

    ret = VOQ_ERR;
    voq_lock(&lock_flag);

    voq_assert_mod_goto(out);

    chan = voq_get_chan(chan_id);
    vfmw_check_goto(chan != TD_NULL, out);

    voq_clear_un_disp(chan);

    voq_init_side_info(&chan->side_info);

    chan->view_num = 1;
    chan->rcv_try = 0;
    chan->rcv_ok = 0;
    chan->rls_try = 0;
    chan->rls_ok = 0;
    chan->frm_total = 0;
    chan->last_time_ms = 0;
    chan->frm_rate = 0;
    chan->frm_cnt = 0;
    chan->eos.state = VOQ_EOS_STATE_NULL;
    chan->eos.type = VOQ_EOS_TYPE_NULL;
    chan->rep_id = chan->tail;

    ret = VOQ_OK;

out:
    voq_unlock(&lock_flag);
    vdec_record_chr_event(chan_id, MT_VDEC_RESET, MT_VDEC_VFMW_RESET, MT_VFMW_VOQ_RESET_OK);

    return ret;
}

td_s32 voq_destroy(td_s32 chan_id)
{
    td_s32 ret;
    voq_chan *chan = TD_NULL;
    td_ulong lock_flag = 0;
    mem_record mem = { 0 };

    ret = VOQ_ERR;
    voq_sema_down();
    voq_lock(&lock_flag);

    voq_assert_mod_goto(out);

    chan = voq_get_chan(chan_id);
    vfmw_check_goto(chan != TD_NULL, out);

    voq_get_entry()->chan[chan_id] = TD_NULL;
    voq_get_entry()->ctx_size -= (td_u32)sizeof(voq_chan);
    ret = VOQ_OK;

out:

    voq_unlock(&lock_flag);
    voq_sema_up();

    if (chan != TD_NULL) {
        mem.vir_addr = (td_u8 *)(chan);
        mem.is_cached = 0;
        os_free(&mem);
    }

    return ret;
}

STATIC td_void voq_report_inner(voq_chan *chan)
{
    td_s32 idx;
    td_bool try_next = TD_FALSE;
    vfmw_image *img = TD_NULL;

    vfmw_check(chan != TD_NULL);

    idx = chan->rep_id;
    while (idx != chan->tail && chan->img[idx] != TD_NULL) {
        img = chan->img[idx];
        if (fsp_check_img_dec_state(img, FSP_DEC_STATE_ERR) == FSP_OK) {
            idx = voq_inc(idx);
            continue;
        }

        if (img->out_put_flag == 1) {
            break;
        }

        if (voq_is_image_ready(chan, img, &try_next) == TD_TRUE) {
            report_imgrdy(chan->id, img);
            chan->new_frm_cnt++;
            img->out_put_flag = 1;
        }

        if (try_next == TD_FALSE) {
            break;
        }

        idx = voq_inc(idx);
    }

    chan->rep_id = idx;
}

td_s32 voq_insert(td_s32 chan_id, vfmw_image *image_info)
{
    td_s32 ret = VOQ_ERR;
    td_s32 idx;
    voq_chan *chan = TD_NULL;
    td_ulong lock_flag;

    vfmw_check_ret(image_info != TD_NULL, VOQ_ERR);
    voq_lock(&lock_flag);

    voq_assert_mod_goto(out);

    if (voq_is_full(chan_id) == VOQ_OK) {
        goto out;
    }

    chan = voq_get_chan(chan_id);
    vfmw_check_goto(chan != TD_NULL, out);

    for (idx = 0; idx < VOQ_IMAGE_NUM; idx++) {
        if (chan->img[idx] != TD_NULL &&
            chan->img[idx]->image_id == image_info->image_id) {
            dprint(PRN_ERROR, "image_id = %d is in voq \n", image_info->image_id);
            ret = VOQ_OK;
            goto out;
        }
    }

    image_info->last_frame = 0;
    image_info->out_put_flag = 0;
    chan->img[chan->tail] = image_info;
    chan->tail = voq_inc(chan->tail);

    voq_fill_info(chan_id, image_info);
    voq_count(chan_id);

    ret = VOQ_OK;

out:
    voq_report_inner(chan);
    voq_unlock(&lock_flag);

    return ret;
}

td_s32 voq_delete(td_s32 chan_id, const vfmw_image *image_info)
{
    td_s32 ret = VOQ_ERR;
    td_s32 idx;
    td_s32 cur_idx;
    td_s32 nxt_idx;
    td_s32 dlt_idx = -1;
    voq_chan *chan = TD_NULL;
    td_ulong lock_flag;

    vfmw_check_ret(image_info != TD_NULL, VOQ_ERR);
    voq_lock(&lock_flag);

    voq_assert_mod_goto(out);

    chan = voq_get_chan(chan_id);
    vfmw_check_goto(chan != TD_NULL, out);

    if (voq_is_empty(chan) == TD_TRUE) {
        goto out;
    }

    for (idx = 0; idx < VOQ_IMAGE_NUM; idx++) {
        if (chan->img[idx] != TD_NULL &&
            chan->img[idx]->image_id == image_info->image_id) {
            dlt_idx = idx;
        }
    }

    if (dlt_idx == -1 || dlt_idx == chan->tail) {
        goto out;
    }

    for (idx = 0; idx < VOQ_IMAGE_NUM; idx++) {
        cur_idx = (idx + dlt_idx) % VOQ_IMAGE_NUM;
        nxt_idx = (idx + dlt_idx + 1) % VOQ_IMAGE_NUM;
        if (nxt_idx == chan->tail) {
            break;
        }

        chan->img[cur_idx] = chan->img[nxt_idx];
    }

    chan->tail = (chan->tail + VOQ_IMAGE_NUM - 1) % VOQ_IMAGE_NUM;
    chan->img[chan->tail] = TD_NULL;

    ret = VOQ_OK;

out:

    voq_unlock(&lock_flag);

    return ret;
}

td_s32 voq_receive(td_s32 chan_id, vfmw_image *orig_img)
{
    td_s32 ret;
    td_u64 out_frm_ptr;
    vfmw_image *out_img = TD_NULL;
    voq_chan *chan = TD_NULL;
    td_ulong lock_flag;

    ret = VOQ_ERR;
    voq_lock(&lock_flag);

    voq_assert_mod_goto(out_unlock);

    chan = voq_get_chan(chan_id);
    vfmw_check_goto(chan != TD_NULL, out_unlock);
    voq_report_eos_normal(chan);

    chan->rcv_try++;
    out_img = orig_img;
    vfmw_check_goto(out_img != TD_NULL, out);
    out_frm_ptr = orig_img->frame_ptr;

    ret = voq_get_view(chan, &orig_img);
    if (ret != VOQ_OK) {
        goto out;
    }

#ifdef VFMW_MVC_SUPPORT
    if (orig_img->is_3d == 1) {
        ret = voq_get_view1(chan, orig_img);
        if (ret != VOQ_OK) {
            goto out;
        }
    }
#endif

    orig_img->out_put_flag = 0;
    vfmw_check_sec_func(memcpy_s(out_img, sizeof(vfmw_image), orig_img, sizeof(vfmw_image)));
    out_img->frame_ptr = out_frm_ptr;

    if (voq_is_last_frm(out_img, &chan->eos) == TD_TRUE) {
        chan->eos.state = VOQ_EOS_STATE_WAIT;
    }

    out_img->image_id += chan->magic_word;
    if (out_img->image_id_1 != -1) {
        out_img->image_id_1 += (td_s32)chan->magic_word;
    }
    chan->rcv_ok++;

out:
    voq_report_eos_error(chan, ret);
out_unlock:
    voq_unlock(&lock_flag);

    return ret;
}

td_s32 voq_release(td_s32 chan_id, vfmw_image_rls *img)
{
    td_s32 ret = VOQ_ERR;
    voq_chan *chan = TD_NULL;
    td_ulong lock_flag;

    vfmw_check_ret(img != TD_NULL, VOQ_ERR);
    voq_lock(&lock_flag);

    voq_assert_mod_goto(out);

    chan = voq_get_chan(chan_id);
    if (chan == TD_NULL) {
        goto out;
    }

    chan->rls_try++;

    if (chan->history == chan->head) {
        goto out;
    }

    img->image_id -= chan->magic_word;
    ret = voq_release_by_img_id(chan, img->image_id);
    if (ret != VOQ_OK) {
        dprint(PRN_WARN, "%s rel image id %d failed.\n", __func__, img->image_id);
    }

#ifdef VFMW_MVC_SUPPORT
    if (img->is_3d == 1 && img->image_id_1 != -1) {
        img->image_id_1 -= chan->magic_word;
        ret = voq_release_by_img_id(chan, img->image_id_1);
        if (ret != VOQ_OK) {
            dprint(PRN_ERROR, "%s rel image1 id %d failed.\n", __func__, img->image_id_1);
        }
    }
#endif

    if (ret == VOQ_OK) {
        chan->rls_ok++;
    }

out:
    voq_unlock(&lock_flag);

    return ret;
}

td_s32 voq_check_img(td_s32 chan_id, vfmw_image *src_img)
{
    td_s32 ret = VOQ_ERR;
    td_s32 idx;
    td_s32 img_exist;
    td_u32 img_id;
    vfmw_image *img = TD_NULL;
    voq_chan *chan = TD_NULL;
    td_ulong lock_flag;

    vfmw_check_ret(src_img != TD_NULL, VOQ_ERR);
    voq_lock(&lock_flag);

    voq_assert_mod_goto(out);

    chan = voq_get_chan(chan_id);
    vfmw_check_goto(chan != TD_NULL, out);

    img_exist = 0;
    img_id = src_img->image_id - chan->magic_word;

    for (idx = 0; idx < VOQ_IMAGE_NUM; idx++) {
        img = chan->img[idx];
        if (img != TD_NULL && img->image_id == img_id) {
            img_exist = 1;
            if (fsp_check_img_dec_state(img, FSP_DEC_STATE_OVER) == FSP_OK) {
                src_img->error_level = 0;
                ret = VOQ_OK;
                break;
            } else if (fsp_check_img_dec_state(img, FSP_DEC_STATE_ERR) == FSP_OK) {
                src_img->error_level = 1;
                ret = VOQ_OK;
                break;
            }
        }
    }

    if (img_exist == 0) {
        src_img->error_level = 1;
        ret = VOQ_OK;
    }

out:

    voq_unlock(&lock_flag);

    return ret;
}

td_s32 voq_is_img_in(td_s32 chan_id, const vfmw_image *image_info)
{
    td_s32 ret = VOQ_ERR;
    td_s32 idx;
    voq_chan *chan = TD_NULL;
    td_ulong lock_flag;

    vfmw_check_ret(image_info != TD_NULL, VOQ_ERR);
    voq_lock(&lock_flag);

    voq_assert_mod_goto(out);

    chan = voq_get_chan(chan_id);
    vfmw_check_goto(chan != TD_NULL, out);

    if (voq_is_empty(chan) == TD_TRUE) {
        goto out;
    }

    for (idx = 0; idx < VOQ_IMAGE_NUM; idx++) {
        if (chan->img[idx] != TD_NULL) {
            if (image_info->image_id == chan->img[idx]->image_id) {
                ret = VOQ_OK;
                break;
            }
        }
    }

out:

    voq_unlock(&lock_flag);

    return ret;
}

td_s32 voq_report(td_s32 chan_id)
{
    td_s32 idx;
    td_bool try_next = TD_FALSE;
    vfmw_image *img = TD_NULL;
    voq_chan *chan = TD_NULL;
    td_ulong lock_flag;

    voq_lock(&lock_flag);

    voq_assert_mod_goto(err_out);

    chan = voq_get_chan(chan_id);
    vfmw_check_goto(chan != TD_NULL, err_out);

    idx = chan->rep_id;
    while (idx != chan->tail && chan->img[idx] != TD_NULL) {
        img = chan->img[idx];
        if (fsp_check_img_dec_state(img, FSP_DEC_STATE_ERR) == FSP_OK) {
            idx = voq_inc(idx);
            continue;
        }

        if (img->out_put_flag == 1) {
            break;
        }

        if (voq_is_image_ready(chan, img, &try_next) == TD_TRUE) {
            report_imgrdy(chan_id, img);
            chan->new_frm_cnt++;
            img->out_put_flag = 1;
        }

        if (try_next == TD_FALSE) {
            break;
        }

        idx = voq_inc(idx);
    }

    chan->rep_id = idx;
    voq_unlock(&lock_flag);

    return VOQ_OK;

err_out:
    voq_unlock(&lock_flag);

    return VOQ_ERR;
}

td_void voq_last_frm(td_s32 chan_id)
{
    td_s32 last_id;
    voq_chan *chan = TD_NULL;
    td_ulong lock_flag;

    voq_lock(&lock_flag);

    voq_assert_mod_goto(out);

    chan = voq_get_chan(chan_id);
    vfmw_check_goto(chan != TD_NULL, out);

    if (chan->head != chan->tail) {
        last_id = (chan->tail + VOQ_IMAGE_NUM - 1);
        last_id = last_id % VOQ_IMAGE_NUM;
        chan->img[last_id]->last_frame = 1;
        chan->eos.type = VOQ_EOS_TYPE_SUCC;
    } else {
        chan->eos.type = VOQ_EOS_TYPE_FAIL;
        chan->eos.state = VOQ_EOS_STATE_WAIT;
    }

out:
    voq_unlock(&lock_flag);

    return;
}

td_s32 voq_print(td_s32 chan_id)
{
    td_s32 ret;
    td_s32 idx;
    voq_chan *chan = TD_NULL;
    td_ulong lock_flag;

    ret = VOQ_ERR;
    voq_lock(&lock_flag);

    voq_assert_mod_goto(out);

    chan = voq_get_chan(chan_id);
    vfmw_check_goto(chan != TD_NULL, out);

    for (idx = 0; idx < VOQ_IMAGE_NUM; idx++) {
        if (chan->img[idx] != TD_NULL) {
            dprint(PRN_ALWS,
                "[%d] image_id %d image_id_1 %d last_frame %d,"
                "out_put_flag %d Tail %d Head %d >History %d DecState(%d, %d) \n",
                idx, chan->img[idx]->image_id, chan->img[idx]->image_id_1, chan->img[idx]->last_frame,
                chan->img[idx]->out_put_flag, chan->tail, chan->head, chan->history,
                chan->img[idx]->dec_over.bits.top_over, chan->img[idx]->dec_over.bits.bot_over);
        }
    }
    ret = VOQ_OK;

out:
    voq_unlock(&lock_flag);

    return ret;
}

td_s32 voq_set_ext_intf(const voq_ext_intf *intf)
{
    vfmw_unused(intf);
    return VOQ_OK;
}

td_s32 voq_get_info(td_s32 chan_id, td_void *args)
{
    td_s32 ret;
    td_ulong lock_flag;
    voq_chan *chan = TD_NULL;
    voq_info *info = (voq_info *)args;

    ret = FSP_ERR;
    vfmw_check_ret(info != TD_NULL, VOQ_ERR);
    vfmw_check_ret(chan_id >= 0 && chan_id < VOQ_CHAN_NUM, VOQ_ERR);

    voq_lock(&lock_flag);

    voq_assert_mod_goto(out);
    chan = voq_get_chan(chan_id);
    vfmw_check_goto(chan != TD_NULL, out);

    info->recv_frm_num = chan->rcv_ok;
    info->dec_frm_rate = chan->frm_rate;
    ret = VOQ_OK;

out:
    voq_unlock(&lock_flag);

    return ret;
}

td_s32 voq_get_queue_img_num(td_s32 chan_id, td_u32 *read_num, td_u32 *new_num)
{
    td_s32 ret;
    voq_chan *chan = voq_get_chan(chan_id);
    td_ulong lock_flag;
    vfmw_check_ret(chan != TD_NULL, VOQ_ERR);
    vfmw_check_ret(read_num != TD_NULL, VOQ_ERR);
    vfmw_check_ret(new_num != TD_NULL, VOQ_ERR);

    voq_lock(&lock_flag);
    *read_num = (chan->head + VOQ_IMAGE_NUM - chan->history) % VOQ_IMAGE_NUM;
    *new_num  = (chan->tail + VOQ_IMAGE_NUM - chan->head) % VOQ_IMAGE_NUM;
    ret = VOQ_OK;
    voq_unlock(&lock_flag);

    return ret;
}

td_s32 voq_flush(td_s32 chan_id)
{
    td_s32 ret;
    td_u32 idx;
    voq_chan *chan = TD_NULL;
    td_ulong lock_flag;

    ret = VOQ_ERR;

    voq_lock(&lock_flag);
    chan = voq_get_chan(chan_id);
    vfmw_check_goto(chan != TD_NULL, out);

    for (idx = chan->history; idx != chan->head; idx = voq_inc(idx)) {
        chan->img[idx] = TD_NULL;
        chan->history = voq_inc(chan->history);
        chan->rls_try++;
        chan->rls_ok++;
    }
    ret = VOQ_OK;
out:
    voq_unlock(&lock_flag);

    return ret;
}

td_s32 voq_check_eos(td_s32 chan_id, td_s32 *flag)
{
    voq_chan *chan = TD_NULL;
    td_ulong lock_flag;

    vfmw_check_ret(flag != TD_NULL, VOQ_ERR);

    voq_lock(&lock_flag);

    chan = voq_get_chan(chan_id);
    if (chan == TD_NULL || chan->eos.state != VOQ_EOS_STATE_OUT) {
        voq_unlock(&lock_flag);
        return VOQ_ERR;
    }

    *flag = (chan->eos.type == VOQ_EOS_TYPE_FAIL) ? 1 : 0;
    chan->eos.state = VOQ_EOS_STATE_NULL;

    voq_unlock(&lock_flag);

    return VOQ_OK;
}

td_void voq_set_speed(td_s32 chan_id, td_s32 speed)
{
    voq_chan *chan = TD_NULL;
    td_ulong lock_flag;

    voq_lock(&lock_flag);

    chan = voq_get_chan(chan_id);
    if (chan == TD_NULL) {
        voq_unlock(&lock_flag);
        return;
    }

    chan->speed = speed;

    voq_unlock(&lock_flag);

    return;
}

td_s32 voq_config(td_s32 chan_id, const td_void *args)
{
    voq_chan *chan = TD_NULL;

    vfmw_check_ret(args != TD_NULL, VOQ_ERR);
    voq_sema_down();

    chan = voq_get_chan(chan_id);
    if (chan == TD_NULL) {
        voq_sema_up();
        return VOQ_ERR;
    }

    chan->cfg.frm_fence_enable = ((voq_cfg_info*)args)->frm_fence_enable;

    voq_sema_up();

    return VOQ_OK;
}
