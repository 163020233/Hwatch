/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#include "syn_parse.h"
#include "event.h"
#include "syn_ctrl.h"
#include "syn_log.h"
#include "vdec_chr.h"

static syn_parse_ctx g_syn_parse_ctx[VFMW_CHAN_NUM];

td_void syn_parse_get_ctx(syn_parse_ctx **ctx, td_s32 chan_id)
{
    if (ctx != TD_NULL) {
        (*ctx) = &(g_syn_parse_ctx[chan_id]);
    }
}

STATIC td_s32 syn_parse_seg_man_wait(
    const syn_cfg_info *syn_extra_data, stream_seg_packet *seg_man)
{
    if ((syn_extra_data->dynamic_fs_en == 1) &&
        (syn_extra_data->wait_fs_flag == 1) &&
        (seg_man->scd_wrap_around == 1)) {
        vfmw_check_sec_func(memcpy_s(seg_man->pack, sizeof(dec_stream_packet),
            seg_man->last_pack, sizeof(dec_stream_packet)));
        seg_man->scd_wrap_around = 0;

        return SYN_OK;
    }

    seg_man->scd_wrap_around = 0;

    return SYN_ERR;
}

STATIC td_s32 syn_parse_seg_man_last_and_non_conv(stream_seg_packet *seg_man)
{
    if ((seg_man->current_seg->is_last_seg == 1) && (seg_man->convolution_packet_num == 0)) {
        vfmw_check_sec_func(memcpy_s(&(seg_man->pack->stream_pack[0]),
            sizeof(seg_man->pack->stream_pack[0]), seg_man->current_seg, sizeof(stm_seg_info)));
        vfmw_check_sec_func(memset_s(&(seg_man->pack->stream_pack[1]),
            sizeof(stm_seg_info), 0, sizeof(stm_seg_info)));
        seg_man->pack->stream_pack[1].stream_id = -1;
        seg_man->convolution_packet_num = 0;

        return SYN_OK;
    }

    return SYN_ERR;
}

STATIC td_s32 syn_parse_seg_man_non_last(
    td_s32 chan_id, stream_seg_packet *seg_man)
{
    vfmw_check_ret(seg_man->current_seg != TD_NULL, SYN_ERR);

    if ((seg_man->current_seg->is_last_seg == 0) && (seg_man->pack != TD_NULL)) {
        if (seg_man->convolution_packet_num >= 1) { /* 2 :a number */
            stm_release_seg(chan_id, seg_man->pack->stream_pack[0].stream_id);
            stm_release_seg(chan_id, seg_man->current_seg->stream_id);

            vfmw_check_sec_func(memset_s(&(seg_man->pack->stream_pack[0]),
                sizeof(stm_seg_info), 0, sizeof(stm_seg_info)));
            vfmw_check_sec_func(memset_s(&(seg_man->pack->stream_pack[1]),
                sizeof(stm_seg_info), 0, sizeof(stm_seg_info)));

            dprint(PRN_ERROR, "More than two packet!!!ERROR!!!\n");
            report_decsyntax_err(chan_id);
            seg_man->convolution_packet_num = 0;
        } else {
            vfmw_check_sec_func(memcpy_s(&(seg_man->pack->stream_pack[0]),  sizeof(seg_man->pack->stream_pack[0]),
                seg_man->current_seg,  sizeof(stm_seg_info)));
            vfmw_check_sec_func(memset_s(&(seg_man->pack->stream_pack[1]),  sizeof(seg_man->pack->stream_pack[1]),
                0, sizeof(stm_seg_info)));
            seg_man->pack->stream_pack[1].stream_id = -1;

            seg_man->convolution_packet_num++;
        }

        return SYN_OK;
    }

    return SYN_ERR;
}

STATIC td_s32 syn_parse_seg_man_last_and_conv(
    td_s32 chan_id, stream_seg_packet *seg_man)
{
    td_s32 ret = SYN_ERR;

    vfmw_check_ret(seg_man->current_seg != TD_NULL, SYN_ERR);

    if ((seg_man->current_seg->is_last_seg == 1) && (seg_man->convolution_packet_num == 1) &&
        (seg_man->pack != TD_NULL)) {
        if (seg_man->pack->stream_pack[0].length < SEG_TOP_BLANK_SIZE) {
            vfmw_check_sec_func(memcpy_s((seg_man->current_seg->vir_addr - seg_man->pack->stream_pack[0].length),
                SEG_TOP_BLANK_SIZE, seg_man->pack->stream_pack[0].vir_addr, seg_man->pack->stream_pack[0].length));

            seg_man->pack->stream_pack[0].vir_addr =
                seg_man->current_seg->vir_addr - seg_man->pack->stream_pack[0].length;
            seg_man->pack->stream_pack[0].phy_addr =
                seg_man->current_seg->phy_addr - seg_man->pack->stream_pack[0].length;
            seg_man->pack->stream_pack[0].length =
                seg_man->pack->stream_pack[0].length + seg_man->current_seg->length;
            seg_man->pack->stream_pack[0].is_last_seg = 1;
            stm_release_seg(chan_id, seg_man->pack->stream_pack[0].stream_id);
            seg_man->pack->stream_pack[0].stream_id = seg_man->current_seg->stream_id;

            vfmw_check_sec_func(memset_s(&(seg_man->pack->stream_pack[1]),
                sizeof(stm_seg_info), 0, sizeof(stm_seg_info)));
            seg_man->pack->stream_pack[1].stream_id = -1;

            ret = SYN_OK;
        } else {
            vfmw_check_sec_func(memcpy_s(&(seg_man->pack->stream_pack[1]),
                sizeof(seg_man->pack->stream_pack[1]), seg_man->current_seg, sizeof(stm_seg_info)));

            ret = SYN_OK;
        }

        seg_man->convolution_packet_num = 0;
        seg_man->scd_wrap_around = 1;
        vfmw_check_sec_func(memcpy_s(seg_man->last_pack,
            sizeof(dec_stream_packet), seg_man->pack, sizeof(dec_stream_packet)));
    }

    return ret;
}

td_s32 syn_parse_seg_man(td_s32 chan_id, const syn_cfg_info *syn_extra_data, stream_seg_packet *seg_man)
{
    if (seg_man == TD_NULL ||
        syn_extra_data == TD_NULL ||
        seg_man->current_seg == TD_NULL ||
        seg_man->current_seg->vir_addr == TD_NULL) {
        dprint(PRN_ERROR, "seg_man error!\n");
        return SYN_ERR;
    }

    if (syn_parse_seg_man_wait(syn_extra_data, seg_man) == SYN_OK) {
        return SYN_OK;
    }

    if (syn_parse_seg_man_last_and_non_conv(seg_man) == SYN_OK) {
        return SYN_OK;
    }

    if (syn_parse_seg_man_non_last(chan_id, seg_man) == SYN_OK) {
        return SYN_ERR;
    }

    return syn_parse_seg_man_last_and_conv(chan_id, seg_man);
}

STATIC void swap_kn(char *a, char *b, td_u32 width)
{
    char tmp;
    td_u32 width_tmp = width;

    if (a != b) {
        /* Do the swap one character at a time
        to avoid potential alignment problems. */
        while ((width_tmp--) != 0) {
            tmp = *a;
            *a++ = *b;
            *b++ = tmp;
        }
    }
}

STATIC void shortsort(char *low, char *high, td_u32 width, int (*comp)(const void *, const void *))
{
    char *p = TD_NULL;
    char *vfmw_max = TD_NULL;

    vfmw_check(width != 0);
    /* Note: in assertions below, i and j are alway
    inside original bound of array to sort. */
    while (high > low) {
        /* A[i] <= A[j] for i <= j, j > high */
        vfmw_max = low;

        for (p = low + width; p <= high; p += width) {
            /* A[i] <= A[vfmw_max] for low <= i < p */
            if (comp(p, vfmw_max) > 0) {
                vfmw_max = p;
            }
            /* A[i] <= A[vfmw_max] for low <= i <= p */
        }

        /* A[i] <= A[vfmw_max] for low <= i <= high */
        swap_kn(vfmw_max, high, width);

        /* A[i] <= A[high] for i <= high, so A[i] <= A[j] for i <= j, j >= high */
        high -= width;

        /* A[i] <= A[j] for i <= j, j > high, loop top condition established */
    }
    /* A[i] <= A[j] for i <= j, j > low, which implies A[i] <= A[j] for i < j,
       so array is sorted */
}

td_void syn_qsort(td_void *base, td_u32 num, td_u32 width, td_s32 (*comp)(const td_void *, const td_void *))
{
    char *low = TD_NULL;
    char *high = TD_NULL;

    if (num < 2 || width == 0 || base == TD_NULL) { /* 2 :a number */
        return;
    }

    low = base;
    high = (char *)base + width * (num - 1);
    shortsort(low, high, width, comp);
}

td_void syn_set_aspect_ratio(vfmw_image *img, vfmw_vdec_dar code)
{
    vfmw_check(img != TD_NULL);

    switch (code) {
        case VFMW_DAR_UNKNOWN:
            img->aspect_width = 0;
            img->aspect_height = 1;
            break;

        case VFMW_DAR_4_3:
            img->aspect_width = 4; /* 4 :a number */
            img->aspect_height = 3; /* 3 :a number */
            break;

        case VFMW_DAR_16_9:
            img->aspect_width = 16; /* 16 :a number */
            img->aspect_height = 9; /* 9 :a number */
            break;

        case VFMW_DAR_221_100:
            img->aspect_width = 221; /* 221 :a number */
            img->aspect_height = 100; /* 100 :a number */
            break;

        case VFMW_DAR_235_100:
            img->aspect_width = 235; /* 235 :a number */
            img->aspect_height = 100; /* 100 :a number */
            break;

        case VFMW_DAR_IMG_SIZE:
            img->aspect_width = 0;
            img->aspect_height = 2; /* 2 :a number */
            break;

        default:
            img->aspect_width = 0;
            img->aspect_height = 1;
            dprint(PRN_ERROR, "WARNNING: aspect ration (%d) is not expected!\n", code);
            break;
    }

    return;
}

td_s32 syn_is_pts_rewind(td_s32 chan_id, td_s64 current_pts, td_s64 min_pts)
{
    const td_s64 max_pts_ms = 1000;

    vfmw_unused(chan_id);
    return (current_pts - min_pts > max_pts_ms * 1000) ? 1 : 0; /* 1000: pts unit in us */
}

STATIC td_void syn_parse_config_ctx(td_s32 chan_id, const td_void *cfg)
{
    syn_cfg_info *config = TD_NULL;
    syn_parse_ctx *ctx = TD_NULL;

    vfmw_check(cfg != TD_NULL);

    config = (syn_cfg_info *)cfg;

    syn_parse_get_ctx(&ctx, chan_id);
    vfmw_check(ctx->ops != TD_NULL);

    ctx->ops->config_syn_ctx(ctx->syn_ctx, config);

    return;
}

td_void *syn_parse_get_synctx(td_s32 chan_id)
{
    syn_parse_ctx *ctx = TD_NULL;

    vfmw_check_ret_print(chan_id >= 0 && chan_id < VFMW_CHAN_NUM, TD_NULL, "chan_id %d is invalid", chan_id);

    syn_parse_get_ctx(&ctx, chan_id);

    return ctx->syn_ctx;
}

syn_parse_state syn_parse_decode_stream(stm_seg_info *seg, dps_info *dps, td_s32 chan_id)
{
    syn_parse_state ret;
    syn_parse_ctx *ctx = TD_NULL;

    syn_parse_get_ctx(&ctx, chan_id);
    vfmw_check_ret(ctx->ops != TD_NULL, SYN_PARSE_ERR);
    vfmw_check_ret(ctx->syn_ctx != TD_NULL, SYN_PARSE_ERR);

    ret = ctx->ops->decode_stream(seg, dps, ctx->syn_ctx);

    return ret;
}

td_void syn_parse_init_ctx(td_s32 chan_id, const td_void *cfg)
{
    syn_cfg_info *config = TD_NULL;
    syn_parse_ctx *ctx = TD_NULL;

    vfmw_check_print(chan_id >= 0 && chan_id < VFMW_CHAN_NUM, "chan_id %d is invalid", chan_id);
    vfmw_check_print(cfg != TD_NULL, "cfg is TD_NULL");

    config = (syn_cfg_info *)cfg;

    syn_parse_get_ctx(&ctx, chan_id);
    vfmw_check_print(ctx != TD_NULL, "ctx is TD_NULL");

    if (ctx->syn_ctx != TD_NULL) {
        /* mulit config sence */
        syn_parse_config_ctx(chan_id, config);
        return;
    }

    ctx->chan_id = chan_id;
    ctx->vid_std = config->vid_std;

    ctx->ops = syn_ops_get(config->vid_std);
    vfmw_check(ctx->ops != TD_NULL);

    ctx->ops->init_syn_ctx(chan_id, &(ctx->syn_ctx), config);
    ctx->alloc_mem_size = ctx->ops->get_syn_ctx_size(ctx->syn_ctx);

    return;
}

td_void syn_parse_deinit_ctx(td_s32 chan_id)
{
    syn_parse_ctx *ctx = TD_NULL;

    syn_parse_get_ctx(&ctx, chan_id);

    if (ctx->syn_ctx == TD_NULL) {
        return;
    }

    vfmw_check(ctx->ops != TD_NULL);

    ctx->ops->deinit_syn_ctx(ctx->syn_ctx, TD_NULL);
    ctx->alloc_mem_size = 0;
    ctx->syn_ctx = TD_NULL;
    ctx->ops = TD_NULL;
    vdec_record_chr_event(chan_id, MT_VDEC_DESTROY, MT_VDEC_STORE_CLOSE, MT_VDEC_VFMW_SYN_DESTROY_OK);

    return;
}

td_void syn_parse_reset_ctx(td_s32 chan_id, td_void *args)
{
    syn_parse_ctx *ctx = TD_NULL;

    syn_parse_get_ctx(&ctx, chan_id);

    if (ctx->syn_ctx == TD_NULL) {
        return;
    }

    vfmw_check(ctx->ops != TD_NULL);

    ctx->ops->reset_syn_ctx(ctx->syn_ctx, args);

    return;
}

td_s32 syn_parse_open(td_void)
{
    syn_parse_ctx *ctx = TD_NULL;
    const td_s32 index = 0;

    syn_parse_get_ctx(&ctx, index);

    vfmw_check_sec_func(memset_s(ctx, sizeof(g_syn_parse_ctx), 0, sizeof(g_syn_parse_ctx)));

    return SYN_OK;
}

td_s32 syn_parse_close(td_void)
{
    return SYN_OK;
}

td_s32 syn_parse_get_mem_need_size(vfmw_svr_mem_need *mem)
{
    syn_ops_get_chan_mem_max_size(mem);

    return SYN_OK;
}

td_void syn_parse_read_std_log(td_s32 chan_id, td_void *buf, td_s32 len, td_s32 *used)
{
    syn_parse_ctx *ctx = TD_NULL;

    syn_parse_get_ctx(&ctx, chan_id);
    if (ctx == TD_NULL || ctx->ops == TD_NULL) {
        os_dump_proc(buf, len, used, 0, "syn ops not exists.%-60s|\r\n", " ");
        return;
    }

    if (ctx->ops->read_std_log == TD_NULL) {
        os_dump_proc(buf, len, used, 0, "syn read std log ops not complete.%-45s|\n", " ");
        return;
    }

    ctx->ops->read_std_log(ctx->syn_ctx, buf, len, used);

    return;
}

td_s32 syn_parse_get_info(td_s32 chan_id, td_void *args)
{
    syn_parse_ctx *parse_ctx = TD_NULL;
    syn_syntax_info *info = (syn_syntax_info *)args;

    syn_parse_get_ctx(&parse_ctx, chan_id);
    vfmw_check_ret_print(parse_ctx != TD_NULL, SYN_ERR, "%s Chan %d pCtx = TD_NULL\n", __func__, chan_id);

    info->std = parse_ctx->vid_std;

    if (parse_ctx->ops != TD_NULL &&
        parse_ctx->ops->get_parse_info != TD_NULL) {
        parse_ctx->ops->get_parse_info(parse_ctx->syn_ctx, args);
    }

    return SYN_OK;
}

td_s32 syn_parse_cfg_fffb_speed(td_s32 chan_id, const td_void *args)
{
    td_s32 ret;
    syn_parse_ctx *ctx = TD_NULL;

    syn_parse_get_ctx(&ctx, chan_id);
    vfmw_check_ret_print(ctx != TD_NULL, SYN_ERR, "%s chan %d ctx = TD_NULL\n", __func__, chan_id);
    vfmw_check_ret_print(ctx->ops != TD_NULL, SYN_ERR, "%s chan %d ops = TD_NULL\n", __func__, chan_id);
    vfmw_check_ret_print(ctx->ops->ctrl_syn_ctx != TD_NULL, SYN_ERR, "%s chan %d pctx->ops->ctrl_syn_ctx = TD_NULL\n",
                         __func__, chan_id);

    ret = ctx->ops->ctrl_syn_ctx(ctx->syn_ctx, SYNCTX_CMD_CFG_FFFB_SPEED, args);

    return ret;
}

td_s32 syn_parse_cfg_pvr_info(td_s32 chan_id, const td_void *args)
{
    td_s32 ret;
    syn_parse_ctx *ctx = TD_NULL;

    syn_parse_get_ctx(&ctx, chan_id);
    vfmw_check_ret_print(ctx != TD_NULL, SYN_ERR, "%s chan %d pctx = TD_NULL\n", __func__, chan_id);
    vfmw_check_ret_print(ctx->ops != TD_NULL, SYN_ERR, "%s chan %d ops = TD_NULL\n", __func__, chan_id);
    vfmw_check_ret_print(ctx->ops->ctrl_syn_ctx != TD_NULL, SYN_ERR, "%s chan %d ctx->ops->ctrl_syn_ctx = TD_NULL\n",
                         __func__, chan_id);

    ret = ctx->ops->ctrl_syn_ctx(ctx->syn_ctx, SYNCTX_CMD_CFG_PVR_INFO, args);

    return ret;
}

td_s32 judge_framerate_valid(td_u32 frame_rate)
{
    if (frame_rate == 23976 || frame_rate == 24000 || /* 23976 24000 is fps */
        frame_rate == 25000 || frame_rate == 29970 || /* 25000 29970 is fps */
        frame_rate == 30000 || frame_rate == 50000 || /* 30000 50000 is fps */
        frame_rate == 59940 || frame_rate == 60000 || /* 59940 60000 is fps */
        frame_rate == 119880 || frame_rate == 120000) { /* 119880 120000 is fps */
        return SYN_OK;
    } else {
        return SYN_ERR;
    }
}

