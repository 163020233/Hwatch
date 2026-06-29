/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#include "h264.h"
#include "fsp.h"
#include "voque.h"
#include "vctrl.h"
#include "event.h"
#include "vfmw_proc.h"
#include "syn_parse.h"
#include "vfmw_define.h"

td_u8 g_apple_airplay_mirrorinig_flag = 1;

static const td_u32 g_h264_quant_intra_default[4] = { 0x1c140d06, 0x201c140d, 0x25201c14, 0x2a25201c }; /* 4 is num */

static const td_u32 g_h264_quant_inter_default[4] = { 0x18140e0a, 0x1b18140e, 0x1e1b1814, 0x221e1b18 }; /* 4 is num */

static const td_u32 g_h264_quant8_intra_default[16] = { /* 16 is array size */
    0x100d0a06, 0x1b191712, 0x12100b0a, 0x1d1b1917,
    0x1712100d, 0x1f1d1b19, 0x19171210, 0x211f1d1b,
    0x1b191712, 0x24211f1d, 0x1d1b1917, 0x2624211f,
    0x1f1d1b19, 0x28262421, 0x211f1d1b, 0x2a282624
};

static const td_u32 g_h264_quant8_inter_default[16] = { /* 16 is array size */
    0x110f0d09, 0x18161513, 0x13110d0d, 0x19181615,
    0x1513110f, 0x1b191816, 0x16151311, 0x1c1b1918,
    0x18161513, 0x1e1c1b19, 0x19181615, 0x201e1c1b,
    0x1b191816, 0x21201e1c, 0x1c1b1918, 0x2321201e
};

STATIC td_void release_nal(td_s32 chan_id, h264_nalu *nalu);
STATIC td_void init_old_slice(h264_ctx *ctx);
STATIC td_s32 flush_dpb(h264_ctx *ctx);
STATIC td_s32 init_dpb(h264_ctx *ctx);
STATIC td_s32 h264_dec_recycle_image(const h264_ctx *ctx, td_u32 pic_id);
STATIC td_s32 is_ref_list_wrong(const h264_ctx *ctx);
STATIC td_s32 process_pps(h264_ctx *ctx, h264_pps *pps);
STATIC td_s32 pps_equal(const h264_pps *pps1, const h264_pps *pps2);
STATIC td_s32 h264_dec_post_proc(h264_ctx *ctx);
STATIC td_s32 h264_check_stm_len(const h264_ctx *ctx, td_s32 bit_len);

STATIC td_s32 se_err_check(td_u32 se_err_flg)
{
    return (se_err_flg != 0) ? H264_ERR : H264_OK;
}

#define H264_DYN_NAME_LEN 50
STATIC td_void *h264_get_dyn_mem(h264_ctx *ctx, td_u32 len, const td_char *name_xps, td_u32 id)
{
    td_void *addr = TD_NULL;
    td_s32 cnt;
    mem_record mem = { 0 };
#ifdef VFMW_CHECK_MEM_SUPPORT
    td_s32 ret;
    td_char name[H264_DYN_NAME_LEN];
#else
    td_char *name = TD_NULL;
#endif

    vfmw_check_ret(ctx != TD_NULL, TD_NULL);
    for (cnt = 0; cnt < H264_MEM_NUM; cnt++) {
        if (ctx->mem_list[cnt].mem_addr == TD_NULL) {
            break;
        }
    }

    if (cnt >= H264_MEM_NUM) {
        dprint(PRN_ERROR, "%s mem_list full error\n", __func__);
        return TD_NULL;
    }
#ifdef VFMW_CHECK_MEM_SUPPORT
    ret = snprintf_s(name, H264_DYN_NAME_LEN, H264_DYN_NAME_LEN - 1, "dyn_mem_%s_%u", name_xps, id);
    vfmw_check_ret(ret >= 0, TD_NULL);
#else
    vfmw_unused(id);
    vfmw_unused(name_xps);
    name = "dyn_mem";
#endif
    vfmw_check_ret(len < (td_u32)VFMW_INT_MAX, TD_NULL);
    mem.mem_name = name;
    mem.length = vfmw_align_up(len, VFMW_CTX_ALIGN);
    mem.is_cached = 0;
    mem.type = CACHE_MEM_TYPE;
    mem.unid = (td_u32)ctx->chan_id;
    os_malloc(&mem); /* "H264_Mem" */

    addr = (td_void *)(mem.vir_addr);
    if (addr == TD_NULL) {
        dprint(PRN_ERROR, "%s mem_list alloc error\n", __func__);
        return TD_NULL;
    }

    vfmw_check_sec_func(memset_s(addr, mem.length, 0, mem.length));
    ctx->mem_list[cnt].mem_addr = addr;
    ctx->mem_list[cnt].mem_size = mem.length;
    ctx->ctx_mem_size += mem.length;

    return addr;
}

static inline td_void ue_vld(td_u32 code, td_u32 *bits, td_u32 *value, td_u32 zeros)
{
    (*bits) = ((zeros) << 1) + 1;
    (*value) = ((code) >> (32 - (*bits))) - 1; /* 32 is max bits */
}

/*************************************************************************
*   函数原型 :  td_s32 ue_v(h264_ctx *ctx)
*   功能描述 :  无符号变长码解码,指定映射函数和调用函数获取语法元素的码流信息
和解析语法元素值
*   参数说明 :  码流信息和语法元素SE_TRACEstring信息
*   返回值   ： 该语法元素值
*************************************************************************/
STATIC td_u32 ue_v(h264_ctx *ctx, td_char *se_trace_string)
{
    td_u32 num_bits, tmp_bits;
    td_u32 info;
    td_u32 zeros;

    vfmw_unused(se_trace_string);
    /* ======== mb_type ========== */
    tmp_bits = (td_u32)bs_show(&(ctx->bs), 32); /* 32 bit num */
    zeros = bs_zero_ms_32(tmp_bits);
    if (zeros < 16) { /* 16 bit num */
        ue_vld(tmp_bits, &num_bits, &info, zeros);
        bs_skip(&(ctx->bs), (td_s32)num_bits);
    } else {
        if (zeros < 32) { /* 32 bit num */
            /* 因为最大的数为2的31次方，所以zeros+1的最大个数为32 */
            bs_skip(&(ctx->bs), (td_s32)zeros);
            info = (td_u32)(bs_show(&(ctx->bs), (td_s32)(zeros + 1)) - 1);
            bs_skip(&(ctx->bs), (td_s32)(zeros + 1));
            num_bits = (zeros << 1) + 1;
        } else {
            /* 这时有32个零，说明有错误发生 */
            ctx->se_err_flg = 1;
            info = 0xffffffff;
            num_bits = 32; /* 32 bit num */
        }
    }

    ctx->cur_nal->nal_bitoffset += num_bits;

    return info;
}

static inline td_void se_vld(td_u32 code, td_u32 *bits, td_s32 *val, td_u32 zeros)
{
    td_s32 tmp;

    (*bits) = ((zeros) << 1) + 1;
    tmp = (td_s32)(((code) >> (32 - (*bits))) >> 1); /* 32 bit num */
    (*val) = (((code >> (32 - *bits)) & 1) != 0) ? -tmp : tmp; /* 32 bit num */
}

/*************************************************************************
*   函数原型 :  td_s32 se_v(h264_ctx *ctx)
*   功能描述 :  有符号变长码解码,指定映射函数和调用函数获取语法元素的码流信息
和解析语法元素值
*   参数说明 :  码流信息和语法元素se_trace_string信息
*   返回值   ： 该语法元素值
*************************************************************************/
STATIC td_s32 se_v(h264_ctx *ctx, td_char *se_trace_string)
{
    td_u32 num_bits, tmp_bits;
    td_s32 info;
    td_u32 zeros;
    td_u32 info_last_bit;

    vfmw_unused(se_trace_string);
    /* ======== mb_type ========== */
    tmp_bits = (td_u32)bs_show(&(ctx->bs), 32); /* 32 bit num */
    zeros = bs_zero_ms_32(tmp_bits);
    if (zeros < 16) { /* 16 bit num */
        se_vld(tmp_bits, &num_bits, &info, zeros);
        bs_skip(&(ctx->bs), (td_s32)num_bits);
    } else {
        /* 因为最大的数为2的31次方，所以zeros的最大个数为32，所以在这种情况下需要BS_Show两次 */
        if (zeros < 32) { /* 32 bit num */
            bs_skip(&(ctx->bs), (td_s32)zeros);
            info = bs_get(&(ctx->bs), (td_s32)zeros);
            info_last_bit = (td_u32)bs_get(&(ctx->bs), 1);
            info = (((info_last_bit & 1) != 0) ? -info : info);
            num_bits = (zeros << 1) + 1;
        } else {
            /* 这时有32个零，说明有错误发生 */
            ctx->se_err_flg = 1;
            info = 0x7fffffff;
            num_bits = 32; /* 32 bit num */
        }
    }

    ctx->cur_nal->nal_bitoffset += num_bits;

    return info;
}

/*************************************************************************
*   函数原型 :  td_s32 u_v(h264_ctx *ctx)
*   功能描述 :  定长语法元素解码
*   参数说明 :  语法元素长度和se_trace_string、码流信息
*   返回值   ： 语法元素值
*************************************************************************/
STATIC td_s32 u_v(h264_ctx *ctx, td_u32 len_in_bits, td_char *se_trace_string)
{
    td_s32 info;

    vfmw_unused(se_trace_string);
    info = bs_get(&(ctx->bs), (td_s32)len_in_bits);
    ctx->cur_nal->nal_bitoffset += len_in_bits;

    return info;
}

/*************************************************************************
*   函数原型 :  td_s32 u_1(h264_ctx *ctx)
*   功能描述 :  定长语法元素解码，解析1bit
*   参数说明 :  语法元素长度和se_trace_string、码流信息
*   返回值   ： 语法元素值
*************************************************************************/
STATIC td_u32 u_1(h264_ctx *ctx, td_char *se_trace_string)
{
    td_u32 info;

    vfmw_unused(se_trace_string);
    info = (td_u32)bs_get(&(ctx->bs), 1);
    ctx->cur_nal->nal_bitoffset += 1;

    return info;
}

STATIC td_u8 h264_is_eos_nal(h264_ctx *ctx)
{
    td_u32 code0, code1; /* add for eos nal */

    code0 = (td_u32)bs_get(&(ctx->bs), 32); /* 32 bit num */
    code1 = (td_u32)bs_get(&(ctx->bs), 32); /* 32 bit num */
    if (code0 == VFMW_STREAM_END_CODE1 && code1 == VFMW_STREAM_END_CODE2) {
        return 1;
    } else {
        return 0;
    }
}

STATIC td_s32 clear_curr_nal(h264_ctx *ctx)
{
    if (ctx->cur_nal != TD_NULL) {
        release_nal(ctx->chan_id, ctx->cur_nal);
        ctx->cur_nal = TD_NULL;
    }

    return H264_OK;
}

STATIC td_s32 clear_all_nal(h264_ctx *ctx)
{
    clear_curr_nal(ctx);

    return H264_OK;
}

STATIC td_s32 clear_curr_slice(h264_ctx *ctx)
{
    dprint(PRN_WARN, "clear curr slice.\n");

    clear_curr_nal(ctx);
    ctx->total_slice++;

    return H264_OK;
}

STATIC td_s32 clear_all_slice(h264_ctx *ctx)
{
    ctx->slice_para_num = 0;
    ctx->max_bytes_received = 0;

    return H264_OK;
}

STATIC td_s32 h264_clear_curr_pic(h264_ctx *ctx)
{
    if (ctx->slice_para_num > 0) {
        dps_release_seg(ctx->chan_id, ctx->cur_dps);
    }

    ctx->have_nal_not_decoded = 0;
    clear_all_slice(ctx);
    init_old_slice(ctx);

    return H264_OK;
}

STATIC td_s32 clear_dpb(h264_ctx *ctx)
{
    td_s32 ret;
    ret = flush_dpb(ctx);
    if (ret != H264_OK) {
        dprint(PRN_ERROR, "line %d: flush dpb return %d\n", __LINE__, ret);
    }

    init_dpb(ctx);

    ctx->idr_found = 0;
    ctx->first_frame_displayed = FIRST_DISP_INIT;

    return H264_OK;
}

STATIC td_void clear_all_for_fs(h264_ctx *ctx)
{
    td_u32 i;
#ifdef H264_USRDATA_SUPPORT
    td_u32 usrdat_idx;
    fsp_logic *logic = TD_NULL;
#endif
    for (i = 0; i < MAX_FRAME_STORE; i++) {
        ctx->frame_store[i].non_existing = 0; /* a real pic; */
        ctx->frame_store[i].is_used = 0;
        ctx->frame_store[i].is_reference = 0; /* not pic used for ref; */
        ctx->frame_store[i].long_term_frame_idx = 0;
        ctx->frame_store[i].frame_num = 0;
        ctx->frame_store[i].frame_num_wrap = -1;
        ctx->frame_store[i].poc = 0;
        ctx->frame_store[i].is_in_dpb = 0;    /* already out from g_DPB */
        ctx->frame_store[i].is_displayed = 1; /* already out from VO */
        ctx->frame_store[i].pmv_address_idc = MAX_PMV_STORE;
        ctx->frame_store[i].apc_idc = 16; /* 16 apc idc */
        ctx->frame_store[i].frame.frame_store = ctx->frame_store[i].top_field.frame_store =
                                                    ctx->frame_store[i].bottom_field.frame_store = &ctx->frame_store[i];
        ctx->frame_store[i].frame.structure = 0;        /* g_frame is frame, will be change when field paired */
        ctx->frame_store[i].top_field.structure = 1;    /* g_top is top field; */
        ctx->frame_store[i].bottom_field.structure = 2; /* 2 is bottom field */

#ifdef H264_USRDATA_SUPPORT
        logic = fsp_get_logic(ctx->chan_id, ctx->frame_store[i].logic_fs_id);
        if (logic == TD_NULL) {
            continue;
        }
        for (usrdat_idx = 0; usrdat_idx < 4; usrdat_idx++) { /* 4 is usd num */
            if (uint64_ptr(logic->image.usrdat[usrdat_idx]) != TD_NULL) {
                usd_release(ctx->chan_id, uint64_ptr(logic->image.usrdat[usrdat_idx]));
                logic->image.usrdat[usrdat_idx] = ptr_uint64(TD_NULL);
            }
        }
#endif
    }
}
#ifdef H264_USRDATA_SUPPORT
STATIC td_void clear_all_for_usd(h264_ctx *ctx)
{
    td_u32 i;

    /* release usrdat of currpic */
    for (i = 0; i < 4; i++) { /* 4 is usd num */
        if (uint64_ptr(ctx->cur_pic.fs_image.usrdat[i]) != TD_NULL) {
            usd_release(ctx->chan_id, uint64_ptr(ctx->cur_pic.fs_image.usrdat[i]));
            ctx->cur_pic.fs_image.usrdat[i] = ptr_uint64(TD_NULL);
        }
    }

    for (i = 0; i < ctx->total_usrdat_num; i++) {
        if (ctx->usr_dat_array[i] != TD_NULL) {
            usd_release(ctx->chan_id, ctx->usr_dat_array[i]);
            ctx->usr_dat_array[i] = TD_NULL;
        }
    }

    ctx->total_usrdat_num = 0;
}
#endif

STATIC td_s32 clear_all(h264_ctx *ctx, td_s32 flag)
{
    h264_clear_curr_pic(ctx);
    clear_all_nal(ctx);

    init_dpb(ctx);

    /* 关于FSP 的修改依赖于公共模块FSP方法最终实现，目前方法为空 */
    if (flag != 0) {
        fsp_clear_idle(ctx->chan_id);
    } else {
        fsp_empty(ctx->chan_id);
    }

    clear_all_for_fs(ctx);
#ifdef H264_USRDATA_SUPPORT
    clear_all_for_usd(ctx);
#endif
    /* refresh some statistic variable */
    ctx->idr_pic_of_cur_pic = 0;
    ctx->allow_start_dec = 0;
    ctx->slice_para_num = 0;
    ctx->max_bytes_received = 0;
    ctx->imode_out_flag = 0;
    ctx->idr_found = 0;
    ctx->first_frame_displayed = FIRST_DISP_INIT;
    ctx->ppic_found = 0;

    /* init some global var */
    ctx->cur_nal = TD_NULL;
    ctx->cur_mark_idx = 0;
    ctx->mark[0].is_valid = ctx->mark[1].is_valid = 0;
    ctx->mark[0].no_output_of_prior_pics_flag = ctx->mark[1].no_output_of_prior_pics_flag = 0;

    /* init ctx->cur_slice */
    vfmw_check_sec_func(memset_s(&ctx->cur_slice, sizeof(h264_slice), 0, sizeof(h264_slice)));

    ctx->cur_slice.slice_type = I_SLICE;
    ctx->cur_slice.new_pic_flag = 1;
    ctx->cur_pic.state = EMPTY;

    ctx->prev_pic.structure = FRAME;
    ctx->prev_pic.frame_num = 256; /* 256 def frm num */
    ctx->prev_pic.nal_ref_idc = 0;

    return H264_OK;
}

/*************************************************************************
*   函数原型 :  td_s32 get_min_poc(h264_ctx *ctx,td_s32 *poc, td_s32 *pos)
*   功能描述 :  找出最小poc 和最小poc图像位置
*************************************************************************/
STATIC td_void get_min_poc(h264_ctx *ctx, td_s32 *poc, td_u32 *pos)
{
    td_u32 i;

    *pos = 0;
    *poc = 0x7fffffff;

    if (ctx->dpb.used_size > ctx->dpb.size) {
        ctx->dpb.used_size = ctx->dpb.size; /* for error resilence */
    }

    for (i = 0; i < ctx->dpb.used_size; i++) {
        if ((*poc > ctx->dpb.fs[i]->poc) && (ctx->dpb.fs[i]->is_in_dpb != 0)) {
            *poc = ctx->dpb.fs[i]->poc;
            *pos = i;
        }
    }

    return;
}

/*************************************************************************
*   函数原型 :  td_s32 get_max_poc(h264_ctx *ctx,td_s32 *poc, td_s32 *pos)
*   功能描述 :  找出最大poc 和最大poc图像位置
*************************************************************************/
STATIC td_void get_max_poc(h264_ctx *ctx, td_s32 *poc, td_u32 *pos)
{
    td_u32 i;

    *pos = 0;
    *poc = 0;

    if (ctx->dpb.used_size > ctx->dpb.size) {
        ctx->dpb.used_size = ctx->dpb.size; /* for error resilence */
    }

    for (i = 0; i < ctx->dpb.used_size; i++) {
        if ((*poc < ctx->dpb.fs[i]->poc) && (ctx->dpb.fs[i]->is_in_dpb != 0)) {
            *poc = ctx->dpb.fs[i]->poc;
            *pos = i;
        }
    }

    return;
}

/*************************************************************************
*   函数原型 :  td_u32 frame_store_ref_flg(h264_frame_store *fs)
*   功能描述 :  指示该framestore是否用于参考
*************************************************************************/
STATIC td_u32 frame_store_ref_flg(const h264_frame_store *fs)
{
    if (fs == TD_NULL) {
        dprint(PRN_ERROR, "%s fs is TD_NULL\n", __func__);
        return 0;
    }

    return (fs->is_reference > 0);
}

/*************************************************************************
*   函数原型 :  td_u32 short_term_ref_flg(h264_frame_store *fs)
*   功能描述 :  判断该帧存是否为短期参考
*   参数说明 :  帧存指针
*   返回值   ： 判断结果
*************************************************************************/
STATIC td_u32 short_term_ref_flg(const h264_frame_store *fs)
{
    if (fs->is_reference == 3) { /* 3 both ref */
        if ((fs->frame.is_short_term == 1) && (fs->frame.is_long_term == 0)) {
            return 1;
        }
    }

    if ((fs->is_reference & 1) != 0) {
        if ((fs->top_field.is_short_term == 1) && (fs->top_field.is_long_term == 0)) {
            return 1;
        }
    }

    if ((fs->is_reference & 2) != 0) { /* 2 btm ref */
        if ((fs->bottom_field.is_short_term == 1) && (fs->bottom_field.is_long_term == 0)) {
            return 1;
        }
    }

    return 0;
}

/*************************************************************************
*   函数原型 :  td_u32 long_term_ref_flg(h264_frame_store *fs)
*   功能描述 :  判断该帧存是否为长期参考
*   参数说明 :  帧存指针
*   返回值   ： 判断结果
*************************************************************************/
STATIC td_u32 long_term_ref_flg(const h264_frame_store *fs)
{
    if (fs->is_reference == 3) { /* 3 both ref */
        if ((fs->frame.is_short_term == 0) && (fs->frame.is_long_term == 1)) {
            return 1;
        }
    }

    if ((fs->is_reference & 1) != 0) {
        if ((fs->top_field.is_short_term == 0) && (fs->top_field.is_long_term == 1)) {
            return 1;
        }
    }

    if ((fs->is_reference & 2) != 0) { /* 2 btm ref */
        if ((fs->bottom_field.is_short_term == 0) && (fs->bottom_field.is_long_term == 1)) {
            return 1;
        }
    }

    return 0;
}

/*************************************************************************
*      函数原型 :  td_void update_lt_ref_list(h264_ctx *ctx)
*      功能描述 :  更新DPB中长期参考列表
*      参数说明 :  无
*      返回值   ： 无
*************************************************************************/
STATIC td_void update_lt_ref_list(h264_ctx *ctx)
{
    td_u32 i, j;

    for (i = 0, j = 0; i < ctx->dpb.used_size; i++) {
        if (long_term_ref_flg(ctx->dpb.fs[i]) == 1) {
            ctx->dpb.fs_ltref[j++] = ctx->dpb.fs[i];
        }
    }

    ctx->dpb.ltref_frames_in_buffer = j;

    while (j < ctx->dpb.size) {
        ctx->dpb.fs_ltref[j++] = (h264_frame_store *)TD_NULL;
    }

    return;
}

/*************************************************************************
*      函数原型 :  td_void update_ref_list(h264_ctx *ctx)
*      功能描述 :  更新DPB中短期参考列表
*      参数说明 :  无
*      返回值   ： 无
*************************************************************************/
STATIC td_void update_ref_list(h264_ctx *ctx)
{
    td_u32 i, j;

    for (i = 0, j = 0; i < ctx->dpb.used_size; i++) {
        if (short_term_ref_flg(ctx->dpb.fs[i]) == 1) {
            ctx->dpb.fs_ref[j++] = ctx->dpb.fs[i];
        }
    }

    ctx->dpb.ref_frames_in_buffer = j;

    while (j < ctx->dpb.size) {
        ctx->dpb.fs_ref[j++] = (h264_frame_store *)TD_NULL;
    }

    return;
}

/*************************************************************************
*      函数原型 :  td_s32 get_pic_num_x(h264_cur_pic *curr_pic, td_u32)
*      功能描述 :  根据difference_of_pic_nums_minus1计算pic_num,
*      参数说明 :  difference_of_pic_nums_minus1：用于计算pic_num；pCurrPic当前图像结构体指针
*      返回值   ： 无
*************************************************************************/
STATIC td_s32 get_pic_num_x(const h264_cur_pic *curr_pic, td_u32 difference_of_pic_nums_minus1)
{
    td_u32 curr_pic_num;
    td_s32 result;

    if (curr_pic->structure == FRAME) {
        curr_pic_num = curr_pic->frame_num;
    }  else {
        vfmw_check_ret(curr_pic->frame_num < VFMW_INT16_MAX, 0);
        curr_pic_num = 2 * curr_pic->frame_num + 1; /* 2 is double */
    }

    result = (td_s32)(curr_pic_num - (difference_of_pic_nums_minus1 + 1));

    return result;
}

/*************************************************************************
*      函数原型 :  td_void unmark_frame_store_ref(h264_ctx *, h264_frame_store *)
*      功能描述 :  将pFrameStore对因帧存中所有图像标记为不参考，同时释放pmv槽位
*      参数说明 :  pFrameStore帧存指针
*      返回值   ： 无
*************************************************************************/
STATIC td_void unmark_frame_store_ref(h264_ctx *ctx, h264_frame_store *frame_store)
{
    if ((frame_store->is_used & 1) != 0) { /* inlcude top field */
        /* unmark top field and don't modify bottom field reference attribute */
        frame_store->is_reference &= 2; /* 2 btm ref */
        frame_store->top_field.is_long_term = 0;
        frame_store->top_field.is_short_term = 0;
    }

    if ((frame_store->is_used & 2) != 0) { /* 2 include bottom field */
        /* unmark top field and don't modify bottom field reference attribute */
        frame_store->is_reference &= 1;
        frame_store->bottom_field.is_long_term = 0;
        frame_store->bottom_field.is_short_term = 0;
    }

    if (frame_store->is_used == 3) { /* 3 frame, both field pair */
        frame_store->frame.is_short_term = 0;
        frame_store->frame.is_long_term = 0;
        frame_store->is_reference = 0;
    }

    /* 经过以上步骤不一定就把is_reference赋成0，所有最后强制一下 */
    frame_store->is_reference = 0;

    /* 把逻辑帧存标记为不参考 */
    fsp_set_ref(ctx->chan_id, frame_store->logic_fs_id, 0);

    return;
}

/*************************************************************************
*      函数原型 :  td_void unmark_lt_frm_by_frm_idx(td_u32 long_term_frame_idx)
*      功能描述 :  将long_term_pic_num对应的长期参考图像标记为不参考
*      参数说明 :  long_term_pic_num将要标记的图像的long_term_pic_num
*      返回值   ： 无
*************************************************************************/
STATIC td_void unmark_lt_frm_by_frm_idx(h264_ctx *ctx, td_u32 long_term_frame_idx)
{
    td_u32 i;

    for (i = 0; i < ctx->dpb.ltref_frames_in_buffer; i++) {
        if (long_term_frame_idx == ctx->dpb.fs_ltref[i]->long_term_frame_idx) {
            unmark_frame_store_ref(ctx, ctx->dpb.fs_ltref[i]);
            return;
        }
    }

    return;
}

/*************************************************************************
*      函数原型 :  td_void unmark_lt_fld_by_frm_idx(h264_ctx *, h264_frame_store *, td_u32, td_u32)
*      功能描述 :  将long_term_pic_num对应的长期参考图像标记为不参考
*      参数说明 :  long_term_pic_num将要标记的图像的long_term_pic_num
*      返回值   ： 无
*************************************************************************/
STATIC td_void unmark_lt_fld_by_frm_idx(h264_ctx *ctx, const h264_frame_store *fs,
                                        td_u32 structure, td_u32 long_term_frame_idx)
{
    td_u32 i;

    for (i = 0; i < ctx->dpb.ltref_frames_in_buffer; i++) {
        if (long_term_frame_idx != ctx->dpb.fs_ltref[i]->long_term_frame_idx) {
            continue;
        }

        if (structure == TOP_FIELD) {
            /* not long term field pair */
            if ((ctx->dpb.fs_ltref[i]->bottom_field.is_long_term == 0) || (fs != ctx->dpb.fs_ltref[i])) {
                unmark_frame_store_ref(ctx, ctx->dpb.fs_ltref[i]);
                break; /* !!! */
            }
        }

        if (structure == BOTTOM_FIELD) {
            /* not long term field pair */
            if ((ctx->dpb.fs_ltref[i]->top_field.is_long_term == 0) || (fs != ctx->dpb.fs_ltref[i])) {
                unmark_frame_store_ref(ctx, ctx->dpb.fs_ltref[i]);
                break; /* !!! */
            }
        }
    }

    return;
}

STATIC td_void unmark_st_ref_by_frm(h264_ctx *ctx, td_s32 pic_num_x)
{
    td_u32 i;

    for (i = 0; i < ctx->dpb.ref_frames_in_buffer; i++) {
        if (ctx->dpb.fs_ref[i]->frame.pic_num == pic_num_x) {
            if ((ctx->dpb.fs_ref[i]->is_reference == 3) && /* 3 both ref */
                (ctx->dpb.fs_ref[i]->frame.is_long_term == 0) &&
                (ctx->dpb.fs_ref[i]->frame.is_short_term == 1)) {
                unmark_frame_store_ref(ctx, ctx->dpb.fs_ref[i]);
                return;
            }
        }
    }
}

#ifdef H264_FIELD_SUPPORT
STATIC td_void unmark_st_set_ref_by_structure(h264_ctx *ctx, td_s32 pic_num_x, td_u32 i)
{
    vfmw_unused(pic_num_x);
    /* one field reference attribute modified and frame reference attribute need to modify */
    if ((ctx->dpb.fs_ref[i]->frame.structure == FIELD_PAIR) ||
        (ctx->dpb.fs_ref[i]->frame.structure == FRAME)) {
        ctx->dpb.fs_ref[i]->frame.is_short_term = 0;
        ctx->dpb.fs_ref[i]->frame.is_long_term = 0;
    }

    if (ctx->dpb.fs_ref[i]->is_reference == 0) {
        vfmw_check(ctx->dpb.fs_ref[i]->pmv_address_idc < MAX_PMV_STORE);
        ctx->pmv_store_used_flag[ctx->dpb.fs_ref[i]->pmv_address_idc] = 0; /* pmv store */
        fsp_set_ref(ctx->chan_id, ctx->dpb.fs_ref[i]->logic_fs_id, 0);
    }

    return;
}

STATIC td_void unmark_st_ref_by_field(h264_ctx *ctx, td_s32 pic_num_x)
{
    td_u32 i;

    for (i = 0; i < ctx->dpb.ref_frames_in_buffer; i++) {
        if (((ctx->dpb.fs_ref[i]->is_reference & 1) != 0) &&
            (ctx->dpb.fs_ref[i]->top_field.is_long_term == 0) &&
            (ctx->dpb.fs_ref[i]->top_field.is_short_term == 1) &&
            (ctx->dpb.fs_ref[i]->top_field.pic_num == pic_num_x)) { /* include top field */
            /* mark top field unused */
            ctx->dpb.fs_ref[i]->is_reference &= 2; /* 2 btm ref */
            ctx->dpb.fs_ref[i]->top_field.is_short_term = 0;
            unmark_st_set_ref_by_structure(ctx, pic_num_x, i);
            return;
        }

        if (((ctx->dpb.fs_ref[i]->is_reference & 2) != 0) && /* 2 btm ref */
            (ctx->dpb.fs_ref[i]->bottom_field.is_long_term == 0) &&
            (ctx->dpb.fs_ref[i]->bottom_field.is_short_term == 1) &&
            (ctx->dpb.fs_ref[i]->bottom_field.pic_num == pic_num_x)) {
            /* mark bottom field unused */
            ctx->dpb.fs_ref[i]->is_reference &= 1;
            ctx->dpb.fs_ref[i]->bottom_field.is_short_term = 0;
            unmark_st_set_ref_by_structure(ctx, pic_num_x, i);
            return;
        }
    }

    return;
}
#endif

/*************************************************************************
*      函数原型 :  td_void unmark_st_ref(h264_ctx *, h264_cur_pic *, td_u32)
*      功能描述 :  根据difference_of_pic_nums_minus1计算pic_num,
*                  将pic_num对应的短期参考图像标记为不参考
*      参数说明 :  difference_of_pic_nums_minus1：用于计算pic_num；pCurrPic当前图像数据结构体指针
*      返回值   ： 无
*************************************************************************/
STATIC td_void unmark_st_ref(h264_ctx *ctx, const h264_cur_pic *curr_pic, td_s32 difference_of_pic_nums_minus1)
{
    td_s32 pic_num_x;

    /* Get pic_num_x */
    pic_num_x = get_pic_num_x(curr_pic, (td_u32)difference_of_pic_nums_minus1);

    if (curr_pic->structure == FRAME) {
        unmark_st_ref_by_frm(ctx, pic_num_x);
    }
#ifdef H264_FIELD_SUPPORT
    else {
        unmark_st_ref_by_field(ctx, pic_num_x);
    }
#endif
    return;
}

STATIC td_void unmark_lt_ref_by_frm(h264_ctx *ctx, td_s32 long_term_pic_num)
{
    td_u32 i;

    for (i = 0; i < ctx->dpb.ltref_frames_in_buffer; i++) {
        if (long_term_pic_num == ctx->dpb.fs_ltref[i]->frame.long_term_pic_num) {
            if (ctx->dpb.fs_ltref[i]->is_reference == 3 && /* 3 both ref */
                ctx->dpb.fs_ltref[i]->frame.is_long_term == 1 &&
                ctx->dpb.fs_ltref[i]->frame.is_short_term == 0) {
                unmark_frame_store_ref(ctx, ctx->dpb.fs_ltref[i]);
                return;
            }
        }
    }
}

#ifdef H264_FIELD_SUPPORT
STATIC td_void unmark_lt_ref_set_by_structure(h264_ctx *ctx, td_s32 long_term_pic_num, td_u32 i)
{
    vfmw_unused(long_term_pic_num);
    /* one field reference attribute modified and frame reference attribute need to modify */
    if ((ctx->dpb.fs_ltref[i]->frame.structure == FIELD_PAIR) ||
        (ctx->dpb.fs_ltref[i]->frame.structure == FRAME)) {
        ctx->dpb.fs_ltref[i]->frame.is_short_term = 0;
        ctx->dpb.fs_ltref[i]->frame.is_long_term = 0;
    }

    if (ctx->dpb.fs_ltref[i]->is_reference == 0) {
        vfmw_check(ctx->dpb.fs_ltref[i]->pmv_address_idc < MAX_PMV_STORE);
        ctx->pmv_store_used_flag[ctx->dpb.fs_ltref[i]->pmv_address_idc] = 0; /* pmv store */
    }

    return;
}

STATIC td_void unmark_lt_ref_by_field(h264_ctx *ctx, td_s32 long_term_pic_num)
{
    td_u32 i;

    for (i = 0; i < ctx->dpb.ltref_frames_in_buffer; i++) {
        if (((ctx->dpb.fs_ltref[i]->is_reference & 1) != 0) &&
            ctx->dpb.fs_ltref[i]->top_field.is_long_term == 1 &&
            ctx->dpb.fs_ltref[i]->top_field.is_short_term == 0 &&
            long_term_pic_num == ctx->dpb.fs_ltref[i]->top_field.long_term_pic_num) { /* include top field */
            /* mark top field unused */
            ctx->dpb.fs_ltref[i]->is_reference &= 2; /* 2 btm ref */
            ctx->dpb.fs_ltref[i]->top_field.is_long_term = 0;

            unmark_lt_ref_set_by_structure(ctx, long_term_pic_num, i);
            return;
        }

        if (((ctx->dpb.fs_ltref[i]->is_reference & 2) != 0) && /* 2 btm ref */
            (ctx->dpb.fs_ltref[i]->bottom_field.is_long_term == 1) &&
            (ctx->dpb.fs_ltref[i]->bottom_field.is_short_term == 0) &&
            (long_term_pic_num == ctx->dpb.fs_ltref[i]->bottom_field.long_term_pic_num)) {
            /* mark bottom field unused */
            ctx->dpb.fs_ltref[i]->is_reference &= 1;
            ctx->dpb.fs_ltref[i]->bottom_field.is_long_term = 0;

            unmark_lt_ref_set_by_structure(ctx, long_term_pic_num, i);
            return;
        }
    }

    return;
}
#endif

/*************************************************************************
*      函数原型 :  td_void unmark_lt_ref(h264_ctx *, h264_cur_pic  *, td_u32 )
*      功能描述 :  将long_term_pic_num对应的长期参考图像标记为不参考
*      参数说明 :  pCurrPic当前图像数据结构体指针，long_term_pic_num将要标记的图像的long_term_pic_num
*      返回值   ： 无
*************************************************************************/
STATIC td_void unmark_lt_ref(h264_ctx *ctx, const h264_cur_pic *curr_pic, td_s32 long_term_pic_num)
{
    if (curr_pic->structure == FRAME) {
        unmark_lt_ref_by_frm(ctx, long_term_pic_num);
    }
#ifdef H264_FIELD_SUPPORT
    else {
        unmark_lt_ref_by_field(ctx, long_term_pic_num);
    }
#endif
    return;
}

STATIC td_void mark_pic_lt_ref_by_frm(h264_ctx *ctx, td_u32 long_term_frame_idx, td_s32 pic_num_x)
{
    td_u32 i;

    for (i = 0; i < ctx->dpb.ref_frames_in_buffer; i++) {
        if ((ctx->dpb.fs_ref[i]->is_reference == 3) && /* 3 both ref */
            (ctx->dpb.fs_ref[i]->frame.is_long_term == 0) &&
            (ctx->dpb.fs_ref[i]->frame.is_short_term == 1) &&
            (ctx->dpb.fs_ref[i]->frame.pic_num == pic_num_x)) {
            ctx->dpb.fs_ref[i]->long_term_frame_idx = long_term_frame_idx;
            ctx->dpb.fs_ref[i]->frame.long_term_pic_num = (td_s32)long_term_frame_idx;
            /* mark to long term */
            ctx->dpb.fs_ref[i]->frame.is_long_term = 1;
            ctx->dpb.fs_ref[i]->top_field.is_long_term = 1;
            ctx->dpb.fs_ref[i]->bottom_field.is_long_term = 1;
            /* not short term anymore */
            ctx->dpb.fs_ref[i]->frame.is_short_term = 0;
            ctx->dpb.fs_ref[i]->top_field.is_short_term = 0;
            ctx->dpb.fs_ref[i]->bottom_field.is_short_term = 0;
            return;
        }
    }
}

STATIC td_void mark_pic_lt_ref_by_top_fld(h264_ctx *ctx, td_u32 long_term_frame_idx,
    td_s32 pic_num_x, const h264_cur_pic *curr_pic)
{
    td_u32 i;
    td_u32 add_top = (curr_pic->structure == TOP_FIELD) ? 1 : 0;

    for (i = 0; i < ctx->dpb.ref_frames_in_buffer; i++) {
        if (((ctx->dpb.fs_ref[i]->is_reference & 1) != 0) &&
            (ctx->dpb.fs_ref[i]->top_field.is_short_term == 1) &&
            (ctx->dpb.fs_ref[i]->top_field.is_long_term == 0) &&
            (pic_num_x == ctx->dpb.fs_ref[i]->top_field.pic_num)) {
            /* top field is short term and not long term and pic_num equals pic_num_x */
            if ((ctx->dpb.fs_ref[i]->bottom_field.is_long_term == 1) &&
                (ctx->dpb.fs_ref[i]->long_term_frame_idx != long_term_frame_idx)) {
                /* check bottom field is long term but long_term_frame_idx is diff */
                continue;
            }

            ctx->dpb.fs_ref[i]->long_term_frame_idx = long_term_frame_idx;
            vfmw_check(long_term_frame_idx < VFMW_INT16_MAX);
            ctx->dpb.fs_ref[i]->top_field.long_term_pic_num =
               (td_s32)(2 * long_term_frame_idx + add_top); /* 2 is num */
            ctx->dpb.fs_ref[i]->top_field.is_long_term = 1;  /* mark long term */
            ctx->dpb.fs_ref[i]->top_field.is_short_term = 0; /* mark not short term */

            /* modify frame reference attribute */
            if ((ctx->dpb.fs_ref[i]->frame.structure != FIELD_PAIR) &&
                (ctx->dpb.fs_ref[i]->frame.structure != FRAME)) {
                continue;
            }
            if (ctx->dpb.fs_ref[i]->top_field.is_long_term ==
                ctx->dpb.fs_ref[i]->bottom_field.is_long_term) {
                ctx->dpb.fs_ref[i]->frame.is_long_term = ctx->dpb.fs_ref[i]->top_field.is_long_term;
                ctx->dpb.fs_ref[i]->frame.is_short_term = ctx->dpb.fs_ref[i]->top_field.is_short_term;
            }
            break;
        }
    }
}

STATIC td_void mark_pic_lt_ref_by_btm_fld(h264_ctx *ctx, td_u32 long_term_frame_idx,
    td_s32 pic_num_x, const h264_cur_pic *curr_pic)
{
    td_u32 i;
    td_u32 add_bottom = (curr_pic->structure == BOTTOM_FIELD) ? 1 : 0;

    for (i = 0; i < ctx->dpb.ref_frames_in_buffer; i++) {
        if (((ctx->dpb.fs_ref[i]->is_reference & 2) != 0) && /* 2 include bottom field */
            (ctx->dpb.fs_ref[i]->bottom_field.is_long_term == 0) &&
            (ctx->dpb.fs_ref[i]->bottom_field.is_short_term == 1) &&
            (ctx->dpb.fs_ref[i]->bottom_field.pic_num == pic_num_x)) {
            if ((ctx->dpb.fs_ref[i]->top_field.is_long_term == 1) &&
                (ctx->dpb.fs_ref[i]->long_term_frame_idx != long_term_frame_idx)) {
                continue;
            }

            ctx->dpb.fs_ref[i]->long_term_frame_idx = long_term_frame_idx;
            vfmw_check(long_term_frame_idx < VFMW_INT16_MAX);
            ctx->dpb.fs_ref[i]->bottom_field.long_term_pic_num =
               (td_s32)(2 * long_term_frame_idx + add_bottom); /* 2 is num */
            ctx->dpb.fs_ref[i]->bottom_field.is_long_term = 1;  /* mark long term */
            ctx->dpb.fs_ref[i]->bottom_field.is_short_term = 0; /* mark not short term */

            /* modify frame reference attribute */
            if (ctx->dpb.fs_ref[i]->frame.structure != FIELD_PAIR &&
                ctx->dpb.fs_ref[i]->frame.structure != FRAME) {
                continue;
            }
            if (ctx->dpb.fs_ref[i]->top_field.is_long_term ==
                ctx->dpb.fs_ref[i]->bottom_field.is_long_term) {
                ctx->dpb.fs_ref[i]->frame.is_long_term = ctx->dpb.fs_ref[i]->bottom_field.is_long_term;
                ctx->dpb.fs_ref[i]->frame.is_short_term = ctx->dpb.fs_ref[i]->bottom_field.is_short_term;
            }
            break;
        }
    }
}

/*************************************************************************
*      函数原型 :  td_void mark_pic_lt_ref(td_u32 , td_s32 , h264_cur_pic *)
*      功能描述 :  将long_term_pic_num对应的长期参考图像标记为不参考
*      参数说明 :  pCurrPic当前图像数据结构体指针，long_term_pic_num将要标记的图像的long_term_pic_num
*      返回值   ： 无
*************************************************************************/
STATIC td_void mark_pic_lt_ref(h264_ctx *ctx, td_u32 long_term_frame_idx, td_s32 pic_num_x,
                               const h264_cur_pic *curr_pic)
{
    if (curr_pic->structure == FRAME) {
        mark_pic_lt_ref_by_frm(ctx, long_term_frame_idx, pic_num_x);
    } else {
        mark_pic_lt_ref_by_top_fld(ctx, long_term_frame_idx, pic_num_x, curr_pic);
        mark_pic_lt_ref_by_btm_fld(ctx, long_term_frame_idx, pic_num_x, curr_pic);
    }

    return;
}

STATIC td_void mark_st_to_lt_ref_unuse(h264_ctx *ctx, td_u32 long_term_frame_idx, td_s32 pic_num_x)
{
    td_u32 i;

    for (i = 0; i < ctx->dpb.ref_frames_in_buffer; i++) {
        if (((ctx->dpb.fs_ref[i]->is_reference & 1) != 0) &&
            ctx->dpb.fs_ref[i]->top_field.is_short_term == 1 &&
            ctx->dpb.fs_ref[i]->top_field.is_long_term == 0) { /* include top field */
            if (ctx->dpb.fs_ref[i]->top_field.pic_num == pic_num_x) {
                unmark_lt_fld_by_frm_idx(ctx, ctx->dpb.fs_ref[i], TOP_FIELD, long_term_frame_idx);
                break;
            }
        }

        if (((ctx->dpb.fs_ref[i]->is_reference & 2) != 0) && /* 2 btm ref */
            ctx->dpb.fs_ref[i]->bottom_field.is_short_term == 1 &&
            ctx->dpb.fs_ref[i]->bottom_field.is_long_term == 0) { /* include bottom field */
            if (ctx->dpb.fs_ref[i]->bottom_field.pic_num == pic_num_x) {
                unmark_lt_fld_by_frm_idx(ctx, ctx->dpb.fs_ref[i], BOTTOM_FIELD, long_term_frame_idx);
                break;
            }
        }
    }
}

/*************************************************************************
*      函数原型 :  td_void unmark_lt_ref(h264_ctx *ctx, h264_cur_pic *curr_pic, td_u32 )
*      功能描述 :  将long_term_pic_num对应的长期参考图像标记为不参考
*      参数说明 :  pCurrPic当前图像数据结构体指针，long_term_pic_num将要标记的图像的long_term_pic_num
*      返回值   ： 无
*************************************************************************/
STATIC td_void mark_st_to_lt_ref(h264_ctx *ctx, const h264_cur_pic *curr_pic,
                                 td_u32 difference_of_pic_nums_minus1, td_u32 long_term_frame_idx)
{
    td_s32 pic_num_x;

    /* Get pic_num_x */
    pic_num_x = get_pic_num_x(curr_pic, difference_of_pic_nums_minus1);

    /* then mark long term pic to unused which have this long_term_frame_idx */
    if (curr_pic->structure == FRAME) { /* frame , both field pair */
        unmark_lt_frm_by_frm_idx(ctx, long_term_frame_idx);
    } else {
        /* just find structure which will mark to long term */
        mark_st_to_lt_ref_unuse(ctx, long_term_frame_idx, pic_num_x);
    }

    /* finally mark short pic to long term */
    mark_pic_lt_ref(ctx, long_term_frame_idx, pic_num_x, curr_pic);

    return;
}

/*************************************************************************
*      函数原型 :  td_void update_max_lt_frm_idx(h264_ctx *, td_u32 )
*      功能描述 :  更新DPB中最大长期参考索引
*      参数说明 :  更新后的最大长期参考帧索引max_long_term_frame_idx_plus1
*      返回值   ： 无
*************************************************************************/
STATIC td_void update_max_lt_frm_idx(h264_ctx *ctx, td_u32 max_long_term_frame_idx_plus1)
{
    td_u32 i;

    ctx->dpb.max_long_term_pic_idx = max_long_term_frame_idx_plus1;

    /* check for invalid frames */
    for (i = 0; i < ctx->dpb.ltref_frames_in_buffer; i++) {
        /* 这里的类型转换一定要，不然当max_long_term_frame_idx_plus1等于0的时候就出错了 */
        if (ctx->dpb.fs_ltref[i]->long_term_frame_idx >= ctx->dpb.max_long_term_pic_idx) {
            unmark_frame_store_ref(ctx, ctx->dpb.fs_ltref[i]);
        }
    }
}

/*************************************************************************
*      函数原型 :  td_void unmark_all_st_ref(h264_ctx *ctx)
*      功能描述 :  将所有短期参考帧标记为不参考
*      参数说明 :  无
*      返回值   ： 无
*************************************************************************/
STATIC td_void unmark_all_st_ref(h264_ctx *ctx)
{
    td_u32 i;

    for (i = 0; i < ctx->dpb.ref_frames_in_buffer; i++) {
        unmark_frame_store_ref(ctx, ctx->dpb.fs_ref[i]);
    }

    return;
}

/*************************************************************************
*      函数原型 :  td_void mark_curr_pic_lt (h264_ctx *, h264_cur_pic *, td_u32)
*      功能描述 :  把当前解码图像标记为长期参考
*      参数说明 :  pCurrPic当前图像数据结构体指针，
*                  long_term_frame_idx分配给当前解码图像的长期参考帧索引
*      返回值   ： 无
*************************************************************************/
STATIC td_void mark_curr_pic_lt(h264_ctx *ctx, h264_cur_pic *curr_pic, td_u32 long_term_frame_idx)
{
    /* remove long term pictures with same long_term_frame_idx */
    if (curr_pic->structure == FRAME) { /* frame ,field pair */
        /* mark pic with long_term_frame_idx */
        unmark_lt_frm_by_frm_idx(ctx, long_term_frame_idx);
    } else {
        unmark_lt_fld_by_frm_idx(ctx, curr_pic->frame_store, curr_pic->structure, long_term_frame_idx);
    }

    /* mark current pic */
    curr_pic->is_long_term = 1;
    curr_pic->is_short_term = 0;
    curr_pic->long_term_frame_idx = long_term_frame_idx;

    return;
}

/*************************************************************************
*   函数原型 :  td_void remove_frame_store_out_dpb(h264_ctx *ctx, td_u32 pos)
*   功能描述 :  将framestore指针移出dpb
*************************************************************************/
STATIC td_void remove_frame_store_out_dpb(h264_ctx *ctx, td_u32 pos)
{
    td_u32 i;
    fsp_logic *logic = TD_NULL;

    /* release apc */
    /* 容错处理，在用到指针时，无论在什么时候都应该判断是否为空 */
    if (ctx->dpb.fs[pos] == TD_NULL) {
        return;
    }

    ctx->apc.is_used[ctx->dpb.fs[pos]->apc_idc] = 0;
    ctx->apc.used_size = (ctx->apc.used_size > 0) ? (ctx->apc.used_size - 1) : 0;

    logic = fsp_get_logic(ctx->chan_id, ctx->dpb.fs[pos]->logic_fs_id);
    if (logic != TD_NULL) {
        if (logic->disp_state != FSP_DISP_STATE_WAIT) {
            fsp_set_disp(ctx->chan_id, ctx->dpb.fs[pos]->logic_fs_id, 0);
        }

        /* 把逻辑帧存标记为不参考 */
        fsp_set_ref(ctx->chan_id, ctx->dpb.fs[pos]->logic_fs_id, 0);
    }

    ctx->dpb.fs[pos]->is_used = 0;
    ctx->dpb.fs[pos]->is_in_dpb = 0;
    ctx->dpb.fs[pos]->is_reference = 0;
    ctx->dpb.fs[pos]->logic_fs_id = -1;

    for (i = pos; (i < ctx->dpb.used_size - 1) && (i + 1) < 16; i++) { /* 16 :a number */
        ctx->dpb.fs[i] = ctx->dpb.fs[i + 1];
    }

    ctx->dpb.fs[ctx->dpb.used_size - 1] = (h264_frame_store *)TD_NULL;
    ctx->dpb.used_size--;

    return;
}

STATIC td_void remove_unused_fs_unref(const h264_ctx *ctx, const fsp_logic *logic_fs, td_u32 j)
{
    if ((ctx->dpb.fs[j]->is_reference == 0) && (logic_fs->is_ref != 0)) {
        fsp_set_ref(ctx->chan_id, ctx->dpb.fs[j]->logic_fs_id, 0);
    }
}

/*************************************************************************
*   函数原型 :  td_s32 remove_unused_frame_store(h264_ctx *ctx)
*   功能描述 :  清除已经从g_DPB输出的非参考帧，针对已经vo显示与否两种情况对frame store 参数赋值
*************************************************************************/
STATIC td_u32 remove_unused_frame_store(h264_ctx *ctx)
{
    td_u32 i, j, k;
    fsp_logic *logic_fs = TD_NULL;

    if (ctx == TD_NULL) {
        return 0;
    }

    /* check for frames that were already output and no longer used for reference */
    /* ctx->dpb.used_size不能直接作为如下循环的控制变量，这是因为Remove操作本身会使used_size
       减1，若用它作循环控制变量，会使得DPB中的某些帧实际上并未遍历到，从而导致本该从DPB删除的帧依然留在DPB里. 这样，
       DPB和FSP的信息是冲突的，可以引发很多问题.
       同州卡顿码流即由此引起，原因是FSP和DPB冲突造成GetAPC()失败，进而清除了DPB，导致丢帧. */
    k = ctx->dpb.used_size;

    for (i = 0, j = 0; i < k; i++) {
        logic_fs = fsp_get_logic(ctx->chan_id, ctx->dpb.fs[j]->logic_fs_id);
        if (logic_fs != TD_NULL) {
            /* 如果fsp帧存已经空闲，要确保该帧立即从DPB中删除 */
            if (logic_fs->is_ref == 0 &&
                (logic_fs->disp_state == FSP_DISP_STATE_IDLE || logic_fs->disp_state == FSP_DISP_STATE_WAIT)) {
                remove_unused_fs_unref(ctx, logic_fs, j);

                ctx->dpb.fs[j]->logic_fs_id = -1;
                ctx->dpb.fs[j]->is_reference = 0;
                /* 如果该帧是第一帧，刚被快速输出且尚未轮到正常输出，此时不要把它剔除出DPB.
                   这样处理的目的是让第一帧还有机会经历正常输出序，这有助于精确丢弃open gop的头2个B帧 */
                ctx->dpb.fs[j]->is_in_dpb = (ctx->dpb.fs[j]->fo_state != FO_STATE_FAST_OUTPUT) ? 0 : 1;
            }
        } else {
            ctx->dpb.fs[j]->is_in_dpb = (ctx->dpb.fs[j]->fo_state != FO_STATE_FAST_OUTPUT) ? 0 : 1;
        }

        if ((frame_store_ref_flg(ctx->dpb.fs[j]) == 0) && (ctx->dpb.fs[j]->is_in_dpb == 0) &&
            (ctx->dpb.fs[j]->fo_state != FO_STATE_FAST_OUTPUT)) {
            ctx->dpb.fs[j]->is_used = 0;
            remove_frame_store_out_dpb(ctx, j);
        } else {
            j++;
        }
    }

    return 0;
}

STATIC td_s32 check_frame_store_other(const h264_ctx *ctx, const h264_frame_store *frame_store,
    const fsp_logic *logic_fs)
{
    /* the control condition would added */
    if ((frame_store->is_used == 0) || (frame_store->non_existing == 1)) {
        dprint(PRN_ERROR, "not used frame: (%d,%d)\n", frame_store->is_used, frame_store->non_existing);
        return H264_ERR;
    }

    if ((frame_store->is_used == 3) && (frame_store->mode_skip_flag == 3)) { /* 3 both fld */
        dprint(PRN_ERROR, "frame skip: is_used=%d,skip=%d\n", frame_store->is_used, frame_store->mode_skip_flag);
        return H264_ERR;
    }

#ifdef H264_FIELD_SUPPORT
    if ((frame_store->is_used == 1) && ((frame_store->mode_skip_flag & 1) == 1)) {
        dprint(PRN_ERROR, "topfield skip: is_used=%d,skip=%d\n", frame_store->is_used, frame_store->mode_skip_flag);
        return H264_ERR;
    }

    if ((frame_store->is_used == 2) && ((frame_store->mode_skip_flag & 2) == 2)) { /* 2 btm fld */
        dprint(PRN_ERROR, "bottom field skip: is_used=%d,skip=%d\n", frame_store->is_used, frame_store->mode_skip_flag);
        return H264_ERR;
    }
#endif
    if (logic_fs->image.error_level > (td_u32)(ctx->extra_data->out_err_thr)) {
        dprint(PRN_WARN, "err(%d) > out_thr(%d)\n", logic_fs->image.error_level, ctx->extra_data->out_err_thr);
        return H264_ERR;
    }

    return H264_OK;
}

/*************************************************************************
*   函数原型 :  td_s32 check_frame_store(H264DEC_FrameStore *frame_store)
*   功能描述 :  检查该图像是否能够输出到VO对列
*   返回值   ： 0:可以输出;-1不能输出
*************************************************************************/
STATIC td_s32 check_frame_store(h264_ctx *ctx, h264_frame_store *frame_store)
{
    fsp_logic *logic_fs = TD_NULL;
    td_s32 already_output_flag = 0;

    if (ctx->idr_found == 0) {
        /*
           该函数是按显示序调用的，之前是通过遇到"第一个非B帧"判定open gop的前几个B帧已经丢弃完毕，这在大多数时候并无问题。
        但这取决于你把怎样的帧当作"非B帧"，一个I场+B场组成的帧理应判为B帧，但在264协议中，这样的帧有可能就是可独立解码的帧，即I帧.
        所以，用帧类型判断open gop的前几个B帧是否丢完，逻辑上存在缺陷.
           修改方法: 改为"显示序在第一个解码帧之前的帧都丢弃". 如果第一帧是I场+B场组成，只要它能解，处理上就等同于I帧. */
        if (frame_store->fo_state == FO_STATE_FAST_OUTPUT || ctx->old_dec_mode != VFMW_DEC_MODE_NORMAL ||
            ctx->extra_data->output_order != VFMW_DISP_ORDER) {
            ctx->idr_found = 1;
        } else {
            return H264_ERR;
        }
    }

    logic_fs = fsp_get_logic(ctx->chan_id, frame_store->logic_fs_id);
    if (logic_fs != TD_NULL) {
        if ((logic_fs->disp_state == FSP_DISP_STATE_WAIT) || (logic_fs->disp_state == FSP_DISP_STATE_IDLE)) {
            already_output_flag = 1;
        }
    } else {
        dprint(PRN_WARN, "plfs(eFoState=%d) is null, logic_fs_id = %d\n",
               frame_store->fo_state, frame_store->logic_fs_id);
        already_output_flag = 1;
    }

    if (already_output_flag == 1) {
        /* 如果该帧是快速输出帧则标记它已经经历了正常输出序，后续不再强求它依然呆在DPB里. */
        if (frame_store->fo_state == FO_STATE_FAST_OUTPUT) {
            frame_store->fo_state = FO_STATE_NORMAL_ORDER_OUTPUT;
        }

        return H264_ERR_FS_OUT_AGAIN;
    }

    return check_frame_store_other(ctx, frame_store, logic_fs);
}

STATIC td_void h264_exchange_pts(const h264_ctx *ctx, const h264_frame_store *frame_store)
{
    td_s64 min_pts = VFMW_MAX_PTS;
    td_u32 i;
    td_s32 min_pts_pos = -1;
    fsp_logic *dst_fs = TD_NULL;
    fsp_logic *logic_fs = TD_NULL;

    vfmw_check(ctx != TD_NULL);
    vfmw_check(frame_store != TD_NULL);

    dst_fs = fsp_get_logic(ctx->chan_id, frame_store->logic_fs_id);
    /* 如果当前帧的PTS为-1，则不交换 */
    if (dst_fs == TD_NULL ||
        dst_fs->image.pts == VFMW_INVALID_PTS) {
        return;
    }

    /* 找DPB中最小的PTS */
    for (i = 0; i < ctx->dpb.used_size; i++) {
        if (i >= 16) { /* 16 ref num */
            break;
        }

        if (ctx->dpb.fs[i] != TD_NULL) {
            logic_fs = fsp_get_logic(ctx->chan_id, ctx->dpb.fs[i]->logic_fs_id);
            if (logic_fs == TD_NULL ||
                logic_fs->disp_state == FSP_DISP_STATE_WAIT ||
                logic_fs->disp_state == FSP_DISP_STATE_IDLE) {
                continue;
            }

            if (logic_fs->image.pts < min_pts) {
                min_pts_pos = (td_s32)i;
                min_pts = logic_fs->image.pts;
            }
        }
    }

    /* 如果最小PTS帧不是当前帧，则当前帧与其互换pts */
    if (min_pts_pos != -1) {
        if (dst_fs->image.pts > min_pts) {
            if (syn_is_pts_rewind(ctx->chan_id, dst_fs->image.pts, min_pts) != 0) {
                return;
            }

            dprint(PRN_PTS, "Exchange pts: %lld <-> %lld\n", dst_fs->image.pts, min_pts);
            logic_fs = fsp_get_logic(ctx->chan_id, ctx->dpb.fs[min_pts_pos]->logic_fs_id);
            vfmw_check(logic_fs != TD_NULL);
            logic_fs->image.pts = dst_fs->image.pts;
            dst_fs->image.pts = min_pts;
        }
    }

    return;
}

STATIC td_void get_image_para_both_used(const h264_ctx *ctx, const h264_frame_store *frame_store,
    fsp_logic *logic_fs, td_u32 *valid_field)
{
    td_u32 pic_type;
    td_u8 top_field_first;

    if (frame_store->frame.structure == 0) {
        *valid_field = 3; /* 3 both used */
        pic_type = frame_store->pic_type;
    } else {
        *valid_field = 3; /* 3 both used */
        pic_type = frame_store->pic_type;

        if (frame_store->mode_skip_flag == 1) {
            /* top field not dec and display bottom field only */
            *valid_field = 2; /* 2 btm used */
            pic_type = frame_store->bottom_field.pic_type;
        } else if (frame_store->mode_skip_flag == 2) { /* 2 btm fld */
            /* bottom field not dec and display top field only */
            *valid_field = 1;
            pic_type = frame_store->top_field.pic_type;
        }

        logic_fs->image.top_fld_type = frame_store->top_field.pic_type;
        logic_fs->image.bottom_fld_type = frame_store->bottom_field.pic_type;
    }

    /* although some field cann't display some time becouse of err_level,
        its poc is right for judge display order */
    if (frame_store->top_field.poc == frame_store->bottom_field.poc) {
        top_field_first = ((logic_fs->image.image_width == 720) && /* 720 wid */
                           (logic_fs->image.image_height == 480)) ? 0 : 1; /* 480 hig */
    } else if (frame_store->top_field.poc > frame_store->bottom_field.poc) {
        top_field_first = 0;
    } else {
        /* TopPOC smaller than BottomPOC, so the field display first  is top field */
        top_field_first = 1;
    }

    /* 如果SEI中已经传了场序，用SEI的信息 */
    if (ctx->pic_timing_sei.pic_struct == 3 || ctx->pic_timing_sei.pic_struct == 5) { /* 3 5 top first */
        top_field_first = 1;
    } else if (ctx->pic_timing_sei.pic_struct == 4 || ctx->pic_timing_sei.pic_struct == 6) { /* 4 6 btm fitst */
        top_field_first = 0;
    }

    logic_fs->image.format.top_field_first = top_field_first;
    logic_fs->image.format.frame_type = (td_u8)pic_type;

    logic_fs->image.top_fld_type = frame_store->top_field.pic_type;
    logic_fs->image.bottom_fld_type = frame_store->bottom_field.pic_type;
}

#ifdef H264_FIELD_SUPPORT
STATIC td_void get_image_para_btm_used(const h264_ctx *ctx, h264_frame_store *frame_store,
    fsp_logic *logic_fs, td_u32 *valid_field)
{
    td_u32 pic_type;

    vfmw_unused(ctx);
    *valid_field = 2; /* 2 btm fld */
    pic_type = frame_store->pic_type;

    logic_fs->image.format.frame_type = (td_u8)pic_type;
    logic_fs->image.error_level = frame_store->err_level;
    logic_fs->image.top_fld_type = 3; /* 3 底场有效时，顶场的类型为无效值 */
    logic_fs->image.bottom_fld_type = frame_store->bottom_field.pic_type;
    frame_store->frame.avg_qp = frame_store->bottom_field.avg_qp;
}

STATIC td_void get_image_para_top_used(const h264_ctx *ctx, h264_frame_store *frame_store,
    fsp_logic *logic_fs, td_u32 *valid_field)
{
    td_u32 pic_type;

    vfmw_unused(ctx);
    *valid_field = 1;
    pic_type = frame_store->pic_type;

    logic_fs->image.format.frame_type = (td_u8)pic_type;
    logic_fs->image.error_level = frame_store->err_level;
    logic_fs->image.top_fld_type = frame_store->top_field.pic_type;
    logic_fs->image.bottom_fld_type = 3; /* 3 顶场有效时，底场的类型为无效值 */
    frame_store->frame.avg_qp = frame_store->top_field.avg_qp;
}
#endif

STATIC td_void get_image_para_3d(const h264_ctx *ctx, fsp_logic *logic_fs)
{
    /* if it is a 3D frame, assign its packing type */
    if (ctx->frame_pack_sei.frame_packing_dec_ok != 0) {
        if (ctx->frame_pack_sei.frame_packing_arrangement_type == 3) { /* 3 sbs */
            logic_fs->image.frm_pack_type = FRAME_PACKING_TYPE_SIDE_BY_SIDE;
        } else if (ctx->frame_pack_sei.frame_packing_arrangement_type == 4) { /* 4 top btm */
            logic_fs->image.frm_pack_type = FRAME_PACKING_TYPE_TOP_BOTTOM;
        } else if (ctx->frame_pack_sei.frame_packing_arrangement_type == 5) { /* 5 interlaced */
            logic_fs->image.frm_pack_type = FRAME_PACKING_TYPE_TIME_INTERLACED;
        } else {
            logic_fs->image.frm_pack_type = FRAME_PACKING_TYPE_NONE;
            dprint(PRN_ERROR, "stream frame packing type is %d! what can we do?\n",
                   ctx->frame_pack_sei.frame_packing_arrangement_type);
        }
    }
}

/*************************************************************************
*   函数原型 :  td_void get_image_para(h264_ctx *, H264DEC_FrameStore *)
*   功能描述 :  赋值要输出的framestore的fs_image结构体
*************************************************************************/
STATIC td_void get_image_para(h264_ctx *ctx, h264_frame_store *frame_store)
{
    td_u32 valid_field = 0;
    td_ulong lock_flag;
    fsp_logic *logic_fs = TD_NULL;

    vfmw_check(frame_store != TD_NULL);

    logic_fs = fsp_get_logic(ctx->chan_id, frame_store->logic_fs_id);
    vfmw_check(logic_fs != TD_NULL);

    fsp_img_format_lock(&lock_flag);
    logic_fs->image.format.frame_type = 0;
    logic_fs->image.frame_rate = ctx->parse_frame_rate;
    logic_fs->image.splice_info.type = ctx->splice_info.type;
    logic_fs->image.splice_info.ori_width = ctx->splice_info.ori_width;
    logic_fs->image.splice_info.ori_height = ctx->splice_info.ori_height;

    if (judge_framerate_valid(ctx->parse_frame_rate) == SYN_OK) {
        logic_fs->image.format.frame_rate_valid = 1;
    } else {
        logic_fs->image.format.frame_rate_valid = 0;
    }

    switch (frame_store->is_used) {
        /* 由于第一帧快速输出，致使第一帧输出(调用此函数)时is_used还未处理，所以通常为0，为避免format的相关字段出现
               不可预料的数值，所以也走is_used = 3的分支 */
        case 0: /* 0 both used */
        case 3: /* 3 both used */
            get_image_para_both_used(ctx, frame_store, logic_fs, &valid_field);
            break;
#ifdef H264_FIELD_SUPPORT
        case 2: /* 2 btm used */
            get_image_para_btm_used(ctx, frame_store, logic_fs, &valid_field);
            break;

        case 1: /* 1 top used */
            get_image_para_top_used(ctx, frame_store, logic_fs, &valid_field);
            break;
#endif
        default:
            break;
    }

    fsp_set_fld_flag(ctx->chan_id, frame_store->logic_fs_id, (td_s32)valid_field);

    /* 若按解码序输出，多半是可视电话应用，此时的源格式应该是逐行 */
    if ((ctx->extra_data->output_order > VFMW_DISP_ORDER) &&
        (frame_store->frame.structure == 0) &&
        (frame_store->is_used == 3)) { /* 3 both fld */
        logic_fs->image.format.source_format = 0;
    }
    fsp_img_format_unlock(&lock_flag);

    h264_exchange_pts(ctx, frame_store);
    get_image_para_3d(ctx, logic_fs);
}

STATIC td_void set_frm_repeat_count(h264_ctx *ctx, vfmw_image *img)
{
    vfmw_unused(img);
    if (!((ctx->extra_data->fast_forward_backward_speed > -2048) && /* 2048 speed */
          (ctx->extra_data->fast_forward_backward_speed <= 2048))) { /* 2048 speed */
        if (ctx->new_pic_counter / 2 >= (ctx->pic_out_counter / 2 + 1 + ctx->dpb.size)) { /* 2 is half */
            if ((ctx->new_pic_counter / 2 - (ctx->pic_out_counter / 2 + 1 + ctx->dpb.size)) >= 2) { /* 2 is num */
                ctx->pic_out_counter += 4; /* 4 pic cnt */
            } else {
                ctx->pic_out_counter += 2; /* 2 pic cnt */
            }
        }
    }
    return;
}

STATIC td_s32 output_frm_to_vo_of_direct(h264_ctx *ctx,
    const h264_frame_store *frame_store, td_u32 is_last_frame, td_bool *go_on)
{
    td_s32 ret;
    vfmw_image *logic_fs_image = TD_NULL;

    *go_on = TD_FALSE;

    if (ctx->direct_out_buf != (h264_frame_store *)TD_NULL) {
        get_image_para(ctx, ctx->direct_out_buf);
        ret = check_frame_store(ctx, ctx->direct_out_buf);
        if (ret == H264_OK) {
            fsp_set_disp(ctx->chan_id, ctx->direct_out_buf->logic_fs_id, 1);
            logic_fs_image = fsp_get_image(ctx->chan_id, ctx->direct_out_buf->logic_fs_id);
            if (logic_fs_image == TD_NULL) {
                dprint(PRN_ERROR, "line; %d logic_fs_image is TD_NULL!\n", __LINE__);
                return H264_ERR;
            }

            if (frame_store == ctx->direct_out_buf && is_last_frame == 1) {
                logic_fs_image->last_frame = 1;
            }

            ret = voq_insert(ctx->chan_id, logic_fs_image);
            if (ret != VOQ_OK) {
                dprint(PRN_ERROR, "InsertVO err:%d, clear_all\n", ret);

                /* !! 插入队列超时且需要退出 */
                ctx->direct_out_buf->is_used = 0;
                fsp_set_disp(ctx->chan_id, ctx->direct_out_buf->logic_fs_id, 0);
                ctx->direct_out_buf = (h264_frame_store *)TD_NULL;
                clear_all(ctx, 1);
                return H264_ERR; /* OUT_FLAG */
            }

            set_frm_repeat_count(ctx, logic_fs_image);
            ctx->num_output_frame++;
            ctx->pic_out_counter += 2; /* 2 pic cnt */
        } else if (ret != H264_ERR_FS_OUT_AGAIN) {
            dprint(PRN_WARN, "H264.c line %d: frame para err(ret=%d), recycle image self\n", __LINE__, ret);
            h264_dec_recycle_image(ctx, (td_u32)ctx->direct_out_buf->logic_fs_id);
        }

        ctx->direct_out_buf->is_used = 0;

        if (frame_store == ctx->direct_out_buf) {
            ctx->direct_out_buf = (h264_frame_store *)TD_NULL;
            return H264_OK;
        } else {
            ctx->direct_out_buf = (h264_frame_store *)TD_NULL;
        }
    }

    *go_on = TD_TRUE;

    return H264_OK;
}

/*************************************************************************
*   函数原型 :  td_s32 output_frm_to_vo(h264_ctx *ctx, h264_frame_store *, td_u32)
*   功能描述 :  输出一帧图像
*************************************************************************/
STATIC td_s32 output_frm_to_vo(h264_ctx *ctx, h264_frame_store *frame_store, td_u32 is_last_frame)
{
    td_s32 ret;
    td_bool go_on = TD_FALSE;
    vfmw_image *logic_fs_image = TD_NULL;

    vfmw_check_ret(ctx != TD_NULL && ctx->extra_data != TD_NULL && frame_store != TD_NULL, H264_OUTPUT_FAIL);

    /* 本函数仅处理显示序输出，如果解码序输出此处仅置必要的标志，并不真正输出图像.
       注意: I帧模式下不适用如下分支，因为I帧模式走direct ouput，效果上等同于解码序
             输出; 而且direct output又依赖本函数输出图象，所以下面的分支不能进. */
    if (ctx->extra_data->output_order > VFMW_DISP_ORDER && (ctx->old_dec_mode != 2)) { /* 2 i mode */
        /* 场景： 客户的码流全是顶场, 解码序。导致的问题: 有时候会不输出图像 */
        /* 解码第一场的时候, 由于只有一个单场, 所以没有输出, 下一场解码又是顶场, 所以把前一场输出, */
        /* 但是运行到这里的时候, 发现是解码序, 走进这个分支, 返回已经输出, 所以这个单场就丢了 */
        if (ctx->direct_out_buf != TD_NULL) {
            ctx->direct_out_buf->is_used = 0;
            ctx->direct_out_buf = (h264_frame_store *)TD_NULL;
        }

        return H264_ALREADY_OUT;
    }

    /* pFrameStore可能已经输出，若如此这里必须及时中止输出帧的操作.
       已输出的帧又走到这里的原因是，某些帧虽已输出但仍作参考，所以还在DPB里，有可能再次被挑中尝试输出. */
    if (check_frame_store(ctx, frame_store) == H264_ERR_FS_OUT_AGAIN) {
        return H264_ALREADY_OUT;
    }

    ret = output_frm_to_vo_of_direct(ctx, frame_store, is_last_frame, &go_on);
    if (go_on == TD_FALSE) {
        return ret;
    }

    get_image_para(ctx, frame_store);
    ret = check_frame_store(ctx, frame_store);
    if (ret == H264_OK) {
        fsp_set_disp(ctx->chan_id, frame_store->logic_fs_id, 1);
        logic_fs_image = fsp_get_image(ctx->chan_id, frame_store->logic_fs_id);
        if (logic_fs_image == TD_NULL) {
            dprint(PRN_ERROR, "line: %d logic_fs_image is TD_NULL!\n", __LINE__);
            return H264_ERR;
        }

        if (is_last_frame == 1) {
            logic_fs_image->last_frame = 1;
        }

        if (voq_insert(ctx->chan_id, logic_fs_image) != VOQ_OK) {
            /* !! 插入队列超时且需要退出 */
            dprint(PRN_ERROR, "InsertVO err:%d, clear_all\n", ret);
            fsp_set_disp(ctx->chan_id, frame_store->logic_fs_id, 0);
            clear_all(ctx, 1);
            return H264_ERR; /* OUT_FLAG */
        }

        set_frm_repeat_count(ctx, logic_fs_image);
        ctx->num_output_frame++;
        ctx->pic_out_counter += 2; /* 2 pic cnt */
    } else if (ret == H264_ERR_FS_OUT_AGAIN) {
        return H264_ALREADY_OUT;
    } else {
        dprint(PRN_WARN, "H264.c line %d: frame para err(ret=%d), recycle image self\n", __LINE__, ret);
        h264_dec_recycle_image(ctx, (td_u32)frame_store->logic_fs_id);
        return H264_OUTPUT_FAIL; /* OUT_FLAG */
    }

    return H264_OK;
}

/*************************************************************************
*   函数原型 :  td_s32 output_frm_from_dpb(h264_ctx *ctx,, td_u32 pos)
*   功能描述 :  从g_DPB中输出一帧，同时将一帧从DPB中删除
*************************************************************************/
STATIC td_s32 output_frm_from_dpb(h264_ctx *ctx, td_u32 pos)
{
    td_s32 ret;

    if (ctx->dpb.fs[pos] == TD_NULL) {
        dprint(PRN_ERROR, "%s chn %d fs[%d] null\n", __func__, ctx->chan_id, pos);
        return SYN_ERR;
    }

    ctx->dpb.fs[pos]->is_in_dpb = 0; /* here, output g_DPB to display */
    ret = output_frm_to_vo(ctx, ctx->dpb.fs[pos], 0);

    /* free frame store and move empty store to end of buffer */
    if (frame_store_ref_flg(ctx->dpb.fs[pos]) == 0) {
        remove_frame_store_out_dpb(ctx, pos);
    }

    return ret;
}

/*************************************************************************
*      函数原型 :  td_void split_frm_to_flds(h264_frame_store *frame_store)
*      功能描述 :  把一帧分成两场
*      参数说明 :  frame_store:两场所在帧存
*      返回值   ： 无
*
*************************************************************************/
STATIC td_void split_frm_to_flds(h264_frame_store *frame_store)
{
    frame_store->top_field.is_long_term = frame_store->frame.is_long_term;
    frame_store->top_field.is_short_term = frame_store->frame.is_short_term;
    frame_store->top_field.structure = TOP_FIELD;
    frame_store->top_field.err_level = frame_store->frame.err_level;
    frame_store->top_field.pic_type = frame_store->frame.pic_type;
    frame_store->top_field.frame_store = frame_store->frame.frame_store;

    frame_store->bottom_field.is_long_term = frame_store->frame.is_long_term;
    frame_store->bottom_field.is_short_term = frame_store->frame.is_short_term;
    frame_store->bottom_field.structure = BOTTOM_FIELD;
    frame_store->bottom_field.err_level = frame_store->frame.err_level;
    frame_store->bottom_field.pic_type = frame_store->frame.pic_type;
    frame_store->bottom_field.frame_store = frame_store->frame.frame_store;

    return;
}

/*************************************************************************
*      函数原型 :  td_void combine_flds_to_frm(h264_frame_store *frame_store)
*      功能描述 :  成对的两场所在帧存拼成一帧
*      参数说明 :  frame_store:两场所在帧存
*      返回值   ： 无
*
*************************************************************************/
#ifdef H264_FIELD_SUPPORT
STATIC td_void combine_flds_to_frm(h264_frame_store *frame_store)
{
    frame_store->poc = frame_store->frame.poc = vfmw_min(frame_store->top_field.poc, frame_store->bottom_field.poc);
    frame_store->frame.structure = FIELD_PAIR; /* field pair more important */
    frame_store->frame.err_level = (frame_store->top_field.err_level +
                                    frame_store->bottom_field.err_level) / 2; /* 2 half */
    frame_store->frame.frame_store = frame_store->top_field.frame_store;

    frame_store->frame.is_long_term = (frame_store->bottom_field.is_long_term == 1) &&
        (frame_store->top_field.is_long_term == 1);
    frame_store->frame.is_short_term = (frame_store->bottom_field.is_short_term == 1 ||
                                        frame_store->bottom_field.is_long_term == 1) &&
                                       (frame_store->top_field.is_short_term == 1 ||
                                       frame_store->top_field.is_long_term == 1) &&
                                       (frame_store->frame.is_long_term == 0);

    /* pic type combine */
    /* 原来的帧级pic_type生成规则是:只要有一场是I场就认为该帧是I帧. 此规则不合常理，修改如下 */
    frame_store->frame.pic_type = ((frame_store->top_field.pic_type == 0) ||
                                   (frame_store->bottom_field.pic_type) == 0) ? 0 :
                                  vfmw_max(frame_store->top_field.pic_type, frame_store->bottom_field.pic_type);
    frame_store->pic_type = frame_store->frame.pic_type;

    frame_store->err_level = frame_store->frame.err_level;

    return;
}
#endif

/*************************************************************************
*   函数原型 :  td_s32    get_apc(h264_ctx *ctx, h264_frame_store *frame_store)
*   功能描述 :  分配apc，没分到时返回错误
*************************************************************************/
STATIC td_s32 get_apc(h264_ctx *ctx, h264_frame_store *frame_store, td_u32 structure)
{
    td_u32 i;
    td_s32 top_poc, bottom_poc;
    fsp_logic *logic_fs = TD_NULL;

    vfmw_check_ret(frame_store != TD_NULL, H264_ERR);

    switch (structure) {
        case TOP_FIELD:
            top_poc = frame_store->top_field.poc;
            bottom_poc = frame_store->top_field.poc;
            break;

        case BOTTOM_FIELD:
            top_poc = frame_store->bottom_field.poc;
            bottom_poc = frame_store->bottom_field.poc;
            break;

        case FRAME:
        default:
            top_poc = frame_store->top_field.poc;
            bottom_poc = frame_store->bottom_field.poc;
    }

    for (i = 0; i < ctx->apc.size; i++) {
        if (ctx->apc.is_used[i] == 0) {
            /* apc is not used, allocate */
            logic_fs = fsp_get_logic(ctx->chan_id, frame_store->logic_fs_id);
            if (logic_fs != TD_NULL) {
                ctx->apc.is_used[i] = 1;
                ctx->apc.idc[i] = 0;
                ctx->apc.logic_id[i] = (td_u32)frame_store->logic_fs_id;
                ctx->apc.poc[0][i] = (td_u32)top_poc;
                ctx->apc.poc[1][i] = (td_u32)bottom_poc;
                ctx->apc.used_size = (ctx->apc.used_size >= 16) ? 16 : (ctx->apc.used_size + 1); /* 16 dpb size */

                frame_store->apc_idc = i;
                return H264_OK;
            } else {
                dprint(PRN_ERROR, "find apc, but logic_fs_id %d abnormal(ref=%d)\n",
                       frame_store->logic_fs_id, frame_store->is_reference);
                return H264_ERR;
            }
        }
    }

    return H264_ERR;
}

STATIC td_s32 insert_frm_in_dpb_frame(h264_ctx *ctx, td_u32 pos, const h264_cur_pic *curr_pic)
{
    td_s32 ret;

    ctx->dpb.fs[pos] = curr_pic->frame_store;
    ctx->dpb.fs[pos]->non_existing = curr_pic->non_existing;
    ctx->dpb.fs[pos]->is_used = 3; /* 3 both used */
    ctx->dpb.fs[pos]->is_in_dpb = curr_pic->frame_store->is_in_dpb;
    ctx->dpb.fs[pos]->is_displayed = curr_pic->frame_store->is_displayed;
    ctx->dpb.fs[pos]->is_reference =
        ((curr_pic->is_long_term != 0) ? 3 : ((curr_pic->is_short_term != 0) ? 3 : 0)); /* 3 both */
    ctx->dpb.fs[pos]->pmv_address_idc = curr_pic->pmv_address_idc;
    ctx->dpb.fs[pos]->logic_fs_id = curr_pic->frame_store->logic_fs_id;
    ctx->dpb.fs[pos]->err_level = ctx->dpb.fs[pos]->frame.err_level = curr_pic->err_level;
    ctx->dpb.fs[pos]->frame.frame_store = ctx->dpb.fs[pos]->top_field.frame_store =
        ctx->dpb.fs[pos]->bottom_field.frame_store = curr_pic->frame_store;
    ctx->dpb.fs[pos]->long_term_frame_idx = curr_pic->long_term_frame_idx; /* 可能无效，为了省判断直接赋值 */
    ctx->dpb.fs[pos]->frame_num = curr_pic->frame_num;
    ctx->dpb.fs[pos]->frame.structure = FRAME;
    ctx->dpb.fs[pos]->frame.is_long_term = curr_pic->is_long_term;
    ctx->dpb.fs[pos]->frame.is_short_term = curr_pic->is_short_term;
    ctx->dpb.fs[pos]->pic_type = ctx->dpb.fs[pos]->frame.pic_type = curr_pic->pic_type;
    ctx->dpb.fs[pos]->mbaff_frame_flag = curr_pic->mbaff_frame_flag;

    ctx->dpb.fs[pos]->poc = curr_pic->thispoc;
    ctx->dpb.fs[pos]->frame.poc = curr_pic->framepoc;
    ctx->dpb.fs[pos]->top_field.poc = curr_pic->toppoc;
    ctx->dpb.fs[pos]->bottom_field.poc = curr_pic->bottompoc;

    ctx->dpb.fs[pos]->mode_skip_flag = (curr_pic->mode_skip_flag == 1) ? 3 : 0; /* 3 both fld */

    split_frm_to_flds(ctx->dpb.fs[pos]);

    ret = get_apc(ctx, curr_pic->frame_store, FRAME);

    return ret;
}

#define h264_set_less(src, b) do { \
    (src) = ((src) < (b)) ? (src) : (b); \
} while (0)

#ifdef H264_FIELD_SUPPORT
STATIC td_s32 insert_frm_in_dpb_top_field_only(h264_ctx *ctx, td_u32 pos, const h264_cur_pic *curr_pic)
{
    ctx->dpb.fs[pos] = curr_pic->frame_store;
    ctx->dpb.fs[pos]->top_field.structure = TOP_FIELD;
    ctx->dpb.fs[pos]->top_field.frame_store = curr_pic->frame_store;
    ctx->dpb.fs[pos]->top_field.is_long_term = curr_pic->is_long_term;
    ctx->dpb.fs[pos]->top_field.is_short_term = curr_pic->is_short_term;
    ctx->dpb.fs[pos]->top_field.poc = curr_pic->toppoc;
    ctx->dpb.fs[pos]->top_field.pic_type = curr_pic->pic_type;
    ctx->dpb.fs[pos]->top_field.err_level = curr_pic->err_level;
    ctx->dpb.fs[pos]->err_level = ctx->dpb.fs[pos]->frame.err_level = ctx->dpb.fs[pos]->top_field.err_level;

    ctx->dpb.fs[pos]->non_existing = curr_pic->non_existing; /* must be 0 */
    ctx->dpb.fs[pos]->is_used = 1;
    ctx->dpb.fs[pos]->is_in_dpb = curr_pic->frame_store->is_in_dpb;
    ctx->dpb.fs[pos]->is_displayed = curr_pic->frame_store->is_displayed;

    if ((curr_pic->is_long_term == 1) || (curr_pic->is_short_term == 1)) {
        ctx->dpb.fs[pos]->is_reference = 1;
    }

    if (curr_pic->is_long_term == 1) {
        ctx->dpb.fs[pos]->long_term_frame_idx = curr_pic->long_term_frame_idx;
    }

    ctx->dpb.fs[pos]->mbaff_frame_flag = 0;
    ctx->dpb.fs[pos]->logic_fs_id = curr_pic->frame_store->logic_fs_id;
    ctx->dpb.fs[pos]->pmv_address_idc = curr_pic->pmv_address_idc;
    ctx->dpb.fs[pos]->frame_num = curr_pic->frame_num;
    ctx->dpb.fs[pos]->poc = curr_pic->thispoc;
    ctx->dpb.fs[pos]->pic_type = curr_pic->pic_type;
    ctx->dpb.fs[pos]->mode_skip_flag = (curr_pic->mode_skip_flag == 1) ? 1 : 0;

    return get_apc(ctx, curr_pic->frame_store, TOP_FIELD);
}

STATIC td_s32 insert_frm_in_dpb_top_field_both(h264_ctx *ctx, td_u32 pos, const h264_cur_pic *curr_pic)
{
    fsp_logic *logic_fs = TD_NULL;

    ctx->dpb.fs[pos]->top_field.structure = TOP_FIELD;
    ctx->dpb.fs[pos]->top_field.pic_type = curr_pic->pic_type;
    ctx->dpb.fs[pos]->top_field.is_long_term = curr_pic->is_long_term;
    ctx->dpb.fs[pos]->top_field.is_short_term = curr_pic->is_short_term;
    ctx->dpb.fs[pos]->top_field.poc = curr_pic->toppoc;
    ctx->dpb.fs[pos]->top_field.err_level = curr_pic->err_level;
    ctx->dpb.fs[pos]->err_level = ctx->dpb.fs[pos]->frame.err_level =
        ctx->dpb.fs[pos]->top_field.err_level + ctx->dpb.fs[pos]->bottom_field.err_level;

    ctx->dpb.fs[pos]->non_existing = (ctx->dpb.fs[pos]->non_existing != 0) ? curr_pic->non_existing : 0;
    ctx->dpb.fs[pos]->is_used |= 1;
    ctx->dpb.fs[pos]->mode_skip_flag = (curr_pic->mode_skip_flag == 1) ?
                                       (ctx->dpb.fs[pos]->mode_skip_flag | 1) : ctx->dpb.fs[pos]->mode_skip_flag;

    ctx->dpb.fs[pos]->logic_fs_id = curr_pic->frame_store->logic_fs_id;
    logic_fs = fsp_get_logic(ctx->chan_id, ctx->dpb.fs[pos]->logic_fs_id);
    vfmw_check_ret(logic_fs != TD_NULL, H264_ERR);

    h264_set_less(logic_fs->image.user_tag, curr_pic->fs_image.user_tag);
    h264_set_less(logic_fs->image.disp_time, curr_pic->fs_image.disp_time);
    h264_set_less(logic_fs->image.disp_enable_flag, curr_pic->fs_image.disp_enable_flag);
    h264_set_less(logic_fs->image.disp_frame_distance, curr_pic->fs_image.disp_frame_distance);
    h264_set_less(logic_fs->image.distance_before_first_frame, curr_pic->fs_image.distance_before_first_frame);
    h264_set_less(logic_fs->image.gop_num, curr_pic->fs_image.gop_num);

    if ((curr_pic->is_long_term == 1) || (curr_pic->is_short_term == 1)) {
        ctx->dpb.fs[pos]->is_reference |= 1;
    }

    if (curr_pic->is_long_term == 1) {
        ctx->dpb.fs[pos]->long_term_frame_idx = curr_pic->long_term_frame_idx;
    }

    ctx->dpb.fs[pos]->pic_type =
        vfmw_max(ctx->dpb.fs[pos]->top_field.pic_type, ctx->dpb.fs[pos]->bottom_field.pic_type);
    ctx->dpb.fs[pos]->poc = curr_pic->thispoc;

    combine_flds_to_frm(ctx->dpb.fs[pos]);

    /* modify apc poc */
    ctx->apc.poc[0][ctx->dpb.fs[pos]->apc_idc] = (td_u32)ctx->dpb.fs[pos]->top_field.poc;

    return H264_OK;
}

STATIC td_s32 insert_frm_in_dpb_top_field(h264_ctx *ctx, td_u32 pos, h264_cur_pic *curr_pic)
{
    td_s32 ret;

    curr_pic->frame_store->frame.structure = FIELD_PAIR;

    if ((curr_pic->field_pair_flag == 0) || (ctx->dpb.fs[pos]) == (h264_frame_store *)TD_NULL) {
        /* 未成对 */
        ret = insert_frm_in_dpb_top_field_only(ctx, pos, curr_pic);
    } else {
        /* 成对，frame_store中的公共变量可以不再赋值，只赋值可能引起改变的 */
        ret = insert_frm_in_dpb_top_field_both(ctx, pos, curr_pic);
    }

    return ret;
}

STATIC td_s32 insert_frm_in_dpb_btm_field_only(h264_ctx *ctx, td_u32 pos, const h264_cur_pic *curr_pic)
{
    ctx->dpb.fs[pos] = curr_pic->frame_store;
    ctx->dpb.fs[pos]->bottom_field.structure = BOTTOM_FIELD;
    ctx->dpb.fs[pos]->bottom_field.frame_store = curr_pic->frame_store;
    ctx->dpb.fs[pos]->bottom_field.is_long_term = curr_pic->is_long_term;
    ctx->dpb.fs[pos]->bottom_field.is_short_term = curr_pic->is_short_term;
    ctx->dpb.fs[pos]->bottom_field.poc = curr_pic->bottompoc;
    ctx->dpb.fs[pos]->bottom_field.pic_type = curr_pic->pic_type;

    ctx->dpb.fs[pos]->bottom_field.err_level = curr_pic->err_level;
    ctx->dpb.fs[pos]->err_level = ctx->dpb.fs[pos]->frame.err_level = ctx->dpb.fs[pos]->top_field.err_level;

    ctx->dpb.fs[pos]->non_existing = curr_pic->non_existing; /* must be 0 */
    ctx->dpb.fs[pos]->is_used = 2; /* 2 btm fld */
    ctx->dpb.fs[pos]->is_in_dpb = curr_pic->frame_store->is_in_dpb;
    ctx->dpb.fs[pos]->is_displayed = curr_pic->frame_store->is_displayed;

    if ((curr_pic->is_long_term == 1) || (curr_pic->is_short_term == 1)) {
        ctx->dpb.fs[pos]->is_reference = 2; /* 2 btm ref */
    }

    if (curr_pic->is_long_term == 1) {
        ctx->dpb.fs[pos]->long_term_frame_idx = curr_pic->long_term_frame_idx;
    }

    ctx->dpb.fs[pos]->mbaff_frame_flag = 0;
    ctx->dpb.fs[pos]->logic_fs_id = curr_pic->frame_store->logic_fs_id;
    ctx->dpb.fs[pos]->pmv_address_idc = curr_pic->pmv_address_idc;
    ctx->dpb.fs[pos]->frame_num = curr_pic->frame_num;
    ctx->dpb.fs[pos]->poc = curr_pic->thispoc;
    ctx->dpb.fs[pos]->pic_type = ctx->dpb.fs[pos]->bottom_field.pic_type = curr_pic->pic_type;
    ctx->dpb.fs[pos]->mode_skip_flag = (curr_pic->mode_skip_flag == 1) ? 2 : 0; /* 2 btm fld */

    return get_apc(ctx, curr_pic->frame_store, BOTTOM_FIELD);
}

STATIC td_s32 insert_frm_in_dpb_btm_field_both(h264_ctx *ctx, td_u32 pos, const h264_cur_pic *curr_pic)
{
    fsp_logic *logic_fs = TD_NULL;

    ctx->dpb.fs[pos]->bottom_field.structure = BOTTOM_FIELD;
    ctx->dpb.fs[pos]->bottom_field.frame_store = curr_pic->frame_store;
    ctx->dpb.fs[pos]->bottom_field.is_long_term = curr_pic->is_long_term;
    ctx->dpb.fs[pos]->bottom_field.is_short_term = curr_pic->is_short_term;
    ctx->dpb.fs[pos]->bottom_field.poc = curr_pic->bottompoc;
    ctx->dpb.fs[pos]->bottom_field.pic_type = curr_pic->pic_type;
    ctx->dpb.fs[pos]->bottom_field.err_level = curr_pic->err_level;
    ctx->dpb.fs[pos]->err_level = ctx->dpb.fs[pos]->frame.err_level =
        ctx->dpb.fs[pos]->top_field.err_level + ctx->dpb.fs[pos]->bottom_field.err_level;

    ctx->dpb.fs[pos]->non_existing = (ctx->dpb.fs[pos]->non_existing != 0) ? curr_pic->non_existing : 0;
    ctx->dpb.fs[pos]->is_used |= 2; /* 2 btm fld */
    ctx->dpb.fs[pos]->mode_skip_flag = (curr_pic->mode_skip_flag == 1) ?
        (ctx->dpb.fs[pos]->mode_skip_flag | 2) : ctx->dpb.fs[pos]->mode_skip_flag; /* 2 btm fld */

    ctx->dpb.fs[pos]->logic_fs_id = curr_pic->frame_store->logic_fs_id;
    logic_fs = fsp_get_logic(ctx->chan_id, ctx->dpb.fs[pos]->logic_fs_id);
    vfmw_check_ret(logic_fs != TD_NULL, H264_ERR);

    h264_set_less(logic_fs->image.user_tag, curr_pic->fs_image.user_tag);
    h264_set_less(logic_fs->image.disp_time, curr_pic->fs_image.disp_time);
    h264_set_less(logic_fs->image.disp_enable_flag, curr_pic->fs_image.disp_enable_flag);
    h264_set_less(logic_fs->image.disp_frame_distance, curr_pic->fs_image.disp_frame_distance);
    h264_set_less(logic_fs->image.distance_before_first_frame, curr_pic->fs_image.distance_before_first_frame);
    h264_set_less(logic_fs->image.gop_num, curr_pic->fs_image.gop_num);

    if ((curr_pic->is_long_term == 1) || (curr_pic->is_short_term == 1)) {
        ctx->dpb.fs[pos]->is_reference |= 2; /* 2 btm fld */
    }

    if (curr_pic->is_long_term == 1) {
        ctx->dpb.fs[pos]->long_term_frame_idx = curr_pic->long_term_frame_idx;
    }

    ctx->dpb.fs[pos]->pic_type =
        vfmw_max(ctx->dpb.fs[pos]->top_field.pic_type, ctx->dpb.fs[pos]->bottom_field.pic_type);
    ctx->dpb.fs[pos]->poc = curr_pic->thispoc;
    combine_flds_to_frm(ctx->dpb.fs[pos]);

    /* modify apc poc */
    ctx->apc.poc[1][ctx->dpb.fs[pos]->apc_idc] = (td_u32)ctx->dpb.fs[pos]->bottom_field.poc;

    return H264_OK;
}

STATIC td_s32 insert_frm_in_dpb_bottom_field(h264_ctx *ctx, td_u32 pos, h264_cur_pic *curr_pic)
{
    td_s32 ret;

    curr_pic->frame_store->frame.structure = FIELD_PAIR;

    if ((curr_pic->field_pair_flag == 0) || (ctx->dpb.fs[pos] == (h264_frame_store *)TD_NULL)) {
        /* 未成对 */
        ret = insert_frm_in_dpb_btm_field_only(ctx, pos, curr_pic);
    } else {
        /* 成对，frame_store中的公共变量可以不再赋值，只赋值可能引起改变的 */
        ret = insert_frm_in_dpb_btm_field_both(ctx, pos, curr_pic);
    }

    return ret;
}
#endif

/*************************************************************************
*   函数原型 :  td_void insert_frm_in_dpb(h264_ctx ctx, td_u32 pos , h264_cur_pic *curr_pic)
*   功能描述 :  插入一幅图像到g_DPB
*************************************************************************/
STATIC td_s32 insert_frm_in_dpb(h264_ctx *ctx, td_u32 pos, h264_cur_pic *curr_pic)
{
    td_s32 ret;

    /* pts 赋值缺 */
    if (pos >= 16) { /* 16 dpb size */
        dprint(PRN_ERROR, "[%s %d], pos is %d out of range\n", __func__, __LINE__, pos);
        return H264_ERR;
    }

    switch (curr_pic->structure) {
        case FRAME:
            ret = insert_frm_in_dpb_frame(ctx, pos, curr_pic);
            if (ret != H264_OK) {
                dprint(PRN_ERROR, "line %d, get apc error, ret %d\n", __LINE__, ret);
                clear_dpb(ctx); /* APC_ERR */
                return H264_ERR;
            }
            break;
#ifdef H264_FIELD_SUPPORT
        case TOP_FIELD:
            ret = insert_frm_in_dpb_top_field(ctx, pos, curr_pic);
            if (ret != H264_OK) {
                dprint(PRN_ERROR, "line %d, get apc error, ret %d\n", __LINE__, ret);
                clear_dpb(ctx); /* APC_ERR */
                return H264_ERR;
            }
            break;

        case BOTTOM_FIELD:
            ret = insert_frm_in_dpb_bottom_field(ctx, pos, curr_pic);
            if (ret != H264_OK) {
                dprint(PRN_ERROR, "line %d, get apc error, ret %d\n", __LINE__, ret);
                clear_dpb(ctx); /* APC_ERR */
                return H264_ERR;
            }
            break;
#endif
        default:
            /* 在出错的情况下，为了不改变DPB状况，只将当前图像置为空， */
            dprint(PRN_ERROR, "InserFrmInDPB: cur pic struct = %d!\n", curr_pic->structure);
            clear_all(ctx, 1);
            return H264_OK;
    }

    /* 逻辑帧标记为待参考 */
    /* 当遇到H264低延时的时候，会出现后级模块VPSS在拿帧后很快就还帧的现象，
     当多参考帧的时候，原来的DPB管理会出现某帧被重入的情况，导致画面卡顿(异常丢帧)，因此在此都插入DPB时都标志"被参考",
     在后续RemoveUnUsedFrameStore中再根据真正是否被参考信息is_reference 来设置会非参考! */
    fsp_set_ref(ctx->chan_id, ctx->dpb.fs[pos]->logic_fs_id, 1);

    if (curr_pic->pic_output_flag != 1) {
        ctx->dpb.fs[pos]->is_displayed = 1;
        fsp_set_disp(ctx->chan_id, ctx->dpb.fs[pos]->logic_fs_id, 0);
    }

    return H264_OK;
}

STATIC td_s32 direct_output_frame(h264_ctx *ctx)
{
    td_s32 ret;

    vfmw_check_ret(ctx->cur_pic.frame_store != TD_NULL, H264_ERR);
    ctx->cur_pic.frame_store->frame.structure = FRAME;

    /* 先输出directbuf，再把正在解码的图像存入directbuf */
    if (ctx->direct_out_buf != (h264_frame_store *)TD_NULL) {
        ret = output_frm_to_vo(ctx, ctx->direct_out_buf, 0);
        if (ret != H264_OK) {
        }

        if (ctx->direct_out_buf != TD_NULL) {
            fsp_clear_logic(ctx->chan_id, ctx->cur_pic.frame_store->logic_fs_id, 1);
        }

        ctx->direct_out_buf = TD_NULL;
    }

    ctx->direct_out_buf = ctx->cur_pic.frame_store;
    if (ctx->direct_out_buf != TD_NULL) {
        ctx->direct_out_buf->frame.frame_store = ctx->cur_pic.frame_store;
        ctx->direct_out_buf->top_field.frame_store = ctx->cur_pic.frame_store;
        ctx->direct_out_buf->bottom_field.frame_store = ctx->cur_pic.frame_store;

        ctx->direct_out_buf->non_existing = ctx->cur_pic.non_existing;
        ctx->direct_out_buf->is_used = 3; /* 3 both fld */
        ctx->direct_out_buf->is_reference = 0;
        ctx->direct_out_buf->is_in_dpb = 0;
        ctx->direct_out_buf->is_displayed = 0;
        ctx->direct_out_buf->logic_fs_id = ctx->cur_pic.frame_store->logic_fs_id;
        ctx->direct_out_buf->err_level = ctx->direct_out_buf->frame.err_level = ctx->cur_pic.err_level;
        ctx->direct_out_buf->frame.structure = FRAME;
        ctx->direct_out_buf->pic_type = ctx->cur_pic.pic_type;
        ctx->direct_out_buf->mbaff_frame_flag = ctx->cur_pic.mbaff_frame_flag;
        ctx->direct_out_buf->mode_skip_flag = (ctx->cur_pic.mode_skip_flag == 1) ? 3 : 0; /* 3 both fld */

        fsp_set_ref(ctx->chan_id, ctx->direct_out_buf->logic_fs_id, 0); /* USE_FSP */
        ret = output_frm_to_vo(ctx, ctx->direct_out_buf, 0);

        fsp_clear_logic(ctx->chan_id, ctx->cur_pic.frame_store->logic_fs_id, 1);
    }

    ctx->direct_out_buf = TD_NULL;

    return ret;
}

#ifdef H264_FIELD_SUPPORT
STATIC td_s32 direct_output_top_field_not_pair(h264_ctx *ctx)
{
    td_s32 ret = H264_OK;

    if (ctx->direct_out_buf != (h264_frame_store *)TD_NULL) {
        /* 之前存在一场，需要输出 */
        ret = output_frm_to_vo(ctx, ctx->direct_out_buf, 0);
        if (ret != H264_OK) {
        }

        ctx->direct_out_buf = ctx->cur_pic.frame_store;
        ctx->direct_out_buf->top_field.frame_store = ctx->cur_pic.frame_store;
    } else {
        ctx->direct_out_buf = ctx->cur_pic.frame_store;
        ctx->direct_out_buf->top_field.frame_store = ctx->cur_pic.frame_store;
    }

    ctx->direct_out_buf->non_existing = ctx->cur_pic.non_existing; /* must be 0 */
    ctx->direct_out_buf->is_used = 1;
    ctx->direct_out_buf->is_in_dpb = 0;
    ctx->direct_out_buf->is_displayed = 0;
    ctx->direct_out_buf->is_reference = 0;
    ctx->direct_out_buf->pic_type = ctx->cur_pic.pic_type;
    ctx->direct_out_buf->mbaff_frame_flag = 0;
    ctx->direct_out_buf->logic_fs_id = ctx->cur_pic.frame_store->logic_fs_id;
    ctx->direct_out_buf->top_field.err_level = ctx->cur_pic.err_level;
    ctx->direct_out_buf->err_level = ctx->direct_out_buf->frame.err_level = ctx->direct_out_buf->top_field.err_level;
    ctx->direct_out_buf->top_field.structure = TOP_FIELD;
    ctx->direct_out_buf->mode_skip_flag = (ctx->cur_pic.mode_skip_flag == 1) ? 1 : 0;

    if (ctx->old_dec_mode == VFMW_DEC_MODE_I) {
        fsp_set_ref(ctx->chan_id, ctx->direct_out_buf->logic_fs_id, 0); /* USE_FSP */
        ret = output_frm_to_vo(ctx, ctx->direct_out_buf, 0);

        if (ctx->direct_out_buf != TD_NULL) {
            fsp_clear_logic(ctx->chan_id, ctx->cur_pic.frame_store->logic_fs_id, 1);
        }

        ctx->direct_out_buf = TD_NULL;
    }

    return ret;
}

STATIC td_s32 direct_output_top_field_pair(h264_ctx *ctx)
{
    td_s32 ret;
    fsp_logic *logic_fs = TD_NULL;

    if (ctx->direct_out_buf == (h264_frame_store *)TD_NULL) {
        ctx->direct_out_buf = ctx->cur_pic.frame_store;
        ctx->direct_out_buf->top_field.frame_store = ctx->cur_pic.frame_store;
    }

    ctx->direct_out_buf->logic_fs_id = ctx->cur_pic.frame_store->logic_fs_id;
    logic_fs = fsp_get_logic(ctx->chan_id, ctx->direct_out_buf->logic_fs_id);
    vfmw_check_ret(logic_fs != TD_NULL, H264_ERR);

    /* 20111212: 两场成对时，需将其中有效的场 pts 赋给 成对的 frame */
    h264_set_less(logic_fs->image.user_tag, ctx->cur_pic.fs_image.user_tag);
    h264_set_less(logic_fs->image.disp_time, ctx->cur_pic.fs_image.disp_time);
    h264_set_less(logic_fs->image.disp_enable_flag, ctx->cur_pic.fs_image.disp_enable_flag);
    h264_set_less(logic_fs->image.disp_frame_distance, ctx->cur_pic.fs_image.disp_frame_distance);
    h264_set_less(logic_fs->image.distance_before_first_frame, ctx->cur_pic.fs_image.distance_before_first_frame);
    h264_set_less(logic_fs->image.gop_num, ctx->cur_pic.fs_image.gop_num);

    ctx->direct_out_buf->non_existing = (ctx->direct_out_buf->non_existing != 0) ? ctx->cur_pic.non_existing : 0;
    ctx->direct_out_buf->is_used |= 1;
    ctx->direct_out_buf->pic_type = vfmw_max(ctx->cur_pic.pic_type, ctx->direct_out_buf->pic_type);
    ctx->direct_out_buf->top_field.structure = TOP_FIELD;
    ctx->direct_out_buf->frame.structure = FIELD_PAIR; /*   field pair more important  */

    ctx->direct_out_buf->top_field.err_level = ctx->cur_pic.err_level;
    ctx->direct_out_buf->frame.err_level = (ctx->direct_out_buf->top_field.err_level +
        ctx->direct_out_buf->bottom_field.err_level) / 2; /* 2 half */
    ctx->direct_out_buf->err_level = ctx->direct_out_buf->frame.err_level;

    ctx->direct_out_buf->frame.frame_store = ctx->direct_out_buf->top_field.frame_store;
    ctx->direct_out_buf->mode_skip_flag = (ctx->cur_pic.mode_skip_flag == 1) ?
        (ctx->direct_out_buf->mode_skip_flag | 1) : ctx->direct_out_buf->mode_skip_flag;
    fsp_set_ref(ctx->chan_id, ctx->direct_out_buf->logic_fs_id, 0); /*   USE_FSP  */
    ret = output_frm_to_vo(ctx, ctx->direct_out_buf, 0);

    if (ctx->direct_out_buf != TD_NULL) {
        fsp_clear_logic(ctx->chan_id, ctx->cur_pic.frame_store->logic_fs_id, 1);
    }

    ctx->direct_out_buf = TD_NULL;

    return ret;
}

STATIC td_s32 direct_output_top_field(h264_ctx *ctx)
{
    td_s32 ret;

    ctx->cur_pic.frame_store->frame.structure = FIELD_PAIR;

    if (ctx->cur_pic.field_pair_flag == 0) {
        /* 不成对 */
        ret = direct_output_top_field_not_pair(ctx);
    } else {
        /* 成对，相信pDirectOutBuf一定不空 */
        ret = direct_output_top_field_pair(ctx);
    }

    return ret;
}

STATIC td_s32 direct_output_bottom_field_not_pair(h264_ctx *ctx)
{
    td_s32 ret = H264_OK;

    if (ctx->direct_out_buf != (h264_frame_store *)TD_NULL) {
        /* 之前存在一场，需要输出 */
        ret = output_frm_to_vo(ctx, ctx->direct_out_buf, 0);
        if (ret != H264_OK) {
        }

        ctx->direct_out_buf = ctx->cur_pic.frame_store;
        ctx->direct_out_buf->bottom_field.frame_store = ctx->cur_pic.frame_store;
    } else {
        ctx->direct_out_buf = ctx->cur_pic.frame_store;
        ctx->direct_out_buf->bottom_field.frame_store = ctx->cur_pic.frame_store;
    }

    ctx->direct_out_buf->non_existing = ctx->cur_pic.non_existing; /* must be 0 */
    ctx->direct_out_buf->is_used = 2; /* 2 btm used */
    ctx->direct_out_buf->is_in_dpb = 0;
    ctx->direct_out_buf->is_displayed = 0;
    ctx->direct_out_buf->is_reference = 0;
    ctx->direct_out_buf->pic_type = ctx->cur_pic.pic_type;
    ctx->direct_out_buf->mbaff_frame_flag = 0;
    ctx->direct_out_buf->logic_fs_id = ctx->cur_pic.frame_store->logic_fs_id;

    ctx->direct_out_buf->bottom_field.err_level = ctx->cur_pic.err_level;
    ctx->direct_out_buf->err_level = ctx->direct_out_buf->frame.err_level =
                                         ctx->direct_out_buf->bottom_field.err_level;

    ctx->direct_out_buf->bottom_field.structure = BOTTOM_FIELD;
    ctx->direct_out_buf->mode_skip_flag = (ctx->cur_pic.mode_skip_flag == 1) ? 2 : 0; /* 2 btm fld */

    if (ctx->old_dec_mode == VFMW_DEC_MODE_I) {
        fsp_set_ref(ctx->chan_id, ctx->direct_out_buf->logic_fs_id, 0); /* USE_FSP */
        ret = output_frm_to_vo(ctx, ctx->direct_out_buf, 0);

        if (ctx->direct_out_buf != TD_NULL) {
            fsp_clear_logic(ctx->chan_id, ctx->cur_pic.frame_store->logic_fs_id, 1);
        }

        ctx->direct_out_buf = TD_NULL;
    }

    return ret;
}

STATIC td_s32 direct_output_bottom_field_pair(h264_ctx *ctx)
{
    td_s32 ret;
    fsp_logic *logic_fs = TD_NULL;

    if (ctx->direct_out_buf == (h264_frame_store *)TD_NULL) {
        ctx->direct_out_buf = ctx->cur_pic.frame_store;
        ctx->direct_out_buf->bottom_field.frame_store = ctx->cur_pic.frame_store;
    }

    ctx->direct_out_buf->logic_fs_id = ctx->cur_pic.frame_store->logic_fs_id;
    logic_fs = fsp_get_logic(ctx->chan_id, ctx->direct_out_buf->logic_fs_id);
    vfmw_check_ret(logic_fs != TD_NULL, H264_ERR);

    /* 20111212: 两场成对时，需将其中有效的场 pts 赋给 成对的 frame */
    h264_set_less(logic_fs->image.user_tag, ctx->cur_pic.fs_image.user_tag);
    h264_set_less(logic_fs->image.disp_time, ctx->cur_pic.fs_image.disp_time);
    h264_set_less(logic_fs->image.disp_enable_flag, ctx->cur_pic.fs_image.disp_enable_flag);
    h264_set_less(logic_fs->image.disp_frame_distance, ctx->cur_pic.fs_image.disp_frame_distance);
    h264_set_less(logic_fs->image.distance_before_first_frame, ctx->cur_pic.fs_image.distance_before_first_frame);
    h264_set_less(logic_fs->image.gop_num, ctx->cur_pic.fs_image.gop_num);

    ctx->direct_out_buf->non_existing = (ctx->direct_out_buf->non_existing != 0) ? ctx->cur_pic.non_existing : 0;
    ctx->direct_out_buf->is_used |= 2; /* 2 btm used */
    ctx->direct_out_buf->pic_type = vfmw_max(ctx->cur_pic.pic_type, ctx->direct_out_buf->pic_type);
    ctx->direct_out_buf->bottom_field.structure = BOTTOM_FIELD;
    ctx->direct_out_buf->frame.structure = FIELD_PAIR; /* field pair more important */

    ctx->direct_out_buf->bottom_field.err_level = ctx->cur_pic.err_level;
    ctx->direct_out_buf->frame.err_level = (ctx->direct_out_buf->top_field.err_level +
                                            ctx->direct_out_buf->bottom_field.err_level) / 2; /* 2 half */
    ctx->direct_out_buf->err_level = ctx->direct_out_buf->frame.err_level;

    ctx->direct_out_buf->frame.frame_store = ctx->direct_out_buf->bottom_field.frame_store;
    ctx->direct_out_buf->mode_skip_flag = (ctx->cur_pic.mode_skip_flag == 1) ?
        (ctx->direct_out_buf->mode_skip_flag | 2) : ctx->direct_out_buf->mode_skip_flag; /* 2 btm fld */
    fsp_set_ref(ctx->chan_id, ctx->direct_out_buf->logic_fs_id, 0); /*   USE_FSP  */
    ret = output_frm_to_vo(ctx, ctx->direct_out_buf, 0);

    if (ctx->direct_out_buf != TD_NULL) {
        fsp_clear_logic(ctx->chan_id, ctx->cur_pic.frame_store->logic_fs_id, 1);
    }

    ctx->direct_out_buf = TD_NULL;

    return ret;
}

STATIC td_s32 direct_output_bottom_field(h264_ctx *ctx)
{
    td_s32 ret;

    ctx->cur_pic.frame_store->frame.structure = FIELD_PAIR;

    if (ctx->cur_pic.field_pair_flag == 0) {
        /* 不成对 */
        ret = direct_output_bottom_field_not_pair(ctx);
    } else {
        /* 成对，相信pDirectOutBuf一定不空 */
        ret = direct_output_bottom_field_pair(ctx);
    }

    return ret;
}
#endif

/*************************************************************************
*      函数原型 :  td_s32 direct_out_put(h264_ctx *ctx)
*      功能描述 :  当前解码图像不作参考，且其poc比DPB中所有没有输出的图像
*                  的poc都小
*      参数说明 :  无
*      返回值   ： DEC_NORMAL:正常处理；
*                  OUT_FLAG:下层函数可能返回该值，系统要求退出。
*
*************************************************************************/
STATIC td_s32 direct_out_put(h264_ctx *ctx)
{
    td_s32 ret;

    ret = H264_OK;

    if (ctx->cur_pic.pic_output_flag != 1) {
        fsp_set_disp(ctx->chan_id, ctx->cur_pic.frame_store->logic_fs_id, 0);
        return H264_OK;
    }

    switch (ctx->cur_pic.structure) {
        case FRAME:
            ret = direct_output_frame(ctx);
            break;
#ifdef H264_FIELD_SUPPORT
        case TOP_FIELD:
            ret = direct_output_top_field(ctx);
            break;

        case BOTTOM_FIELD:
            ret = direct_output_bottom_field(ctx);
            break;
#endif
        default:
            break;
    }

    return ret;
}

/*************************************************************************
*      函数原型 :      td_s32 H264DEC_FlushDPB(h264_ctx *ctx)
*      功能描述 :      全部输出g_DPB内先前图像
*************************************************************************/
STATIC td_s32 flush_dpb(h264_ctx *ctx)
{
    td_u32 i;
    td_s32 ret;
    td_u32 pos;
    td_s32 min_poc;

    /* mark all frames unused */
    for (i = 0; i < ctx->dpb.used_size; i++) {
        if (ctx->dpb.fs[i]->is_reference != 0) {
            unmark_frame_store_ref(ctx, ctx->dpb.fs[i]);
        }
    }

    while (remove_unused_frame_store(ctx) == 1) {
    }

    /* output frames in POC order */
    while (ctx->dpb.used_size != 0) {
        get_min_poc(ctx, &min_poc, &pos);
        ret = output_frm_from_dpb(ctx, pos);
        if (ret == H264_ERR) {
            return H264_ERR;
        }
    }

    return H264_OK;
}

/*************************************************************************
*      函数原型 :      td_s32 H264DEC_FlushDPB(h264_ctx *ctx)
*      功能描述 :      全部输出g_DPB内先前图像, 用于输出最后一帧
*************************************************************************/
STATIC td_s32 h264_dec_get_remain_img(h264_ctx *ctx)
{
    td_u32 i;
    td_s32 ret;
    td_u32 pos;
    td_s32 min_poc;

    ret = LAST_OUTPUT_FAIL;
    vfmw_check_ret(ctx != TD_NULL, LAST_OUTPUT_FAIL);

    /* mark all frames unused */
    for (i = 0; i < ctx->dpb.used_size; i++) {
        if (ctx->dpb.fs[i]->is_reference != 0) {
            unmark_frame_store_ref(ctx, ctx->dpb.fs[i]);
        }
    }

    while (remove_unused_frame_store(ctx) == 1) {
    }

    /* output frames in POC order */
    while (ctx->dpb.used_size != 0) {
        get_min_poc(ctx, &min_poc, &pos);

        ctx->dpb.fs[pos]->is_in_dpb = 0; /* here, output g_DPB to display */
        ret = output_frm_to_vo(ctx, ctx->dpb.fs[pos], ctx->dpb.used_size);

        /* free frame store and move empty store to end of buffer */
        if (frame_store_ref_flg(ctx->dpb.fs[pos]) == 0) {
            remove_frame_store_out_dpb(ctx, pos);
        }
    }

    return ret;
}

/*************************************************************************
*   函数原型 :  td_void sliding_win_mark(td_void)
*   功能描述 :  滑动窗标记
*   参数说明 :  无
*   返回值   ： 无
*************************************************************************/
STATIC td_void sliding_win_mark(h264_ctx *ctx)
{
    td_u32 i;

    /* if this is a reference pic with sliding sliding window, unmark first ref frame */
    if (ctx->dpb.ref_frames_in_buffer == ctx->cur_sps.num_ref_frames - ctx->dpb.ltref_frames_in_buffer) {
        for (i = 0; i < ctx->dpb.used_size; i++) {
            if ((ctx->dpb.fs[i]->is_reference != 0) && short_term_ref_flg(ctx->dpb.fs[i]) == 1) {
                unmark_frame_store_ref(ctx, ctx->dpb.fs[i]);
                update_ref_list(ctx);
                break;
            }
        }
    }

    return;
}

STATIC td_void adaptive_mem_mark_by_mmco(h264_ctx *ctx)
{
    td_u32 i;
    td_u32 mmco_end_flag = 0;
    h264_mmco *mmco_para = ctx->cur_pic.mark_para->mmco;

    /* mmco operate */
    for (i = 0; i < MAX_MMCO_LEN; i++) {
        switch (mmco_para[i].memory_management_control_operation) {
            case 0: /* 0 mark mode */
                mmco_end_flag = 1;
                break;

            case 1: /* 1 mark mode */
                unmark_st_ref(ctx, &ctx->cur_pic, (td_s32)mmco_para[i].difference_of_pic_nums_minus1);
                update_ref_list(ctx);
                break;

            case 2: /* 2 mark mode */
                unmark_lt_ref(ctx, &ctx->cur_pic, (td_s32)mmco_para[i].long_term_pic_num);
                update_lt_ref_list(ctx);
                break;

            case 3: /* 3 mark mode */
                mark_st_to_lt_ref(ctx, &ctx->cur_pic, mmco_para[i].difference_of_pic_nums_minus1,
                                  mmco_para[i].long_term_frame_idx);
                update_ref_list(ctx);
                update_lt_ref_list(ctx);
                break;

            case 4: /* 4 mark mode */
                update_max_lt_frm_idx(ctx, mmco_para[i].max_long_term_frame_idx_plus1);
                update_lt_ref_list(ctx);
                break;

            case 5: /* 5 mark mode */
                unmark_all_st_ref(ctx);
                update_ref_list(ctx);
                update_max_lt_frm_idx(ctx, 0);
                update_lt_ref_list(ctx);
                ctx->cur_poc.last_has_mmco_5 = 1;
                break;

            case 6: /* 6 mark mode */
                mark_curr_pic_lt(ctx, &ctx->cur_pic, mmco_para[i].long_term_frame_idx);
                break;

            default:
                break;
        }

        if (mmco_end_flag == 1) {
            break;
        }
    }
}

/*************************************************************************
*      函数原型 :  td_s32 adaptive_mem_mark(h264_ctx *ctx)
*      功能描述 :  根据当前图像语法元素进行mmco操作
*      参数说明 :  无
*      返回值   ： DEC_NORMAL: 配成功;
*                  OUT_FLAG: 检测到适配层需要退出，返回该标志
*************************************************************************/
STATIC td_s32 adaptive_mem_mark(h264_ctx *ctx)
{
    td_s32 ret;

    adaptive_mem_mark_by_mmco(ctx);

    /* post process of last_has_mmco_5 */
    if (ctx->cur_poc.last_has_mmco_5 == 1) {
        /* updata frame_num and pic_num of cur_pic */
        ctx->cur_pic.frame_num = 0;

        switch (ctx->cur_pic.structure) {
#ifdef H264_FIELD_SUPPORT
            case TOP_FIELD:
                ctx->cur_pic.thispoc = ctx->cur_pic.toppoc = 0;
                ctx->cur_poc.this_poc = ctx->cur_poc.toppoc = 0;
                break;

            case BOTTOM_FIELD:
                ctx->cur_pic.thispoc = ctx->cur_pic.bottompoc = 0;
                ctx->cur_poc.this_poc = ctx->cur_poc.bottompoc = 0;
                break;
#endif
            case FRAME:
                ctx->cur_pic.toppoc -= ctx->cur_pic.thispoc;
                ctx->cur_pic.bottompoc -= ctx->cur_pic.thispoc;
                ctx->cur_pic.thispoc = ctx->cur_pic.framepoc = vfmw_min(ctx->cur_pic.toppoc, ctx->cur_pic.bottompoc);
                ctx->cur_poc.toppoc -= ctx->cur_poc.this_poc;
                ctx->cur_poc.bottompoc -= ctx->cur_poc.this_poc;
                ctx->cur_poc.this_poc = ctx->cur_poc.framepoc = vfmw_min(ctx->cur_poc.toppoc, ctx->cur_poc.bottompoc);
                break;

            default:
                break;
        }

        ret = flush_dpb(ctx);
        if (ret != H264_OK) {
            return H264_ERR;
        }
    }

    return H264_OK;
}

/*************************************************************************
*      函数原型 :  td_s32 idr_mem_marking(h264_ctx *ctx)
*      功能描述 :  当前图像为IDR图像，根据语法对DPB中图像进行标记
*      参数说明 :  无
*      返回值   ： DEC_NORMAL: 配成功;
*                  OUT_FLAG: 检测到适配层需要退出，返回该标志
*************************************************************************/
STATIC td_s32 idr_mem_marking(h264_ctx *ctx)
{
    td_u32 i;
    td_s32 ret;

    /* 所有对之前图像的处理过程中，如果IDR图像是第二场时，怎么避免冲突？ */
    if (ctx->cur_pic.mark_para->no_output_of_prior_pics_flag != 0) {
        for (i = 0; i < ctx->dpb.used_size; i++) {
            /* 当标记为IDR且当前framestore eFoState 为 快速输出时，应当置 idr_found
               否则该framestore清除掉后，永远也无法置IDRFound 20150615
            */
            if (ctx->dpb.fs[i]->fo_state == FO_STATE_FAST_OUTPUT) {
                ctx->idr_found = 1;
            }

            /* mark 为不可用,便于vo显示已经存在于vo buffer中的图像时可以释放图像槽位 */
            ctx->dpb.fs[i]->frame.is_long_term = ctx->dpb.fs[i]->frame.is_short_term = 0;
            ctx->dpb.fs[i]->top_field.is_long_term = ctx->dpb.fs[i]->top_field.is_short_term = 0;
            ctx->dpb.fs[i]->bottom_field.is_long_term = ctx->dpb.fs[i]->bottom_field.is_short_term = 0;
            ctx->dpb.fs[i]->is_reference = 0;
            fsp_set_ref(ctx->chan_id, ctx->dpb.fs[i]->logic_fs_id, 0);

            /* 此时三种情况: */
            /* 1: 图像未输出g_DPB,则不需再输出,且可以分给后续解码 */
            /* 2: 图像输出到vo,且已经显示,只需设其为空 */
            /* 3: 图像输出到vo,但还没显示,该图像的释放由vo完成 */
            ctx->dpb.fs[i]->is_used = 0;
            ctx->dpb.fs[i]->is_in_dpb = 0;
            ctx->dpb.fs[i]->is_displayed = 1;
            /* CNcomment: 这个帧存都释放掉了，它对应的APC也要释放掉 */
            vfmw_check_ret(ctx->dpb.fs[i]->apc_idc < 16, H264_ERR); /* 16 array size */
            ctx->apc.is_used[ctx->dpb.fs[i]->apc_idc] = 0;

            if (fsp_get_disp(ctx->chan_id, ctx->dpb.fs[i]->logic_fs_id) != FSP_DISP_STATE_WAIT) {
                fsp_set_disp(ctx->chan_id, ctx->dpb.fs[i]->logic_fs_id, 0);
            }

            ctx->dpb.fs[i] = (h264_frame_store *)TD_NULL;
            ctx->dpb.fs_ref[i] = (h264_frame_store *)TD_NULL;
            ctx->dpb.fs_ltref[i] = (h264_frame_store *)TD_NULL;
        }

        ctx->dpb.used_size = 0;

        for (i = 0; i < ctx->total_pmv_num; i++) {
            ctx->pmv_store_used_flag[i] = 0; /* 特别注意，不要破坏了当前图像的槽位占用关系 */
        }
    } else {
        ret = flush_dpb(ctx);
        if (ret != H264_OK) {
            return H264_ERR;
        }
    }

    update_ref_list(ctx);
    update_lt_ref_list(ctx);

    if (ctx->cur_pic.mark_para->long_term_reference_flag != 0) {
        ctx->dpb.max_long_term_pic_idx = 1;
        ctx->cur_pic.long_term_frame_idx = 0;
        ctx->cur_pic.is_long_term = 1;
        ctx->cur_pic.is_short_term = 0;
    } else {
        ctx->dpb.max_long_term_pic_idx = 0; /* 与上面的路径不同，此处无实际意义 */
        ctx->cur_pic.is_long_term = 0;
        ctx->cur_pic.is_short_term = 1;
    }

    return H264_OK;
}

STATIC td_void h264_sliding_marking(h264_ctx *ctx)
{
    /* sliding win mark */
    if ((ctx->cur_pic.nal_unit_type != NALU_TYPE_IDR) && (ctx->cur_pic.nal_ref_idc != 0) &&
        (ctx->cur_pic.mark_para->adaptive_ref_pic_marking_mode_flag == 0) && (ctx->cur_pic.field_pair_flag == 0)) {
        sliding_win_mark(ctx);
        ctx->cur_pic.is_long_term = 0;
    }
}

/*************************************************************************
*      函数原型 :  td_s32 marking(h264_ctx *ctx)
*      功能描述 :  根据当前图像的mark参数对DPB中图像进行标记；
*      参数说明 :  无
*      返回值   ： DEC_NORMAL: 配成功;
*                  OUT_FLAG: 检测到适配层需要退出，返回该标志
*************************************************************************/
STATIC td_s32 marking(h264_ctx *ctx)
{
    td_s32 ret;

    ctx->cur_poc.last_has_mmco_5 = 0;
    ctx->cur_poc.last_pic_bottom_field = (td_s32)(ctx->cur_pic.structure == BOTTOM_FIELD);

    /* IDR mark and adaptive mmco mark */
    if ((ctx->cur_pic.nal_unit_type == NALU_TYPE_IDR) ||
        ((ctx->cur_pic.pic_type == 0) &&
        (((ctx->extra_data->pvr_info.b_frm_ref_flag == 1) &&
        (ctx->extra_data->pvr_info.idr_flag == 0) &&
        (ctx->extra_data->pvr_info.continuous_flag == 0) &&
        (!((ctx->extra_data->fast_forward_backward_speed >= -1024) && /* 1024 speed */
        (ctx->extra_data->fast_forward_backward_speed <= 1024))))))) { /* 1024 speed */
        ret = idr_mem_marking(ctx);
        if (ret != H264_OK) {
            return H264_ERR;
        }
    } else if ((ctx->cur_pic.mark_para->adaptive_ref_pic_marking_mode_flag != 0) && (ctx->cur_pic.nal_ref_idc != 0)) {
        /* adaptive memory management */
        ret = adaptive_mem_mark(ctx);
        if (ret != H264_OK) {
            return H264_ERR;
        }
    }

    h264_sliding_marking(ctx);

    /* mark cur_pic */
    if (ctx->cur_pic.nal_ref_idc != 0) {
        ctx->cur_pic.is_short_term = (ctx->cur_pic.is_long_term == 0);
    }

    return H264_OK;
}

/*****************************************************************************
    简化的滑窗dpb管理策略.
        当dec_order_output == DEC_ORDER_SIMPLE_DPB时使用此策略.
        在视频电话等领域中途不发I帧. 这样如果发生了误码，解码器不能清除参考图.
    但是作为通用解码器这样非常危险，所以在这种应用中先把DPB管理机制进行简化，
    而通用码流的处理则沿用标准策略。
 *****************************************************************************/
STATIC td_void simple_slide_dpb(h264_ctx *ctx)
{
    if (ctx->dpb.used_size >= ctx->dpb.size) {
        /* 把第0个帧存标识为不参考，并从DPB中删除这个帧 */
        unmark_frame_store_ref(ctx, ctx->dpb.fs[0]);
        remove_frame_store_out_dpb(ctx, 0);
    }

    ctx->cur_pic.is_long_term = 0;

    /* mark cur_pic */
    if (ctx->cur_pic.nal_ref_idc != 0) {
        ctx->cur_pic.is_short_term = (ctx->cur_pic.is_long_term == 0);
    }
}

STATIC td_s32 imode_process(h264_ctx *ctx)
{
    td_s32 ret = H264_OK;

    vfmw_check_ret(ctx != TD_NULL, H264_ERR);
    vfmw_check_ret(ctx->cur_pic.frame_store != TD_NULL, H264_ERR);

    if (ctx->cur_pic.pic_output_flag == 0) {
        fsp_set_disp(ctx->chan_id, ctx->cur_pic.frame_store->logic_fs_id, 0);
    } else {
        ret = direct_out_put(ctx);

        /* pmv槽位设置成缺省值 */
        if (ctx->cur_pic.pmv_address_idc != ctx->total_pmv_num) {
            ctx->pmv_store_used_flag[ctx->cur_pic.pmv_address_idc] = 0; /* pmv store */
            ctx->cur_pic.pmv_address_idc = ctx->total_pmv_num;
        }

        if (ret == H264_ALREADY_OUT) {
            ctx->cur_pic.frame_store->is_in_dpb = 0;
            ctx->cur_pic.frame_store->is_used = 0;
        }
    }

    return ret;
}

STATIC td_s32 dec_order_process_get_para(h264_ctx *ctx, h264_cur_pic **pic, h264_frame_store **frame)
{
    td_u32 one_fld_flg;
    vfmw_image *img = TD_NULL;
    h264_cur_pic *out_pic = TD_NULL;
    h264_frame_store *out_frame_store = TD_NULL;

    one_fld_flg = (ctx->cur_pic.structure == TOP_FIELD) || (ctx->cur_pic.structure == BOTTOM_FIELD);
    if ((one_fld_flg != 0) && (ctx->first_frame_displayed == FIRST_DISP_UNPAIRED)) {
        out_pic = &ctx->cur_pic;
        out_frame_store = ctx->last_pic.frame_store;
        vfmw_check_ret(out_frame_store != TD_NULL, H264_ERR);
        /* 第一帧快速输出第2场POC还未更新就输出导致顶低场优先标志出错 */
        if (ctx->cur_pic.structure == TOP_FIELD) {
            out_frame_store->top_field.poc = ctx->cur_pic.toppoc;
        } else if (ctx->cur_pic.structure == BOTTOM_FIELD) {
            out_frame_store->bottom_field.poc = ctx->cur_pic.bottompoc;
        }
    } else {
        out_pic = &ctx->cur_pic;
        out_frame_store = ctx->cur_pic.frame_store;
    }

    vfmw_check_ret(out_frame_store != TD_NULL, H264_ERR);

    /** 由于error的处理在后面才做，所以这里必须临时计算一下.
        后续会改成UpdateCurrFrameInfo()的方式处理，届时如下if(){}需要删掉 */
    if (out_pic->structure != FRAME && out_pic->field_pair_flag == 1) {
        out_frame_store->is_used = 3; /* 3 both */
        out_frame_store->err_level += out_pic->err_level;
    } else {
        out_frame_store->err_level = out_pic->err_level;
    }

    get_image_para(ctx, out_frame_store);
    img = fsp_get_image(ctx->chan_id, out_frame_store->logic_fs_id);
    vfmw_check_ret(img != TD_NULL, H264_ERR);

    if (out_pic->structure != FRAME && out_pic->field_pair_flag == 1) {
        if (out_pic->fs_image.pts == VFMW_INVALID_PTS && ctx->last_pic.fs_image.pts != VFMW_INVALID_PTS) {
            img->pts = ctx->last_pic.fs_image.pts;
        }
    } else {
        img->pts = out_pic->fs_image.pts;
    }

    img->user_tag = out_pic->fs_image.user_tag;
    img->disp_time = out_pic->fs_image.disp_time;
    img->disp_enable_flag = out_pic->fs_image.disp_enable_flag;
    img->disp_frame_distance = out_pic->fs_image.disp_frame_distance;
    img->distance_before_first_frame = out_pic->fs_image.distance_before_first_frame;
    img->gop_num = out_pic->fs_image.gop_num;

    img->afd.active_format_flag = out_pic->fs_image.afd.active_format_flag;
    img->afd.active_format = out_pic->fs_image.afd.active_format;

    img->error_level = out_frame_store->err_level;

    *pic = out_pic;
    *frame = out_frame_store;

    return H264_OK;
}

STATIC td_s32 dec_order_process(h264_ctx *ctx)
{
    td_u8 flag;
    h264_frame_store *frame_store = TD_NULL;
    vfmw_image *img = TD_NULL;
    td_u32 one_fld_flg;

    frame_store = ctx->cur_pic.frame_store;
    vfmw_check_ret(frame_store != TD_NULL, H264_ERR);
    one_fld_flg = (ctx->cur_pic.structure == TOP_FIELD) || (ctx->cur_pic.structure == BOTTOM_FIELD);
    flag = ((ctx->cur_pic.structure == FRAME) || (frame_store->is_used > 0) || (one_fld_flg != 0));
    if (flag == 0) {
        return H264_OK;
    }

    if ((one_fld_flg != 0) && (ctx->first_frame_displayed == FIRST_DISP_INIT)) {
        ctx->first_frame_displayed = FIRST_DISP_UNPAIRED;
        ctx->last_pic = ctx->cur_pic;
    } else if (((one_fld_flg != 0) && (ctx->first_frame_displayed == FIRST_DISP_UNPAIRED)) ||
               (ctx->cur_pic.structure == FRAME) || (frame_store->is_used > 0)) {
        h264_cur_pic *out_pic = TD_NULL;
        h264_frame_store *out_frame_store = TD_NULL;

        if (dec_order_process_get_para(ctx, &out_pic, &out_frame_store) != H264_OK) {
            return H264_ERR;
        }

        img = fsp_get_image(ctx->chan_id, out_frame_store->logic_fs_id);
        vfmw_check_ret(img != TD_NULL, H264_ERR);

        /* 如果I帧有错误，上报一个事件通知应用层 */
        if (img->error_level > 0 && (img->format.frame_type == 0)) {
            dprint(PRN_ERROR, "line %d, REPORT_IFRAME_ERR\n", __LINE__);
        }

        flag = ((img->error_level == 0 || img->error_level <= (td_u32)(ctx->extra_data->out_err_thr)) &&
                (ctx->cur_pic.pic_output_flag == 1));
        if (flag != 0) {
            fsp_set_disp(ctx->chan_id, out_frame_store->logic_fs_id, 1);

            if (voq_insert(ctx->chan_id, img) != VOQ_OK) {
                fsp_set_disp(ctx->chan_id, out_frame_store->logic_fs_id, 0);
            } else {
                ctx->num_output_frame++;
            }

            set_frm_repeat_count(ctx, img);
            ctx->pic_out_counter += 2; /* 2 pic cnt */
        }

        if (ctx->first_frame_displayed != FIRST_DISP_OUTPUT) {
            /* 标记是快速输出帧，这种帧虽被输出，但在经历正常输出序后才能从DPB中删除，比较特殊 */
            out_frame_store->fo_state = FO_STATE_FAST_OUTPUT;
        }

        ctx->first_frame_displayed = FIRST_DISP_OUTPUT;
    }

    return H264_OK;
}

STATIC td_s32 simple_dpb_process(h264_ctx *ctx)
{
    td_s32 ret;

    if (ctx->cur_pic.nal_unit_type == NALU_TYPE_IDR) {
        ret = idr_mem_marking(ctx);
        if (ret != H264_OK) {
            return H264_ERR;
        }
    }

    simple_slide_dpb(ctx);

    /* 把当前图像存入DPB */
    ret = insert_frm_in_dpb(ctx, ctx->dpb.used_size, &ctx->cur_pic);
    if (ret != H264_OK) {
        dprint(PRN_ERROR, "line %d, return %d\n", __LINE__, ret);
        return H264_ERR;
    }

    ctx->dpb.used_size++;

    update_ref_list(ctx);
    update_lt_ref_list(ctx);

    return H264_OK;
}

STATIC td_s32 disp_order_process_mark(h264_ctx *ctx, td_bool *go_on)
{
    td_s32 ret;

    *go_on = TD_FALSE;

    /* 如果是参考帧，marking处理 */
    if (ctx->cur_pic.nal_ref_idc != 0) {
        /* all pic dec down, add ref_err_thr; if this pic will display, the code return IDR_LOST */
        if ((ctx->ref_error_thr < ctx->cur_pic.err_level) && (ctx->cur_pic.non_existing == 0)) {
            dprint(PRN_ERROR, "err(%d) > ref_thr(%d)\n", ctx->cur_pic.err_level, ctx->ref_error_thr);

            h264_clear_curr_pic(ctx);
            clear_dpb(ctx);
            return H264_ERR;
        }

        ret = marking(ctx);
        if (ret != H264_OK) {
            dprint(PRN_ERROR, "marking return %d\n", ret);
            return ret;
        }

        /* 2011-11-23, marking之后DPB中的参考帧数量会发生改变，所以需要及时更新ref_frames_in_buffer，ltref_frames_in_buffer. 否则
           这两个数还是marking之前的值，会给后续处理带来误判(比如触发稍后的"DPB满错误判断") */
        update_ref_list(ctx);
        update_lt_ref_list(ctx);

        /* 20111213, marking之后可能会产生'un-used'帧存，这些帧存要尽快从DPB中清除出去，因为此时很可能fsp已经清除了相应记录，
          还让这些帧存呆在DPB中遗患无穷 */
        remove_unused_frame_store(ctx);
    }

    *go_on = TD_TRUE;

    return H264_OK;
}

STATIC td_s32 disp_order_process_pair(h264_ctx *ctx, td_bool *go_on)
{
    td_s32 ret;
    td_u32 i;

    vfmw_check_ret(ctx->cur_pic.frame_store != TD_NULL, H264_ERR);

    *go_on = TD_FALSE;

    /* 当前图像输入 */
    if (ctx->cur_pic.field_pair_flag == 1) {
        if (ctx->cur_pic.frame_store == ctx->direct_out_buf &&
            ctx->direct_out_buf != (h264_frame_store *)TD_NULL) {
            /* first field in directoutput buffer */
            ret = direct_out_put(ctx);
            if (ret == H264_ERR) {
                dprint(PRN_ERROR, "line %d, return %d\n", __LINE__, ret);
            }

            if (ret == H264_ALREADY_OUT) {
                ctx->cur_pic.frame_store->is_in_dpb = 0;
                ctx->cur_pic.frame_store->is_used = 0;
            }

            return ret;
        }

        for (i = 0; i < ctx->dpb.used_size; i++) {
            if (ctx->cur_pic.frame_store == ctx->dpb.fs[i]) {
                /* first field already in g_DPB */
                ret = insert_frm_in_dpb(ctx, i, &(ctx->cur_pic));
                vfmw_check_ret(ret == H264_OK, H264_ERR);

                update_ref_list(ctx);
                update_lt_ref_list(ctx);
                return H264_OK;
            }
        }

        /* 如果成对场到处理流程能到这里，说明之前出了什么问题，应该丢弃当前图像，后续添加 */
    }

    *go_on = TD_TRUE;

    return H264_OK;
}

STATIC td_s32 disp_order_process_full(h264_ctx *ctx, td_bool *go_on)
{
    td_u32 pos;
    td_s32 min_poc;

    vfmw_check_ret(ctx->extra_data != TD_NULL, H264_ERR);
    *go_on = TD_FALSE;

    /* DPB满错误判断 */
    if ((ctx->cur_pic.nal_ref_idc != 0) &&
        (ctx->dpb.size <= (ctx->dpb.ref_frames_in_buffer + ctx->dpb.ltref_frames_in_buffer))) {
        if (ctx->extra_data->output_order == VFMW_DEC_ORDER_SKYPLAY) {
            get_min_poc(ctx, &min_poc, &pos);
            remove_frame_store_out_dpb(ctx, pos);
            update_ref_list(ctx);
            update_lt_ref_list(ctx);
        } else {
            /* 后续可以添加容错处理，比如按滑动窗再mark一次 */
            dprint(PRN_ERROR, "line %d: ctx->dpb.size = %d, ref %d, ltref %d\n", __LINE__, ctx->dpb.size,
                   ctx->dpb.ref_frames_in_buffer, ctx->dpb.ltref_frames_in_buffer);
            clear_dpb(ctx);
            return H264_ERR;
        }
    }

    *go_on = TD_TRUE;

    return H264_OK;
}

STATIC td_s32 disp_order_process_out(h264_ctx *ctx, td_bool *go_on)
{
    td_s32 ret;
    td_u32 pos;
    td_s32 min_poc;
    td_u32 pre_pos = 0xffffffff;
    td_u32 pre_used_size = 0xffffffff;
    td_u8 flag;

    vfmw_check_ret(ctx->cur_pic.frame_store != TD_NULL, H264_ERR);
    *go_on = TD_FALSE;

    /* 处理图像输出 */
    while (ctx->dpb.used_size >= ctx->dpb.size) {
        get_min_poc(ctx, &min_poc, &pos); /* 可以添加对查找的最小pos和位置的错误进行判断 */
        flag = (((ctx->cur_pic.nal_ref_idc == 0) && (ctx->cur_pic.thispoc < min_poc)) ||
                (ctx->dpb.size == 0));
        if (flag != 0) {
            if ((ctx->old_dec_mode == 2) && (ctx->cur_pic.structure != 0)) { /* 2 i mode */
                ctx->last_field_buf = ctx->cur_pic.frame_store;
            }

            ret = direct_out_put(ctx);
            if (ret == H264_ERR) {
                dprint(PRN_ERROR, "line %d, return %d\n", __LINE__, ret);
            }

            if (ret == H264_ALREADY_OUT) {
                ctx->cur_pic.frame_store->is_in_dpb = 0;
                ctx->cur_pic.frame_store->is_used = 0;
            }

            return ret;
        }

        /* 容错处理: 在used_size不变前提下，如重复获取同一个pos，会导致死循环，须强制退出。 2014.04.19 */
        if (ctx->dpb.used_size == pre_used_size && pos == pre_pos) {
            dprint(PRN_ERROR, "%s: pos(%d) = pre_pos, force return.\n", __func__, pos);
            return H264_ERR;
        } else {
            pre_pos = pos;
            pre_used_size = ctx->dpb.used_size;
        }

        ret = output_frm_from_dpb(ctx, pos);
        if (ret == H264_ERR) {
            dprint(PRN_ERROR, "line %d, return %d\n", __LINE__, ret);
            return H264_ERR;
        }
    }

    *go_on = TD_TRUE;

    return H264_OK;
}

STATIC td_void disp_order_process_check(h264_ctx *ctx)
{
    td_u32 i;

    /* 插入DPB前的错误判断 */
    if (ctx->cur_pic.nal_ref_idc == 0 || ctx->cur_pic.is_long_term == 1) {
        return;
    }

    for (i = 0; i < ctx->dpb.ref_frames_in_buffer; i++) {
        if ((ctx->dpb.fs_ref[i]->frame_num == ctx->cur_pic.frame_num) &&
            (ctx->dpb.fs_ref[i]->fo_state != FO_STATE_FAST_OUTPUT)) {
            ctx->dpb.fs_ref[i]->is_reference = 0;

            if (ctx->dpb.fs_ref[i]->is_in_dpb == 1) {
                ctx->dpb.fs_ref[i]->is_displayed = 1;
                ctx->dpb.fs_ref[i]->is_in_dpb = 0;
            }

            /* pmv槽位设置成缺省值 */
            if (ctx->dpb.fs_ref[i]->pmv_address_idc != ctx->total_pmv_num) {
                ctx->pmv_store_used_flag[ctx->dpb.fs_ref[i]->pmv_address_idc] = 0; /* pmv store */
                ctx->dpb.fs_ref[i]->pmv_address_idc = ctx->total_pmv_num;
            }
        }
    }
}

STATIC td_s32 disp_order_process(h264_ctx *ctx)
{
    td_s32 ret;
    td_bool go_on = TD_FALSE;

    /* 如果是参考帧，marking处理 */
    ret = disp_order_process_mark(ctx, &go_on);
    if (go_on == TD_FALSE) {
        return ret;
    }

    /* 当前图像输入 */
    ret = disp_order_process_pair(ctx, &go_on);
    if (go_on == TD_FALSE) {
        return ret;
    }

    /* DPB满错误判断 */
    ret = disp_order_process_full(ctx, &go_on);
    if (go_on == TD_FALSE) {
        return ret;
    }

    /* 处理图像输出 */
    ret = disp_order_process_out(ctx, &go_on);
    if (go_on == TD_FALSE) {
        return ret;
    }

    /* 插入DPB前的错误判断 */
    disp_order_process_check(ctx);

    /* 把当前图像存入DPB */
    ret = insert_frm_in_dpb(ctx, ctx->dpb.used_size, &ctx->cur_pic);
    ctx->prev_pic.dpb_pos = ctx->dpb.used_size;

    if (ret != H264_OK) {
        dprint(PRN_ERROR, "line %d, return %d\n", __LINE__, ret);
        return H264_ERR;
    }

    ctx->dpb.used_size++;

    update_ref_list(ctx);
    update_lt_ref_list(ctx);

    return H264_OK;
}

/*************************************************************************
*      函数原型 :  td_s32 store_pic_in_dpb(h264_ctx *ctx,)
*      功能描述 :  根据当前图像的mark参数对DPB中图像进行标记；
*                  把当前图像存入DPB
*      参数说明 :  无
*      返回值   ： DEC_NORMAL: 配成功;
*                  DPB_ERR: 在存入DPB时发现DPB出现重大错误，需要firmware重启
*                  OUT_FLAG: 检测到适配层需要退出，返回该标志
*************************************************************************/
STATIC td_s32 store_pic_in_dpb(h264_ctx *ctx, luma_info *luma)
{
    td_s32 ret;

    vfmw_unused(luma);
    ret = H264_ERR;
    ctx->cur_poc.last_has_mmco_5 = 0;
    ctx->cur_poc.last_pic_bottom_field = (td_s32)(ctx->cur_pic.structure == BOTTOM_FIELD);

    /* 错误处理，如果当前图像为空，需要退出 */
    /* 可以考虑报一个错误，上层处理 */
    vfmw_check_goto_print((ctx->cur_pic.state != EMPTY), store_pic_exit, "cur_pic.state is 'EMPTY'\n");

    ctx->prev_store_pic_counter = ctx->pic_counter;

    /******************** 补丁1: I帧模式下仅走directoutput以简化流程 ************************/
    if (ctx->old_dec_mode == 2) { /* 2 i mode */
        ret = imode_process(ctx);
        goto store_pic_exit;
    }

    /******************** 补丁2: 解码序输出，或者第一帧快速输出的特殊处理 ************************/
    /* 处理解码序输出: 如果CurrPic是帧图则直接输出，如果是场图则拼场以后输出。 */
    if ((ctx->extra_data->output_order > VFMW_DISP_ORDER || (ctx->first_frame_displayed != FIRST_DISP_OUTPUT)) &&
         (ctx->cur_pic.non_existing == 0)) {
        ret = dec_order_process(ctx);
        if (ret == H264_ERR) {
            dprint(PRN_ERROR, "Dec Order Process failed!\n");
            goto store_pic_exit;
        }
    }

    /******************** 补丁3: 为可视对讲定制的简化DPB处理，目的是做到误码时不发I帧也能解码 *****/
    /* 如果s32DecOrderOutput == VFMW_DEC_ORDER_SIMPLE_DPB, 则旁路原DPB管理机制，仅用滑窗方式管理参考帧.
       20110803 */
    /* 20120302, 原来处理没有区分是否参考全做slide dpb, 实际上非参考帧不能参与滑窗，否则挤掉了真正的参考帧.
       云游戏SVC-T就有这种情形. 解决方法是加上(ctx->cur_pic.nal_ref_idc != 0)的判断。 */
    if ((ctx->extra_data->output_order == VFMW_DEC_ORDER_SIMPLE_DPB) && (ctx->cur_pic.nal_ref_idc != 0)) {
        ret = simple_dpb_process(ctx);
        if (ret != H264_OK) {
            dprint(PRN_ERROR, "%s call simple_dpb_process Failed\n", __func__);
        }
        goto store_pic_exit;
    }

    /******************** 开始标准的DPB处理流程 ************************/
    ret = disp_order_process(ctx);
    if (ret == H264_ERR) {
        dprint(PRN_ERROR, "%s call disp_order_process Failed\n", __func__);
    }

store_pic_exit:

    if (ret == H264_ERR) {
        fsp_clear_logic(ctx->chan_id, ctx->cur_pic.frame_store->logic_fs_id, 1);
        dprint(PRN_ERROR, "line %d, return %d\n", __LINE__, ret);
        /* 20111223, 既然Store失败，该帧的帧存应该释放掉，免得引起虚拟帧存泄漏 */
        ctx->cur_pic.frame_store->is_in_dpb = 0;
        ctx->cur_pic.frame_store->is_used = 0;
    }

    ctx->cur_pic.state = EMPTY;

    return ret;
}

/*************************************************************************
*   函数原型 :  td_void release_nal(H264DEC_NALU *p_nal)
*   功能描述 :  释放一个nal
*************************************************************************/
STATIC td_void release_nal(td_s32 chan_id, h264_nalu *nalu)
{
    td_u32 i;

    vfmw_check(nalu != TD_NULL);

    nalu->is_valid = 0;
    nalu->nal_integ = 0;

    /* release twice while first slice is two segment */
    for (i = 0; i < nalu->nal_segment; i++) {
        /* add release buf; */
        if (nalu->stream[i].stream_buffer != TD_NULL) {
            stm_release_seg(chan_id, nalu->stream[i].stream_id);
            nalu->stream[i].stream_buffer = TD_NULL;
            nalu->stream[i].bits_offset = 0;
            nalu->stream[i].bitstream_length = 0;
        }
    }

    nalu->nal_used_segment = 0;
    nalu->nal_segment = 0;

    return;
}

/*********************************************************************************
* 函数原型
* td_s32 h264_arrange_vahb_mem(h264_ctx *ctx, td_u32 img_width, td_u32 img_height, td_u32 ImgNum, td_u32 pmv_blk_num)
* 功能描述
* H264协议下VAHB地址规划，同时计算VAHB存储占用量。
*
*  参数说明
*  ImgWidth： 图像宽度，以象素为单位
*  ImgHeight：图像高度， 以象素为单位
*  ImgNum：   需要在VAHB空间中开辟的图像存储块个数，
*  PmvBlkNum：需要在VAHB空间中开辟的MV信息存储块个数
*  返回值
*  如果规划成功返回1，否则返回错误码
*  数据依赖
*  g_VfmwCfgInfo：VFMW的配置信息，定义于public.c
*  g_Memory：     VFMW的存储分配信息，定义于public.c
*  g_VfmwState：  VFMW的状态信息，定义于public.c
*  g_H264MemInfo：H.264模块地址规划信息，定义于h264.c
*  算法描述
*  从g_VfmwCfgInfo中获取每个图像需要支持多少个slice的信息；
*  从g_Memory中获取VAHB起始地址，并且将H264消耗的VAHB空间填入g_Memory.VAHB_access_size
*  刷新g_VfmwState中相关的状态信息；
*  H.264模块的地址明细存入g_H264MemInfo，供H.264其它函数使用
*********************************************************************************/
STATIC td_s32 h264_arrange_vahb_mem(const h264_ctx *ctx, td_u32 img_width, td_u32 img_height, td_u32 *img_num,
                                    td_u32 pmv_blk_num, td_u32 bit_depth)
{
    td_s32 ret;
    td_s32 dec_num;
    td_s32 pmv_num;
    fsp_cfg fsp_inst_cfg;

    vfmw_unused(img_num);
    vfmw_unused(pmv_blk_num);
    vfmw_unused(bit_depth);
    dec_num = (td_s32)(ctx->dpb.size + 1);
    pmv_num = (td_s32)(ctx->dpb.size + 1);

    if (ctx->cur_sps.frame_mbs_only_flag == 0) {
        dec_num += 1; /* for single field */
    }

    vfmw_check_sec_func(memset_s(&fsp_inst_cfg, sizeof(fsp_cfg), 0, sizeof(fsp_cfg)));

    fsp_inst_cfg.vid_std = VFMW_H264;
    /* force 8bit decode avoid vdh die */
    fsp_inst_cfg.bit_depth = 8; /* 8 bit depth */
    fsp_inst_cfg.dec_width = img_width;
    fsp_inst_cfg.dec_height = img_height;
    fsp_inst_cfg.disp_width = ctx->cur_sps.disp_width;
    fsp_inst_cfg.disp_height = ctx->cur_sps.disp_height;
    fsp_inst_cfg.need_dec_num = (td_u32)((ctx->extra_data->dec_mode == VFMW_DEC_MODE_FIRST_I ||
        ctx->extra_data->dec_mode == VFMW_DEC_MODE_I) ? 1 : dec_num);
    fsp_inst_cfg.need_disp_num = fsp_inst_cfg.need_dec_num;
    fsp_inst_cfg.need_pmv_num = (td_u32)((ctx->extra_data->dec_mode == VFMW_DEC_MODE_FIRST_I ||
        ctx->extra_data->dec_mode == VFMW_DEC_MODE_I) ? 0 : pmv_num);
    fsp_inst_cfg.splice_info.ori_width = ctx->splice_info.ori_width;
    fsp_inst_cfg.splice_info.ori_height = ctx->splice_info.ori_height;
    fsp_inst_cfg.splice_info.type = ctx->splice_info.type;

    /* 配置FSP实例，并分割帧存 */
    ret = fsp_config(ctx->chan_id, &fsp_inst_cfg);
    if (ret != FSP_OK) {
        return SYN_PARSE_RESOURCE_INVALID;
    }

    return SYN_OK;
}

STATIC td_s32 repair_list_get_cnt(h264_ctx *ctx, td_u32 *i_list_cnt)
{
    switch (ctx->cur_slice.slice_type) {
        case P_SLICE:
            *i_list_cnt = 1;
            break;

        case B_SLICE:
            *i_list_cnt = 2; /* 2 list num */
            break;

        case I_SLICE:
            *i_list_cnt = 0;
            break;

        default:
            dprint(PRN_ERROR, "slice_type %d invalid, clear current slice.\n", ctx->cur_slice.slice_type);
            clear_curr_slice(ctx);
            return H264_ERR; /* NULL_LIST */
    }

    return H264_OK;
}

/*************************************************************************
* 函数原型 : td_s32 repair_list(h264_ctx *ctx)
* 功能描述 :  修补list,只考虑去掉list中的NULL
*************************************************************************/
STATIC td_s32 repair_list(h264_ctx *ctx)
{
    td_u8 found;
    td_u32 i, i_list, i_list_cnt;
    h264_store_pic *repair_pic = (h264_store_pic *)TD_NULL;

    if (repair_list_get_cnt(ctx, &i_list_cnt) != H264_OK) {
        return H264_ERR;
    }

    for (i_list = 0; i_list < i_list_cnt; i_list++) {
        if (ctx->cur_slice.listx_size[i_list] <= 0) {
            if (ctx->extra_data->output_order != VFMW_DEC_ORDER_SIMPLE_DPB) {
                dprint(PRN_ERROR, "listx_size[%d] = %d, clear current slice.\n",
                       i_list, ctx->cur_slice.listx_size[i_list]);
                clear_curr_slice(ctx);
                return H264_ERR; /* NULL_LIST */
            } else {
                /* 如果运行于DEC_ORDER_SIMPLE_DPB, 则用当前pic填充所有list节点 */
                ctx->cur_slice.listx_size[i_list] = 1;
                ctx->listx[i_list][0] = &ctx->cur_pic.frame_store->frame;
            }
            continue; /* delete else{} for nesting depth5 */
        }

        found = 0;
        for (i = 0; i < ctx->cur_slice.listx_size[i_list]; i++) {
            /* !! the condition must add */
            if (ctx->listx[i_list][i] != (h264_store_pic *)TD_NULL) {
                found = 1;
                repair_pic = ctx->listx[i_list][i];
                break;
            }
        }

        if ((found == 0) || (repair_pic == TD_NULL)) {
            if (ctx->extra_data->output_order != VFMW_DEC_ORDER_SIMPLE_DPB) {
                /* !!! 所有list元素为空，可以把list置为空让硬件去容错， */
                /* !!! 也可以把slice丢掉。 */
                /* !!! 前者为使得p bslice中全i宏块时可解，但需硬件容错 */
                /* !!! 后者对硬件最为安全，但需要修补很多 slice */
                dprint(PRN_ERROR, "repair_pic/found(%d) = 0, clear current slice.\n", found);
                clear_curr_slice(ctx);
                return H264_ERR; /* NULL_LIST */
            } else {
                /* 如果运行于DEC_ORDER_SIMPLE_DPB, 则用当前pic填充所有list节点 */
                repair_pic = &ctx->cur_pic.frame_store->frame;
            }
        }

        for (i = 0; i < ctx->cur_slice.listx_size[i_list]; i++) {
            /* !! the condition must add */
            if (ctx->listx[i_list][i] == (h264_store_pic *)TD_NULL) {
                ctx->listx[i_list][i] = repair_pic;
            }
        }
    }

    return H264_OK;
}
#ifdef H264_FIELD_SUPPORT
STATIC h264_store_pic *get_short_term_pic_point_field(const h264_ctx *ctx, td_s32 pic_num, td_u32 i)
{
    if ((ctx->dpb.fs_ref[i]->is_reference & 1) && /* top field */
        (ctx->dpb.fs_ref[i]->top_field.is_short_term == 1) &&
        (ctx->dpb.fs_ref[i]->top_field.is_long_term == 0) &&
        (ctx->dpb.fs_ref[i]->top_field.pic_num == pic_num)) {
        return &ctx->dpb.fs_ref[i]->top_field;
    }

    if ((ctx->dpb.fs_ref[i]->is_reference & 2) && /* 2 bottom field */
        (ctx->dpb.fs_ref[i]->bottom_field.is_short_term == 1) &&
        (ctx->dpb.fs_ref[i]->bottom_field.is_long_term == 0) &&
        (ctx->dpb.fs_ref[i]->bottom_field.pic_num == pic_num)) {
        return &ctx->dpb.fs_ref[i]->bottom_field;
    }

    return TD_NULL;
}
#endif
/*************************************************************************
* 函数原型 :  H264DEC_StorablePicture*  get_short_term_pic_point(h264_ctx *, td_s32)
* 功能描述 :  根据picnum找到对应短期参考pic的指针
*************************************************************************/
STATIC h264_store_pic *get_short_term_pic_point(const h264_ctx *ctx, td_s32 pic_num)
{
    td_u32 i;
    td_s32 structure = (ctx->cur_slice.field_pic_flag != 0) ? (ctx->cur_slice.bottom_field_flag != 0) ?
                       BOTTOM_FIELD : TOP_FIELD : FRAME;

    for (i = 0; i < ctx->dpb.ref_frames_in_buffer; i++) {
        if (structure == FRAME) {
            if ((ctx->dpb.fs_ref[i]->is_reference == 3) && /* 3 frame, field pair */
                (ctx->dpb.fs_ref[i]->frame.is_short_term == 1) &&
                (ctx->dpb.fs_ref[i]->frame.is_long_term == 0) &&
                (ctx->dpb.fs_ref[i]->frame.pic_num == pic_num)) {
                return &ctx->dpb.fs_ref[i]->frame;
            }
        }
#ifdef H264_FIELD_SUPPORT
        else {
            h264_store_pic *ret = TD_NULL;
            ret = get_short_term_pic_point_field(ctx, pic_num, i);
            if (ret != TD_NULL) {
                return ret;
            }
        }
#endif
    }

    return TD_NULL;
}
#ifdef H264_FIELD_SUPPORT
STATIC h264_store_pic *get_long_term_pic_point_field(const h264_ctx *ctx, td_s32 long_term_pic_num,
    td_u32 i)
{
    if (((ctx->dpb.fs_ltref[i]->is_reference & 1) != 0) && /* top field */
        (ctx->dpb.fs_ltref[i]->top_field.is_short_term == 0) &&
        (ctx->dpb.fs_ltref[i]->top_field.is_long_term == 1) &&
        (ctx->dpb.fs_ltref[i]->top_field.long_term_pic_num == long_term_pic_num)) {
        return &ctx->dpb.fs_ltref[i]->top_field;
    }

    if (((ctx->dpb.fs_ltref[i]->is_reference & 2) != 0) && /* 2 bottom field */
        (ctx->dpb.fs_ltref[i]->bottom_field.is_short_term == 0) &&
        (ctx->dpb.fs_ltref[i]->bottom_field.is_long_term == 1) &&
        (ctx->dpb.fs_ltref[i]->bottom_field.long_term_pic_num == long_term_pic_num)) {
        return &ctx->dpb.fs_ltref[i]->bottom_field;
    }

    return TD_NULL;
}
#endif
/*************************************************************************
* 函数原型 :  H264DEC_StorablePicture*  get_long_term_pic_point(h264_ctx *ctx,td_s32)
* 功能描述 :  根据LongtermPicNum找到对应长期参考pic的指针
*************************************************************************/
STATIC h264_store_pic *get_long_term_pic_point(const h264_ctx *ctx, td_s32 long_term_pic_num)
{
    td_u32 i;
    td_s32 structure = (ctx->cur_slice.field_pic_flag != 0) ? (ctx->cur_slice.bottom_field_flag != 0) ?
                       BOTTOM_FIELD : TOP_FIELD : FRAME;

    for (i = 0; i < ctx->dpb.ltref_frames_in_buffer; i++) {
        if (structure == FRAME) {
            if ((ctx->dpb.fs_ltref[i]->is_reference == 3) && /* 3 frame, field pair */
                (ctx->dpb.fs_ltref[i]->frame.is_short_term == 0) &&
                (ctx->dpb.fs_ltref[i]->frame.is_long_term == 1) &&
                (ctx->dpb.fs_ltref[i]->frame.long_term_pic_num == long_term_pic_num)) {
                return &ctx->dpb.fs_ltref[i]->frame;
            }
        }
#ifdef H264_FIELD_SUPPORT
        else {
            h264_store_pic *ret = TD_NULL;
            ret =  get_long_term_pic_point_field(ctx, long_term_pic_num, i);
            if (ret != TD_NULL) {
                return ret;
            }
        }
#endif
    }

    return TD_NULL;
}

/*************************************************************************
* 函数原型 : td_void H264DEC_ReorderShortTerm(h264_store_pic **ref_pic_list_x,
*  td_s32 num_ref_idx_lX_active_minus1, td_s32 pic_num_l_x, td_s32 *ref_idx_l_x)
* 功能描述 :  重排短期参考列表
*************************************************************************/
STATIC td_void recorder_st_list(h264_ctx *ctx, h264_store_pic **ref_pic_list_x,
                                td_s32 num_ref_idx_lx_active_minus1, td_s32 pic_num_l_x, td_s32 *ref_idx_l_x)
{
    td_s32 c_idx, n_idx;
    h264_store_pic *pic_l_x;

    pic_l_x = get_short_term_pic_point(ctx, pic_num_l_x);

    for (c_idx = num_ref_idx_lx_active_minus1 + 1; c_idx > *ref_idx_l_x; c_idx--) {
        ref_pic_list_x[c_idx] = ref_pic_list_x[c_idx - 1];
    }

    ref_pic_list_x[(*ref_idx_l_x)++] = pic_l_x;

    n_idx = *ref_idx_l_x;

    for (c_idx = *ref_idx_l_x; c_idx <= num_ref_idx_lx_active_minus1 + 1; c_idx++) {
        if (ref_pic_list_x[c_idx]) {
            if ((ref_pic_list_x[c_idx]->is_long_term != 0) || (ref_pic_list_x[c_idx]->pic_num != pic_num_l_x)) {
                ref_pic_list_x[n_idx++] = ref_pic_list_x[c_idx];
            }
        }
    }

    return;
}

/*************************************************************************
* 函数原型 : td_void H264DEC_ReorderLongTerm(h264_store_pic **ref_pic_list_x,
td_s32 num_ref_idx_lX_active_minus1, td_s32 long_term_pic_num, td_s32 *ref_idx_l_x)
* 功能描述 :  重排长期期参考列表
*************************************************************************/
STATIC td_void recorder_lt_list(h264_ctx *ctx, h264_store_pic **ref_pic_list_x,
                                td_s32 num_ref_idx_lx_active_minus1, td_s32 long_term_pic_num, td_s32 *ref_idx_l_x)
{
    td_s32 c_idx, n_idx;
    h264_store_pic *pic_l_x;

    pic_l_x = get_long_term_pic_point(ctx, long_term_pic_num);

    for (c_idx = num_ref_idx_lx_active_minus1 + 1; c_idx > *ref_idx_l_x; c_idx--) {
        ref_pic_list_x[c_idx] = ref_pic_list_x[c_idx - 1];
    }

    ref_pic_list_x[(*ref_idx_l_x)++] = pic_l_x;

    n_idx = *ref_idx_l_x;

    for (c_idx = *ref_idx_l_x; c_idx <= num_ref_idx_lx_active_minus1 + 1; c_idx++) {
        if (ref_pic_list_x[c_idx]) {
            if ((ref_pic_list_x[c_idx]->is_long_term == 0) ||
                (ref_pic_list_x[c_idx]->long_term_pic_num != long_term_pic_num)) {
                ref_pic_list_x[n_idx++] = ref_pic_list_x[c_idx];
            }
        }
    }

    return;
}

STATIC td_void recorder_ref_pic_list_para1(h264_ctx *ctx, td_u32 list_id,
    td_u32 *num_ref_idx_lx_active_minus1, td_u32 **reordering_of_pic_nums_idc,
    td_u32 **abs_diff_pic_num_minus1, td_u32 **long_term_pic_idx)
{
    if (list_id == 0) {
        *num_ref_idx_lx_active_minus1 = ctx->cur_slice.num_ref_idx_l0_active_minus1;
        *reordering_of_pic_nums_idc = ctx->cur_reorder.reordering_of_pic_nums_idc_l0;
        *abs_diff_pic_num_minus1 = ctx->cur_reorder.abs_diff_pic_num_minus1_l0;
        *long_term_pic_idx = ctx->cur_reorder.long_term_pic_idx_l0;
    } else {
        *num_ref_idx_lx_active_minus1 = ctx->cur_slice.num_ref_idx_l1_active_minus1;
        *reordering_of_pic_nums_idc = ctx->cur_reorder.reordering_of_pic_nums_idc_l1;
        *abs_diff_pic_num_minus1 = ctx->cur_reorder.abs_diff_pic_num_minus1_l1;
        *long_term_pic_idx = ctx->cur_reorder.long_term_pic_idx_l1;
    }
}

STATIC td_void recorder_ref_pic_list_para2(h264_ctx *ctx,
    td_s32 *max_pic_num, td_s32 *curr_pic_num)
{
    td_s32 max_frame_num;
    h264_sps *sps = TD_NULL;
    h264_pps *pps = TD_NULL;
    td_s32 structure = (ctx->cur_slice.field_pic_flag != 0) ? (ctx->cur_slice.bottom_field_flag != 0) ?
                       BOTTOM_FIELD : TOP_FIELD : FRAME;

    pps = ctx->pps[ctx->cur_slice.pic_parameter_set_id];
    sps = ctx->sps[pps->seq_parameter_set_id];

    max_frame_num = 1 << (sps->log2_max_frame_num_minus4 + 4); /* 4 is num */

    if (structure == FRAME) {
        *max_pic_num = max_frame_num;
        *curr_pic_num = (td_s32)ctx->cur_slice.frame_num;
    }
#ifdef H264_FIELD_SUPPORT
    else {
        vfmw_check(max_frame_num < VFMW_INT_MAX);
        vfmw_check(ctx->cur_slice.frame_num < (td_u32)VFMW_INT_MAX);
        *max_pic_num = 2 * max_frame_num; /* 2 double */
        *curr_pic_num = 2 * ctx->cur_slice.frame_num + 1; /* 2 double */
    }
#endif
}


/*************************************************************************
* 函数原型 : td_s32 recorder_ref_pic_list()
* 功能描述 :  参考列表重排函数
*************************************************************************/
STATIC td_void recorder_ref_pic_list(h264_ctx *ctx, h264_store_pic **list, td_u32 *list_size, td_u32 list_id)
{
    td_u32 i;
    td_s32 max_pic_num, curr_pic_num, pic_num_lx_no_wrap, pic_num_lx_pred, pic_num_l_x;
    td_s32 ref_idx_l_x = 0;
    td_u32 num_ref_idx_lx_active_minus1;
    td_u32 *reordering_of_pic_nums_idc = TD_NULL;
    td_u32 *abs_diff_pic_num_minus1 = TD_NULL;
    td_u32 *long_term_pic_idx = TD_NULL;

    vfmw_unused(list_size);
    curr_pic_num = max_pic_num = 0;

    recorder_ref_pic_list_para1(ctx, list_id, &num_ref_idx_lx_active_minus1,
        &reordering_of_pic_nums_idc, &abs_diff_pic_num_minus1, &long_term_pic_idx);
    recorder_ref_pic_list_para2(ctx, &max_pic_num, &curr_pic_num);

    pic_num_lx_pred = curr_pic_num;

    for (i = 0; i < (num_ref_idx_lx_active_minus1 + 2); i++) { /* 2 idc ofs */
        if (reordering_of_pic_nums_idc[i] == 3) { /* 3 idc val */
            break;
        }

        if (reordering_of_pic_nums_idc[i] >= 2) { /* 2 vfmw_max idc */
            recorder_lt_list(ctx, list, (td_s32)num_ref_idx_lx_active_minus1, (td_s32)long_term_pic_idx[i],
                &ref_idx_l_x);
            continue; /* delete else{} for nesting depth5 */
        }

        if (reordering_of_pic_nums_idc[i] == 0) {
            if (pic_num_lx_pred - ((td_s32)abs_diff_pic_num_minus1[i] + 1) < 0) {
                pic_num_lx_no_wrap = (pic_num_lx_pred - ((td_s32)abs_diff_pic_num_minus1[i] + 1)) + max_pic_num;
            } else {
                pic_num_lx_no_wrap = pic_num_lx_pred - ((td_s32)abs_diff_pic_num_minus1[i] + 1);
            }
        } else {
            if (pic_num_lx_pred + (((td_s32)abs_diff_pic_num_minus1[i] + 1)) >= max_pic_num) {
                pic_num_lx_no_wrap = pic_num_lx_pred + (td_s32)abs_diff_pic_num_minus1[i] + 1 - max_pic_num;
            } else {
                pic_num_lx_no_wrap = pic_num_lx_pred + (td_s32)abs_diff_pic_num_minus1[i] + 1;
            }
        }

        pic_num_lx_pred = pic_num_lx_no_wrap;

        if (pic_num_lx_no_wrap > curr_pic_num) {
            pic_num_l_x = pic_num_lx_no_wrap - max_pic_num;
        } else {
            pic_num_l_x = pic_num_lx_no_wrap;
        }

        recorder_st_list(ctx, list, (td_s32)num_ref_idx_lx_active_minus1, pic_num_l_x, &ref_idx_l_x);
    }
}

/*************************************************************************
* 函数原型 :  td_s32 ReorderlistX(h264_ctx *ctx)
* 功能描述 :  参考列表重排
*************************************************************************/
STATIC td_void recorder_list_x(h264_ctx *ctx)
{
    if (ctx->cur_slice.slice_type != I_SLICE) {
        if (ctx->cur_reorder.ref_pic_list_reordering_flag_l0 != 0) {
            recorder_ref_pic_list(ctx, ctx->listx[0], &(ctx->cur_slice.listx_size[0]), 0);
        }

        /* that's a definition */
        ctx->cur_slice.listx_size[0] = ctx->cur_slice.num_ref_idx_l0_active_minus1 + 1;
    }
#ifdef H264_B_FRAME_SUPPORT
    if (ctx->cur_slice.slice_type == B_SLICE) {
        if (ctx->cur_reorder.ref_pic_list_reordering_flag_l1 != 0) {
            recorder_ref_pic_list(ctx, ctx->listx[1], &(ctx->cur_slice.listx_size[1]), 1);
        }

        /* that's a definition */
        ctx->cur_slice.listx_size[1] = ctx->cur_slice.num_ref_idx_l1_active_minus1 + 1;
    }
#endif
    return;
}

/*************************************************************************
* 函数原型 : td_s32 is_st_ref_flg(h264_store_pic *s)
* 功能描述 :  判断pic是否是短期参考
*************************************************************************/
#if defined(H264_FIELD_SUPPORT) || defined(H264_B_FRAME_SUPPORT)
STATIC td_u32 is_st_ref_flg(const h264_store_pic *s)
{
    return (td_u32)(((s->is_short_term == 1) && (s->is_long_term == 0)));
}

STATIC td_u32 is_lt_ref_flg(const h264_store_pic *s)
{
    return (td_u32)(((s->is_short_term == 0) && (s->is_long_term == 1)));
}
#endif

/*************************************************************************
* 函数原型 : td_s32 compare_pic_by_pic_num_desc(const td_void *arg1, const td_void *arg2)
* 功能描述 :  按降序比较storablepicture结构体中的元素pic_num的大小，
*              ps:这一类比较函数比较的元素对应协议中规定的值，这些值也许与JM中存放的结构体
*              不一致.为了保证正确性，可以检视现有存放位置上的元素是否正确
*************************************************************************/
STATIC td_s32 compare_pic_by_pic_num_desc(const td_void *arg1, const td_void *arg2)
{
    if ((*(h264_store_pic **)arg1)->pic_num < (*(h264_store_pic **)arg2)->pic_num) {
        return 1;
    }

    if ((*(h264_store_pic **)arg1)->pic_num > (*(h264_store_pic **)arg2)->pic_num) {
        return -1;
    } else {
        return 0;
    }
}

STATIC td_s32 compare_pic_by_lt_pic_num_asc(const td_void *arg1, const td_void *arg2)
{
    if ((*(h264_store_pic **)arg1)->long_term_pic_num < (*(h264_store_pic **)arg2)->long_term_pic_num) {
        return -1;
    }

    if ((*(h264_store_pic **)arg1)->long_term_pic_num > (*(h264_store_pic **)arg2)->long_term_pic_num) {
        return 1;
    } else {
        return 0;
    }
}

#ifdef H264_FIELD_SUPPORT
STATIC td_s32 compare_fs_by_frame_num_desc(const td_void *arg1, const td_void *arg2)
{
    if ((*(h264_frame_store **)arg1)->frame_num_wrap < (*(h264_frame_store **)arg2)->frame_num_wrap) {
        return 1;
    }

    if ((*(h264_frame_store **)arg1)->frame_num_wrap > (*(h264_frame_store **)arg2)->frame_num_wrap) {
        return -1;
    } else {
        return 0;
    }
}
#endif

#ifdef H264_B_FRAME_SUPPORT
STATIC td_s32 compare_fs_by_lt_pic_idx_asc(const td_void *arg1, const td_void *arg2)
{
    if ((*(h264_frame_store **)arg1)->long_term_frame_idx < (*(h264_frame_store **)arg2)->long_term_frame_idx) {
        return -1;
    }

    if ((*(h264_frame_store **)arg1)->long_term_frame_idx > (*(h264_frame_store **)arg2)->long_term_frame_idx) {
        return 1;
    } else {
        return 0;
    }
}

STATIC td_s32 h264_compare_pic_by_poc_asc(const td_void *arg1, const td_void *arg2)
{
    if ((*(h264_store_pic **)arg1)->poc < (*(h264_store_pic **)arg2)->poc) {
        return -1;
    }

    if ((*(h264_store_pic **)arg1)->poc > (*(h264_store_pic **)arg2)->poc) {
        return 1;
    } else {
        return 0;
    }
}

STATIC td_s32 compare_pic_by_poc_desc(const td_void *arg1, const td_void *arg2)
{
    if ((*(h264_store_pic **)arg1)->poc < (*(h264_store_pic **)arg2)->poc) {
        return 1;
    }

    if ((*(h264_store_pic **)arg1)->poc > (*(h264_store_pic **)arg2)->poc) {
        return -1;
    } else {
        return 0;
    }
}

STATIC td_s32 compare_fs_by_poc_asc(const td_void *arg1, const td_void *arg2)
{
    if ((*(h264_frame_store **)arg1)->poc < (*(h264_frame_store **)arg2)->poc) {
        return -1;
    }

    if ((*(h264_frame_store **)arg1)->poc > (*(h264_frame_store **)arg2)->poc) {
        return 1;
    } else {
        return 0;
    }
}

STATIC td_s32 compare_fs_by_poc_desc(const td_void *arg1, const td_void *arg2)
{
    if ((*(h264_frame_store **)arg1)->poc < (*(h264_frame_store **)arg2)->poc) {
        return 1;
    }

    if ((*(h264_frame_store **)arg1)->poc > (*(h264_frame_store **)arg2)->poc) {
        return -1;
    } else {
        return 0;
    }
}
#endif

#if defined(H264_FIELD_SUPPORT) || defined(H264_B_FRAME_SUPPORT)
STATIC td_void gen_pic_list_from_frm_list_top(h264_frame_store **fs_list,
    td_u32 list_idx, h264_store_pic **list, td_u32 *list_size, td_s32 long_term)
{
    td_u32 top_idx = 0;
    td_u32 bot_idx = 0;
    td_u32 (*is_ref)(const h264_store_pic *s) = TD_NULL;

    /* top */
    if (long_term != 0) {
        is_ref = is_lt_ref_flg;
    } else {
        is_ref = is_st_ref_flg;
    }

    while ((top_idx < list_idx) || (bot_idx < list_idx)) {
        for (; top_idx < list_idx; top_idx++) {
            if ((fs_list[top_idx]->is_used & 1) == 0) {
                continue;
            }

            if (is_ref(&fs_list[top_idx]->top_field) != 0) {
                /* short term ref pic */
                list[*list_size] = &fs_list[top_idx]->top_field;
                (*list_size)++;
                top_idx++;
                break;
            }
        }

        for (; bot_idx < list_idx; bot_idx++) {
            if ((fs_list[bot_idx]->is_used & 2) == 0) { /* 2 btm fld */
                continue;
            }

            if (is_ref(&fs_list[bot_idx]->bottom_field) != 0) {
                /* short term ref pic */
                list[*list_size] = &fs_list[bot_idx]->bottom_field;
                (*list_size)++;
                bot_idx++;
                break;
            }
        }
    }
}

STATIC td_void gen_pic_list_from_frm_list_btm(h264_frame_store **fs_list,
    td_u32 list_idx, h264_store_pic **list, td_u32 *list_size, td_s32 long_term)
{
    td_u32 top_idx = 0;
    td_u32 bot_idx = 0;
    td_u32 (*is_ref)(const h264_store_pic *s) = TD_NULL;

    /* top */
    if (long_term != 0) {
        is_ref = is_lt_ref_flg;
    } else {
        is_ref = is_st_ref_flg;
    }

    while ((top_idx < list_idx) || (bot_idx < list_idx)) {
        for (; bot_idx < list_idx; bot_idx++) {
            if ((fs_list[bot_idx]->is_used & 2) == 0) { /* 2 btm fld */
                continue;
            }

            if (is_ref(&fs_list[bot_idx]->bottom_field) != 0) {
                /* short term ref pic */
                list[*list_size] = &fs_list[bot_idx]->bottom_field;
                (*list_size)++;
                bot_idx++;
                break;
            }
        }

        for (; top_idx < list_idx; top_idx++) {
            if ((fs_list[top_idx]->is_used & 1) == 0) {
                continue;
            }

            if (is_ref(&fs_list[top_idx]->top_field) != 0) {
                /* short term ref pic */
                list[*list_size] = &fs_list[top_idx]->top_field;
                (*list_size)++;
                top_idx++;
                break;
            }
        }
    }
}

STATIC td_s32 gen_pic_list_from_frm_list(td_u32 curr_structure, h264_frame_store **fs_list,
    td_u32 list_idx, h264_store_pic **list, td_u32 *list_size, td_s32 long_term)
{
    td_u32 list_idx_tmp = list_idx;

    if (list_idx_tmp >= 32) { /* 32 ref num */
        list_idx_tmp = 31; /* 31 for error resilence */
    }

    if (curr_structure == TOP_FIELD) {
        gen_pic_list_from_frm_list_top(fs_list, list_idx_tmp, list, list_size, long_term);
    }

    if (curr_structure == BOTTOM_FIELD) {
        gen_pic_list_from_frm_list_btm(fs_list, list_idx_tmp, list, list_size, long_term);
    }

    return 0;
}
#endif

STATIC td_void init_list_p_slice_frame(h264_ctx *ctx)
{
    td_u32 i;
    td_u32 list0_idx = 0;

    for (i = 0; i < ctx->dpb.ref_frames_in_buffer; i++) {
        if ((ctx->dpb.fs_ref[i]->is_reference == 3) && /* 3 both ref */
            (ctx->dpb.fs_ref[i]->frame.is_long_term == 0) &&
            (ctx->dpb.fs_ref[i]->frame.is_short_term == 1)) { /* frame, field pair */
            vfmw_check(list0_idx < MAX_LIST_SIZE);
            ctx->listx[0][list0_idx++] = &ctx->dpb.fs_ref[i]->frame;
        }
    }

    /* order list 0 by PicNum : short term */
    syn_qsort((td_void *)ctx->listx[0], list0_idx, sizeof(h264_store_pic *), compare_pic_by_pic_num_desc);
    ctx->cur_slice.listx_size[0] = list0_idx;

    /* long term handling */
    for (i = 0; i < ctx->dpb.ltref_frames_in_buffer; i++) {
        if (ctx->dpb.fs_ltref[i]->is_reference == 3 && /* 3 both ref */
            ctx->dpb.fs_ltref[i]->frame.is_long_term == 1 &&
            ctx->dpb.fs_ltref[i]->frame.is_short_term == 0) { /* frame, field pair */
            vfmw_check(list0_idx < MAX_LIST_SIZE);
            ctx->listx[0][list0_idx++] = &ctx->dpb.fs_ltref[i]->frame;
        }
    }

    syn_qsort((td_void *)&ctx->listx[0][ctx->cur_slice.listx_size[0]], list0_idx - ctx->cur_slice.listx_size[0],
              sizeof(h264_store_pic *), compare_pic_by_lt_pic_num_asc);
    ctx->cur_slice.listx_size[0] = list0_idx;

    return;
}

#ifdef H264_FIELD_SUPPORT
STATIC td_void init_list_p_slice_field(h264_ctx *ctx)
{
    td_u32 i;
    td_u32 list0_idx = 0;
    td_u32 list_lt_idx = 0;
    h264_frame_store *fs_list0[16]; /* 16 ref num */
    h264_frame_store *fs_list_lt[16]; /* 16 ref num */

    for (i = 0; i < ctx->dpb.ref_frames_in_buffer; i++) {
        if (ctx->dpb.fs_ref[i]->is_reference != 0) {
            vfmw_check(list0_idx < 16); /* 16 array size */
            fs_list0[list0_idx++] = ctx->dpb.fs_ref[i];
        }
    }

    syn_qsort((td_void *)fs_list0, list0_idx, sizeof(h264_frame_store *), compare_fs_by_frame_num_desc);

    ctx->cur_slice.listx_size[0] = 0;
    gen_pic_list_from_frm_list(ctx->cur_pic.structure, fs_list0, list0_idx, ctx->listx[0],
                               &(ctx->cur_slice.listx_size[0]), 0);

    /* long term handling */
    for (i = 0; i < ctx->dpb.ltref_frames_in_buffer; i++) {
        vfmw_check(list_lt_idx < 16); /* 16 array size */
        fs_list_lt[list_lt_idx++] = ctx->dpb.fs_ltref[i];
    }

    syn_qsort((td_void *)fs_list_lt, list_lt_idx, sizeof(h264_frame_store *), compare_fs_by_lt_pic_idx_asc);
    gen_pic_list_from_frm_list(ctx->cur_pic.structure, fs_list_lt, list_lt_idx, ctx->listx[0],
                               &(ctx->cur_slice.listx_size[0]), 1);

    return;
}
#endif

#ifdef H264_B_FRAME_SUPPORT
STATIC td_void init_list_b_slice_frame(h264_ctx *ctx)
{
    td_u32 i, j, list0_idx_1;
    td_u32 list0_idx = 0;

    for (i = 0; i < ctx->dpb.ref_frames_in_buffer; i++) {
        if (ctx->dpb.fs_ref[i]->is_reference == 3) { /* 3 frame, field pair */
            if (ctx->cur_pic.framepoc >= ctx->dpb.fs_ref[i]->frame.poc) {
                vfmw_check(list0_idx < MAX_LIST_SIZE);
                ctx->listx[0][list0_idx++] = &ctx->dpb.fs_ref[i]->frame;
            }
        }
    }

    syn_qsort((td_void *)ctx->listx[0], list0_idx, sizeof(h264_store_pic *), compare_pic_by_poc_desc);
    list0_idx_1 = list0_idx;

    for (i = 0; i < ctx->dpb.ref_frames_in_buffer; i++) {
        if (ctx->dpb.fs_ref[i]->is_reference == 3) {  /* 3 frame, field pair */
            if (ctx->cur_pic.framepoc < ctx->dpb.fs_ref[i]->frame.poc) { /* 这个判断其实可以不要了 */
                vfmw_check(list0_idx < MAX_LIST_SIZE);
                ctx->listx[0][list0_idx++] = &ctx->dpb.fs_ref[i]->frame;
            }
        }
    }

    if (list0_idx_1 < MAX_LIST_SIZE) {
        syn_qsort((td_void *)&ctx->listx[0][list0_idx_1], list0_idx - list0_idx_1, sizeof(h264_store_pic *),
                  h264_compare_pic_by_poc_asc);
    }

    /* 因为上面已经排过序了，所以这里不用再排序，只用倒一下序就可以了 */
    vfmw_check(list0_idx_1 <= MAX_LIST_SIZE);
    for (j = 0; j < list0_idx_1; j++) {
        ctx->listx[1][(list0_idx - list0_idx_1) + j] = ctx->listx[0][j];
    }

    for (j = list0_idx_1; j < list0_idx; j++) {
        ctx->listx[1][j - list0_idx_1] = ctx->listx[0][j];
    }

    ctx->cur_slice.listx_size[0] = ctx->cur_slice.listx_size[1] = list0_idx;

    /* long term handling */
    for (i = 0; i < ctx->dpb.ltref_frames_in_buffer; i++) {
        if (ctx->dpb.fs_ltref[i]->is_used == 3) { /* 3 frame, field pair */
            if (ctx->dpb.fs_ltref[i]->frame.is_long_term == 1) {
                vfmw_check(list0_idx < MAX_LIST_SIZE);
                ctx->listx[0][list0_idx] = &ctx->dpb.fs_ltref[i]->frame;
                ctx->listx[1][list0_idx++] = &ctx->dpb.fs_ltref[i]->frame;
            }
        }
    }

    syn_qsort((td_void *)&ctx->listx[0][ctx->cur_slice.listx_size[0]], list0_idx - ctx->cur_slice.listx_size[0],
              sizeof(h264_store_pic *), compare_pic_by_lt_pic_num_asc);
    syn_qsort((td_void *)&ctx->listx[1][ctx->cur_slice.listx_size[0]], list0_idx - ctx->cur_slice.listx_size[0],
              sizeof(h264_store_pic *), compare_pic_by_lt_pic_num_asc);
    ctx->cur_slice.listx_size[0] = ctx->cur_slice.listx_size[1] = list0_idx;

    return;
}

STATIC td_void init_list_b_slice_field(h264_ctx *ctx)
{
    td_u32 i, j, list0_idx_1;
    td_u32 list0_idx = 0;
    td_u32 list_lt_idx = 0;
    h264_frame_store *fs_list0[16], *fs_list1[16], *fs_list_lt[16]; /* 16 ref num */

    for (i = 0; i < ctx->dpb.ref_frames_in_buffer; i++) {
        if (ctx->dpb.fs_ref[i]->is_used != 0 &&
            ctx->cur_pic.thispoc >= ctx->dpb.fs_ref[i]->poc) {
            vfmw_check(list0_idx < 16); /* 16:array size */
            fs_list0[list0_idx++] = ctx->dpb.fs_ref[i];
        }
    }

    syn_qsort((td_void *)fs_list0, list0_idx, sizeof(h264_frame_store *), compare_fs_by_poc_desc);
    list0_idx_1 = list0_idx;

    for (i = 0; i < ctx->dpb.ref_frames_in_buffer; i++) {
        if (ctx->dpb.fs_ref[i]->is_used != 0 &&
            ctx->cur_pic.thispoc < ctx->dpb.fs_ref[i]->poc) {
            vfmw_check(list0_idx < 16); /* 16:array size */
            fs_list0[list0_idx++] = ctx->dpb.fs_ref[i];
        }
    }

    if (list0_idx_1 < 16) { /* 16 array size */
        syn_qsort((td_void *)&fs_list0[list0_idx_1], list0_idx - list0_idx_1, sizeof(h264_frame_store *),
            compare_fs_by_poc_asc);
    }

    vfmw_check(list0_idx <= 16); /* 16 array size */
    vfmw_check(list0_idx_1 <= 16); /* 16 array size */
    for (j = 0; j < list0_idx_1; j++) {
        fs_list1[(list0_idx - list0_idx_1) + j] = fs_list0[j];
    }

    for (j = list0_idx_1; j < list0_idx; j++) {
        fs_list1[j - list0_idx_1] = fs_list0[j];
    }

    ctx->cur_slice.listx_size[0] = 0;
    ctx->cur_slice.listx_size[1] = 0;
    gen_pic_list_from_frm_list(ctx->cur_pic.structure, fs_list0, list0_idx, ctx->listx[0],
                               &(ctx->cur_slice.listx_size[0]), 0);
    gen_pic_list_from_frm_list(ctx->cur_pic.structure, fs_list1, list0_idx, ctx->listx[1],
                               &(ctx->cur_slice.listx_size[1]), 0);

    /* long term handling */
    for (i = 0; i < ctx->dpb.ltref_frames_in_buffer; i++) {
        vfmw_check(list_lt_idx < 16); /* 16:array size */
        fs_list_lt[list_lt_idx++] = ctx->dpb.fs_ltref[i];
    }

    syn_qsort((td_void *)fs_list_lt, list_lt_idx, sizeof(h264_frame_store *), compare_fs_by_lt_pic_idx_asc);

    gen_pic_list_from_frm_list(ctx->cur_pic.structure, fs_list_lt, list_lt_idx, ctx->listx[0],
                               &(ctx->cur_slice.listx_size[0]), 1);
    gen_pic_list_from_frm_list(ctx->cur_pic.structure, fs_list_lt, list_lt_idx, ctx->listx[1],
                               &(ctx->cur_slice.listx_size[1]), 1);
}
#endif

STATIC td_s32 init_list_p_slice(h264_ctx *ctx)
{
    if (ctx->cur_pic.structure == FRAME) {
        init_list_p_slice_frame(ctx);
    }
#ifdef H264_FIELD_SUPPORT
    else {
        init_list_p_slice_field(ctx);
    }
#endif
    ctx->cur_slice.listx_size[1] = 0;

    if (ctx->cur_slice.listx_size[0] == 0) {
        td_s8 need_decode_flag = 0;
        if (ctx->extra_data->output_order == VFMW_DEC_ORDER_SIMPLE_DPB) {
            need_decode_flag = 1;
        } else if (ctx->dpb.used_size > 0) {
            /* P帧找不到参考帧，DPB有帧则强解 */
            need_decode_flag = 1;
        } else if (ctx->slice_para_num > 0 &&
            ((dec_slice_param_h264 *)ctx->cur_dps->slice_head)->slice_type == I_SLICE) {
            /* P帧第一个Slice为ISlice则强解 */
            need_decode_flag = 1;
        }
        /*  CNcomment: list是空的: 如果工作于DEC_ORDER_SIMPLE_DPB模式，则用当前帧作为参考图强制解码;
                        否则返回list错误.  */
        if (need_decode_flag != 0) {
            if (ctx->cur_pic.structure == FRAME) {
                ctx->listx[0][0] = &ctx->cur_pic.frame_store->frame;
            } else if (ctx->cur_pic.structure == TOP_FIELD) {
                ctx->listx[0][0] = &ctx->cur_pic.frame_store->top_field;
            } else {
                ctx->listx[0][0] = &ctx->cur_pic.frame_store->bottom_field;
            }

            ctx->cur_slice.listx_size[0] = 1;
        } else {
            dprint(PRN_WARN, "for P slice size of list equal 0.ctx->dpb.ref_frames_in_buffer:%d \n",
                ctx->dpb.ref_frames_in_buffer);
            clear_curr_slice(ctx);
            return H264_ERR; /* INIT_LIST_ERR */
        }
    }

    return H264_OK;
}

#ifdef H264_B_FRAME_SUPPORT
STATIC td_s32 init_list_b_slice(h264_ctx *ctx)
{
    td_u32 j;
    h264_store_pic *tmp_s = TD_NULL;

    if (ctx->cur_pic.structure == FRAME) {
        init_list_b_slice_frame(ctx);
    } else {
        init_list_b_slice_field(ctx);
    }

    if ((ctx->cur_slice.listx_size[0] == ctx->cur_slice.listx_size[1]) && (ctx->cur_slice.listx_size[0] > 1)) {
        /* check if lists are identical, if yes swap first two elements of g_pListX[1] */
        td_s32 diff = 0;

        for (j = 0; j < (td_u32)(ctx->cur_slice.listx_size[0]); j++) {
            if (ctx->listx[0][j] != ctx->listx[1][j]) {
                diff = 1;
                break;
            }
        }

        if (diff == 0) {
            tmp_s = ctx->listx[1][0];
            ctx->listx[1][0] = ctx->listx[1][1];
            ctx->listx[1][1] = tmp_s;
        }
    }

    if ((ctx->cur_slice.listx_size[0] == 0) && (ctx->cur_slice.listx_size[1] == 0)) {
        dprint(PRN_ERROR, "for B slice size of two list all equal 0.\n");
        clear_curr_slice(ctx);
        return H264_ERR; /* INIT_LIST_ERR */
    }

    return H264_OK;
}
#endif

STATIC td_s32 init_list_x(h264_ctx *ctx)
{
    td_u32 i;

    if (ctx->cur_slice.slice_type == P_SLICE) {
        if (init_list_p_slice(ctx) == H264_ERR) {
            return H264_ERR;
        }
    }
#ifdef H264_B_FRAME_SUPPORT
    else { /* B-Slice */
        if (init_list_b_slice(ctx) == H264_ERR) {
            return H264_ERR;
        }
    }
#endif
    /* set vfmw_max size */
    ctx->cur_slice.listx_size[0] =
        vfmw_min(ctx->cur_slice.listx_size[0], ctx->cur_slice.num_ref_idx_l0_active_minus1 + 1);
    ctx->cur_slice.listx_size[1] =
        vfmw_min(ctx->cur_slice.listx_size[1], ctx->cur_slice.num_ref_idx_l1_active_minus1 + 1);

    /* set the unused list entries to TD_NULL */
    for (i = ctx->cur_slice.listx_size[0]; i < (MAX_LIST_SIZE); i++) {
        ctx->listx[0][i] = TD_NULL;
    }

    for (i = ctx->cur_slice.listx_size[1]; i < (MAX_LIST_SIZE); i++) {
        ctx->listx[1][i] = TD_NULL;
    }

    return H264_OK;
}

STATIC td_void dump_list_l0(const h264_ctx *ctx)
{
    td_u32 i;

    for (i = 0; i < ctx->cur_slice.listx_size[0]; i++) {
        switch (ctx->listx[0][i]->structure) {
            case FRAME:
                dprint(PRN_REF, "%d ", ctx->listx[0][i]->frame_store->poc);
                break;

            case TOP_FIELD:
                dprint(PRN_REF, "%dt ", ctx->listx[0][i]->frame_store->poc);
                break;

            case BOTTOM_FIELD:
                dprint(PRN_REF, "%db ", ctx->listx[0][i]->frame_store->poc);
                break;

            default:
                break;
        }
    }
}

STATIC td_void dump_list_l1(const h264_ctx *ctx)
{
    td_u32 i;

    for (i = 0; i < ctx->cur_slice.listx_size[1]; i++) {
        switch (ctx->listx[1][i]->structure) {
            case FRAME:
                dprint(PRN_REF, "%d ", ctx->listx[1][i]->frame_store->poc);
                break;

            case TOP_FIELD:
                dprint(PRN_REF, "%dt ", ctx->listx[1][i]->frame_store->poc);
                break;

            case BOTTOM_FIELD:
                dprint(PRN_REF, "%db ", ctx->listx[1][i]->frame_store->poc);
                break;

            default:
                break;
        }
    }
}

STATIC td_void dump_list(const h264_ctx *ctx)
{
    td_u32 i, j; /* just for write log */

    for (j = 0; j < 2; j++) { /* 2 vfmw_max num */
        for (i = 0; i < ctx->cur_slice.listx_size[j]; i++) {
            if (ctx->listx[j][i]->frame_store != TD_NULL) {
                dprint(PRN_REF, "list[%d][%d]: frame_num=%d, poc=%d\n", j, i,
                       ctx->listx[j][i]->frame_store->frame_num, ctx->listx[j][i]->frame_store->poc);
            }
        }
    }

    if (ctx->cur_slice.listx_size[0] > 0) {
        dprint(PRN_REF, "L0: ");

        dump_list_l0(ctx);

        dprint(PRN_REF, "\n");
    }

    if (ctx->cur_slice.listx_size[1] > 0) {
        dprint(PRN_REF, "L1:");

        dump_list_l1(ctx);

        dprint(PRN_REF, "\n");
    }
}

STATIC td_s32 find_nearest_poc_pic_id(const h264_ctx *ctx)
{
    td_u32 i;
    td_s32 nearest_pic_id;
    td_s32 curr_poc, nearest_poc, nearest_poc_diff, new_poc_diff;

    if (ctx->dpb.used_size < 1) {
        return -1; /* return  a err pic id */
    }

    curr_poc = ctx->cur_pic.thispoc;
    nearest_pic_id = 0;
    nearest_poc = ctx->dpb.fs[0]->poc;
    nearest_poc_diff = vfmw_abs(curr_poc - nearest_poc);

    for (i = 1; i < ctx->dpb.used_size; i++) {
        new_poc_diff = vfmw_abs(curr_poc - ctx->dpb.fs[i]->poc);
        if (new_poc_diff < nearest_poc_diff) {
            nearest_pic_id = ctx->dpb.fs[i]->logic_fs_id;
            nearest_poc_diff = new_poc_diff;
        }
    }

    return nearest_pic_id;
}

STATIC td_void find_min_ref_idx_by_frame(h264_ctx *ctx)
{
    td_u32 i, j;
    td_u32 min_ref_idx;

    for (i = 0; i < 16; i++) { /* 16 vfmw_max num */
        min_ref_idx = 32; /* 32 init idx */

        for (j = 0; j < ctx->cur_slice.listx_size[0]; j++) {
            if (i == ctx->listx[0][j]->frame_store->apc_idc) {
                /* find same apc, compare refidx */
                min_ref_idx = vfmw_min(j, min_ref_idx);
                ctx->apc.ref_idx[2 * i] = min_ref_idx; /* 2 double */
                ctx->apc.ref_idx[2 * i + 1] = min_ref_idx; /* 2 double */
            }
        }

        if (min_ref_idx >= 32) { /* 32 vfmw_max num */
            ctx->apc.ref_idx[2 * i] = 0; /* 2 double */
            ctx->apc.ref_idx[2 * i + 1] = 0; /* 2 double */
        }
    }
}
#ifdef H264_FIELD_SUPPORT
STATIC td_void find_min_ref_idx_by_field(h264_ctx *ctx)
{
    td_u32 i, j;
    td_u32 min_ref_idx;

    for (i = 0; i < 32; i++) { /* 32 vfmw_max num */
        min_ref_idx = 32; /* 32 init idx */

        for (j = 0; j < ctx->cur_slice.listx_size[0]; j++) {
            if (i == (((ctx->listx[0][j]->frame_store->apc_idc) << 1) |
                      (td_u32)(ctx->listx[0][j]->structure == 2))) { /* 2 btm fld */
                /* find same apc, compare refidx */
                min_ref_idx = vfmw_min(j, min_ref_idx);
                ctx->apc.ref_idx[i] = min_ref_idx;
            }
        }

        if (min_ref_idx >= 32) { /* 32 vfmw_max idx */
            ctx->apc.ref_idx[i] = 0;
        }
    }
}
#endif
STATIC td_void find_min_ref_idx(h264_ctx *ctx)
{
    if (ctx->cur_slice.slice_type == I_SLICE) {
        return;
    }

    /* frame reference */
    if (ctx->cur_pic.structure == 0) {
        find_min_ref_idx_by_frame(ctx);
    }
#ifdef H264_FIELD_SUPPORT
    else { /* field reference */
        find_min_ref_idx_by_field(ctx);
    }
#endif
    return;
}

STATIC td_s32 dec_list(h264_ctx *ctx)
{
    td_s32 ret;

    ctx->cur_slice.picid_nearpoc = find_nearest_poc_pic_id(ctx);
    ctx->cur_slice.picid_refidx0 = -1; /* default pic id */

    if (ctx->cur_slice.slice_type == I_SLICE) {
        ctx->cur_slice.listx_size[0] = 0;
        ctx->cur_slice.listx_size[1] = 0;
        return H264_OK;
    }

    ret = init_list_x(ctx);
    if (ret != H264_OK) {
        dprint(PRN_REF, "init list error.\n");
        return H264_ERR;
    }

    dprint(PRN_REF, "L0: list after init.\n");
    dump_list(ctx);

    recorder_list_x(ctx);
    ret = repair_list(ctx);
    if (ret != H264_OK) {
        return H264_ERR;
    }

    /* add find apc->refidx(vfmw_min) */
    find_min_ref_idx(ctx);

    ctx->cur_slice.picid_refidx0 = (ctx->listx[0][0] != TD_NULL) ? ctx->listx[0][0]->frame_store->logic_fs_id : -1;
    dprint(PRN_REF, "L0: list after reorder.\n");
    dump_list(ctx);

    return H264_OK;
}

STATIC td_void dpb_refresh(h264_ctx *ctx)
{
    td_u32 i;

    for (i = 0; i < 16; i++) { /* 16 dpb size */
        if (ctx->dpb.fs[i] != TD_NULL && ctx->dpb.fs[i]->logic_fs_id != -1) {
            fsp_clear_logic(ctx->chan_id, ctx->dpb.fs[i]->logic_fs_id, 1);
        }

        if (ctx->dpb.fs_ref[i] != TD_NULL && ctx->dpb.fs_ref[i]->logic_fs_id != -1) {
            fsp_clear_logic(ctx->chan_id, ctx->dpb.fs_ref[i]->logic_fs_id, 1);
        }

        if (ctx->dpb.fs_ltref[i] != TD_NULL && ctx->dpb.fs_ltref[i]->logic_fs_id != -1) {
            fsp_clear_logic(ctx->chan_id, ctx->dpb.fs_ltref[i]->logic_fs_id, 1);
        }

        ctx->dpb.fs[i] = ctx->dpb.fs_ref[i] = ctx->dpb.fs_ltref[i] = TD_NULL;
    }

    return;
}

STATIC td_s32 init_dpb(h264_ctx *ctx)
{
    td_u32 i, j;
    td_s32 ret;

    if (ctx == TD_NULL) {
        return H264_ERR;
    }

    clear_all_slice(ctx);

    /* dpb refresh */
    dpb_refresh(ctx);

    ctx->dpb.size = ctx->cur_sps.dpb_size_plus1 - 1;
    ctx->dpb.used_size = 0;
    ctx->dpb.ref_frames_in_buffer = 0;
    ctx->dpb.ltref_frames_in_buffer = 0;
    ctx->dpb.max_long_term_pic_idx = 0;

    /* 20120110: 在将pDirectOutBuf清空前应判断他是否还残留有数据，有说明已分配了逻辑帧存，需要释放掉 */
    if ((ctx->direct_out_buf != TD_NULL) && (ctx->old_dec_mode == VFMW_DEC_MODE_I)) {
        ret = output_frm_to_vo(ctx, ctx->direct_out_buf, 0);
        if (ret != H264_OK) {
            dprint(PRN_ERROR, "[%s][%d] output_frm_to_vo return %d\n", __func__, __LINE__, ret);
        }
    }

    if (ctx->direct_out_buf != TD_NULL) {
        fsp_clear_logic(ctx->chan_id, ctx->direct_out_buf->logic_fs_id, 1);
        if ((ctx->cur_pic.frame_store != TD_NULL) &&
            (ctx->direct_out_buf->logic_fs_id != ctx->cur_pic.frame_store->logic_fs_id)) {
            fsp_clear_logic(ctx->chan_id, ctx->cur_pic.frame_store->logic_fs_id, 1);
        }

        ctx->direct_out_buf->is_used = 0;
        ctx->direct_out_buf->is_in_dpb = 0;
        ctx->direct_out_buf = TD_NULL;
    }

    /* apc init */
    vfmw_check_sec_func(memset_s(&ctx->apc, sizeof(h264_apc), 0, sizeof(h264_apc)));

    ctx->apc.size = ctx->dpb.size;
    ctx->apc.used_size = 0;

    /* pmv slot refresh */
    for (i = 0; i < MAX_PMV_STORE; i++) {
        ctx->pmv_store_used_flag[i] = 0;
    }

    /* list refresh */
    for (i = 0; i < 2; i++) { /* 2 list num */
        for (j = 0; j < MAX_LIST_SIZE; j++) {
            ctx->listx[i][j] = TD_NULL;
        }

        ctx->cur_slice.listx_size[i] = 0;
    }

    return H264_OK;
}

STATIC td_void get_back_pic_from_vo_queue(h264_ctx *ctx)
{
    td_u32 i;
#ifdef H264_USRDATA_SUPPORT
    td_u32 usrdat_idx = 0;
#endif
    fsp_logic *logic_fs = TD_NULL;

    vfmw_check(ctx != TD_NULL);

    /* clean all frame store */
    for (i = 0; i < ctx->total_fs_num; i++) {
        ctx->frame_store[i].is_used = 0;
        ctx->frame_store[i].is_reference = 0;

        logic_fs = fsp_get_logic(ctx->chan_id, ctx->frame_store[i].logic_fs_id);
        if (logic_fs == TD_NULL) {
            continue;
        }
#ifdef H264_USRDATA_SUPPORT
        for (usrdat_idx = 0; usrdat_idx < 4; usrdat_idx++) { /* 4 usd num */
            if (uint64_ptr(logic_fs->image.usrdat[usrdat_idx]) != TD_NULL) {
                usd_release(ctx->chan_id, uint64_ptr(logic_fs->image.usrdat[usrdat_idx]));
                logic_fs->image.usrdat[usrdat_idx] = ptr_uint64(TD_NULL);
            }
        }
#endif
    }

    /* clean all pmv store */
    for (i = 0; i < ctx->total_pmv_num; i++) {
        ctx->pmv_store_used_flag[i] = 0;
    }

    return;
}

STATIC td_u32 get_rerange_flag(const h264_ctx *ctx, td_u32 *is_dpb_changed)
{
    td_u32 rerange_flag;
    h264_sps *sps;
    h264_pps *pps;
    td_u32 oldw;
    td_u32 oldh;
    td_u32 neww;
    td_u32 newh;
    td_u32 dpb_changed;
    td_u32 tmp1, tmp2;

    rerange_flag = 0;
    pps = ctx->pps[ctx->cur_slice.pic_parameter_set_id];
    sps = ctx->sps[pps->seq_parameter_set_id];

    /* 2011-11-1, oldw, oldh用之前成功分配帧存后记录的数值，这样避免了判断宽高是否
       变化时还得看AllowStartDec. 之前的写法比较费解且有隐患. */
    oldw = (td_u32)ctx->cur_width_in_mb;
    oldh = (td_u32)ctx->cur_height_in_mb;
    neww = sps->pic_width_in_mbs_minus1 + 1;
    tmp1 = (sps->pic_height_in_map_units_minus1 + 1);
    tmp2 = (2 - sps->frame_mbs_only_flag); /* 2 mbaff flag */
    vfmw_check_ret(tmp1 < VFMW_INT16_MAX, 0);
    vfmw_check_ret(tmp2 < VFMW_INT16_MAX, 0);
    newh = tmp1 * tmp2;

    /* 20111202, dpb如果变大也需要重新划分帧存，所以归到帧存尺寸变化处理 */
    if ((neww != oldw) || (newh != oldh) || (sps->dpb_size_plus1 > (ctx->dpb.size + 1))) {
        rerange_flag = 1;
    }

    dpb_changed = 0;
    dpb_changed |= ((sps->dpb_size_plus1 == ctx->cur_sps.dpb_size_plus1) ? 0 : 1);
    dpb_changed |= (td_u32)(ctx->cur_sps.seq_parameter_set_id != sps->seq_parameter_set_id);
    dpb_changed |= rerange_flag;
    *is_dpb_changed = dpb_changed;

    return rerange_flag;
}

STATIC td_void dec_poc_order_cnt_type_0_1st(h264_ctx *ctx)
{
    h264_sps *sps;
    h264_pps *pps;
    td_u32 max_pic_order_cnt_lsb;
    td_s32 idr_flag = (td_s32)(ctx->cur_slice.nal_unit_type == NALU_TYPE_IDR);

    pps = ctx->pps[ctx->cur_slice.pic_parameter_set_id];
    sps = ctx->sps[pps->seq_parameter_set_id];
    max_pic_order_cnt_lsb = (1 << (sps->log2_max_pic_order_cnt_lsb_minus4 + 4)); /* 4 is num */

    if (idr_flag != 0) {
        ctx->cur_poc.prev_pic_order_cnt_msb = 0;
        ctx->cur_poc.prev_pic_order_cnt_lsb = 0;
    } else {
        if (ctx->cur_poc.last_has_mmco_5 != 0) {
            if (ctx->cur_poc.last_pic_bottom_field != 0) {
                ctx->cur_poc.prev_pic_order_cnt_msb = 0;
                ctx->cur_poc.prev_pic_order_cnt_lsb = 0;
            } else {
                ctx->cur_poc.prev_pic_order_cnt_msb = 0;
                ctx->cur_poc.prev_pic_order_cnt_lsb = (td_u32)ctx->cur_poc.toppoc;
            }
        }
    }

    /* Calculate the MSBs of current picture */
    if (ctx->cur_poc.pic_order_cnt_lsb < ctx->cur_poc.prev_pic_order_cnt_lsb &&
        (ctx->cur_poc.prev_pic_order_cnt_lsb - ctx->cur_poc.pic_order_cnt_lsb) >=
        (max_pic_order_cnt_lsb / 2)) { /* 2 is num */
        ctx->cur_poc.pic_order_cnt_msb = ctx->cur_poc.prev_pic_order_cnt_msb + (td_s32)max_pic_order_cnt_lsb;
    } else if ((ctx->cur_poc.pic_order_cnt_lsb > ctx->cur_poc.prev_pic_order_cnt_lsb) &&
               (ctx->cur_poc.pic_order_cnt_lsb - ctx->cur_poc.prev_pic_order_cnt_lsb) >
               (max_pic_order_cnt_lsb / 2)) { /* 2 is num */
        ctx->cur_poc.pic_order_cnt_msb = ctx->cur_poc.prev_pic_order_cnt_msb - (td_s32)max_pic_order_cnt_lsb;
    } else {
        ctx->cur_poc.pic_order_cnt_msb = ctx->cur_poc.prev_pic_order_cnt_msb;
    }
}

STATIC td_void dec_poc_order_cnt_type_0_2nd(h264_ctx *ctx)
{
    if (ctx->cur_slice.field_pic_flag == 0) {
        /* frame pix */
        ctx->cur_poc.toppoc = ctx->cur_poc.pic_order_cnt_msb + (td_s32)ctx->cur_poc.pic_order_cnt_lsb;
        ctx->cur_poc.bottompoc = ctx->cur_poc.toppoc + ctx->cur_poc.delta_pic_order_cnt_bottom;
        ctx->cur_poc.this_poc = ctx->cur_poc.framepoc = ((ctx->cur_poc.toppoc < ctx->cur_poc.bottompoc) ?
                                                         ctx->cur_poc.toppoc : ctx->cur_poc.bottompoc);
    }
#ifdef H264_FIELD_SUPPORT
    else if (!ctx->cur_slice.bottom_field_flag) {
        /* top field */
        ctx->cur_poc.this_poc = ctx->cur_poc.toppoc = ctx->cur_poc.pic_order_cnt_msb +
                                                      (td_s32)ctx->cur_poc.pic_order_cnt_lsb;
    } else {
        /* bottom field */
        ctx->cur_poc.this_poc = ctx->cur_poc.bottompoc = ctx->cur_poc.pic_order_cnt_msb +
                                                         (td_s32)ctx->cur_poc.pic_order_cnt_lsb;
    }
#endif
    ctx->cur_poc.framepoc = ctx->cur_poc.this_poc;

    if (ctx->cur_poc.frame_num != ctx->cur_poc.prev_frame_num) {
        ctx->cur_poc.prev_frame_num = ctx->cur_poc.frame_num;
    }

    if (ctx->cur_slice.nal_ref_idc != 0) {
        ctx->cur_poc.prev_pic_order_cnt_lsb = ctx->cur_poc.pic_order_cnt_lsb;
        ctx->cur_poc.prev_pic_order_cnt_msb = ctx->cur_poc.pic_order_cnt_msb;
    }
}

STATIC td_void dec_poc_order_cnt_type_0(h264_ctx *ctx)
{
    /* 1st */
    dec_poc_order_cnt_type_0_1st(ctx);

    /* 2nd */
    dec_poc_order_cnt_type_0_2nd(ctx);

    return;
}

STATIC td_void dec_poc_order_cnt_type_1_1st(h264_ctx *ctx)
{
    h264_sps *sps;
    h264_pps *pps;
    td_u32 max_frame_num;
    td_s32 idr_flag = (td_s32)(ctx->cur_slice.nal_unit_type == NALU_TYPE_IDR);

    pps = ctx->pps[ctx->cur_slice.pic_parameter_set_id];
    sps = ctx->sps[pps->seq_parameter_set_id];
    max_frame_num = 1 << (sps->log2_max_frame_num_minus4 + 4); /* 4 is num */

    if (idr_flag != 0) {
        ctx->cur_poc.frame_num_offset = 0; /* first pix of IDRGOP, */
        /* g_CurrPOC.delta_pic_order_cnt[0]=0;  ignore first delta */
    } else {
        if (ctx->cur_poc.last_has_mmco_5 != 0) {
            ctx->cur_poc.prev_frame_num_offset = 0;
            ctx->cur_poc.prev_frame_num = 0;
        }

        if (ctx->cur_poc.frame_num < ctx->cur_poc.prev_frame_num) {
            /* not first pix of IDRGOP */
            ctx->cur_poc.frame_num_offset = (td_u32)ctx->cur_poc.prev_frame_num_offset + max_frame_num;
        } else {
            ctx->cur_poc.frame_num_offset = (td_u32)ctx->cur_poc.prev_frame_num_offset;
        }
    }
}

STATIC td_void dec_poc_order_cnt_type_1_2nd(h264_ctx *ctx)
{
    h264_sps *sps;
    h264_pps *pps;

    pps = ctx->pps[ctx->cur_slice.pic_parameter_set_id];
    sps = ctx->sps[pps->seq_parameter_set_id];

    if (sps->num_ref_frames_in_pic_order_cnt_cycle != 0) {
        ctx->cur_poc.abs_frame_num = ctx->cur_poc.frame_num_offset + ctx->cur_poc.frame_num;
    } else {
        ctx->cur_poc.abs_frame_num = 0;
    }

    if ((ctx->cur_slice.nal_ref_idc == 0) && (ctx->cur_poc.abs_frame_num > 0)) {
        ctx->cur_poc.abs_frame_num--;
    }
}

STATIC td_void dec_poc_order_cnt_type_1_3rd(h264_ctx *ctx)
{
    h264_sps *sps;
    h264_pps *pps;
    td_s32 i;

    pps = ctx->pps[ctx->cur_slice.pic_parameter_set_id];
    sps = ctx->sps[pps->seq_parameter_set_id];

    ctx->cur_poc.expected_delta_per_poc_cycle = 0;

    for (i = 0; i < (td_s32)sps->num_ref_frames_in_pic_order_cnt_cycle; i++) {
        ctx->cur_poc.expected_delta_per_poc_cycle += sps->offset_for_ref_frame[i];
    }

    if (ctx->cur_poc.abs_frame_num != 0) {
        if (sps->num_ref_frames_in_pic_order_cnt_cycle == 0) {
            dprint(PRN_ERROR, "%s sps->num_ref_frames_in_pic_order_cnt_cycle is zero!\n", __func__);
            return;
        }

        ctx->cur_poc.poc_cycle_cnt = (td_s32)((ctx->cur_poc.abs_frame_num - 1) /
                                     sps->num_ref_frames_in_pic_order_cnt_cycle);
        ctx->cur_poc.frame_num_in_poc_cycle = (td_s32)((ctx->cur_poc.abs_frame_num - 1) %
                                              sps->num_ref_frames_in_pic_order_cnt_cycle);
        ctx->cur_poc.expected_poc = ctx->cur_poc.poc_cycle_cnt * ctx->cur_poc.expected_delta_per_poc_cycle;
        for (i = 0; i <= (td_s32)ctx->cur_poc.frame_num_in_poc_cycle; i++) {
            ctx->cur_poc.expected_poc += sps->offset_for_ref_frame[i];
        }
    } else {
        ctx->cur_poc.expected_poc = 0;
    }

    if (ctx->cur_slice.nal_ref_idc == 0) {
        ctx->cur_poc.expected_poc += sps->offset_for_non_ref_pic;
    }

    if (ctx->cur_slice.field_pic_flag == 0) {
        ctx->cur_poc.toppoc = ctx->cur_poc.expected_poc + ctx->cur_poc.delta_pic_order_cnt[0]; /* frame pix */
        ctx->cur_poc.bottompoc = ctx->cur_poc.toppoc + sps->offset_for_top_to_bottom_field +
                                 ctx->cur_poc.delta_pic_order_cnt[1];
        ctx->cur_poc.this_poc = ctx->cur_poc.framepoc = ((ctx->cur_poc.toppoc < ctx->cur_poc.bottompoc) ?
                                                         ctx->cur_poc.toppoc : ctx->cur_poc.bottompoc);
    }
#ifdef H264_FIELD_SUPPORT
    else if (!(ctx->cur_slice.bottom_field_flag)) { /* top field */
        ctx->cur_poc.this_poc = ctx->cur_poc.toppoc =
            ctx->cur_poc.expected_poc + ctx->cur_poc.delta_pic_order_cnt[0];
    } else { /* bottom field */
        ctx->cur_poc.this_poc = ctx->cur_poc.bottompoc = ctx->cur_poc.expected_poc +
            sps->offset_for_top_to_bottom_field + ctx->cur_poc.delta_pic_order_cnt[0];
    }
#endif
    ctx->cur_poc.framepoc = ctx->cur_poc.this_poc;

    ctx->cur_poc.prev_frame_num = ctx->cur_poc.frame_num;
    ctx->cur_poc.prev_frame_num_offset = (td_s32)ctx->cur_poc.frame_num_offset;
}

STATIC td_void dec_poc_order_cnt_type_1(h264_ctx *ctx)
{
    /* 1st */
    dec_poc_order_cnt_type_1_1st(ctx);

    /* 2nd */
    dec_poc_order_cnt_type_1_2nd(ctx);

    /* 3rd */
    dec_poc_order_cnt_type_1_3rd(ctx);

    return;
}

STATIC td_void dec_poc_order_cnt_type_2(h264_ctx *ctx)
{
    h264_sps *sps;
    h264_pps *pps;
    td_u32 max_frame_num;
    td_s32 idr_flag = (td_s32)(ctx->cur_slice.nal_unit_type == NALU_TYPE_IDR);

    pps = ctx->pps[ctx->cur_slice.pic_parameter_set_id];
    sps = ctx->sps[pps->seq_parameter_set_id];
    max_frame_num = 1 << (sps->log2_max_frame_num_minus4 + 4); /* 4 is num */

    if (idr_flag != 0) {
        ctx->cur_poc.frame_num_offset = 0; /* first pix of IDRGOP, */
        ctx->cur_poc.this_poc = ctx->cur_poc.framepoc = ctx->cur_poc.toppoc = ctx->cur_poc.bottompoc = 0;
    } else {
        if (ctx->cur_poc.last_has_mmco_5 != 0) {
            ctx->cur_poc.prev_frame_num = 0;
            ctx->cur_poc.prev_frame_num_offset = 0;
        }

        if (ctx->cur_poc.frame_num < ctx->cur_poc.prev_frame_num) {
            ctx->cur_poc.frame_num_offset = (td_u32)ctx->cur_poc.prev_frame_num_offset + max_frame_num;
        } else {
            ctx->cur_poc.frame_num_offset = (td_u32)ctx->cur_poc.prev_frame_num_offset;
        }

        ctx->cur_poc.abs_frame_num = ctx->cur_poc.frame_num_offset + ctx->cur_poc.frame_num;

        if (ctx->cur_slice.nal_ref_idc == 0) {
            ctx->cur_poc.this_poc = (td_s32)((ctx->cur_poc.abs_frame_num << 1) - 1); /* 1 double */
        } else {
            ctx->cur_poc.this_poc = (td_s32)(ctx->cur_poc.abs_frame_num << 1); /* 1 double */
        }

        if (ctx->cur_slice.field_pic_flag == 0) {
            ctx->cur_poc.toppoc = ctx->cur_poc.bottompoc = ctx->cur_poc.framepoc = ctx->cur_poc.this_poc;
        }
#ifdef H264_FIELD_SUPPORT
        else if (!ctx->cur_slice.bottom_field_flag) {
            ctx->cur_poc.toppoc = ctx->cur_poc.framepoc = ctx->cur_poc.this_poc;
        } else {
            ctx->cur_poc.bottompoc = ctx->cur_poc.framepoc = ctx->cur_poc.this_poc;
        }
#endif
    }

    ctx->cur_poc.prev_frame_num = ctx->cur_poc.frame_num;
    ctx->cur_poc.prev_frame_num_offset = (td_s32)ctx->cur_poc.frame_num_offset;

    return;
}

STATIC td_void dec_poc(h264_ctx *ctx)
{
    h264_sps *sps;
    h264_pps *pps;

    pps = ctx->pps[ctx->cur_slice.pic_parameter_set_id];
    sps = ctx->sps[pps->seq_parameter_set_id];

    switch (sps->pic_order_cnt_type) {
        case 0: /* 0 poc type val */
            dec_poc_order_cnt_type_0(ctx);
            break;

        case 1: /* 1 poc type val */
            dec_poc_order_cnt_type_1(ctx);
            break;

        case 2: /* 2 poc type val */
            dec_poc_order_cnt_type_2(ctx);
            break;

        default:
            /* error must occurs */
            break;
    }

    return;
}

STATIC td_void calc_pic_num_type_frame(h264_ctx *ctx)
{
    td_u32 i;
    h264_sps *sps = ctx->sps[ctx->pps[ctx->cur_slice.pic_parameter_set_id]->seq_parameter_set_id];
    const td_u32 max_frame_num = 1 << (sps->log2_max_frame_num_minus4 + 4); /* 4 is num */

    for (i = 0; i < ctx->dpb.ref_frames_in_buffer; i++) {
        if ((ctx->dpb.fs_ref[i]->is_reference == 3) && /* 3 frame, field pair */
            (ctx->dpb.fs_ref[i]->frame.is_short_term == 1) &&
            (ctx->dpb.fs_ref[i]->frame.is_long_term == 0)) {
            if (ctx->dpb.fs_ref[i]->frame_num > ctx->cur_pic.frame_num) {
                ctx->dpb.fs_ref[i]->frame_num_wrap = (td_s32)(ctx->dpb.fs_ref[i]->frame_num - max_frame_num);
            } else {
                ctx->dpb.fs_ref[i]->frame_num_wrap = (td_s32)ctx->dpb.fs_ref[i]->frame_num;
            }

            ctx->dpb.fs_ref[i]->frame.pic_num = ctx->dpb.fs_ref[i]->frame_num_wrap;
        }
    }

    /* update long_term_pic_num */
    for (i = 0; i < ctx->dpb.ltref_frames_in_buffer; i++) {
        if (ctx->dpb.fs_ltref[i]->is_reference == 3) { /* 3 field pair */
            if ((ctx->dpb.fs_ltref[i]->frame.is_short_term == 0) &&
                (ctx->dpb.fs_ltref[i]->frame.is_long_term == 1)) {
                ctx->dpb.fs_ltref[i]->frame.long_term_pic_num = (td_s32)ctx->dpb.fs_ltref[i]->long_term_frame_idx;
            }
        }
    }

    return;
}

#ifdef H264_FIELD_SUPPORT
STATIC td_void calc_long_term_pic_num_type_field(h264_ctx *ctx, td_s32 add_top, td_s32 add_bottom)
{
    td_u32 i, lt_ref_idx;

    /* update long_term_pic_num */
    for (i = 0; i < ctx->dpb.ltref_frames_in_buffer; i++) {
        if (ctx->dpb.fs_ltref[i]->is_reference != 0) {
            lt_ref_idx = ctx->dpb.fs_ltref[i]->long_term_frame_idx;
            if (((ctx->dpb.fs_ltref[i]->is_reference & 1) != 0) &&
                (ctx->dpb.fs_ltref[i]->top_field.is_short_term == 0) &&
                (ctx->dpb.fs_ltref[i]->top_field.is_long_term == 1)) { /* include top field */
                vfmw_check(lt_ref_idx < VFMW_INT16_MAX);
                ctx->dpb.fs_ltref[i]->top_field.long_term_pic_num =
                    (td_s32)(2 * lt_ref_idx + (td_u32)add_top); /* 2 is num */
            }

            if (((ctx->dpb.fs_ltref[i]->is_reference & 2) != 0) && /* 2 mb size */
                (ctx->dpb.fs_ltref[i]->bottom_field.is_short_term == 0) &&
                (ctx->dpb.fs_ltref[i]->bottom_field.is_long_term == 1)) {
                vfmw_check(lt_ref_idx < VFMW_INT16_MAX);
                ctx->dpb.fs_ltref[i]->bottom_field.long_term_pic_num =
                    (td_s32)(2 * lt_ref_idx + (td_u32)add_bottom); /* 2 is num */
            }
        }
    }
}

STATIC td_void calc_pic_num_type_field(h264_ctx *ctx)
{
    td_u32 i;
    h264_sps *sps = ctx->sps[ctx->pps[ctx->cur_slice.pic_parameter_set_id]->seq_parameter_set_id];
    const td_u32 max_frame_num = 1 << (sps->log2_max_frame_num_minus4 + 4); /* 4 is num */
    const td_s32 max_check_frame_num = 1 << 16; /* 16 is num */
    td_s32 add_top, add_bottom;

    add_top = (ctx->cur_pic.structure == TOP_FIELD) ? 1 : 0;
    add_bottom = (ctx->cur_pic.structure == BOTTOM_FIELD) ? 1 : 0;

    for (i = 0; i < ctx->dpb.ref_frames_in_buffer; i++) {
        if (ctx->dpb.fs_ref[i]->is_reference != 0) {
            if (ctx->dpb.fs_ref[i]->frame_num > ctx->cur_pic.frame_num) {
                ctx->dpb.fs_ref[i]->frame_num_wrap = (td_s32)(ctx->dpb.fs_ref[i]->frame_num - max_frame_num);
            } else {
                ctx->dpb.fs_ref[i]->frame_num_wrap = (td_s32)ctx->dpb.fs_ref[i]->frame_num;
            }

            if ((ctx->dpb.fs_ref[i]->is_reference & 1) &&
                (ctx->dpb.fs_ref[i]->top_field.is_short_term == 1) &&
                (ctx->dpb.fs_ref[i]->top_field.is_long_term == 0)) { /* include top field */
                vfmw_check(ctx->dpb.fs_ref[i]->frame_num_wrap <= max_check_frame_num);
                ctx->dpb.fs_ref[i]->top_field.pic_num = (2 * ctx->dpb.fs_ref[i]->frame_num_wrap); /* 2 num */
                ctx->dpb.fs_ref[i]->top_field.pic_num += add_top;
            }

            if ((ctx->dpb.fs_ref[i]->is_reference & 2) && /* 2 is sh ref */
                (ctx->dpb.fs_ref[i]->bottom_field.is_short_term == 1) &&
                (ctx->dpb.fs_ref[i]->bottom_field.is_long_term == 0)) { /* include bottom field */
                vfmw_check(ctx->dpb.fs_ref[i]->frame_num_wrap <= max_check_frame_num);
                ctx->dpb.fs_ref[i]->bottom_field.pic_num = (2 * ctx->dpb.fs_ref[i]->frame_num_wrap); /* 2 num */
                ctx->dpb.fs_ref[i]->bottom_field.pic_num += add_bottom;
            }
        }
    }

    calc_long_term_pic_num_type_field(ctx, add_top, add_bottom);
}
#endif
STATIC td_void calc_pic_num(h264_ctx *ctx)
{
    /* pic num calculate */
    if (ctx->cur_pic.structure == FRAME) {
        calc_pic_num_type_frame(ctx);
    }
#ifdef H264_FIELD_SUPPORT
    else { /* field */
        calc_pic_num_type_field(ctx);
    }
#endif
}

STATIC td_s32 is_out_dpb(const h264_ctx *ctx, const h264_frame_store *frame_store)
{
    td_u32 i;

    if (frame_store == TD_NULL) {
        return 1;
    }

    for (i = 0; i < ctx->dpb.used_size; i++) {
        if (ctx->dpb.fs[i] == frame_store) {
            return 0;
        }
    }

    return 1;
}

STATIC td_s32 proc_single_field(h264_ctx *ctx)
{
    vfmw_check_ret(ctx != TD_NULL, H264_ERR);
    vfmw_check_ret(ctx->extra_data != TD_NULL, H264_ERR);

    if (ctx->cur_pic.frame_store != TD_NULL &&
        ctx->old_dec_mode != VFMW_DEC_MODE_I &&
        ctx->cur_pic.frame_store->is_used != 0 &&
        ctx->cur_pic.frame_store->is_used != 3) { /* 3 both used */
        /* only prev field needs proc by next frm */
        get_image_para(ctx, ctx->cur_pic.frame_store);
    }

    /* output prev single field */
    if (ctx->prev_pic.structure != FRAME &&
        ctx->cur_pic.non_existing == 0 &&
        ctx->cur_pic.frame_store != TD_NULL &&
        ctx->cur_pic.pic_output_flag == 1 &&
        (ctx->extra_data->output_order == VFMW_DEC_ORDER ||
        ctx->first_frame_displayed == FIRST_DISP_UNPAIRED ||
        ctx->is_stream_end_flag == 1)) {
        vfmw_image *img_out = TD_NULL;
        td_s32 logic_id = (td_s32)ctx->prev_pic.image_id;
        fsp_logic *logic_fs = TD_NULL;

        if (ctx->first_frame_displayed == FIRST_DISP_UNPAIRED) {
            /* pvr xplay swicth first frm is single field */
            ctx->first_frame_displayed = FIRST_DISP_OUTPUT;
        }

        if (ctx->is_stream_end_flag == 1) {
            logic_fs = fsp_get_logic(ctx->chan_id, logic_id);
            if (logic_fs == TD_NULL ||
                logic_fs->dec_state.data == FSP_DEC_STATE_IDLE ||
                logic_fs->disp_state != FSP_DISP_STATE_ALLOC) {
                /* last pic dec err or output can not output */
                return H264_ERR;
            }
        }

        img_out = fsp_get_image(ctx->chan_id, logic_id);
        get_image_para(ctx, ctx->cur_pic.frame_store);

        if (img_out != TD_NULL) {
            fsp_set_disp(ctx->chan_id, logic_id, 1);

            if (voq_insert(ctx->chan_id, img_out) != VOQ_OK) {
                fsp_set_disp(ctx->chan_id, logic_id, 0);
            }
        }
    }

    return H264_OK;
}

#ifdef H264_FIELD_SUPPORT
STATIC td_void process_pair_field(h264_ctx *ctx, td_u32 structure)
{
    if ((ctx->old_dec_mode == VFMW_DEC_MODE_I || ctx->prev_pic.frame_num == ctx->cur_slice.frame_num) &&
        ((ctx->cur_slice.nal_ref_idc == 0 && ctx->prev_pic.nal_ref_idc == 0) ||
         (ctx->cur_slice.nal_ref_idc != 0 && ctx->prev_pic.nal_ref_idc != 0)) &&
        ((ctx->pic_counter - ctx->prev_store_pic_counter) <= 2)) { /* 2, a number */
            ctx->cur_pic.field_pair_flag = 1;

            if (ctx->cur_pic.frame_store != TD_NULL) {
                if ((structure == BOTTOM_FIELD) &&
                    (ctx->cur_pic.frame_store->is_used != 1)) { /* 1 bot used */
                    ctx->cur_pic.field_pair_flag = 0;
                }

                if ((structure == TOP_FIELD) &&
                    (ctx->cur_pic.frame_store->is_used != 2)) { /* 2 bot used */
                    ctx->cur_pic.field_pair_flag = 0;
                }
            } else {
                ctx->cur_pic.field_pair_flag = 0;
            }
        }

    return;
}
#endif

STATIC td_s32 alloc_frame_find_empty_frame_store(h264_ctx *ctx)
{
    td_u32 i;

    ctx->cur_pic.state = EMPTY;

    for (i = 0; i < ctx->total_fs_num; i++) {
        if ((ctx->frame_store[i].is_used == 0) && (is_out_dpb(ctx, &ctx->frame_store[i]) == 1)) {
            ctx->cur_pic.state = PIC_DECODING;
            ctx->cur_pic.frame_store = &ctx->frame_store[i];
            ctx->cur_pic.frame_store->err_level = 0;
            /* error_level必须标记为有错，否则在解gap，或者其它原因其实并未解码但发生了StorePicInDPB()的操作，
            则这个本当不该输出的图有可能被输出 */
            ctx->cur_pic.err_level = 101; /* 101 init error */
            ctx->cur_pic.fs_image.error_level = 0;
            ctx->cur_pic.frame_store->mode_skip_flag = 0;
            ctx->cur_pic.frame_store->is_displayed = 0;
            ctx->cur_pic.frame_store->is_reference = 0;
            ctx->cur_pic.frame_store->is_in_dpb = 1;
            ctx->cur_pic.frame_store->fo_state = FO_STATE_NA;
            /* 添加一些变量清空，便于存入DPB时的正确性 */
            break;
        }
    }

    if (ctx->cur_pic.state != PIC_DECODING) {
        dprint(PRN_ERROR, "FS_ALLOC_ERR, clear_all\n");
        clear_all(ctx, 1);
        return SYN_PARSE_RESOURCE_INVALID; /* FS_ALLOC_ERR */
    }

    return H264_OK;
}

STATIC td_s32 alloc_frame_find_empty_logic_store(h264_ctx *ctx, td_s32 *logic_fs_id_pointer, td_s32 is_dec_gap)
{
    fsp_logic *logic_fs = TD_NULL;
    h264_sps *sps = TD_NULL;
    h264_pps *pps = TD_NULL;

    pps = ctx->pps[ctx->cur_slice.pic_parameter_set_id];
    sps = ctx->sps[pps->seq_parameter_set_id];

    *logic_fs_id_pointer = fsp_new_logic(ctx->chan_id, FSP_TYPE_EXCLU);
    logic_fs = fsp_get_logic(ctx->chan_id, *logic_fs_id_pointer);
    if (sps->gaps_in_frame_num_value_allowed_flag == 0 && is_dec_gap == 1 && logic_fs != TD_NULL) {
        dprint(PRN_ERROR, "%s dec gap error occurs.\n", __func__);
        logic_fs->ref_err = 1;
    }

    if (*logic_fs_id_pointer >= 0 && logic_fs != TD_NULL) {
        /* FrameStore与fs_image不再具有一一对应关系，为延续已有处理，把新的image拷贝过来 */
        vfmw_check_sec_func(memcpy_s(&ctx->cur_pic.fs_image, sizeof(vfmw_image),
                                     &logic_fs->image, sizeof(logic_fs->image)));

        ctx->cur_pic.frame_store->logic_fs_id = *logic_fs_id_pointer;
    } else {
        ctx->cur_pic.state = EMPTY;
        ctx->cur_pic.frame_store->is_in_dpb = 0;
        ctx->cur_pic.frame_store = TD_NULL;
        return SYN_PARSE_RESOURCE_INVALID;
    }

    return H264_OK;
}

STATIC td_s32 alloc_frame_store_single_field(h264_ctx *ctx, td_s32 *logic_fs_id_pointer, td_s32 is_dec_gap)
{
    td_s32 ret;

    proc_single_field(ctx);

    /* allocate frame_store */
    ret = alloc_frame_find_empty_frame_store(ctx);
    if (ret != H264_OK) {
        return ret;
    }

    /* FrameStore申请到之后, 还要新建逻辑帧存，这样才能得到真正的地址 */
    ret = alloc_frame_find_empty_logic_store(ctx, logic_fs_id_pointer, is_dec_gap);
    if (ret != H264_OK) {
        return ret;
    }

    return H264_OK;
}

STATIC td_void alloc_frame_store_set_field_pair_flag(h264_ctx *ctx, td_s32 is_dec_gap,
    td_u32 structure)
{
    vfmw_unused(is_dec_gap);
    vfmw_unused(structure);
    if (ctx->old_dec_mode == VFMW_DEC_MODE_I) {
        ctx->cur_pic.field_pair_flag = 0;
    }
#ifdef H264_FIELD_SUPPORT
    else {
        /* 成对场的判断 */
        ctx->cur_pic.field_pair_flag = 0;

        if ((structure == BOTTOM_FIELD && ctx->prev_pic.structure == TOP_FIELD) ||
            (structure == TOP_FIELD && ctx->prev_pic.structure == BOTTOM_FIELD)) {
            process_pair_field(ctx, structure);
        } else {
            ctx->cur_pic.field_pair_flag = 0;
        }
    }
#endif
}

/*************************************************************************
*      函数原型 :  td_s32 alloc_frame_store(td_void)
*      功能描述 :  给当前图像分配一个帧存空间,pmv槽位
*      参数说明 :  无
*      返回值   ： DEC_NORMAL:配成功;
*                  FS_ALLOC_ERR:能分配到PMV槽位,解码错误得不可恢复，
*                               需要重新启动解码（从某种程度上说该
*                               类错误出现，必定是出现了bug需要定位
*                               解决）
*                  OUT_FLAG: 检测到适配层需要退出，返回该标志
*************************************************************************/
STATIC td_s32 alloc_frame_store(h264_ctx *ctx, td_s32 is_dec_gap)
{
    td_s32 logic_fs_id = 0;
    td_u32 structure =
        (ctx->cur_slice.field_pic_flag != 0) ?
        (ctx->cur_slice.bottom_field_flag != 0 ? BOTTOM_FIELD : TOP_FIELD) : FRAME;

    alloc_frame_store_set_field_pair_flag(ctx, is_dec_gap, structure);

    if (ctx->cur_pic.field_pair_flag == 0 || ctx->cur_pic.frame_store == TD_NULL) {
        if (alloc_frame_store_single_field(ctx, &logic_fs_id, is_dec_gap) != H264_OK) {
            return SYN_PARSE_RESOURCE_INVALID;
        }
    }

    ctx->cur_pic.structure = (td_u8)structure;
    /* 给frame.structure指定存储结构很重要，给dnr的帧场属性就用这个. */
    ctx->cur_pic.frame_store->frame.structure = (ctx->cur_slice.field_pic_flag != 0) ? FIELD_PAIR : FRAME;

    if (ctx->cur_pic.field_pair_flag == 1) {
        ctx->prev_pic.structure = FRAME;
        ctx->prev_pic.frame_num = ctx->cur_slice.frame_num;
        ctx->prev_pic.nal_ref_idc = ctx->cur_slice.nal_ref_idc;
        ctx->prev_pic.pic_counter = ctx->pic_counter;
        ctx->prev_pic.image_id = (td_u32)logic_fs_id;
        ctx->prev_pic.frame_store_flag = 0;
    } else {
        /* update prevpic for next field pair judge */
        ctx->prev_pic.structure = (td_u8)structure;
        ctx->prev_pic.frame_num = ctx->cur_slice.frame_num;
        ctx->prev_pic.nal_ref_idc = ctx->cur_slice.nal_ref_idc;
        ctx->prev_pic.pic_counter = ctx->pic_counter;
        ctx->prev_pic.image_id = (td_u32)logic_fs_id;

        if (structure != FRAME) {
            ctx->prev_pic.frame_store_flag = 1;
        } else {
            ctx->prev_pic.frame_store_flag = 0;
        }
    }

    return H264_OK;
}
STATIC td_s32 h264_judge_gap_frame_valid(h264_ctx *ctx)
{
    td_u32 i;
    fsp_logic *logic_fs = TD_NULL;

    for (i = 0; i < ctx->dpb.used_size; i++) {
        if (ctx->dpb.fs[i] == TD_NULL) {
            dprint(PRN_ERROR, "pos: %d, pHevcCtx->DPB.fs[i]: is null\n", i);
            continue;
        }

        logic_fs = fsp_get_logic(ctx->chan_id, ctx->cur_pic.frame_store->logic_fs_id);
        if (logic_fs == TD_NULL) {
            dprint(PRN_ERROR, "%s %d, FSP_GetLogicFs failed! pstLFs is TD_NULL\n", __func__, __LINE__);
            return H264_ERR;
        }

        if (ctx->dpb.fs[i]->poc == ctx->cur_pic.thispoc && logic_fs->is_dec_gap == TD_TRUE &&
            ctx->dpb.fs[i]->logic_fs_id != ctx->cur_pic.frame_store->logic_fs_id) {
            dprint(PRN_ERROR, "same poc(%d) but gap fs id %d not equal logic id %d, remove the gap frame\n",
                ctx->cur_pic.thispoc, ctx->cur_pic.frame_store->logic_fs_id,
                ctx->dpb.fs[i]->logic_fs_id);
            fsp_clear_logic(ctx->chan_id, ctx->cur_pic.frame_store->logic_fs_id, 1);
            ctx->cur_pic.frame_store = TD_NULL;
            return H264_ERR;
        }
    }

    return H264_OK;
}

STATIC td_void dec_gap_process_para_init(h264_ctx *ctx, td_s32 unused_short_term_frame_num,
    td_s32 nal_ref_idc)
{
    ctx->cur_pic.nal_unit_type = NALU_TYPE_SLICE;
    ctx->cur_pic.frame_store->frame_num = (td_u32)unused_short_term_frame_num;
    ctx->cur_pic.frame_num = (td_u32)unused_short_term_frame_num;
    ctx->cur_pic.frame_store->frame.pic_num = unused_short_term_frame_num;
    ctx->cur_pic.nal_ref_idc = (td_u32)nal_ref_idc;
    ctx->cur_pic.frame_store->frame.long_term_pic_num = 0;
    ctx->cur_pic.frame_store->is_displayed = 1;
    ctx->cur_pic.frame_store->is_in_dpb = 0;
    ctx->cur_pic.non_existing = ctx->cur_pic.frame_store->non_existing = 1;

    /* CurrMark */
    ctx->cur_pic.mark_para->adaptive_ref_pic_marking_mode_flag = 0;
}

STATIC td_void dec_gap_process(h264_ctx *ctx, td_u32 max_poc_in_dpb)
{
    td_s32 ret;
    td_u32 count = 0;
    td_s32 curr_frame_num = (td_s32)ctx->cur_slice.frame_num;
    const td_s32 max_frame_num = 1 << (ctx->cur_sps.log2_max_frame_num_minus4 + 4); /* 4 is num */
    td_s32 unused_short_term_frame_num = (td_s32)((ctx->cur_slice.prev_frame_num + 1) % (td_u32)max_frame_num);
    td_s32 nal_ref_idc = (ctx->cur_sps.gaps_in_frame_num_value_allowed_flag == 1) ? 2 : 0; /* 2 ref idc */
    td_u32 count_tmp = 0;

    while (curr_frame_num != unused_short_term_frame_num && ctx->dpb.used_size >= count) {
        count_tmp++;
        if (count_tmp > MAX_CYCLE_NUM) {
            dprint(PRN_ERROR, "line %d: loop count more > %d break!\n", __LINE__, MAX_CYCLE_NUM);
            break;
        }

        /* clear prev_pic structure to fobid pair between gap */
        ctx->prev_pic.structure = 0;

        if (alloc_frame_store(ctx, 1) != H264_OK) {
            dprint(PRN_ERROR, "cann't allocate frame store when gap find\n");
            return;
        }

        if (h264_judge_gap_frame_valid(ctx) == H264_ERR) {
            ctx->cur_slice.prev_frame_num = (td_u32)unused_short_term_frame_num;
            unused_short_term_frame_num = (unused_short_term_frame_num + 1) % max_frame_num;
            continue;
        }

        /* pic para init */
        dec_gap_process_para_init(ctx, unused_short_term_frame_num, nal_ref_idc);

        if (ctx->cur_sps.pic_order_cnt_type != 0) {
            ctx->cur_poc.frame_num = (td_u32)unused_short_term_frame_num;
            dec_poc(ctx);
            ctx->cur_pic.thispoc = ctx->cur_poc.this_poc;
            ctx->cur_pic.framepoc = ctx->cur_poc.framepoc;
            ctx->cur_pic.toppoc = ctx->cur_poc.toppoc;
            ctx->cur_pic.bottompoc = ctx->cur_poc.bottompoc;

            if ((max_poc_in_dpb < (td_u32)(ctx->cur_pic.toppoc)) &&
                (max_poc_in_dpb < (td_u32)(ctx->cur_pic.bottompoc))) {
                count++;
            }
        }

        ctx->cur_pic.is_long_term = 0;
        ret = store_pic_in_dpb(ctx, TD_NULL);
        if (ret != H264_OK) {
            dprint(PRN_ERROR, "line %d: store gap pic err, ret=%d\n", __LINE__, ret);
            return;
        }

        ctx->cur_slice.prev_frame_num = (td_u32)unused_short_term_frame_num;

        unused_short_term_frame_num = (unused_short_term_frame_num + 1) % max_frame_num;
    }

    return;
}

STATIC td_s32 dec_gap(h264_ctx *ctx)
{
    td_s32 curr_frame_num = (td_s32)ctx->cur_slice.frame_num;
    const td_s32 max_frame_num = 1 << (ctx->cur_sps.log2_max_frame_num_minus4 + 4); /* 4 is num */
    const td_s32 unused_short_term_frame_num =
        (td_s32)((ctx->cur_slice.prev_frame_num + 1) % (td_u32)max_frame_num);
    td_s32 tmp1 = ctx->cur_poc.delta_pic_order_cnt[0];
    td_s32 tmp2 = ctx->cur_poc.delta_pic_order_cnt[1];
    td_s32 tmp3 = (td_s32)ctx->mark[ctx->cur_mark_idx].adaptive_ref_pic_marking_mode_flag;
    td_u32 pos = 0;
    td_s32 max_poc_in_dpb = 0;

    (td_void)(curr_frame_num); /* avoid to build err when printing is not supported. */
    (td_void)(unused_short_term_frame_num);
    ctx->cur_pic.mark_para = &(ctx->mark[ctx->cur_mark_idx]); /* avoid coredump */

    /* 如果DPB为空则不应该解GAP，此时的GAP是假像. 但处理此假GAP时不能返回H264_ERR，因为除了这个GAP不处理
       之外其它InitPic操作还得继续, 尤其是OldSLice的相关信息若不刷新会导致后续slice判断帧边界出错 */
    if (ctx->dpb.used_size == 0) {
        dprint(PRN_ERROR, "GAP found while dpb is empty!\n");
        return H264_OK;
    }

    /* save poc para of currslice */
    ctx->cur_poc.delta_pic_order_cnt[0] = ctx->cur_poc.delta_pic_order_cnt[1] = 0;

    get_max_poc(ctx, &max_poc_in_dpb, &pos);

    dec_gap_process(ctx, (td_u32)max_poc_in_dpb);

    ctx->cur_poc.delta_pic_order_cnt[0] = tmp1;
    ctx->cur_poc.delta_pic_order_cnt[1] = tmp2;
    ctx->cur_pic.mark_para->adaptive_ref_pic_marking_mode_flag = (td_u8)tmp3;

    return H264_OK;
}

STATIC td_void update_image_param(vfmw_image *dest_image, const vfmw_image *src_image)
{
    td_ulong lock_flag;

    dest_image->image_width = src_image->image_width;
    dest_image->image_height = src_image->image_height;
    dest_image->disp_info.disp_width = src_image->disp_info.disp_width;
    dest_image->disp_info.disp_height = src_image->disp_info.disp_height;
    dest_image->bottom_offset = src_image->bottom_offset;

    dest_image->bit_depth = src_image->bit_depth;
    if (dest_image->pts == VFMW_INVALID_PTS) {
        dest_image->pts = src_image->pts;
    }
    dest_image->user_tag = src_image->user_tag;
    dest_image->disp_time = src_image->disp_time;
    dest_image->disp_enable_flag = src_image->disp_enable_flag;
    dest_image->disp_frame_distance = src_image->disp_frame_distance;
    dest_image->distance_before_first_frame = src_image->distance_before_first_frame;
    dest_image->gop_num = src_image->gop_num;
    dest_image->dv_dual_layer = src_image->dv_dual_layer;
    dest_image->afd.active_format = src_image->afd.active_format;
    dest_image->afd.active_format_flag = src_image->afd.active_format_flag;
    dest_image->aspect_height = src_image->aspect_height;
    dest_image->aspect_width = src_image->aspect_width;
    dest_image->is_fld_flag = src_image->is_fld_flag;
    fsp_img_format_lock(&lock_flag);
    dest_image->format = src_image->format;
    fsp_img_format_unlock(&lock_flag);

}

STATIC td_void h264_dec_empty_cur_pic_fs(h264_ctx *ctx)
{
    if (ctx->cur_pic.state != EMPTY) {
        if (ctx->cur_pic.frame_store != TD_NULL) {
            if (ctx->cur_pic.frame_store->is_used == 0) {
                fsp_clear_logic(ctx->chan_id, ctx->cur_pic.frame_store->logic_fs_id, 1);
                ctx->cur_pic.frame_store->is_used = 0;
                ctx->cur_pic.frame_store->is_in_dpb = 0;
            }
        } else {
            dprint(PRN_ERROR, "%s %d frame_store is null!!\n", __FUNCTION__, __LINE__);
        }

        ctx->cur_pic.state = EMPTY;
    }

    return;
}

STATIC td_s32 h264_dec_resize_arrange_mem(h264_ctx *ctx, td_u32 rerange_flag)
{
    td_u8 flag;
    td_s32 ret;
    td_bool is_arrange_mem = TD_TRUE;
    td_u32 align_value = 16;
    td_u32 image_width, image_height;
    td_u32 max_dec_w_align_up, max_dec_h_align_up;

    td_u32 bit_depth = vfmw_max(ctx->cur_sps.bit_depth_luma, ctx->cur_sps.bit_depth_chroma);

    /* when dpb size is gotten, frame_store can use is gotten too */
    ctx->total_pmv_num = ctx->cur_sps.dpb_size_plus1;

    vfmw_check_ret(ctx->cur_sps.pic_width_in_mbs_minus1 + 1 < VFMW_INT16_MAX, H264_ERR);
    vfmw_check_ret(ctx->cur_sps.pic_height_in_map_units_minus1 + 1 < VFMW_INT16_MAX, H264_ERR);
    vfmw_check_ret(2 - ctx->cur_sps.frame_mbs_only_flag < VFMW_INT16_MAX, H264_ERR); /* 2 mbaff flag */
    image_width = (ctx->cur_sps.pic_width_in_mbs_minus1 + 1) * 16; /* 16 mb size */
    image_height = (ctx->cur_sps.pic_height_in_map_units_minus1 + 1) *
                   (2 - ctx->cur_sps.frame_mbs_only_flag) * 16; /* 2 mbaff flag 16 mb size */

    flag = ((rerange_flag != 0) && (ctx->cur_width_in_mb == 0));
    if (flag != 0) {
        /* when dpb size is gotten, frame_store can use is gotten too */
        ctx->total_pmv_num = ctx->cur_sps.dpb_size_plus1;

        flag = ((image_width > (td_u32)(ctx->extra_data->max_width)) ||
            (image_height > (td_u32)(ctx->extra_data->max_height)));
        vfmw_check_ret_print(flag == 0, H264_ERR, "resolution error.\n");

        flag = ((td_u32)(ctx->extra_data->max_ref_num) < ctx->dpb.size);
        vfmw_check_ret_print(flag == 0, H264_ERR, "max_ref_frame_num < ctx->dpb.size\n");

        if (ctx->extra_data->support_b_frame == 0) {
            ctx->total_pmv_num = 1;
        }
    }

    max_dec_w_align_up = vfmw_align_up(ctx->extra_data->max_dec_width, align_value);
    max_dec_h_align_up = vfmw_align_up(ctx->extra_data->max_dec_height, align_value);
    if ((ctx->extra_data->is_max_buf_size_en == TD_TRUE) &&
        (image_width * image_height <= max_dec_w_align_up * max_dec_h_align_up) &&
        (ctx->cur_width_in_mb != 0)) {
        is_arrange_mem = TD_FALSE;
    } else {
        is_arrange_mem = TD_TRUE;
    }

    if (is_arrange_mem == TD_TRUE) {
        ret = h264_arrange_vahb_mem(ctx, image_width, image_height,&ctx->total_fs_num, ctx->total_pmv_num, bit_depth);
        if (ret == SYN_PARSE_RESOURCE_INVALID) {
            return ret;
        }
    }

    /* 记录当前的解码宽高，目的是方便后续判断宽高是否发生变化 */
    ctx->cur_width_in_mb = (td_s32)(image_width / 16); /* 16 mb size */
    ctx->cur_height_in_mb = (td_s32)(image_height / 16); /* 16 mb size */

    return H264_OK;
}

STATIC td_s32 h264_dec_resize_acquire_mem(h264_ctx *ctx, td_u32 rerange_flag, td_u32 dpb_changed)
{
    td_u8 flag;
    td_s32 ret;

    flag = ((dpb_changed != 0) || (ctx->extra_data->wait_fs_flag == 1));
    if (flag != 0) {
        ctx->seq_cnt++;
        flag = ((rerange_flag != 0) || (ctx->extra_data->wait_fs_flag == 1));
        if (flag != 0) {
            get_back_pic_from_vo_queue(ctx);
        } else {
            ret = flush_dpb(ctx);
            vfmw_check_ret_print(ret == H264_OK, H264_ERR, "line %d: flush dpb return %d\n", __LINE__, ret);
        }

        ret = init_dpb(ctx);
        vfmw_check_ret_print(ret == H264_OK, H264_ERR, "line %d: init dpb return %d\n", __LINE__, ret);

        if (flag != 0) {
            ret = h264_dec_resize_arrange_mem(ctx, rerange_flag);
            if (ret != H264_OK) {
                return ret;
            }
        }
    }

    return H264_OK;
}
#ifdef H264_PVR_SUPPORT
STATIC td_s32 process_cur_frame_num_bigger_pre(h264_ctx *ctx, td_u32 max_frame_num)
{
    td_s32 ret;

    if (ctx->cur_sps.dpb_size_plus1 < (ctx->cur_slice.frame_num - ctx->cur_slice.prev_frame_num)) {
        if (ctx->cur_slice.slice_type == I_SLICE) {
            ctx->cur_slice.prev_frame_num = (ctx->cur_slice.frame_num == 0) ? max_frame_num - 1 :
                                            ctx->cur_slice.frame_num - 1;
            ret = flush_dpb(ctx);
            vfmw_check_ret_print(ret == H264_OK, H264_ERR, "line %d: flush dpb return %d\n", __LINE__, ret);
        }

        /* 根据外部配置的错误阈值来决定是否丢帧 */
        if ((ctx->cur_slice.slice_type != I_SLICE) && (ctx->out_error_thr == 0)) {
            h264_clear_curr_pic(ctx);

            if (g_apple_airplay_mirrorinig_flag == 0) {
                clear_dpb(ctx);
            }

            dprint(PRN_ERROR, "line %d: frame gap(=%d) > dpb size(=%d)\n", __LINE__,
                (ctx->cur_slice.frame_num - ctx->cur_slice.prev_frame_num), ctx->cur_sps.dpb_size_plus1);
            return H264_ERR;
        }
    }

    return H264_OK;
}

STATIC td_s32 process_pre_frame_num_bigger_cur(h264_ctx *ctx, td_u32 max_frame_num)
{
    td_s32 ret;

    if (ctx->cur_sps.dpb_size_plus1 <
        (ctx->cur_slice.frame_num + max_frame_num - ctx->cur_slice.prev_frame_num)) {
        if (ctx->cur_slice.slice_type == I_SLICE) {
            ctx->cur_slice.prev_frame_num = (ctx->cur_slice.frame_num == 0) ? max_frame_num - 1 :
                                            ctx->cur_slice.frame_num - 1;
            ret = flush_dpb(ctx);
            vfmw_check_ret_print(ret == H264_OK, H264_ERR, "line %d: flush dpb return %d\n", __LINE__, ret);
        }

        /* 根据外部配置的错误阈值来决定是否丢帧 */
        if ((ctx->cur_slice.slice_type != I_SLICE) && (ctx->out_error_thr == 0)) {
            h264_clear_curr_pic(ctx);

            if (g_apple_airplay_mirrorinig_flag == 0) {
                clear_dpb(ctx);
            }

            dprint(PRN_ERROR, "line %d: frame gap(=%d) > dpb size(=%d)\n", __LINE__,
                   (ctx->cur_slice.frame_num - ctx->cur_slice.prev_frame_num), ctx->cur_sps.dpb_size_plus1);
            return H264_ERR;
        }
    }

    return H264_OK;
}
STATIC td_s32 h264_dec_pvr_flush_dpb(h264_ctx *ctx, td_u32 max_frame_num)
{
    td_u8 flag;
    td_s32 ret = H264_OK;

    flag = ((ctx->extra_data->more_gap_en == 0) &&
            (!((ctx->extra_data->pvr_info.b_frm_ref_flag == 1) &&
               (ctx->extra_data->pvr_info.idr_flag == 0) &&
               (ctx->extra_data->pvr_info.continuous_flag == 0) &&
               (!((ctx->extra_data->fast_forward_backward_speed >= -1024) && /* 1024 speed */
                  (ctx->extra_data->fast_forward_backward_speed <= 1024)))))); /* 1024 speed */
    if (flag) {
        if (ctx->cur_slice.frame_num > ctx->cur_slice.prev_frame_num) {
            ret = process_cur_frame_num_bigger_pre(ctx, max_frame_num);
        } else {
            ret = process_pre_frame_num_bigger_cur(ctx, max_frame_num);
        }
    }

    return ret;
}
#endif
#ifdef H264_USRDATA_SUPPORT
STATIC td_void h264_dec_usr_data(h264_ctx *ctx)
{
    td_s32 uidxpic;
    td_u32 uidxsei;

    for (uidxpic = 0; uidxpic < 4; uidxpic++) { /* 4 usd num */
        if ((uint64_ptr(ctx->cur_pic.fs_image.usrdat[uidxpic]) == TD_NULL) &&
            (ctx->total_usrdat_num > 0)) {
            /* add usrdata */
            ctx->cur_pic.fs_image.usrdat[uidxpic] = ptr_uint64(ctx->usr_dat_array[0]);
            for (uidxsei = 0; uidxsei < ctx->total_usrdat_num - 1; uidxsei++) {
                ctx->usr_dat_array[uidxsei] = ctx->usr_dat_array[uidxsei + 1];
            }
            vfmw_check(ctx->total_usrdat_num <= MAX_USERDATA_NUM);
            ctx->usr_dat_array[ctx->total_usrdat_num - 1] = TD_NULL;
            ctx->total_usrdat_num--;
        }
    }
    if (ctx->total_usrdat_num > 0) {
        for (uidxpic = (td_s32)ctx->total_usrdat_num; uidxpic > 0; uidxpic--) {
            usd_release(ctx->chan_id, ctx->usr_dat_array[uidxpic - 1]);
            ctx->usr_dat_array[uidxpic - 1] = TD_NULL;
        }
    }
    return;
}
#endif

STATIC td_u32 h264_dec_source_format(const h264_ctx *ctx, td_u32 structure_field, td_u32 structure_frame)
{
    td_u8 flag1;
    td_u8 flag2;
    td_u32 source_format = 0;

    flag1 = ((ctx->cur_sps.mb_adaptive_frame_field_flag != 0) && (structure_frame > 0));
    flag2 = ((ctx->cur_sps.frame_mbs_only_flag == 1) || (structure_frame > 0 && structure_field == 0));

    if (flag1 != 0) {
        source_format = 1; /* MBAFF not support, set to interlace */
    } else if (flag2 != 0) {
        flag1 = ((ctx->pic_timing_sei.pic_struct == 3) || /* 3 top first */
                 (ctx->pic_timing_sei.pic_struct == 4)); /* 4 btm first */
        source_format = (flag1 != 0) ? 1 : 0;
    } else if (structure_field > 0) {
        source_format = 1;
    }

    return source_format;
}

STATIC td_void h264_dec_structure_update(const h264_ctx *ctx, td_u32 *structure_field, td_u32 *structure_frame)
{
    td_u8 flag;
    td_u32 structure_field_tmp = *structure_field;
    td_u32 structure_frame_tmp = *structure_frame;

    flag = (ctx->cur_sps.pic_order_cnt_type != 1 && ctx->cur_slice.first_mb_in_slice == 0);
    if (flag != 0) {
        if (ctx->cur_slice.field_pic_flag == 1) {
            structure_field_tmp++;
            *structure_field = structure_field_tmp;
        } else {
            structure_frame_tmp++;
            *structure_frame = structure_frame_tmp;
        }
    }

    return;
}

STATIC td_s32 init_pic_refresh_sps_pps(h264_ctx *ctx, td_u32 rerange_flag, td_u32 dpb_changed)
{
    td_u8 flag;
    td_s32 ret;
    h264_sps *sps = TD_NULL;
    h264_pps *pps = TD_NULL;

    pps = ctx->pps[ctx->cur_slice.pic_parameter_set_id];
    sps = ctx->sps[pps->seq_parameter_set_id];

    flag = ((ctx->cur_sps.seq_parameter_set_id != sps->seq_parameter_set_id) || (sps->is_refreshed != 0));
    if (flag != 0) {
        /* new sps copy and init dpb */
        sps->is_refreshed = 0;
        vfmw_check_sec_func(memmove_s(&(ctx->cur_sps), sizeof(h264_sps), sps, sizeof(h264_sps)));
    }

    fsp_refresh(ctx->chan_id);

    ret = h264_dec_resize_acquire_mem(ctx, rerange_flag, dpb_changed);
    vfmw_check_ret_print(ret == H264_OK, ret, "%s, %d return %d\n", __func__, __LINE__, ret);

    flag = ((ctx->cur_pps.pic_parameter_set_id != pps->pic_parameter_set_id) || (pps->is_refreshed != 0));
    if (flag != 0) {
        pps->is_refreshed = 0;
        vfmw_check_sec_func(memmove_s(&(ctx->cur_pps), sizeof(h264_pps), pps, sizeof(h264_pps)));
    }

    return H264_OK;
}

STATIC td_s32 init_pic_gap_process(h264_ctx *ctx)
{
    td_u8 flag;
    td_s32 ret;
    const td_u32 max_frame_num = 1 << (ctx->cur_sps.log2_max_frame_num_minus4 + 4); /* 4 is num */

    if (ctx->cur_poc.last_has_mmco_5 != 0) {
        ctx->cur_slice.prev_frame_num = 0;
        ctx->prev_pic.frame_num = 0;
    }

    /* 如果运行于DEC_ORDER_SIMPLE_DPB模式，不处理gap. 20110729 */
    /* 如果运行于I帧模式，不处理gap, 20110830 */
    flag = ((ctx->old_dec_mode != VFMW_DEC_MODE_I) &&
           (ctx->extra_data->output_order != VFMW_DEC_ORDER_SIMPLE_DPB) &&
           (ctx->extra_data->output_order != VFMW_DEC_ORDER_SKYPLAY) &&
           (ctx->cur_slice.frame_num != ctx->cur_slice.prev_frame_num) &&
           (ctx->cur_slice.frame_num != ((ctx->cur_slice.prev_frame_num + 1) % max_frame_num)));
    if (flag != 0) {
        if (ctx->cur_sps.gaps_in_frame_num_value_allowed_flag == 0 && ctx->cur_slice.slice_type != I_SLICE) {
            dprint(PRN_ERROR, "%s report dec gap\n", __func__);
            report_dec_err(ctx->chan_id, ctx->extra_data->pts, SYNTAX_NO_REF);
        }
#ifdef H264_PVR_SUPPORT
        ret = h264_dec_pvr_flush_dpb(ctx, max_frame_num);
        vfmw_check_ret_print(ret == H264_OK, H264_ERR, "%s, %d return error\n", __func__, __LINE__);
#endif
        ret = dec_gap(ctx);
        vfmw_check_ret_print(ret == H264_OK, ret, "line %d: dec gap failed\n", __LINE__);
    }

    if (ctx->cur_slice.nal_ref_idc != 0) {
        ctx->cur_slice.prev_frame_num = ctx->cur_slice.frame_num;
    }

    return H264_OK;
}

STATIC td_void init_pic_get_pic_mb(h264_ctx *ctx)
{
    vfmw_check((2 - ctx->cur_sps.frame_mbs_only_flag) < VFMW_INT16_MAX); /* 2 mbaff flag */
    vfmw_check((ctx->cur_sps.pic_height_in_map_units_minus1 + 1) < VFMW_INT16_MAX);
    vfmw_check((ctx->cur_sps.pic_width_in_mbs_minus1 + 1) < VFMW_INT16_MAX);
    ctx->cur_pic.pic_height_in_mb = ((ctx->cur_sps.pic_height_in_map_units_minus1 + 1) *
        (2 - ctx->cur_sps.frame_mbs_only_flag)) / (1 + ctx->cur_slice.field_pic_flag); /* 2 mbaff flag */
    ctx->cur_pic.frame_height_in_mb = ((ctx->cur_sps.pic_height_in_map_units_minus1 + 1) *
        (2 - ctx->cur_sps.frame_mbs_only_flag)); /* 2 mbaff flag */
    ctx->cur_pic.pic_width_in_mb = (ctx->cur_sps.pic_width_in_mbs_minus1 + 1);
    vfmw_check(ctx->cur_pic.pic_width_in_mb < VFMW_INT16_MAX);
    vfmw_check(ctx->cur_pic.pic_height_in_mb < VFMW_INT16_MAX);
    ctx->cur_pic.pic_mbs = ctx->cur_pic.pic_width_in_mb * ctx->cur_pic.pic_height_in_mb;
}

STATIC td_void init_pic_parameter_part_one(h264_ctx *ctx)
{
    h264_pps *pps = ctx->pps[ctx->cur_slice.pic_parameter_set_id];
    h264_sps *sps = ctx->sps[pps->seq_parameter_set_id];

    ctx->cur_pic.mode_skip_flag = 0;
    ctx->cur_pic.state = PIC_DECODING;
    ctx->cur_pic.nal_unit_type = ctx->cur_slice.nal_unit_type;
    ctx->cur_pic.frame_num = ctx->cur_slice.frame_num;
    ctx->cur_pic.nal_ref_idc = ctx->cur_slice.nal_ref_idc;
    ctx->cur_pic.thispoc = ctx->cur_poc.this_poc;
    ctx->cur_pic.framepoc = ctx->cur_poc.framepoc;
    ctx->cur_pic.toppoc = ctx->cur_poc.toppoc;
    ctx->cur_pic.bottompoc = ctx->cur_poc.bottompoc;
    ctx->cur_pic.non_existing = 0;
    ctx->cur_pic.is_long_term = ctx->cur_pic.is_short_term = 0;
    ctx->cur_pic.stream_base_addr = MAX_VAL_UADDR;

    if (ctx->cur_slice.field_pic_flag != 0) {
        ctx->cur_slice.field_pic_flag = 1; /* just avoid 1+ctx->cur_slice.field_pic_flag == 0 */
    }

    init_pic_get_pic_mb(ctx);

    ctx->cur_pic.entropy_coding_mode_flag = ctx->cur_pps.entropy_coding_mode_flag;
    ctx->cur_pic.mbaff_frame_flag = (ctx->cur_slice.field_pic_flag == 0) &&
        (ctx->cur_sps.mb_adaptive_frame_field_flag != 0);

    if (ctx->cur_slice.nal_ref_idc != 0) {
        ctx->cur_slice.prev_frame_num = ctx->cur_slice.frame_num;
    }

    /* add mark para */
    ctx->cur_pic.mark_para = &(ctx->mark[ctx->cur_mark_idx]);

    /* !!! PTS的处理还不能这么简单。暂时如此 */
    dprint(PRN_PTS, "Pic pts %lld\n", ctx->extra_data->pts);
    ctx->cur_pic.fs_image.pts = ctx->extra_data->pts;
    ctx->extra_data->pts = VFMW_INVALID_PTS;

    ctx->cur_pic.fs_image.user_tag = ctx->extra_data->user_tag;
    ctx->cur_pic.fs_image.disp_time = ctx->extra_data->disp_time;
    ctx->cur_pic.fs_image.disp_enable_flag = ctx->extra_data->disp_enable_flag;
    ctx->cur_pic.fs_image.disp_frame_distance = ctx->extra_data->disp_frame_distance;
    ctx->cur_pic.fs_image.distance_before_first_frame = ctx->extra_data->distance_before_1st_frame;
    ctx->cur_pic.fs_image.gop_num = ctx->extra_data->gop_num;
    ctx->cur_pic.fs_image.dv_dual_layer = ctx->extra_data->dv_dual_layers;
    ctx->cur_pic.fs_image.bit_depth = sps->bit_depth_luma;
    ctx->cur_pic.fs_image.image_width = (ctx->cur_sps.pic_width_in_mbs_minus1 + 1) << 4; /* 4 bit ofs */
    vfmw_check((ctx->cur_sps.pic_height_in_map_units_minus1 + 1) < VFMW_INT16_MAX);
    vfmw_check((2 - ctx->cur_sps.frame_mbs_only_flag) < VFMW_INT16_MAX); /* 2 mbaff flag */
    ctx->cur_pic.fs_image.image_height = ((ctx->cur_sps.pic_height_in_map_units_minus1 + 1) *
                                       (2 - ctx->cur_sps.frame_mbs_only_flag)) << 4; /* 2 flag 4 bit */
    ctx->cur_pic.fs_image.top_offset = sps->frame_crop_top_offset;
    ctx->cur_pic.fs_image.bottom_offset = sps->frame_crop_bottom_offset;
    ctx->err_nal_cnt = 0;

    return;
}

STATIC td_s32 init_pic_parameter_part_two(h264_ctx *ctx)
{
    td_s32 dar;
    td_u32 chroma_format, i;
    td_u32 structure_field = 0;
    td_u32 structure_frame = 0;
    td_u32 source_format;
    td_ulong lock_flag;

    dar = (ctx->cur_sps.vui_seq_parameters.aspect_ratio & 0x7);
    chroma_format = (ctx->cur_sps.chroma_format_idc == 1) ? 0 : 1; /* 4:2:0, 0;  4:0:0, 1; */

    h264_dec_structure_update(ctx, &structure_field, &structure_frame);

    source_format = h264_dec_source_format(ctx, structure_field, structure_frame);

    ctx->ref_pic_single_size = 0;
    for (i = 0; i < H264_APC_SIZE; i++) {
        ctx->ref_pic_single_fsp_id[i] = INVALID_IDX;
    }

    fsp_img_format_lock(&lock_flag);
    ctx->cur_pic.fs_image.format.frame_type = 0;
    ctx->cur_pic.fs_image.format.sample_type = (td_u8)chroma_format;
    ctx->cur_pic.fs_image.format.video_format = (td_u8)ctx->cur_sps.vui_seq_parameters.video_format;
    ctx->cur_pic.fs_image.format.source_format = (td_u8)source_format;
    ctx->cur_pic.fs_image.format.field_valid_flag = 0;
    ctx->cur_pic.fs_image.format.top_field_first = 2; /* 2 init val */
    ctx->cur_pic.fs_image.format.aspect_ratio = (td_u8)dar;
    if (ctx->pic_timing_sei.pic_struct == 5 || ctx->pic_timing_sei.pic_struct == 6) { /* 5 :a number 6 :a number */
        ctx->cur_pic.fs_image.format.repeat_first_field = 1;
    }
    fsp_img_format_unlock(&lock_flag);

    if (ctx->cur_slice.slice_type == I_SLICE) {
        ctx->afd_info_data.active_format_flag = ctx->cur_afd_info.active_format_flag;
        ctx->afd_info_data.active_format = ctx->cur_afd_info.active_format;
    }

    ctx->cur_pic.fs_image.afd.active_format_flag = ctx->afd_info_data.active_format_flag;
    ctx->cur_pic.fs_image.afd.active_format = ctx->afd_info_data.active_format;

    ctx->cur_afd_info.active_format_flag = TD_FALSE;
    ctx->cur_afd_info.active_format = 0;

    /* set display AspectWidth and AspectHeight */
    syn_set_aspect_ratio(&(ctx->cur_pic.fs_image), (vfmw_vdec_dar)dar);

    ctx->cur_pic.fs_image.is_fld_flag = (ctx->cur_pic.structure != 0);

    if (ctx->cur_pic.frame_store == TD_NULL) {
        dprint(PRN_ERROR, "%s %d TD_NULL == ctx->cur_pic.frame_store!!\n", __FUNCTION__, __LINE__);
        return H264_ERR;
    }

    return H264_OK;
}

STATIC td_s32 init_pic_parameter_part_three(h264_ctx *ctx)
{
    td_s8 store_type;
    vfmw_image *img = TD_NULL;
    h264_pps *pps = ctx->pps[ctx->cur_slice.pic_parameter_set_id];
    h264_sps *sps = ctx->sps[pps->seq_parameter_set_id];

    ctx->cur_pic.fs_image.disp_info.disp_center_x = sps->disp_center_x;
    ctx->cur_pic.fs_image.disp_info.disp_center_y = sps->disp_center_y;
    ctx->cur_pic.fs_image.disp_info.disp_width = sps->disp_width;
    ctx->cur_pic.fs_image.disp_info.disp_height = sps->disp_height;

    store_type = (ctx->cur_pic.fs_image.is_fld_flag == 0) ? FSP_STORE_TYPE_FRAME : FSP_STORE_TYPE_FIELD;
    fsp_set_store_type(ctx->chan_id, ctx->cur_pic.frame_store->logic_fs_id, store_type);
#ifdef H264_USRDATA_SUPPORT
    h264_dec_usr_data(ctx);
#endif
    ctx->total_usrdat_num = 0;
    ctx->cur_pic.pic_type = 0; /* 默认Islice，在每个slice解析时更新 */

    img = fsp_get_image(ctx->chan_id, ctx->cur_pic.frame_store->logic_fs_id);
    vfmw_check_ret_print(img != TD_NULL, H264_ERR, "%s %d TD_NULL == img is TD_NULL!!\n", __FUNCTION__, __LINE__);

    update_image_param(img, &ctx->cur_pic.fs_image);

    calc_pic_num(ctx);

    return H264_OK;
}

STATIC td_void dec_check_i_mode_change_clear_dpb(h264_ctx *ctx)
{
    td_u32 ipb_dec_mode = (ctx->advanced_cfg & 0x70000) >> 16; /* 16 bit ofs */

    if ((ctx->cur_slice.slice_type == I_SLICE) &&
        (ctx->old_dec_mode != ipb_dec_mode) &&
        (ctx->old_dec_mode == VFMW_DEC_MODE_I ||
        ctx->old_dec_mode == VFMW_DEC_MODE_FIRST_I) &&
        (ipb_dec_mode != VFMW_DEC_MODE_I) &&
        (ipb_dec_mode == VFMW_DEC_MODE_IP ||
        ipb_dec_mode == VFMW_DEC_MODE_NORMAL ||
        ipb_dec_mode == VFMW_DEC_MODE_DROP_INVALID_B)) {
        ctx->old_dec_mode = ipb_dec_mode;
        clear_dpb(ctx);
    }

    return;
}

#ifndef H264_RESOLUTION_CHANGE_SUPPORT
STATIC td_s32 check_resolution_change(h264_ctx *ctx)
{
    h264_sps *sps;
    h264_pps *pps;
    td_u32 oldw,oldh,neww,newh;
    td_u32 max_dec_w_align_up, max_dec_h_align_up;
    td_u32 align_value = 16;

    pps = ctx->pps[ctx->cur_slice.pic_parameter_set_id];
    sps = ctx->sps[pps->seq_parameter_set_id];

    if (ctx->cur_width_in_mb == 0 ||  ctx->cur_height_in_mb == 0) {
        return H264_OK;
    }

    oldw = (td_u32)ctx->cur_width_in_mb;
    oldh = (td_u32)ctx->cur_height_in_mb;
    neww = sps->pic_width_in_mbs_minus1 + 1;
    newh = (sps->pic_height_in_map_units_minus1 + 1) * (2 - sps->frame_mbs_only_flag); /* 2 mbaff flag */;
    max_dec_w_align_up = vfmw_align_up(ctx->extra_data->max_dec_width, align_value);
    max_dec_h_align_up = vfmw_align_up(ctx->extra_data->max_dec_height, align_value);

    if (ctx->extra_data->is_max_buf_size_en == TD_TRUE) {
        if (neww * align_value * newh * align_value <= max_dec_w_align_up * max_dec_h_align_up) {
            return H264_OK;
        } else {
            dprint(PRN_ERROR, "%s, %d max_buf_size_en is true but new[%d, %d] > max[%d %d]\n", __func__, __LINE__,
                neww * align_value, newh * align_value, max_dec_w_align_up, max_dec_h_align_up);
            report_unsupport(ctx->chan_id);
            return H264_ERR;
        }
    }

    if (oldw != neww || oldh != newh) {
        dprint(PRN_ERROR, "%s, %d resolution change is unsupport! [%d, %d] ==> [%d %d]\n", __func__, __LINE__,
            oldw * align_value, oldh * align_value, neww * align_value, newh * align_value);
        report_unsupport(ctx->chan_id);
        return H264_ERR;
    }

    return H264_OK;
}
#endif
/*************************************************************************
*      函数原型 :  td_s32 init_pic(h264_ctx *ctx)
*      功能描述 :  依据当前slice的语法元素检查SPS、PPS的更新；
*                  对frame_num进行处理；
*                  初始化新图像的参数；
*      参数说明 :  无
*      返回值   ： SPS更新时，VHB空间分配分配失败的标志
分配帧存空间、pmv空间失败的标志；
*                  gap处理时调用DPB模块的返回值；
*                  gap过大的标志。
*************************************************************************/
STATIC td_s32 init_pic(h264_ctx *ctx)
{
    td_s32 ret;
    td_u32 rerange_flag;
    td_u32 dpb_changed;

#if defined(VFMW_H264_SLC_LDY_SUPPORT)
    td_u8 flag;
    ctx->slc_ldy_cnt = 0;
    flag = (ctx->cur_pic.state != EMPTY && ctx->cur_dps->sed_state == SED_TRY);
    if (flag) {
        return H264_OK;
    }
#endif

    h264_dec_empty_cur_pic_fs(ctx);

    dec_check_i_mode_change_clear_dpb(ctx);

    dpb_changed = 0;
    rerange_flag = get_rerange_flag(ctx, &dpb_changed);
#ifndef H264_RESOLUTION_CHANGE_SUPPORT
    ret = check_resolution_change(ctx);
    vfmw_check_ret(ret == H264_OK, H264_ERR);
#endif
    /* 1 refresh SPS and PPS */
    /* sps id变化,sps一定更新,同id号的SPS自身有更新,当前slice是重新解码的恢复点 */
    ret = init_pic_refresh_sps_pps(ctx, rerange_flag, dpb_changed);
    if (ret != H264_OK) {
        return ret;
    }

    /* 2 gap process */
    ret = init_pic_gap_process(ctx);
    if (ret != H264_OK) {
        return ret;
    }

    /* 3 calculate poc of g_CurrPic */
    ctx->cur_poc.frame_num = ctx->cur_slice.frame_num;
    dec_poc(ctx);

    /* 4 分配帧存和PMV空间 */
    ret = alloc_frame_store(ctx, 0);
    if (ret != H264_OK) {
        return ret;
    }

    /* 5 初始化当前解码图像的参数 */
    /* 可以根据成对场而减少重复赋值 */
    /* pmv_address_idc,structure, *frame_store均在帧存分配函数中产生 */
    init_pic_parameter_part_one(ctx);

    ret = init_pic_parameter_part_two(ctx);
    if (ret != H264_OK) {
        return ret;
    }

    ret = init_pic_parameter_part_three(ctx);
    if (ret != H264_OK) {
        return ret;
    }

    return H264_OK;
}

STATIC td_s32 write_pic_msg(h264_ctx *ctx)
{
    td_s32 ret;

#ifdef VFMW_H264_SLC_LDY_SUPPORT
    if (ctx->cur_dps->ldy_mode == LDY_SED && ctx->cur_pic.nal_ref_idc &&
        ctx->extra_data->dec_mode != VFMW_DEC_MODE_I) {
        fsp_set_ref(ctx->chan_id, ctx->cur_pic.frame_store->logic_fs_id, 1);
    }
    if (ctx->cur_dps->ldy_mode == LDY_SLC) {
        ctx->slc_ldy_cnt++;
    }
#endif

    ret = dps_set_pic_param(ctx->chan_id, ctx, ctx->cur_dps);
    if (ret == DPS_ERR) {
        h264_clear_curr_pic(ctx);
        dprint(PRN_ERROR, "dps_set_pic_param ERR!\n");
        return H264_ERR;
    }

    return H264_OK;
}

/* BEGIN: Added  2010/9/27 */
STATIC td_void update_pic_qp_inf(td_s32 slc_qp, h264_store_pic *cur_pic)
{
    cur_pic->max_qp = (cur_pic->max_qp < slc_qp) ? slc_qp : cur_pic->max_qp;
    cur_pic->min_qp = (cur_pic->min_qp > slc_qp) ? slc_qp : cur_pic->min_qp;

    return;
}

VFMW_ACC_SLICE_H264 STATIC td_s32 write_slice_msg(h264_ctx *ctx)
{
    td_u32 slice_qp_y;
    td_u32 cnt, option;
    td_u32 curr_slice_slot_num = ctx->slice_para_num;
    td_s32 is_new_pic = (ctx->cur_slice.new_pic_flag == 1) ? 1 : 0;
    h264_pps *pps = &ctx->cur_pps;

    vfmw_check_ret(ctx->cur_nal != TD_NULL, H264_ERR);

    if (ctx->slice_para_num == 0 && is_new_pic != 1) {
        dprint(PRN_ERROR, "Slice ERR!\n");
        return H264_ERR;
    }

    if (is_new_pic != 0) {
        dps_release_seg(ctx->chan_id, ctx->cur_dps);
        dps_reset_node(ctx->chan_id, ctx->cur_dps);
    }

    if (dps_set_slice_param(ctx->chan_id, ctx, ctx->cur_dps) == DPS_ERR) {
        dprint(PRN_ERROR, "dps_set_pic_param ERR!\n");
        return H264_ERR;
    }

    if (ctx->slice_para_num == 0 && ctx->cur_slice.slice_output_flag == 0) {
        ctx->cur_pic.pic_output_flag = 0;
    } else if (ctx->slice_para_num == 0) {
        ctx->cur_pic.pic_output_flag = 1;
    } else {
        ctx->cur_pic.pic_output_flag &= ctx->cur_slice.slice_output_flag;
    }
    ctx->slice_para_num++;
    slice_qp_y = (td_u32)(pps->pic_init_qp_minus26 + 26 + ctx->cur_slice.slice_qp_delta); /* 26 qp val */

    if (curr_slice_slot_num == 0) { /* if it's the first slice of cur pic, do init */
        ctx->cur_pic.frame_store->frame.max_qp = (td_s32)slice_qp_y;
        ctx->cur_pic.frame_store->frame.min_qp = (td_s32)slice_qp_y;
    } else { /* otherwise, update */
        update_pic_qp_inf((td_s32)slice_qp_y, &ctx->cur_pic.frame_store->frame);
    }

#if defined(VFMW_H264_SLC_LDY_SUPPORT)
    option = (ctx->half_nal_try == TRY_IDLE) ? 1 : 0;
#else
    option = 1;
#endif
    if (option != 0) {
        for (cnt = 0; cnt < ctx->cur_nal->nal_segment; cnt++) {
            ctx->max_bytes_received += ctx->cur_nal->stream[cnt].bitstream_length;
        }

        ctx->cur_nal = TD_NULL;
    }

    ctx->slice_num_in_pic++;

    return H264_OK;
}

STATIC td_s32 dec_check_proc_firsti(h264_ctx *ctx)
{
    vfmw_check_ret(ctx != TD_NULL, H264_ERR);

    if (ctx->first_i_flag == 0 &&
        ctx->cur_pic.pic_type == 0) {
        if (ctx->cur_pic.structure == FRAME ||
            ctx->cur_pic.field_pair_flag == 1) {
            ctx->first_i_flag = 1;
            return H264_OK;
        }
    }

    h264_clear_curr_pic(ctx);
    clear_dpb(ctx);

    return H264_ERR;
}

STATIC td_s32 dec_check_mode_old_dec_mode(h264_ctx *ctx)
{
    switch (ctx->old_dec_mode) {
        case VFMW_DEC_MODE_FIRST_I:
            if (dec_check_proc_firsti(ctx) != H264_OK) {
                return H264_ERR;
            }
            break;

        case VFMW_DEC_MODE_I:
            /* I dec mode, dec I pic only */
            if (ctx->cur_pic.pic_type != 0) { /* P、B pic */
                /* p(1), b(2) pic, not start vdm dec; all mb not dec calculate to err_level */
                ctx->cur_pic.mode_skip_flag = 1;
                h264_clear_curr_pic(ctx);
                return H264_ERR;
            }
            break;

        case VFMW_DEC_MODE_IP:
            if ((ctx->cur_pic.pic_type == 2) && (ctx->cur_pic.nal_ref_idc == 0)) { /* 2 b frm */
                /* b(2) pic, and not ref pic */
                ctx->cur_pic.mode_skip_flag = 1;
                h264_clear_curr_pic(ctx);
                return H264_ERR;
            }
            break;

        case VFMW_DEC_MODE_NORMAL:
        case VFMW_DEC_MODE_DROP_INVALID_B:
        default:
            break;
    }

    return H264_OK;
}

STATIC td_s32 dec_check_mode(h264_ctx *ctx)
{
    td_u32 ipb_dec_mode;

    vfmw_check_ret(ctx != TD_NULL, H264_ERR);

    /* clean up information, clean here for statistic err mb when not start vdm or  reset */
    ipb_dec_mode = (ctx->advanced_cfg & 0x70000) >> 16; /* 16 bit ofs */

    if ((ctx->old_dec_mode != ipb_dec_mode) &&
        (ctx->old_dec_mode == VFMW_DEC_MODE_I ||
        ctx->old_dec_mode == VFMW_DEC_MODE_FIRST_I) &&
        (ipb_dec_mode != VFMW_DEC_MODE_I) &&
        (ipb_dec_mode == VFMW_DEC_MODE_IP ||
        ipb_dec_mode == VFMW_DEC_MODE_NORMAL ||
        ipb_dec_mode == VFMW_DEC_MODE_DROP_INVALID_B)) {
        ctx->old_dec_mode = ipb_dec_mode;
        h264_clear_curr_pic(ctx);
        clear_dpb(ctx);
        return H264_ERR; /* DPB_ERR */
    }

    ctx->old_dec_mode = ipb_dec_mode;

    if (dec_check_mode_old_dec_mode(ctx) == H264_ERR) {
        return H264_ERR;
    }

    return H264_OK;
}

STATIC td_s32 dec_check_set_ppic_found(h264_ctx *ctx)
{
    /* 容错处理，但在回归标准码流时，5X6 的码流有I 后面紧接着可解的B帧，这样做会丢掉该B,
       而导致后面出错，回归时注释掉 */
    if ((ctx->old_dec_mode == VFMW_DEC_MODE_DROP_INVALID_B) && (ctx->cur_pic.pic_type == 0)) {
        if (ctx->ppic_found == 0) {
            ctx->ppic_found = 2; /* 2 find the first I */
        } else {
            ctx->ppic_found = 1; /* find annother I */
        }
    }

    if ((ctx->cur_pic.pic_type == 2) && (ctx->ppic_found == 2) && /* 2 b frm */
        (ctx->extra_data->output_order == VFMW_DISP_ORDER) &&
        (ctx->extra_data->allow_b_before_p != 1)) { /* 发现B帧 */
        dprint(PRN_ERROR, "Discard this B(poc=%d) befor P, is_ref_idc=%d.\n",
               ctx->cur_pic.thispoc, ctx->cur_pic.nal_ref_idc);
        h264_clear_curr_pic(ctx);

        fsp_set_disp(ctx->chan_id, ctx->cur_pic.frame_store->logic_fs_id, 0);
        return H264_ERR;
    } else if (ctx->cur_pic.pic_type == 1) {
        ctx->ppic_found = 1;
    }

    return H264_OK;
}

/*************************************************************************
* 函数原型 : td_s32 dec_check(h264_ctx *ctx)
* 功能描述 : 解码前检查
* 参数说明 :
* 返回值   ：
*************************************************************************/
STATIC td_s32 dec_check(h264_ctx *ctx)
{
    vfmw_check_ret(ctx->extra_data != TD_NULL, H264_ERR);
    vfmw_check_ret(ctx->cur_pic.frame_store != TD_NULL, H264_ERR);

    /* 统计帧数 */
    if (ctx->cur_pic.structure == FRAME || (ctx->cur_pic.structure != FRAME && ctx->cur_pic.field_pair_flag == 1)) {
        ctx->num_total_frame++;
        ctx->num_total_frame = (ctx->num_total_frame < ctx->num_output_frame) ?
                               ctx->num_output_frame : ctx->num_total_frame;
        ctx->extra_data->err_frm_num = ctx->num_total_frame - ctx->num_output_frame;
    }

    if (dec_check_mode(ctx) != H264_OK) {
        return H264_ERR;
    }

    if (ctx->slice_para_num == 0) {
        h264_clear_curr_pic(ctx);
        report_err_frame((td_s8)ctx->chan_id, (td_s8)ctx->cur_pic.pic_type, 0);
        return H264_ERR;
    }

    if (dec_check_set_ppic_found(ctx) == H264_ERR) {
        return H264_ERR;
    }

    if (ctx->total_pic_num >= (START_FRAME + 1)) {
        /* 发现I帧, 上报事件 */
        if (ctx->cur_pic.pic_type == 0) {
            report_find_iframe(ctx->chan_id, 0);
        }
    }

    ctx->cur_pic.frame_stream_size = ctx->cur_pic.cur_nal_size;

    return H264_OK;
}

/*************************************************************************
*      函数原型 : td_s32 slice_check(h264_ctx *ctx)
*      功能描述 : 根据快进快退的状态只播放I帧，其它Nalu Type过滤；根据第一次进入解码器只播放I帧，其它Nalu Type过滤;
*      参数说明 : td_void
*      返回值   ：DEC_NORMAL, DEC_ERR
*************************************************************************/
STATIC td_s32 slice_check(h264_ctx *ctx)
{
    td_u32 slice_type;
    td_u32 total_mb_min1;
    ctx->cur_slice.first_mb_in_slice = ue_v(ctx, "SH: first_mb_in_slice");

    vfmw_check_ret(se_err_check(ctx->se_err_flg) == H264_OK, H264_ERR);

    if (ctx->cur_slice.first_mb_in_slice >= H264_MAX_MB_NUM_IN_PIC) {
        dprint(PRN_ERROR, "slicecheck first_mb_in_slice >= MAX_MB_NUM_IN_PIC error.\n");
        return H264_ERR;
    }

    vfmw_check_ret(ctx->cur_width_in_mb < VFMW_INT16_MAX, H264_ERR);
    vfmw_check_ret(ctx->cur_height_in_mb < VFMW_INT16_MAX, H264_ERR);
    total_mb_min1 = (td_u32)(ctx->cur_width_in_mb * ctx->cur_height_in_mb - 1);
    if (ctx->cur_slice.first_mb_in_slice > total_mb_min1) {
        return H264_ERR;
    }

    /* end */
    slice_type = ue_v(ctx, "SH: slice_type");
    vfmw_check_ret(se_err_check(ctx->se_err_flg) == H264_OK, H264_ERR);

    switch (slice_type) {
        case 2: /* 2 p slc */
        case 4: /* 4 p slc */
        case 7: /* 7 p slc */
        case 9: /* 9 p slc */
            ctx->cur_slice.slice_type = I_SLICE;
            break;

        case 0: /* 0 p slc */
        case 3: /* 3 p slc */
        case 5: /* 5 p slc */
        case 8: /* 8 p slc */
            ctx->cur_slice.slice_type = P_SLICE;
            break;

        case 1: /* 1 b slc */
        case 6: /* 6 b slc */
#ifdef H264_B_FRAME_SUPPORT
            ctx->cur_slice.slice_type = B_SLICE;
#else
            dprint(PRN_ERROR, "B slice type = %d, unsupport\n", slice_type);
            report_unsupport(ctx->chan_id);
            return H264_ERR;
#endif
            break;

        default:
            dprint(PRN_ERROR, "slice type = %d, err\n", slice_type);
            return H264_ERR;
    }

    return H264_OK;
}

/*************************************************************************
*      函数原型 :  td_s32 pps_sps_check(h264_ctx *ctx)
*      功能描述 :  校验pps、sps是否得到
*      参数说明 :
*      返回值   ： DEC_NORMAL; DEC_ERR;
*************************************************************************/
STATIC td_s32 pps_sps_check(const h264_ctx *ctx)
{
    h264_pps *pps = TD_NULL;
    h264_sps *sps = TD_NULL;

    /* check pps is get or not */
    pps = (ctx->pps[ctx->cur_slice.pic_parameter_set_id]);

    if (pps == TD_NULL) {
        dprint(PRN_WARN, "pps is null ppsid = %d havn't decode\n", ctx->cur_slice.pic_parameter_set_id);
        return H264_ERR;
    }

    if (pps->is_valid == 0) {
        dprint(PRN_ERROR, "ppsid = %d havn't decode\n", ctx->cur_slice.pic_parameter_set_id);
        return H264_ERR;
    }

    /* check sps is get or not */
    sps = (ctx->sps[pps->seq_parameter_set_id]);

    if (sps == TD_NULL) {
        dprint(PRN_ERROR, "sps is null ppsid = %d havn't decode\n", pps->seq_parameter_set_id);
        return H264_ERR;
    }

    if (sps->is_valid == 0) {
        dprint(PRN_ERROR, "sps ppsid = %d havn't decode\n", pps->seq_parameter_set_id);
        return H264_ERR;
    }

    return H264_OK;
}

/*************************************************************************
*      函数原型 :  td_s32 pps_sps_check_tmp_id(h264_ctx *ctx, td_u32 tmp_pic_parameter_set_id)
*      功能描述 :  校验pps、sps是否得到
*      参数说明 :
*      返回值   ： DEC_NORMAL; DEC_ERR;
*************************************************************************/
STATIC td_s32 pps_sps_check_tmp_id(const h264_ctx *ctx, td_u32 tmp_pic_parameter_set_id)
{
    h264_pps *pps = TD_NULL;
    h264_sps *sps = TD_NULL;

    /* check pps is get or not */
    pps = (ctx->pps[tmp_pic_parameter_set_id]);

    if (pps == TD_NULL) {
        dprint(PRN_WARN, "pps is null ppsid = %d havn't decode\n", tmp_pic_parameter_set_id);
        return H264_ERR;
    }

    if (pps->is_valid == 0) {
        dprint(PRN_ERROR, "ppsid = %d havn't decode\n", tmp_pic_parameter_set_id);
        return H264_ERR;
    }

    if (pps->seq_parameter_set_id > (td_u32)(ctx->max_sps_num - 1) || ctx->max_sps_num <= 0) {
        dprint(PRN_ERROR, "pps_sps_id: seq_parameter_set_id(%u) out of range [0, %d).\n",
            pps->seq_parameter_set_id, ctx->max_sps_num);
        return H264_ERR;
    }

    /* check sps is get or not */
    sps = (ctx->sps[pps->seq_parameter_set_id]);

    if (sps == TD_NULL) {
        dprint(PRN_ERROR, "sps is null spsid = %d havn't decode\n", pps->seq_parameter_set_id);
        return H264_ERR;
    }

    if (sps->is_valid == 0) {
        dprint(PRN_ERROR, "spsid = %d havn't decode\n", pps->seq_parameter_set_id);
        return H264_ERR;
    }

    return H264_OK;
}

/*************************************************************************
*      函数原型 :  td_u32 is_new_pic(h264_ctx *ctx)
*      功能描述 :  比较slice 语法元素，确定是否是new pic
*      参数说明 :  无
*      返回值   ： 比较结果
*************************************************************************/
STATIC td_u32 is_new_pic(const h264_ctx *ctx)
{
    td_u32 result = 0;

    result |= (td_u32)(ctx->prev_slice.pic_parameter_set_id != ctx->cur_slice.pic_parameter_set_id);
    result |= (td_u32)(ctx->prev_slice.seq_parameter_set_id != ctx->cur_sps.seq_parameter_set_id);
    result |= (td_u32)(ctx->prev_slice.frame_num != ctx->cur_slice.frame_num);
    result |= (td_u32)(ctx->prev_slice.field_pic_flag != ctx->cur_slice.field_pic_flag);

    if ((ctx->prev_slice.field_pic_flag != 0) && (ctx->cur_slice.field_pic_flag != 0)) {
        result |= (td_u32)(ctx->prev_slice.bottom_field_flag != ctx->cur_slice.bottom_field_flag);
    }

    result |= (td_u32)((ctx->prev_slice.nal_ref_idc != ctx->cur_slice.nal_ref_idc) &&
                       ((ctx->prev_slice.nal_ref_idc == 0) || (ctx->cur_slice.nal_ref_idc == 0)));
    result |= (td_u32)(ctx->prev_slice.nal_unit_type != ctx->cur_slice.nal_unit_type);

    if (ctx->cur_slice.nal_unit_type == NALU_TYPE_IDR && ctx->prev_slice.nal_unit_type == NALU_TYPE_IDR) {
        result |= (td_u32)(ctx->prev_slice.idr_pic_id != ctx->cur_slice.idr_pic_id);
    }

    if (ctx->cur_sps.pic_order_cnt_type == 0) {
        result |= (td_u32)(ctx->prev_slice.pic_oder_cnt_lsb != ctx->cur_poc.pic_order_cnt_lsb);
        result |= (td_u32)(ctx->prev_slice.delta_pic_oder_cnt_bottom != ctx->cur_poc.delta_pic_order_cnt_bottom);
    }

    if (ctx->cur_sps.pic_order_cnt_type == 1) {
        result |= (td_u32)(ctx->prev_slice.delta_pic_order_cnt[0] != ctx->cur_poc.delta_pic_order_cnt[0]);
        result |= (td_u32)(ctx->prev_slice.delta_pic_order_cnt[1] != ctx->cur_poc.delta_pic_order_cnt[1]);
    }

    /* 有的码流，特别是一些快进快退的码流，非得要这一行才能通过 */
    result |= (td_u32)(ctx->cur_slice.first_mb_in_slice == 0);

    return result;
}

STATIC td_s32 ref_pic_list_reordering_b_or_p_slice(h264_ctx *ctx, td_u32 max_pic_num)
{
    td_u32 i = 0;

    ctx->cur_reorder.ref_pic_list_reordering_flag_l0 = (td_u8)u_1(ctx, "SH: ref_pic_list_reordering_flag_l0");

    if (ctx->cur_reorder.ref_pic_list_reordering_flag_l0 == 0) {
        return H264_OK;
    }

    do {
        vfmw_check_ret(i < 33, H264_ERR); /* 33:array size */
        ctx->cur_reorder.reordering_of_pic_nums_idc_l0[i] = ue_v(ctx, "SH: reordering_of_pic_nums_idc_l0");

        if ((ctx->cur_reorder.reordering_of_pic_nums_idc_l0[i] > 3) || (ctx->se_err_flg) != 0) { /* 3 vfmw_max val */
            dprint(PRN_ERROR, "reorder idc l0 = %d, g_SeErrFlag=%d\n",
                   ctx->cur_reorder.reordering_of_pic_nums_idc_l0[i], ctx->se_err_flg);
            return H264_ERR;
        }

        if (ctx->cur_reorder.reordering_of_pic_nums_idc_l0[i] == 3) { /* 3 is idc */
            break;
        }

        if ((ctx->cur_reorder.reordering_of_pic_nums_idc_l0[i] == 0) ||
            (ctx->cur_reorder.reordering_of_pic_nums_idc_l0[i] == 1)) {
            ctx->cur_reorder.abs_diff_pic_num_minus1_l0[i] = ue_v(ctx, "SH: abs_diff_pic_num_minus1_l0");

            if ((ctx->cur_reorder.abs_diff_pic_num_minus1_l0[i] >= max_pic_num) || (ctx->se_err_flg) != 0) {
                dprint(PRN_ERROR, "abs_diff_pic_num_minus1_l0 = %d, g_SeErrFlag=%d\n",
                       ctx->cur_reorder.abs_diff_pic_num_minus1_l0[i], ctx->se_err_flg);
                return H264_ERR;
            }
        } else if (ctx->cur_reorder.reordering_of_pic_nums_idc_l0[i] == 2) { /* 2 is idc */
            if (i < 32) { /* 32 vfmw_max val */
                ctx->cur_reorder.long_term_pic_idx_l0[i] = ue_v(ctx, "SH: long_term_pic_idx_l0");
            } else {
                dprint(PRN_ERROR, "long_term_pic_idx_l0 index = %d\n", i);
            }
        }

        i++;
    } while (i < 33 && ctx->cur_reorder.reordering_of_pic_nums_idc_l0[i - 1] != 3); /* 3 idc 33 idx */

    return H264_OK;
}

#ifdef H264_B_FRAME_SUPPORT
STATIC td_s32 ref_pic_list_reordering_b_slice(h264_ctx *ctx, td_u32 max_pic_num)
{
    td_u32 i;

    ctx->cur_reorder.ref_pic_list_reordering_flag_l1 = (td_u8)u_1(ctx, "SH: ref_pic_list_reordering_flag_l1");

    if (ctx->cur_reorder.ref_pic_list_reordering_flag_l1 != 0) {
        i = 0;
        do {
            vfmw_check_ret(i < 33, H264_ERR); /* 33 array size */
            ctx->cur_reorder.reordering_of_pic_nums_idc_l1[i] = ue_v(ctx, "SH: reordering_of_pic_nums_idc_l1");

            if ((ctx->cur_reorder.reordering_of_pic_nums_idc_l1[i] > 3) || ctx->se_err_flg != 0) { /* 3 vfmw_max idc */
                dprint(PRN_ERROR, "reorder idc l1 = %d, g_SeErrFlag=%d\n",
                       ctx->cur_reorder.reordering_of_pic_nums_idc_l1[i], ctx->se_err_flg);
                return H264_ERR;
            }

            if (ctx->cur_reorder.reordering_of_pic_nums_idc_l1[i] == 3) { /* 3 is idc */
                break;
            }

            if ((ctx->cur_reorder.reordering_of_pic_nums_idc_l1[i] == 0) ||
                (ctx->cur_reorder.reordering_of_pic_nums_idc_l1[i] == 1)) {
                ctx->cur_reorder.abs_diff_pic_num_minus1_l1[i] = ue_v(ctx, "SH: abs_diff_pic_num_minus1_l1");

                if ((ctx->cur_reorder.abs_diff_pic_num_minus1_l1[i] >= max_pic_num) || ctx->se_err_flg != 0) {
                    dprint(PRN_ERROR, "abs_diff_pic_num_minus1_l1 = %d, g_SeErrFlag=%d\n",
                           ctx->cur_reorder.abs_diff_pic_num_minus1_l1[i], ctx->se_err_flg);
                    return H264_ERR;
                }
            } else if (ctx->cur_reorder.reordering_of_pic_nums_idc_l1[i] == 2) { /* 2 is idc */
                if (i < 32) { /* 32 vfmw_max val */
                    ctx->cur_reorder.long_term_pic_idx_l1[i] = ue_v(ctx, "SH: long_term_pic_idx_l1");
                } else {
                    dprint(PRN_ERROR, "long_term_pic_idx_l1 index = %d\n", i);
                }
            }

            i++;
        } while (i < 33 && ctx->cur_reorder.reordering_of_pic_nums_idc_l1[i - 1] != 3); /* 3 idc 33 idx */
    }

    return H264_OK;
}
#endif

/***********************************************************************************
* Function:    ref_pic_list_reordering(h264_ctx *ctx)
* Description: 解析Reorder参数
* Input:       在码流解析时会消耗码流
* Return:      DEC_NORMAL      可以解码MMCO数据
*              DEC_ERR         不可以解码
* Others:      无
***********************************************************************************/
STATIC td_s32 ref_pic_list_reordering(h264_ctx *ctx)
{
    td_u32 max_pic_num;
    h264_pps *pps = (ctx->pps[ctx->cur_slice.pic_parameter_set_id]);
    h264_sps *sps = (ctx->sps[pps->seq_parameter_set_id]);

    ctx->cur_reorder.ref_pic_list_reordering_flag_l0 = 0;
    ctx->cur_reorder.ref_pic_list_reordering_flag_l1 = 0;

    if (ctx->cur_slice.field_pic_flag != 0) {
        max_pic_num = 1 << (sps->log2_max_frame_num_minus4 + 5); /* 5 is num */
    } else {
        max_pic_num = 1 << (sps->log2_max_frame_num_minus4 + 4); /* 4 is num */
    }

    if ((ctx->cur_slice.slice_type == P_SLICE) || (ctx->cur_slice.slice_type == B_SLICE)) {
        if (ref_pic_list_reordering_b_or_p_slice(ctx, max_pic_num) != H264_OK) {
            return H264_ERR;
        }
    }
#ifdef H264_B_FRAME_SUPPORT
    if (ctx->cur_slice.slice_type == B_SLICE) {
        if (ref_pic_list_reordering_b_slice(ctx, max_pic_num) != H264_OK) {
            return H264_ERR;
        }
    }
#endif
    vfmw_check_ret(se_err_check(ctx->se_err_flg) == H264_OK, H264_ERR);

    return H264_OK;
}

STATIC td_void fill_wp_parameter_table(h264_ctx *ctx)
{
    td_u32 i;
    td_s32 uv_default_weight;
    td_s32 y_default_weight;
    h264_pps *pps = (ctx->pps[ctx->cur_slice.pic_parameter_set_id]);
    h264_sps *sps = (ctx->sps[pps->seq_parameter_set_id]);

    uv_default_weight = 0;
    y_default_weight = 1 << ctx->cur_slice.luma_log2_weight_denom;

    /* 1 == chroma_format_idc -- 4:2:0 */
    if (sps->chroma_format_idc != 0) {
        uv_default_weight = 1 << ctx->cur_slice.chroma_log2_weight_denom;
    }

    for (i = 0; i <= ctx->cur_slice.num_ref_idx_l0_active_minus1; i++) { /* fw */
        if (u_1(ctx, "SH: luma_weight_flag_l0") != 0) {
            ctx->cur_slice.wp_weight_l0_y[i] = se_v(ctx, "SH: luma_weight_l0");
            ctx->cur_slice.wp_offset_l0_y[i] = se_v(ctx, "SH: luma_offset_l0");
        } else {
            ctx->cur_slice.wp_weight_l0_y[i] = y_default_weight;
            ctx->cur_slice.wp_offset_l0_y[i] = 0;
        }

        if (sps->chroma_format_idc != 0) {
            if (u_1(ctx, "SH: chroma_weight_flag_l0") != 0) {
                ctx->cur_slice.wp_weight_l0_u[i] = se_v(ctx, "SH: chroma_weight_l0");
                ctx->cur_slice.wp_offset_l0_u[i] = se_v(ctx, "SH: chroma_offset_l0");
                ctx->cur_slice.wp_weight_l0_v[i] = se_v(ctx, "SH: chroma_weight_l0");
                ctx->cur_slice.wp_offset_l0_v[i] = se_v(ctx, "SH: chroma_offset_l0");
            } else {
                ctx->cur_slice.wp_weight_l0_u[i] = uv_default_weight;
                ctx->cur_slice.wp_offset_l0_u[i] = 0;
                ctx->cur_slice.wp_weight_l0_v[i] = uv_default_weight;
                ctx->cur_slice.wp_offset_l0_v[i] = 0;
            }
        } else {
            ctx->cur_slice.wp_weight_l0_u[i] = 0;
            ctx->cur_slice.wp_offset_l0_u[i] = 0;
            ctx->cur_slice.wp_weight_l0_v[i] = 0;
            ctx->cur_slice.wp_offset_l0_v[i] = 0;
        }
    }
}

#ifdef H264_B_FRAME_SUPPORT
STATIC td_void fill_bw_parameter_table_b_slice(h264_ctx *ctx)
{
    td_u32 i;
    td_s32 uv_default_weight;
    td_s32 y_default_weight;
    h264_pps *pps = (ctx->pps[ctx->cur_slice.pic_parameter_set_id]);
    h264_sps *sps = (ctx->sps[pps->seq_parameter_set_id]);

    uv_default_weight = 0;
    y_default_weight = 1 << ctx->cur_slice.luma_log2_weight_denom;

    /* 1 == chroma_format_idc -- 4:2:0 */
    if (sps->chroma_format_idc != 0) {
        uv_default_weight = 1 << ctx->cur_slice.chroma_log2_weight_denom;
    }

    for (i = 0; i <= ctx->cur_slice.num_ref_idx_l1_active_minus1; i++) {
        if (u_1(ctx, "SH: luma_weight_flag_l1") != 0) {
            ctx->cur_slice.wp_weight_l1_y[i] = se_v(ctx, "SH: luma_weight_l1");
            ctx->cur_slice.wp_offset_l1_y[i] = se_v(ctx, "SH: luma_offset_l1");
        } else {
            ctx->cur_slice.wp_weight_l1_y[i] = y_default_weight;
            ctx->cur_slice.wp_offset_l1_y[i] = 0;
        }

        if (sps->chroma_format_idc != 0) {
            if (u_1(ctx, "SH: chroma_weight_flag_l1") != 0) {
                ctx->cur_slice.wp_weight_l1_u[i] = se_v(ctx, "SH: chroma_weight_l1");
                ctx->cur_slice.wp_offset_l1_u[i] = se_v(ctx, "SH: chroma_offset_l1");
                ctx->cur_slice.wp_weight_l1_v[i] = se_v(ctx, "SH: chroma_weight_l1");
                ctx->cur_slice.wp_offset_l1_v[i] = se_v(ctx, "SH: chroma_offset_l1");
            } else {
                ctx->cur_slice.wp_weight_l1_u[i] = uv_default_weight;
                ctx->cur_slice.wp_offset_l1_u[i] = 0;
                ctx->cur_slice.wp_weight_l1_v[i] = uv_default_weight;
                ctx->cur_slice.wp_offset_l1_v[i] = 0;
            }
        } else {
            ctx->cur_slice.wp_weight_l1_u[i] = 0;
            ctx->cur_slice.wp_offset_l1_u[i] = 0;
            ctx->cur_slice.wp_weight_l1_v[i] = 0;
            ctx->cur_slice.wp_offset_l1_v[i] = 0;
        }
    }
}
#endif

STATIC td_void fill_bw_parameter_table(h264_ctx *ctx)
{
    vfmw_unused(ctx);
#ifdef H264_B_FRAME_SUPPORT
    h264_pps *pps = (ctx->pps[ctx->cur_slice.pic_parameter_set_id]);

    if ((pps->weighted_bipred_idc == 1) && (ctx->cur_slice.slice_type == B_SLICE)) {
        fill_bw_parameter_table_b_slice(ctx);
    }
#endif
}

/***********************************************************************************
* Function:    pred_weight_table(h264_ctx *ctx)
* Description: 解析weighted参数
* Input:       在码流解析时会消耗码流
* Output:      ctx->cur_slice.wp_...   Slice结构体里的weighted参数
* Return:      DEC_NORMAL           可以解码weighted参数
*              DEC_ERR              不可以解码
* Others:      无
***********************************************************************************/
STATIC td_s32 pred_weight_table(h264_ctx *ctx)
{
    h264_pps *pps = (ctx->pps[ctx->cur_slice.pic_parameter_set_id]);
    h264_sps *sps = (ctx->sps[pps->seq_parameter_set_id]);

    ctx->cur_slice.luma_log2_weight_denom = ue_v(ctx, "SH: luma_log2_weight_denom");

    /* 1 == chroma_format_idc -- 4:2:0 */
    if (sps->chroma_format_idc != 0) {
        ctx->cur_slice.chroma_log2_weight_denom = ue_v(ctx, "SH: chroma_log2_weight_denom");
    }

    if ((ctx->cur_slice.luma_log2_weight_denom > 7) || /* 7 vfmw_max val */
        (ctx->cur_slice.chroma_log2_weight_denom > 7) || ctx->se_err_flg != 0) { /* 7 vfmw_max val */
        /* 这里可以钳位而继续解析 */
        dprint(PRN_ERROR, "WP log2 exceed  and err flag = %d\n", ctx->se_err_flg);
        return H264_ERR;
    }

    /* fill wp_parameter table */
    fill_wp_parameter_table(ctx);

    /* bw */
    fill_bw_parameter_table(ctx);

    vfmw_check_ret(se_err_check(ctx->se_err_flg) == H264_OK, H264_ERR);

    return H264_OK;
}

STATIC td_s32 save_mmco_data(h264_ctx *ctx)
{
    td_u32 mmco_num;
    h264_mmco *mmco = TD_NULL;
    h264_pps *pps = (ctx->pps[ctx->cur_slice.pic_parameter_set_id]);
    h264_sps *sps = (ctx->sps[pps->seq_parameter_set_id]);

    mmco_num = 0;
    vfmw_check_ret(ctx->cur_mark_idx < 2, H264_ERR); /* 2 array size */

    do {
        if (mmco_num >= MAX_MMCO_LEN) {
            dprint(PRN_ERROR, "overflow MAX_MMCO_LEN\n");
            return H264_ERR;
        }

        mmco = &(ctx->mark[ctx->cur_mark_idx].mmco[mmco_num]);

        mmco->memory_management_control_operation = ue_v(ctx, "SH: memory_management_control_operation");

        if ((mmco->memory_management_control_operation == 1) ||
            (mmco->memory_management_control_operation == 3)) { /* 3, a number */
            mmco->difference_of_pic_nums_minus1 = ue_v(ctx, "SH: difference_of_pic_nums_minus1");
        }

        if (mmco->memory_management_control_operation == 2) { /* 2, a number */
            mmco->long_term_pic_num = ue_v(ctx, "SH: long_term_pic_num");
        }

        if ((mmco->memory_management_control_operation == 3) || /* 3, a number */
            (mmco->memory_management_control_operation == 6)) { /* 6, a number */
            mmco->long_term_frame_idx = ue_v(ctx, "SH: long_term_frame_idx");
        }

        if (mmco->memory_management_control_operation == 4) { /* 4, a number */
            mmco->max_long_term_frame_idx_plus1 = ue_v(ctx, "SH: max_long_term_frame_idx_plus1");

            if ((sps->num_ref_frames < mmco->max_long_term_frame_idx_plus1) ||
                (mmco->max_long_term_frame_idx_plus1 < 1) || ctx->se_err_flg != 0) {
                /* 这里的判断需要进一步的考虑 */
            }
        }

        if ((mmco->memory_management_control_operation > 6) || ctx->se_err_flg != 0) { /* 6, a number */
            dprint(PRN_ERROR, "mmco exceed 6\n");
            return H264_ERR;
        }

        mmco_num++;
    } while (mmco->memory_management_control_operation != 0);

    return H264_OK;
}

STATIC td_s32 no_save_mmco_data(h264_ctx *ctx)
{
    td_u32 mmco_num;
    h264_pps *pps = (ctx->pps[ctx->cur_slice.pic_parameter_set_id]);
    h264_sps *sps = (ctx->sps[pps->seq_parameter_set_id]);
    td_u32 memory_management_control_operation;
    td_u32 max_long_term_frame_idx_plus1;

    mmco_num = 0;

    do {
        if (mmco_num >= MAX_MMCO_LEN) {
            return H264_ERR;
        }

        memory_management_control_operation = ue_v(ctx, "SH: memory_management_control_operation");
        /* 3, a number */
        if ((memory_management_control_operation == 1) || (memory_management_control_operation == 3)) {
            ue_v(ctx, "SH: difference_of_pic_nums_minus1");
        }

        if (memory_management_control_operation == 2) { /* 2, a number */
            ue_v(ctx, "SH: long_term_pic_num");
        }

        /* 3, a number, 6, a number */
        if ((memory_management_control_operation == 3) || (memory_management_control_operation == 6)) {
            ue_v(ctx, "SH: long_term_frame_idx");
        }

        if (memory_management_control_operation == 4) { /* 4, a number */
            max_long_term_frame_idx_plus1 = ue_v(ctx, "SH: max_long_term_frame_idx_plus1");
            if ((sps->num_ref_frames < max_long_term_frame_idx_plus1) ||
                (max_long_term_frame_idx_plus1 < 1) || ctx->se_err_flg != 0) {
            }
        }

        if ((memory_management_control_operation > 6) || ctx->se_err_flg != 0) { /* 6, a number */
            dprint(PRN_ERROR, "mmco exceed 6\n");
            return H264_ERR;
        }

        mmco_num++;
    } while (memory_management_control_operation != 0);

    return H264_OK;
}
/***********************************************************************************
* Function:    dec_mmco(h264_ctx *ctx,td_u32 save)
* Description: 解析MMCO参数
* Input:       在码流解析时会消耗码流
*              save                        是否需要保存MMCO参数
* Return:      DEC_NORMAL                  可以解码MMCO数据
*              DEC_ERR                     不可以解码
* Others:      无
***********************************************************************************/
STATIC td_s32 dec_mmco(h264_ctx *ctx, td_u32 save)
{
    td_s32 ret;

    if (save != 0) {
        ret = save_mmco_data(ctx);
    } else { /* don't save */
        ret = no_save_mmco_data(ctx);
    }

    return ret;
}

STATIC td_s32 process_new_pic_or_mark_invalid(h264_ctx *ctx, h264_mark *mark)
{
    td_u32 tmp_mark_idx;

    tmp_mark_idx = ctx->cur_mark_idx;

    if (ctx->cur_slice.nal_unit_type == NALU_TYPE_IDR) {
        mark->no_output_of_prior_pics_flag = (td_u8)u_1(ctx, "SH: no_output_of_prior_pics_flag");
        mark->long_term_reference_flag = (td_u8)u_1(ctx, "SH: long_term_reference_flag");
    } else {
        mark->adaptive_ref_pic_marking_mode_flag = (td_u8)u_1(ctx, "SH: adaptive_ref_pic_marking_mode_flag");
        if (mark->adaptive_ref_pic_marking_mode_flag != 0) {
            if (dec_mmco(ctx, 1) == H264_ERR) { /* save MMCO */
                mark->is_valid = 0;
                ctx->cur_mark_idx = tmp_mark_idx;
                dprint(PRN_ERROR, "MMCO para would save dec err\n");
                return H264_ERR;
            }

            mark->is_valid = 1;
        }
    }

    return H264_OK;
}

STATIC td_s32 process_ref_pic_marking(h264_ctx *ctx, h264_mark *mark)
{
    td_s32 ret;

    if ((ctx->cur_slice.new_pic_flag != 0) || (mark->is_valid == 0)) {
        ret = process_new_pic_or_mark_invalid(ctx, mark);
        if (ret != H264_OK) {
            return ret;
        }
    } else {
        if (ctx->cur_slice.nal_unit_type == NALU_TYPE_IDR) {
            if (mark->no_output_of_prior_pics_flag != u_1(ctx, "SH: no_output_of_prior_pics_flag")) {
                /* some one wrong, but don't know which used */
                dprint(PRN_ERROR, "mulitislice diff no_out_of_prior_pics_flag, used first.\n");
                return H264_ERR;
            }

            if (mark->long_term_reference_flag != u_1(ctx, "SH: long_term_reference_flag")) {
                /* some one wrong, but don't know which used */
                dprint(PRN_ERROR, "mulitislice diff long_term_reference_flag, used first.\n");
                return H264_ERR;
            }
        } else {
            if (mark->adaptive_ref_pic_marking_mode_flag != u_1(ctx, "SH: adaptive_ref_pic_marking_mode_flag")) {
                /* some one wrong, but don't know which used */
                dprint(PRN_ERROR, "mulitislice diff adaptive_ref_pic_marking_mode_flag, used first.\n");
                dprint(PRN_ERROR, "TotalPicNum=%d, SliceNumInPic=%d, total_nal=%d\n",
                    ctx->total_pic_num, ctx->slice_num_in_pic, ctx->total_nal);
                return H264_ERR;
            }

            if (mark->adaptive_ref_pic_marking_mode_flag != 0) {
                return dec_mmco(ctx, 0); /* don't save */
            }
        }
    }

    return H264_OK;
}

/***********************************************************************************
* Function:    dec_ref_pic_marking(h264_ctx *ctx)
* Description: 解析MMCO参数
* Input:       在码流解析时会消耗码流
* Return:      DEC_NORMAL               可以解码MMCO数据
*              DEC_ERR                  不可以解码
* Others:      无
***********************************************************************************/
STATIC td_s32 dec_ref_pic_marking(h264_ctx *ctx)
{
    td_s32 ret;
    h264_mark *mark = TD_NULL;

    if (h264_check_stm_len(ctx, 16) != H264_OK) { /* 16 bit num */
        return H264_ERR;
    }

    if (ctx->cur_slice.new_pic_flag != 0) {
        ctx->cur_mark_idx = (ctx->cur_mark_idx != 0) ? 0 : 1;
    }

    mark = &(ctx->mark[ctx->cur_mark_idx]);

    ret = process_ref_pic_marking(ctx, mark);

    return ret;
}

STATIC td_void process_slice_header_curr_poc(h264_ctx *ctx, const h264_pps *pps,
    const h264_sps *sps)
{
    ctx->cur_poc.delta_pic_order_cnt[0] = 0;
    ctx->cur_poc.delta_pic_order_cnt[1] = 0;

    if ((sps->pic_order_cnt_type == 1) && (sps->delta_pic_order_always_zero_flag == 0)) {
        ctx->cur_poc.delta_pic_order_cnt[0] = se_v(ctx, "SH: delta_pic_order_cnt[0]");

        if ((pps->pic_order_present_flag == 1) && (ctx->cur_slice.field_pic_flag == 0)) {
            ctx->cur_poc.delta_pic_order_cnt[1] = se_v(ctx, "SH: delta_pic_order_cnt[1]");
        }
    }
}

VFMW_ACC_SLICE_H264 STATIC td_s32 process_slice_header_first_part_set_ctx(h264_ctx *ctx,
    const h264_pps *pps, const h264_sps *sps)
{
    td_u32 pic_size;

    /* Get picture structure */
    ctx->cur_slice.field_pic_flag = 0;
    ctx->cur_slice.bottom_field_flag = 0;

    if (sps->frame_mbs_only_flag == 0) {
        ctx->cur_slice.field_pic_flag = (td_u8)u_1(ctx, "SH: field_pic_flag");

        if (ctx->cur_slice.field_pic_flag != 0) {
#ifdef H264_FIELD_SUPPORT
            ctx->cur_slice.bottom_field_flag = u_1(ctx, "SH: bottom_field_flag");
#else
            dprint(PRN_ERROR, "field is no support report unsupport!\n");
            report_unsupport(ctx->chan_id);
            return H264_ERR;
#endif
        }
    }

    /* check first_mb_in_slice */
    vfmw_check_ret((sps->pic_width_in_mbs_minus1 + 1) < VFMW_INT16_MAX, H264_ERR);
    vfmw_check_ret((sps->pic_height_in_map_units_minus1 + 1) < VFMW_INT16_MAX, H264_ERR);
    vfmw_check_ret(((sps->frame_mbs_only_flag == 0) + 1) < VFMW_INT16_MAX, H264_ERR);
    pic_size = (sps->pic_width_in_mbs_minus1 + 1) * (sps->pic_height_in_map_units_minus1 + 1) *
        ((sps->frame_mbs_only_flag == 0) + 1);
    ctx->cur_slice.mbaff_frame_flag = ((ctx->cur_slice.field_pic_flag == 0) &&
        (sps->mb_adaptive_frame_field_flag != 0));

    if ((ctx->cur_slice.field_pic_flag == 1) || (ctx->cur_slice.mbaff_frame_flag == 1)) {
        /* vfmw_max first_mb_in_slice is half pic_size - 1 */
        vfmw_check_ret((pic_size / 2) > ctx->cur_slice.first_mb_in_slice, H264_ERR); /* 2 is half */
    } else {
        /* vfmw_max first_mb_in_slice is pic_size - 1 */
        vfmw_check_ret(pic_size > ctx->cur_slice.first_mb_in_slice, H264_ERR);
    }

    /* Get idr_pic_id */
    if (ctx->cur_slice.nal_unit_type == NALU_TYPE_IDR) {
        ctx->cur_slice.idr_pic_id = ue_v(ctx, "SH: idr_pic_id");

        if ((ctx->cur_slice.idr_pic_id > 65535) || (ctx->se_err_flg != 0)) { /* 65535 vfmw_max val */
            dprint(PRN_ERROR, "idr_pic_id bigger than 65535\n");
            /*  CNcomment:  可以去掉这个返回强解 */
        }
    }

    /* Get POC parameters */
    ctx->cur_poc.pic_order_cnt_lsb = 0;
    ctx->cur_poc.delta_pic_order_cnt_bottom = 0;

    if (sps->pic_order_cnt_type == 0) {
        ctx->cur_poc.pic_order_cnt_lsb = (td_u32)u_v(ctx, sps->log2_max_pic_order_cnt_lsb_minus4 + 4, /* 4 is num */
                                                     "SH: pic_order_cnt_lsb");

        if ((pps->pic_order_present_flag != 0) && (ctx->cur_slice.field_pic_flag == 0)) {
            ctx->cur_poc.delta_pic_order_cnt_bottom = se_v(ctx, "SH: delta_pic_order_cnt_bottom");
        }
    }

    process_slice_header_curr_poc(ctx, pps, sps);

    return H264_OK;
}

VFMW_ACC_SLICE_H264 STATIC td_s32 process_slice_header_first_part_judging(h264_ctx *ctx)
{
    td_u32 tmp_pic_parameter_set_id;

    tmp_pic_parameter_set_id = ue_v(ctx, "SH: pic_parameter_set_id");
    if (((td_u32)(ctx->max_pps_num - 1) < tmp_pic_parameter_set_id) || ctx->se_err_flg != 0 || ctx->max_pps_num <= 0) {
        dprint(PRN_ERROR, "pps id:%d in sliceheader err\n", tmp_pic_parameter_set_id);
        return H264_ERR; /* 可以考虑不返回，折算pps id强解 */
    }

    if (tmp_pic_parameter_set_id >= H264_MAX_PPS_NUM) {
        dprint(PRN_ERROR, "%s overflow error.\n", __func__);
        return H264_ERR;
    }

    if (pps_sps_check_tmp_id(ctx, tmp_pic_parameter_set_id) != H264_OK) {
        dprint(PRN_WARN, "PPS or SPS of this slice not valid\n");
        return H264_ERR;
    }

    ctx->cur_slice.pic_parameter_set_id = tmp_pic_parameter_set_id;

    return H264_OK;
}

VFMW_ACC_SLICE_H264 STATIC td_s32 h264_check_gap_and_new(h264_ctx *ctx, const h264_sps *sps)
{
#if defined(VFMW_H264_SLC_LDY_SUPPORT)
    if (ctx->half_nal_try == TRY_HALF) {
        const td_u32 max_frame_num = 1 << (sps->log2_max_frame_num_minus4 + 4); /* 4 is num */
        if (ctx->cur_slice.frame_num != ctx->cur_slice.prev_frame_num &&
            ctx->cur_slice.frame_num != ((ctx->cur_slice.prev_frame_num + 1) % max_frame_num)) {
            /* half seg try do not process gap avoid err */
            return H264_ERR;
        }

        /* half seg only check firat mb avoid err */
        ctx->cur_slice.new_pic_flag = (ctx->cur_slice.first_mb_in_slice == 0);
        return H264_OK;
    }
#else
    vfmw_unused(sps);
#endif

    ctx->cur_slice.new_pic_flag = (td_u8)is_new_pic(ctx);
    if ((ctx->cur_slice.new_pic_flag == 0) &&
        (ctx->cur_slice.first_mb_in_slice <= ctx->prev_slice.first_mb_in_slice)) {
        dprint(PRN_ERROR, "first_mb_in_slice in currslice <= prevslice\n");
        return H264_ERR;
    }

    return H264_OK;
}

/***********************************************************************************
* Function:    process_sliceHeader(h264_ctx *ctx)
* Description: 解析Slice Header, 判断是否新图像
* Input:       语法解析回消耗码流
* Output:      Slice层语法元素,是否新图像
* Return:      DEC_NORMAL          可以解码此Nalu数据
*              DEC_ERR             不可以解码
* Others:      无
***********************************************************************************/
VFMW_ACC_SLICE_H264 STATIC td_s32 process_slice_header_first_part(h264_ctx *ctx)
{
    td_u32 redundant_pic_cnt;
    td_s32 ret;

    h264_sps *sps = TD_NULL;
    h264_pps *pps = TD_NULL;

    if (process_slice_header_first_part_judging(ctx) != H264_OK) {
        dprint(PRN_WARN, "process_slice_header_first_part_judging\n");
        return H264_ERR;
    }

    pps = (ctx->pps[ctx->cur_slice.pic_parameter_set_id]);
    sps = (ctx->sps[pps->seq_parameter_set_id]);

    /* 这个判断的条件可能还有问题 */
    if (((ctx->cur_slice.nal_unit_type == NALU_TYPE_IDR) || (sps->num_ref_frames == 0)) &&
        (ctx->cur_slice.slice_type != I_SLICE)) {
        dprint(PRN_ERROR, "recover point but isn't I_SLICE.\n");
        return H264_ERR;
    }

    if (h264_check_stm_len(ctx, 64) != H264_OK) { /* 64 bit num */
        return H264_ERR;
    }

    ctx->cur_slice.frame_num = (td_u32)u_v(ctx, sps->log2_max_frame_num_minus4 + 4, "SH: frame_num"); /* 4 is num */
    vfmw_check_ret(se_err_check(ctx->se_err_flg) == H264_OK, H264_ERR);

    if (ctx->cur_slice.nal_unit_type == NALU_TYPE_IDR) {
        ctx->cur_slice.prev_frame_num = ctx->cur_slice.frame_num;

        if ((ctx->cur_slice.frame_num != 0) && (ctx->cur_slice.nal_unit_type == NALU_TYPE_IDR)) {
            dprint(PRN_ERROR, "IDR NAL but frame_num!=0.\n");
        }
    }

    ret = process_slice_header_first_part_set_ctx(ctx, pps, sps);
    if (ret != H264_OK) {
        return ret;
    }

    vfmw_check_ret(se_err_check(ctx->se_err_flg) == H264_OK, H264_ERR);

    if (pps->redundant_pic_cnt_present_flag != 0) {
        redundant_pic_cnt = ue_v(ctx, "SH: redundant_pic_cnt");
        if ((redundant_pic_cnt > 0) || ctx->se_err_flg != 0) {
            dprint(PRN_ERROR, "redundant pic not support.\n");
            ctx->spec_invalid = SPEC_REDUNDANT_PIC;
            report_unsupport(ctx->chan_id);
            return H264_ERR;
        }
    }

    if (h264_check_gap_and_new(ctx, sps) != H264_OK) {
        return H264_ERR;
    }

    return H264_OK;
}

STATIC td_s32 process_slice_header_field_pic_flag(h264_ctx *ctx)
{
    if (ctx->cur_slice.field_pic_flag == 0) {
        if ((ctx->cur_slice.num_ref_idx_l0_active_minus1 > 15) || /* 15 vfmw_max val */
            ((ctx->cur_slice.num_ref_idx_l1_active_minus1 > 15) && /* 15 vfmw_max val */
            (ctx->cur_slice.slice_type == B_SLICE)) ||
            (ctx->se_err_flg != 0)) {
            dprint(PRN_ERROR, "frame num_ref_idx_lx_active_minus1 exceed\n");

            /* 可以考虑钳位然后强解 */
            return H264_ERR;
        }
    }
#ifdef H264_FIELD_SUPPORT
    else {
        if ((ctx->cur_slice.num_ref_idx_l0_active_minus1 > 31) || /* 31 vfmw_max val */
            ((ctx->cur_slice.num_ref_idx_l1_active_minus1 > 31) && /* 31 vfmw_max val */
            (ctx->cur_slice.slice_type == B_SLICE)) ||
            (ctx->se_err_flg != 0)) {
            dprint(PRN_ERROR, "field num_ref_idx_lx_active_minus1 exceed\n");

            /* 可以考虑钳位然后强解 */
            return H264_ERR;
        }
    }
#endif
    return H264_OK;
}

VFMW_ACC_SLICE_H264 STATIC td_s32 process_slice_header_second_part_slice_type(h264_ctx *ctx, const h264_pps *pps)
{
    td_u32 num_ref_idx_active_override_flag;

    /* Get num_ref_idx_lx_active_minus1 */
    num_ref_idx_active_override_flag = 0;
    ctx->cur_slice.num_ref_idx_l0_active_minus1 = pps->num_ref_idx_l0_active_minus1;
    ctx->cur_slice.num_ref_idx_l1_active_minus1 = pps->num_ref_idx_l1_active_minus1;

    if ((ctx->cur_slice.slice_type == P_SLICE) || (ctx->cur_slice.slice_type == B_SLICE)) {
        num_ref_idx_active_override_flag = u_1(ctx, "SH: num_ref_idx_override_flag");
        if (num_ref_idx_active_override_flag != 0) {
            ctx->cur_slice.num_ref_idx_l0_active_minus1 = ue_v(ctx, "SH: num_ref_idx_l0_active_minus1");
            vfmw_check_ret(ctx->cur_slice.num_ref_idx_l0_active_minus1 < 32, H264_ERR); /* 32: list size */

            if (ctx->cur_slice.slice_type == B_SLICE) {
                ctx->cur_slice.num_ref_idx_l1_active_minus1 = ue_v(ctx, "SH: num_ref_idx_l1_active_minus1");
                vfmw_check_ret(ctx->cur_slice.num_ref_idx_l1_active_minus1 < 32, H264_ERR); /* 32: list size */
            }
        }

        if (process_slice_header_field_pic_flag(ctx) == H264_ERR) {
            return H264_ERR;
        }
    }

    if (ctx->cur_slice.slice_type == I_SLICE) {
        ctx->cur_slice.num_ref_idx_l0_active_minus1 = 32; /* 32 invalid val */
    }

    if (ctx->cur_slice.slice_type != B_SLICE) {
        ctx->cur_slice.num_ref_idx_l1_active_minus1 = 32; /* 32 invalid val */
    }

    return H264_OK;
}

VFMW_ACC_SLICE_H264 STATIC td_s32 process_slice_header_set_ctx(h264_ctx *ctx, const h264_pps *pps)
{
    /* Get ref_pic_list_reordering parameters */
    if (ref_pic_list_reordering(ctx) != H264_OK) {
        dprint(PRN_ERROR, "reordering para dec err\n");
        return H264_ERR;
    }

    /* Get pred_weight_table */
    ctx->cur_slice.apply_weights_flag = (((pps->weighted_pred_flag != 0) && (ctx->cur_slice.slice_type == P_SLICE)) ||
                                         ((pps->weighted_bipred_idc == 1) && (ctx->cur_slice.slice_type == B_SLICE)));
    ctx->cur_slice.luma_log2_weight_denom = 0;
    ctx->cur_slice.chroma_log2_weight_denom = 0;

    if (ctx->cur_slice.apply_weights_flag != 0) {
        if (pred_weight_table(ctx) != H264_OK) {
            dprint(PRN_ERROR, "wpt dec err\n");
            return H264_ERR;
        }
    }

    /* Read MMCO marking parameters */
    if (ctx->cur_slice.nal_ref_idc != 0) {
        if (dec_ref_pic_marking(ctx) != H264_OK) {
            dprint(PRN_ERROR, "mark para dec err\n");
            return H264_ERR;
        }
    }

    /* Get cabac_init_idc */
    ctx->cur_slice.cabac_init_idc = 0;

    if ((pps->entropy_coding_mode_flag != 0) &&
        ((ctx->cur_slice.slice_type == P_SLICE) || (ctx->cur_slice.slice_type == B_SLICE))) {
        ctx->cur_slice.cabac_init_idc = ue_v(ctx, "SH: cabac_init_idc");

        if ((ctx->cur_slice.cabac_init_idc >= 3) || (ctx->se_err_flg != 0)) { /* 3 vfmw_max val */
            dprint(PRN_ERROR, "cabac_init_idc bigger than 3\n");
            /* 可以猜测一个值往下继续解析 */
            return H264_ERR;
        }
    }

    /* Get slice_qp_delta */
    ctx->cur_slice.slice_qp_delta = se_v(ctx, "SH: slice_qp_delta");

    /* Get deblocking filter control parameters */
    ctx->cur_slice.disable_deblocking_filter_idc = 0;
    ctx->cur_slice.slice_alpha_c0_offset_div2 = 0;
    ctx->cur_slice.slice_beta_offset_div2 = 0;

    return H264_OK;
}

STATIC td_s32 process_slice_header_second_part0(h264_ctx *ctx, const h264_pps *pps)
{
    if (pps->deblocking_filter_control_present_flag != 0) {
        ctx->cur_slice.disable_deblocking_filter_idc = ue_v(ctx, "SH: disable_deblocking_filter_idc");

        if ((ctx->cur_slice.disable_deblocking_filter_idc > 2) || (ctx->se_err_flg != 0)) { /* 2 vfmw_max val */
            dprint(PRN_ERROR, "disable_deblocking_filter_idc dec err\n");

            /* 可以考虑如何强解 */
            return H264_ERR;
        }

        if (ctx->cur_slice.disable_deblocking_filter_idc != 1) {
            ctx->cur_slice.slice_alpha_c0_offset_div2 = se_v(ctx, "SH: slice_alpha_c0_offset_div2");

            if ((ctx->cur_slice.slice_alpha_c0_offset_div2 < -6) || /* -6 vfmw_min val */
                (ctx->cur_slice.slice_alpha_c0_offset_div2 > 6) || (ctx->se_err_flg != 0)) { /* 6 vfmw_max val */
                dprint(PRN_ERROR, "slice_alpha_c0_offset_div2 dec err\n");

                /* 可以钳位强解 */
                return H264_ERR;
            }

            ctx->cur_slice.slice_beta_offset_div2 = se_v(ctx, "SH: slice_beta_offset_div2");

            if ((ctx->cur_slice.slice_beta_offset_div2 < -6) || /* -6 vfmw_min val */
                (ctx->cur_slice.slice_beta_offset_div2 > 6) || (ctx->se_err_flg != 0)) { /* 6 vfmw_max val */
                dprint(PRN_ERROR, "slice_beta_offset_div2 dec err\n");

                /* 可以钳位强解 */
                return H264_ERR;
            }
        }
    }

    return H264_OK;
}

/***********************************************************************************
* Function:    process_sliceHeader(h264_ctx *ctx)
* Description: 解析Slice Header, 判断是否新图像
* Input:       语法解析回消耗码流
* Output:      Slice层语法元素,是否新图像
* Return:      DEC_NORMAL          可以解码此Nalu数据
*              DEC_ERR             不可以解码
* Others:      无
***********************************************************************************/
VFMW_ACC_SLICE_H264 STATIC td_s32 process_slice_header_second_part(h264_ctx *ctx)
{
    h264_pps *pps = TD_NULL;
    td_s32 ret;

    if (((td_u32)(ctx->max_pps_num - 1) < ctx->cur_slice.pic_parameter_set_id) || (ctx->se_err_flg != 0) ||
        ctx->max_pps_num <= 0) {
        dprint(PRN_ERROR, "pps id:%d in sliceheader err\n", ctx->cur_slice.pic_parameter_set_id);
        return H264_ERR; /* 可以考虑不返回，折算pps id强解 */
    }

    if (pps_sps_check(ctx) != H264_OK) {
        dprint(PRN_WARN, "PPS or SPS of this slice not valid\n");
        return H264_ERR;
    }

    pps = (ctx->pps[ctx->cur_slice.pic_parameter_set_id]);

    /* Get mv direct predmode */
    if (ctx->cur_slice.slice_type == B_SLICE) {
        ctx->cur_slice.direct_spatial_mv_pred_flag = (td_u8)u_1(ctx, "SH: direct_spatial_mv_pred_flag");
    }

    ret = process_slice_header_second_part_slice_type(ctx, pps);
    if (ret != H264_OK) {
        dprint(PRN_ERROR, "%s err\n", __func__);
        return ret;
    }

    ret = process_slice_header_set_ctx(ctx, pps);
    if (ret != H264_OK) {
        dprint(PRN_ERROR, "%s err\n", __func__);
        return ret;
    }

    ret = process_slice_header_second_part0(ctx, pps);
    if (ret == H264_ERR) {
        return H264_ERR;
    }

    return H264_OK;
}

/*************************************************************************
*   函数原型 :  exit_slice (h264_ctx *ctx)
*   功能描述 :  保存前一个slice的参数, 为了能正确判断出new_pic_flag
*   参数说明 :  无
*   返回值   ： 无
*************************************************************************/
STATIC td_void exit_slice(h264_ctx *ctx)
{
    h264_sps *sps = TD_NULL;
    h264_pps *pps = TD_NULL;

    pps = (ctx->pps[ctx->cur_slice.pic_parameter_set_id]);
    sps = (ctx->sps[pps->seq_parameter_set_id]);

    ctx->prev_slice.pic_parameter_set_id = ctx->cur_slice.pic_parameter_set_id;

    ctx->prev_slice.seq_parameter_set_id = sps->seq_parameter_set_id;
    ctx->prev_slice.first_mb_in_slice = ctx->cur_slice.first_mb_in_slice;
    ctx->prev_slice.frame_num = ctx->cur_slice.frame_num;
    ctx->prev_slice.field_pic_flag = ctx->cur_slice.field_pic_flag;

    if (ctx->cur_slice.field_pic_flag != 0) {
        ctx->prev_slice.bottom_field_flag = ctx->cur_slice.bottom_field_flag;
    }

    ctx->prev_slice.nal_ref_idc = ctx->cur_slice.nal_ref_idc;
    ctx->prev_slice.nal_unit_type = ctx->cur_slice.nal_unit_type;

    if (ctx->cur_slice.nal_unit_type == NALU_TYPE_IDR) {
        ctx->prev_slice.idr_pic_id = ctx->cur_slice.idr_pic_id;
    }

    if (sps->pic_order_cnt_type == 0) {
        ctx->prev_slice.pic_oder_cnt_lsb = ctx->cur_poc.pic_order_cnt_lsb;
        ctx->prev_slice.delta_pic_oder_cnt_bottom = ctx->cur_poc.delta_pic_order_cnt_bottom;
    }

    if (sps->pic_order_cnt_type == 1) {
        ctx->prev_slice.delta_pic_order_cnt[0] = ctx->cur_poc.delta_pic_order_cnt[0];
        ctx->prev_slice.delta_pic_order_cnt[1] = ctx->cur_poc.delta_pic_order_cnt[1];
    }

    ctx->total_slice++;
}

/*************************************************************************
*      函数原型 :  td_void pic_type_statistic(h264_ctx *ctx)
*      功能描述 :
*      参数说明 :
*      返回值   ：
*************************************************************************/
STATIC td_void pic_type_statistic(h264_ctx *ctx)
{
    switch (ctx->cur_slice.slice_type) {
#ifdef H264_B_FRAME_SUPPORT
        case B_SLICE:
            ctx->cur_pic.pic_type = 2; /* 2 is B frm */
            ctx->cur_pic.is_idr_pic = 0;
            break;
#endif
        case P_SLICE:
            if (ctx->cur_pic.pic_type != 2) { /* 2 is B frm */
                ctx->cur_pic.pic_type = 1;
            }

            ctx->cur_pic.is_idr_pic = 0;
            break;

        case I_SLICE:
            if (ctx->cur_slice.nal_unit_type == NALU_TYPE_IDR) {
                ctx->cur_pic.is_idr_pic = 1;
            } else {
                ctx->cur_pic.is_idr_pic = 0;
            }
            break;

        default:
            break;
    }
}

STATIC td_void calc_stream_bitsoffset(h264_ctx *ctx, td_s32 *flag)
{
    td_u32 i, tmp;
    td_u32 code_len_byte;

    code_len_byte = 0;
    for (i = 0; i < (td_u32)ctx->cur_nal->nal_used_segment; i++) {
        code_len_byte += ctx->cur_nal->stream[i].bitstream_length;
    }

    ctx->cur_nal->stream[ctx->cur_nal->nal_used_segment].bits_offset = ctx->cur_nal->nal_bitoffset &
                                                                      7; /* 7 align */
    ctx->cur_nal->stream[ctx->cur_nal->nal_used_segment].bytes_pos =
        (ctx->cur_nal->nal_bitoffset / 8 - code_len_byte) + /* 8 bit num */
        ctx->cur_nal->stream[ctx->cur_nal->nal_used_segment].stream_phy_addr;
    tmp = ctx->cur_nal->stream[ctx->cur_nal->nal_used_segment].bitstream_length;
    vfmw_check(tmp < (td_u32)VFMW_INT_MAX);
    ctx->cur_nal->stream[ctx->cur_nal->nal_used_segment].valid_bitlen = tmp * 8; /* 8 bit num */
    ctx->cur_nal->stream[ctx->cur_nal->nal_used_segment].valid_bitlen -=
        (ctx->cur_nal->nal_bitoffset - (code_len_byte * 8)); /* 8 bit num */
    tmp = (td_u32)(ctx->cur_nal->stream[ctx->cur_nal->nal_used_segment].bytes_pos & H264_STREAM_BASE_ALIGN);
    vfmw_check(tmp < (td_u32)VFMW_INT_MAX);
    ctx->cur_nal->stream[ctx->cur_nal->nal_used_segment].bits_offset += 8 * tmp; /* 8 bit num */
    ctx->cur_nal->stream[ctx->cur_nal->nal_used_segment].bytes_pos =
        (td_u32)(ctx->cur_nal->stream[ctx->cur_nal->nal_used_segment].bytes_pos) & (~H264_STREAM_BASE_ALIGN);

    /* last sement sub trailing zeros, when cavlc sun stop one */
    if ((ctx->cur_nal->nal_segment < 1) || (ctx->cur_nal->nal_segment > 2)) { /* 2 nal num */
        dprint(PRN_ERROR, "nal_segment = %d is not expected value\n", ctx->cur_nal->nal_segment);
        *flag = H264_ERR;
        return;
    }

    /* other segment bitsoffset and so on */
    for (i = ctx->cur_nal->nal_used_segment + 1; (i < ctx->cur_nal->nal_segment) && (i < 2); i++) { /* 2 nal num */
        ctx->cur_nal->stream[i].bits_offset = 0;
        ctx->cur_nal->stream[i].bytes_pos = ctx->cur_nal->stream[i].stream_phy_addr;
        tmp = ctx->cur_nal->stream[i].bitstream_length;
        vfmw_check(tmp < (td_u32)VFMW_INT_MAX);
        ctx->cur_nal->stream[i].valid_bitlen = tmp * 8; /* 8 bit num */
        tmp = (td_u32)(ctx->cur_nal->stream[i].bytes_pos & 3); /* 3 is num */
        vfmw_check(tmp < (td_u32)VFMW_INT_MAX);
        ctx->cur_nal->stream[i].bits_offset += 8 * tmp; /* 8 3 is bit */
        ctx->cur_nal->stream[i].bytes_pos = (ctx->cur_nal->stream[i].bytes_pos) & (~H264_STREAM_BASE_ALIGN);
    }
    return;
}

/*************************************************************************
*      函数原型 :  td_void calc_stream_bits(h264_ctx *ctx)
*      功能描述 :
*      参数说明 :
*      返回值   ：
*************************************************************************/
STATIC td_void calc_stream_bits(h264_ctx *ctx)
{
    td_u32 i;
    td_s32 flag;

    if (ctx->cur_nal == TD_NULL) {
        dprint(PRN_ERROR, "%s:%d nal is null!\n", __func__, __LINE__);
        return;
    }

    if (ctx->cur_pps.entropy_coding_mode_flag != 0) {
        /* cabac, byte_aligned */
        ctx->cur_nal->nal_bitoffset = (ctx->cur_nal->nal_bitoffset + 7) & (~7); /* 7 align */
        /* stop one is valide bits */
        ctx->cur_nal->nal_trail_zero_bit_num -= 1;
    }

    /* calculate bytes used befor curr segment */
    flag = H264_OK;
    calc_stream_bitsoffset(ctx, &flag);
    if (flag == H264_ERR) {
        return;
    }

    ctx->cur_nal->stream[ctx->cur_nal->nal_segment - 1].valid_bitlen -= ctx->cur_nal->nal_trail_zero_bit_num;

    /* find stream_base_addr */
    for (i = ctx->cur_nal->nal_used_segment; i < ctx->cur_nal->nal_segment; i++) {
        if (ctx->cur_nal->stream[i].bytes_pos < ctx->cur_pic.stream_base_addr) {
            ctx->cur_pic.stream_base_addr = ctx->cur_nal->stream[i].bytes_pos;
        }
    }
}

STATIC td_s32 h264_dec_slice_head(h264_ctx *ctx, td_bool *go_on)
{
    td_s32 ret;
    td_u32 total_mbs, max_stream_thr;

    *go_on = TD_FALSE;

    ctx->cur_slice.nal_unit_type = ctx->cur_nal->nal_unit_type;
    ctx->cur_slice.nal_ref_idc = ctx->cur_nal->nal_ref_idc;

    ret = process_slice_header_first_part(ctx);
    if (ret != H264_OK) {
        dprint(PRN_WARN, "sliceheader dec err\n");
        clear_curr_slice(ctx);
        return H264_ERR;
    }

    if (h264_check_stm_len(ctx, 32) != H264_OK) { /* 32 is bit num */
        return H264_ERR;
    }

    ret = process_slice_header_second_part(ctx);
    if (ret != H264_OK) {
        dprint(PRN_WARN, "sliceheader dec err\n");
        clear_curr_slice(ctx);
        return H264_ERR;
    }

    if (h264_check_stm_len(ctx, 16) != H264_OK) { /* 16 is bit num */
        return H264_ERR;
    }

    /*  CNcomment: ctx->MaxBytesReceived阈值之前为定值MAX_BYTES_START，现改为w*h*1.5/3 */
    /* 2011-11-1, Q总发现"石斑"花屏是因为如下阈值太小导致误判所致, 所以改成标清和高清分别用不同的阈值. */
    vfmw_check_ret(ctx->cur_pic.pic_width_in_mb < VFMW_INT16_MAX, H264_ERR);
    vfmw_check_ret(ctx->cur_pic.pic_height_in_mb < VFMW_INT16_MAX, H264_ERR);
    total_mbs = ctx->cur_pic.pic_width_in_mb * ctx->cur_pic.pic_height_in_mb;

    vfmw_check_ret(total_mbs < (td_u32)VFMW_INT_MAX, H264_ERR);
    if (total_mbs > 1620) { /* 1620 is mb num */
        max_stream_thr = total_mbs * 128 * 2; /* 128 * 2 bit num */
    } else {
        /* 2011-11-23, 龙华测试5x6码流时发现TotalMbs*256作为阈值仍然太小，导致误判. 在超小图解码时，此阈值
           应相应加大，只要保证buf安全，阈值尽可能大才能减少误判. */
        max_stream_thr = vfmw_max(16 * 1024, (total_mbs * 256 * 3 / 2)); /* 16 * 1024 and 256 * 3 / 2 bit num */
    }

    if ((ctx->slice_para_num >= (td_u32)(ctx->max_slice_num)) || (ctx->max_bytes_received > max_stream_thr)) {
        dprint(PRN_ERROR, "Start Reason: slice_para_num, MaxBytesReceived = %d, %d(thr=%d)\n",
               ctx->slice_para_num, ctx->max_bytes_received, max_stream_thr);
        return H264_ERR;
    }

    *go_on = TD_TRUE;

    return H264_OK;
}

STATIC td_s32 h264_dec_set_field_type(const h264_ctx *ctx)
{
    fsp_logic *logic_fs = TD_NULL;

    vfmw_check_ret(ctx->cur_pic.frame_store != TD_NULL, H264_ERR);
    logic_fs = fsp_get_logic(ctx->chan_id, ctx->cur_pic.frame_store->logic_fs_id);
    vfmw_check_ret(logic_fs != TD_NULL, H264_ERR);
    switch (ctx->cur_pic.structure) {
        case TOP_FIELD:
            logic_fs->image.top_fld_type = ctx->cur_pic.pic_type;
            break;
        case BOTTOM_FIELD:
            logic_fs->image.bottom_fld_type = ctx->cur_pic.pic_type;
            break;
        default:
            break;
    }

    return H264_OK;
}

STATIC td_s32 h264_dec_slice_post(h264_ctx *ctx, td_bool *go_on)
{
    td_s32 ret;

    *go_on = TD_FALSE;

    pic_type_statistic(ctx);

    ret = dec_list(ctx);
    if (ret != H264_OK) {
        dprint(PRN_WARN, "dec_list error, ret=%d\n", ret);
        if ((ctx->extra_data->out_err_thr != 100) && /* 100 : vfmw_max err */
            (ctx->cur_pic.nal_ref_idc != 0) && (ctx->cur_slice.slice_type == P_SLICE)) {
            h264_clear_curr_pic(ctx);
            clear_dpb(ctx);
            dprint(PRN_ERROR, "%s:%d clear all!\n", __func__, __LINE__);
        }
        return H264_ERR;
    }

    ret = is_ref_list_wrong(ctx);
    if (ret != H264_OK) {
        dprint(PRN_ERROR, "dec list err.\n");
        clear_curr_slice(ctx);
        return H264_ERR;
    }

    calc_stream_bits(ctx);

    if (h264_dec_set_field_type(ctx) != H264_OK) {
        dprint(PRN_ERROR, "h264_dec_set_field_type err.\n");
        clear_curr_slice(ctx);
        return H264_ERR;
    }

    ret = write_slice_msg(ctx);
    if (ret == SYN_PARSE_RESOURCE_INVALID) {
        return SYN_PARSE_RESOURCE_INVALID;
    } else if (ret != H264_OK) {
        dprint(PRN_ERROR, "H264 write_slice_msg err.\n");
        return H264_ERR;
    }

    *go_on = TD_TRUE;

    return H264_OK;
}

/*************************************************************************
*      函数原型 :  td_s32 h264_dec_slice(h264_ctx *ctx)
*      功能描述 :  获得一个完整的nal，包括地址、长度、包括拖尾0；解析该nal。
*      参数说明 :  码流虚拟地址、物理地址、长度(肯定包括拖尾0和stopbit)
*      返回值   ： 解析类型
*************************************************************************/
VFMW_ACC_SLICE_H264 STATIC td_s32 h264_dec_slice(h264_ctx *ctx)
{
    td_s32 ret;
    td_bool go_on = TD_FALSE;
    td_bool option;

    ret = h264_dec_slice_head(ctx, &go_on);
    if (go_on != TD_TRUE) {
        return ret;
    }

    if (ctx->cur_slice.new_pic_flag != 0) {
        if (ctx->cur_slice.nal_unit_type == NALU_TYPE_IDR) {
            ctx->idr_pic_of_cur_pic = ctx->total_pic_num;
        }

        ctx->slice_num_in_pic = 0;
        ctx->total_pic_num++;

        if (ctx->cur_pic.structure == 0) {
            ctx->total_frame_num++;
        } else {
            ctx->total_field_num++;
        }

        ret = init_pic(ctx);
        if (ret == SYN_PARSE_RESOURCE_INVALID) {
            return ret;
        } else if (ret != H264_OK) {
            dprint(PRN_ERROR, "init pic err, find next recover point or next valid sps, pps, or exit\n");
            return H264_ERR;
        }
    }

    ret = h264_dec_slice_post(ctx, &go_on);
    if (go_on != TD_TRUE) {
        return ret;
    }

#if defined(VFMW_H264_SLC_LDY_SUPPORT)
    option = (ctx->half_nal_try == TRY_IDLE) ? 1 : 0;
#else
    option = 1;
#endif
    if (option != 0) {
        exit_slice(ctx);
    }

    ctx->allow_start_dec = 1;

    return H264_OK;
}

/***********************************************************************************
* Function:    td_void scaling_list(h264_ctx *ctx, td_s32 *scaling_list, td_s32 size_of_scaling_list,
                                    td_u32 *use_dflt_scaling_matrix)
* Description: 解析量化表，并拼位
* Input:         量化表存放指针，量化表大小，使用缺省指针
* Output:      缺省使用表指针赋值
* Return:      无
* Others:      无
***********************************************************************************/
STATIC td_void scaling_list(h264_ctx *ctx, td_u32 *scaling_list_org, td_u32 *scaling_list,
                            td_s32 size_of_scaling_list, td_u32 *use_dflt_scaling_matrix)
{
    td_s32 j;
    td_s32 delta_scale, last_scale, next_scale;
    td_s32 tmp_scale;
    td_u32 w_posit, b_posit;
    td_u32 scanj;
    td_u8 flag = 0;
    last_scale = 8;
    next_scale = 8;
    const td_u8 h264_zz_scan[16] = { /* 16 is array size */
        0, 1, 4, 8, 5, 2, 3, 6, 9, 12, 13, 10, 7, 11, 14, 15
    };
    const td_u8 h264_zz_scan8[64] = { /* 64 is array size */
        0, 1, 8, 16, 9, 2, 3, 10, 17, 24, 32, 25, 18, 11, 4, 5,
        12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13, 6, 7, 14, 21, 28,
        35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51,
        58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63
    };

    for (j = 0; j < size_of_scaling_list; j++) {
        scanj = (size_of_scaling_list == 16) ? h264_zz_scan[j] : h264_zz_scan8[j];

        if (next_scale != 0) {
            delta_scale = se_v(ctx, "   : delta_sl   ");

            flag = check_sint_plus_overflow(last_scale, delta_scale);
            vfmw_check_print(flag == 0, "%s, %d: integer operation overflow\n", __func__, __LINE__);
            flag = check_sint_plus_overflow((last_scale + delta_scale), 256);
            vfmw_check_print(flag == 0, "%s, %d: integer operation overflow\n", __func__, __LINE__);

            next_scale = (last_scale + delta_scale + 256) % 256;
            *use_dflt_scaling_matrix = (scanj == 0 && next_scale == 0);
        }

        tmp_scale = (next_scale == 0) ? last_scale : next_scale;
        scaling_list_org[scanj] = (td_u32)tmp_scale;
        last_scale = tmp_scale;

        /* 计算拼位的位置 */
        w_posit = scanj / 4; /* 4 is for calcu the position */
        b_posit = scanj % 4; /* 4 is for calcu the position */

        vfmw_check(b_posit < 0xff);
        scaling_list[w_posit] = (scaling_list[w_posit] & (~(0xff << (8 * b_posit)))) |   /* 8 is posit */
                                (((td_u32)tmp_scale & 0xff) << (8 * b_posit)); /* 8 is posit */
    }
}

STATIC td_s32 h264_dec_pps_parsing(h264_ctx *ctx, td_u32 pic_parameter_set_id, td_u32 seq_parameter_set_id)
{
    h264_pps *pps_tmp = &ctx->pps_tmp;

    if (ctx->pps[pic_parameter_set_id]->is_valid != 0) {
        pps_tmp->pic_parameter_set_id = pic_parameter_set_id;
        pps_tmp->seq_parameter_set_id = seq_parameter_set_id;

        if (process_pps(ctx, pps_tmp) != H264_OK) {
            dprint(PRN_ERROR, "PPS decode error.\n");
            return H264_ERR;
        }

        if (pps_equal(pps_tmp, (ctx->pps[pic_parameter_set_id])) != H264_OK) {
            pps_tmp->is_refreshed = 1;
            pps_tmp->is_valid = 1;

            vfmw_check_sec_func(memmove_s((ctx->pps[pic_parameter_set_id]),
                                          sizeof(h264_pps), pps_tmp, sizeof(h264_pps)));
        }
    } else {
        ctx->pps[pic_parameter_set_id]->pic_parameter_set_id = pic_parameter_set_id;
        ctx->pps[pic_parameter_set_id]->seq_parameter_set_id = seq_parameter_set_id;

        if (process_pps(ctx, (ctx->pps[pic_parameter_set_id])) != H264_OK) {
            dprint(PRN_ERROR, "PPS decode error.\n");
            ctx->pps[pic_parameter_set_id]->is_refreshed = 1;
            ctx->pps[pic_parameter_set_id]->is_valid = 0;
            return H264_ERR;
        } else {
            ctx->pps[pic_parameter_set_id]->is_refreshed = 1;
            ctx->pps[pic_parameter_set_id]->is_valid = 1;
        }
    }
    return H264_OK;
}

/***********************************************************************************
* Function:    h264_dec_pps(h264_ctx *ctx)
* Description: 解析PPS, 并完成容错处理
* Input:       在码流解析时会消耗码流
* Return:      DEC_NORMAL   可以解码此Nalu数据
*              DEC_ERR      不可以解码
* Others:      无
***********************************************************************************/
STATIC td_s32 h264_dec_pps(h264_ctx *ctx)
{
    td_s32 ret;
    td_u32 pic_parameter_set_id;
    td_u32 seq_parameter_set_id;

    pic_parameter_set_id = ue_v(ctx, "PPS: pic_parameter_set_id");
    if ((pic_parameter_set_id > (td_u32)(ctx->max_pps_num - 1)) || (pic_parameter_set_id > H264_MAX_PPS_NUM - 1) ||
        ctx->max_pps_num <= 0) {
        dprint(PRN_ERROR, "pic_parameter_set_id out of range.\n");

        /* 可以考虑钳位强解 */
        return H264_ERR;
    }

    seq_parameter_set_id = ue_v(ctx, "PPS: seq_parameter_set_id");
    if ((seq_parameter_set_id > (td_u32)(ctx->max_sps_num - 1)) || (seq_parameter_set_id > (H264_MAX_SPS_NUM - 1)) ||
        ctx->max_sps_num <= 0) {
        dprint(PRN_ERROR, "seq_parameter_set_id(%u) out of range[0, %d).\n", seq_parameter_set_id, ctx->max_sps_num);
        /* 可以考虑钳位强解, 钳位之后注意sps可用性的判断 */
        return H264_ERR;
    }

    if (ctx->sps[seq_parameter_set_id] == TD_NULL) {
        ctx->sps[seq_parameter_set_id] =
            (h264_sps *)h264_get_dyn_mem(ctx, sizeof(h264_sps), "sps", seq_parameter_set_id);
    }

    if (ctx->sps[seq_parameter_set_id] == TD_NULL) {
        dprint(PRN_ERROR, "SPS haven't decode, it is null.\n");
        return H264_ERR;
    }

    if (ctx->sps[seq_parameter_set_id]->is_valid == 0) {
        dprint(PRN_ERROR, "SPS haven't decode.\n");
        return H264_ERR;
    }

    if (ctx->pps[pic_parameter_set_id] == TD_NULL) {
        ctx->pps[pic_parameter_set_id] =
            (h264_pps *)h264_get_dyn_mem(ctx, sizeof(h264_pps), "pps", pic_parameter_set_id);
    }

    if (ctx->pps[pic_parameter_set_id] == TD_NULL) {
        dprint(PRN_ERROR, "SPS haven't decode, it is null.\n");
        return H264_ERR;
    }

    ret = h264_dec_pps_parsing(ctx, pic_parameter_set_id, seq_parameter_set_id);
    if (ret != H264_OK) {
        dprint(PRN_ERROR, "h264_dec_pps_parsing error\n");
        return ret;
    }

    return H264_OK;
}

STATIC td_s32 process_pps_init(h264_ctx *ctx, h264_pps *pps)
{
    td_u8 flag;

    /* num_slice_groups_minus1=0时表明无片分组 */
    if ((pps->num_slice_groups_minus1 != 0) || (ctx->se_err_flg != 0)) {
        dprint(PRN_ERROR, "Baseline stream with FMO(num_slice_groups_minus1=%d,se_err_flg=%d), not support.\n",
               pps->num_slice_groups_minus1, ctx->se_err_flg);
        return H264_ERR;
    }

    pps->num_ref_idx_l0_active_minus1 = ue_v(ctx, "PPS: num_ref_idx_l0_active_minus1");

    flag = ((pps->num_ref_idx_l0_active_minus1 > 31) || (ctx->se_err_flg != 0)); /* 31 is ref idx */
    /* 可以考虑钳位强解 */
    vfmw_check_ret_print(flag == 0, H264_ERR, "num_ref_idx_l0_active_minus1 out of range.\n");

    pps->num_ref_idx_l1_active_minus1 = ue_v(ctx, "PPS: num_ref_idx_l1_active_minus1");

    flag = ((pps->num_ref_idx_l1_active_minus1 > 31) || (ctx->se_err_flg != 0)); /* 31 is ref idx */
    /* 可以考虑钳位强解 */
    vfmw_check_ret_print(flag == 0, H264_ERR, "num_ref_idx_l1_active_minus1 out of range.\n");

    pps->weighted_pred_flag = (td_u8)u_1(ctx, "PPS: weighted prediction flag");
    vfmw_check_ret(se_err_check(ctx->se_err_flg) == H264_OK, H264_ERR);

    pps->weighted_bipred_idc = (td_u32)u_v(ctx, 2, "PPS: weighted_bipred_idc"); /* 2 is bit num */

    flag = (pps->weighted_bipred_idc >= 3); /* 3 vfmw_max idc */
    /* 可以考虑钳位强解 */
    vfmw_check_ret_print(flag == 0, H264_ERR, "weighted_bipred_idc out of range\n");

    pps->pic_init_qp_minus26 = se_v(ctx, "PPS: pic_init_qp_minus26");
    se_v(ctx, "PPS: pic_init_qs_minus26");
    pps->chroma_qp_index_offset = se_v(ctx, "PPS: chroma_qp_index_offset");
    pps->deblocking_filter_control_present_flag = (td_u8)u_1(ctx, "PPS: deblocking_filter_control_present_flag");
    pps->constrained_intra_pred_flag = (td_u8)u_1(ctx, "PPS: constrained_intra_pred_flag");
    pps->redundant_pic_cnt_present_flag = (td_u8)u_1(ctx, "PPS: redundant_pic_cnt_present_flag");

    /* main profile not support redundant_pic_cnt or not support redundant_pic_cnt more than 0 ? */
    if (pps->redundant_pic_cnt_present_flag != 0) {
        dprint(PRN_ERROR, "redundant pic not support when find redundant slice later, exit\n");
    }

    /* init para may not present in stream */
    pps->second_chroma_qp_index_offset = pps->chroma_qp_index_offset;
    pps->transform_8x8_mode_flag = 0;
    pps->pic_scaling_matrix_present_flag = 0;

    return H264_OK;
}

STATIC td_void process_pps_q4x4defaultflag(h264_ctx *ctx, h264_pps *pps, td_u32 q4x4_size)
{
    td_u32 q4x4defaultflag[6]; /* 6 is array size */
    td_u32 i;

    for (i = 0; i < 6; i++) { /* 6 is vfmw_max num */
        q4x4defaultflag[i] = u_1(ctx, "SPS: seq_scaling_list_present_flag");
        pps->pic_scaling_list_present_flag[i] = (td_s8)q4x4defaultflag[i];

        if (q4x4defaultflag[i] == 1) {
            scaling_list(ctx, pps->scaling_list4x4[i], pps->qmatrix4x4[i], (td_s32)q4x4_size, &q4x4defaultflag[i]);

            if (q4x4defaultflag[i] == 1) {
                vfmw_check_sec_func(memcpy_s(pps->qmatrix4x4[i], sizeof(pps->qmatrix4x4[i]),
                                             (i < 3) ? g_h264_quant_intra_default : /* 3 is idx */
                                             g_h264_quant_inter_default, q4x4_size));
            }
        } else {
            if (!(i == 0 || i == 3)) {
                vfmw_check_sec_func(memcpy_s(pps->qmatrix4x4[i], sizeof(pps->qmatrix4x4[i]),
                                             pps->qmatrix4x4[i - 1], q4x4_size));
                continue;
            }

            vfmw_check(pps->seq_parameter_set_id < H264_MAX_SPS_NUM);
            if (ctx->sps[pps->seq_parameter_set_id]->seq_scaling_matrix_present_flag == 1) {
                /* copy frome sps */
                vfmw_check_sec_func(memcpy_s(pps->qmatrix4x4[i], sizeof(pps->qmatrix4x4[i]),
                    ctx->sps[pps->seq_parameter_set_id]->qmatrix4x4[i], q4x4_size));
            } else {
                vfmw_check_sec_func(memcpy_s(pps->qmatrix4x4[i], sizeof(pps->qmatrix4x4[i]),
                                             (i == 0) ? g_h264_quant_intra_default :
                                             g_h264_quant_inter_default, q4x4_size));
            }
        }
    }
    return;
}

STATIC td_void process_pps_q8x8defaultflag(h264_ctx *ctx, h264_pps *pps, td_u32 q8x8_size)
{
    td_u32 q8x8defaultflag[2]; /* 2 is array size */
    td_u32 i;

    for (i = 0; i < 2; i++) { /* 2 vfmw_max num */
        if (pps->transform_8x8_mode_flag == 1) {
            q8x8defaultflag[i] = u_1(ctx, "PPS: pic_scaling_list_present_flag");
        } else {
            q8x8defaultflag[i] = 0;
        }

        pps->pic_scaling_list_present_flag[6 + i] = (td_s8)q8x8defaultflag[i]; /* 6 idx ofs */

        if (q8x8defaultflag[i] == 1) {
            scaling_list(ctx, pps->scaling_list8x8[i], pps->qmatrix8x8[i], (td_s32)q8x8_size, &q8x8defaultflag[i]);

            if (q8x8defaultflag[i] == 1) {
                vfmw_check_sec_func(memcpy_s(pps->qmatrix8x8[i], sizeof(pps->qmatrix8x8[i]),
                                             (i == 0) ? g_h264_quant8_intra_default :
                                             g_h264_quant8_inter_default, q8x8_size));
            }
        } else {
            vfmw_check(pps->seq_parameter_set_id < H264_MAX_SPS_NUM);
            if (ctx->sps[pps->seq_parameter_set_id]->seq_scaling_matrix_present_flag == 1) {
                /* fall back B, copy frome sps */
                vfmw_check_sec_func(memcpy_s(pps->qmatrix8x8[i], sizeof(pps->qmatrix8x8[i]),
                                             ctx->sps[pps->seq_parameter_set_id]->qmatrix8x8[i], q8x8_size));
            } else {
                vfmw_check_sec_func(memcpy_s(pps->qmatrix8x8[i], sizeof(pps->qmatrix8x8[i]),
                                             (i == 0) ? g_h264_quant8_intra_default :
                                             g_h264_quant8_inter_default, q8x8_size));
            }
        }
    }
    return;
}

/***********************************************************************************
* Function:    process_pps(h264_ctx *ctx, h264_pps *pps)
* Description: 解析PPS, 并完成容错处理
* Input:       在码流解析时会消耗码流
* Output:      pps         将PPS参数暂存到一个结构体
* Return:      DEC_NORMAL   可以解码此Nalu数据
*              DEC_ERR      不可以解码
* Others:      无
***********************************************************************************/
STATIC td_s32 process_pps(h264_ctx *ctx, h264_pps *pps)
{
    td_u32 residbits;
    td_s32 ret;
    td_u8 flag;
    td_u32 q4x4_size;
    td_u32 q8x8_size;

    pps->entropy_coding_mode_flag = (td_u8)u_1(ctx, "PPS: entropy_coding_mode_flag");
    vfmw_check_ret(se_err_check(ctx->se_err_flg) == H264_OK, H264_ERR);

    pps->pic_order_present_flag = (td_u8)u_1(ctx, "PPS: pic_order_present_flag");
    vfmw_check_ret(se_err_check(ctx->se_err_flg) == H264_OK, H264_ERR);

    q4x4_size = 16; /* 16, a number */
    q8x8_size = 64; /* 64, a number */
    pps->num_slice_groups_minus1 = ue_v(ctx, "PPS: num_slice_groups_minus1");

    ret = process_pps_init(ctx, pps);
    if (ret != H264_OK) {
        return ret;
    }

    /* more_rbsp_data, residue bits more than trail zero */
    residbits = (td_u32)bs_residual_bits(&(ctx->bs));
    if (residbits >= (ctx->cur_nal->nal_trail_zero_bit_num + 3)) { /* 3 is bit num */
        pps->transform_8x8_mode_flag = u_1(ctx, "PPS: transform_8x8_mode_flag");
        pps->pic_scaling_matrix_present_flag = (td_u8)u_1(ctx, "PPS: pic_scaling_matrix_present_flag");
        vfmw_check_ret(pps->pic_scaling_matrix_present_flag <= 1, H264_ERR);

        if (pps->pic_scaling_matrix_present_flag == 1) {
            process_pps_q4x4defaultflag(ctx, pps, q4x4_size);
            process_pps_q8x8defaultflag(ctx, pps, q8x8_size);
        }

        pps->second_chroma_qp_index_offset = se_v(ctx, "PPS: second_chroma_qp_index_offset");

        flag = ((pps->second_chroma_qp_index_offset > 12) || /* 12 vfmw_max val */
                (pps->second_chroma_qp_index_offset < -12) || (ctx->se_err_flg != 0)); /* -12 vfmw_min val */
        vfmw_check_ret_print(flag == 0, H264_ERR, "second_chroma_qp_index_offset out of range.\n");
    }

    return H264_OK;
}

STATIC td_void pps_equal_flag(const h264_pps *pps1, const h264_pps *pps2, td_u32 i, td_u32 *equal)
{
    td_u32 j;

    vfmw_check(i < 12); /* 12 array size */
    if (pps1->pic_scaling_list_present_flag[i] != 0) {
        if (i < 6) { /* 6 is idx */
            for (j = 0; j < 16; j++) { /* 16 vfmw_max num */
                *equal &= (td_u32)(pps1->scaling_list4x4[i][j] == pps2->scaling_list4x4[i][j]);
            }
        } else {
            vfmw_check(i - 6 < 6); /* 6 array size */
            for (j = 0; j < 64; j++) { /* 64 vfmw_max num */
                *equal &= (td_u32)(pps1->scaling_list8x8[i - 6][j] ==  /* 6 idx */
                                  pps2->scaling_list8x8[i - 6][j]);  /* 6 idx */
            }
        }
    }
    return;
}

/***********************************************************************************
* Function:    pps_equal(h264_pps *pps1, h264_pps *pps2)
* Description: 比较两个SPS是否相等
* Input:       pps1, pps2  两个待比较的PPS
* Output:      无
* Return:      DEC_TRUE   两个PPS是相等的
*              DEC_FALSE  两个PPS不相等
* Others:      无
***********************************************************************************/
STATIC td_s32 pps_equal(const h264_pps *pps1, const h264_pps *pps2)
{
    td_u32 i;
    td_u32 equal;
    td_s32 ret;

    vfmw_check_ret(pps1 != TD_NULL, H264_ERR);
    vfmw_check_ret(pps2 != TD_NULL, H264_ERR);

    equal = 1;
    equal &= (td_u32)(pps1->pic_parameter_set_id == pps2->pic_parameter_set_id);
    equal &= (td_u32)(pps1->seq_parameter_set_id == pps2->seq_parameter_set_id);
    equal &= (td_u32)(pps1->entropy_coding_mode_flag == pps2->entropy_coding_mode_flag);
    equal &= (td_u32)(pps1->pic_order_present_flag == pps2->pic_order_present_flag);
    equal &= (td_u32)(pps1->num_ref_idx_l0_active_minus1 == pps2->num_ref_idx_l0_active_minus1);
    equal &= (td_u32)(pps1->num_ref_idx_l1_active_minus1 == pps2->num_ref_idx_l1_active_minus1);
    equal &= (td_u32)(pps1->weighted_pred_flag == pps2->weighted_pred_flag);
    equal &= (td_u32)(pps1->weighted_bipred_idc == pps2->weighted_bipred_idc);
    equal &= (td_u32)(pps1->pic_init_qp_minus26 == pps2->pic_init_qp_minus26);
    equal &= (td_u32)(pps1->chroma_qp_index_offset == pps2->chroma_qp_index_offset);
    equal &= (td_u32)(pps1->deblocking_filter_control_present_flag == pps2->deblocking_filter_control_present_flag);
    equal &= (td_u32)(pps1->constrained_intra_pred_flag == pps2->constrained_intra_pred_flag);

    /* Fidelity Range Extensions Stuff */
    /* It is initialized to zero, so should be ok to check all the time. */
    equal &= (td_u32)(pps1->transform_8x8_mode_flag == pps2->transform_8x8_mode_flag);
    equal &= (td_u32)(pps1->pic_scaling_matrix_present_flag == pps2->pic_scaling_matrix_present_flag);

    if (pps1->pic_scaling_matrix_present_flag != 0) {
        for (i = 0; i < (6 + (pps1->transform_8x8_mode_flag << 1)); i++) { /* 6 idx ofs */
            equal &= (td_u32)(pps1->pic_scaling_list_present_flag[i] == pps2->pic_scaling_list_present_flag[i]);
            pps_equal_flag(pps1, pps2, i, &equal);
        }
    }

    equal &= (td_u32)(pps1->second_chroma_qp_index_offset == pps2->second_chroma_qp_index_offset);

    ret = (equal != 0) ? H264_OK : H264_ERR;

    return ret;
}

STATIC td_s32 vui_parameters_vcl_flag(h264_ctx *ctx, h264_uvi_seq_params *vui)
{
    vui->vcl_hrd_parameters_present_flag = (td_u8)u_1(ctx, "VUI: vcl_hrd_parameters_present_flag");

    if (vui->vcl_hrd_parameters_present_flag != 0) {
        td_u32 sched_se1_idx;
        h264_hdr_params *hrd = &(vui->vcl_hrd_parameters);

        hrd->cpb_cnt_minus1 = ue_v(ctx, "VUI: cpb_cnt_minus1");
        hrd->bit_rate_scale = (td_u8)u_v(ctx, 4, "VUI: bit_rate_scale"); /* 4 bit num */
        hrd->cpb_size_scale = (td_u8)u_v(ctx, 4, "VUI: cpb_size_scale"); /* 4 bit num */

        if ((hrd->cpb_cnt_minus1 > 31) || ctx->se_err_flg != 0) { /* 31 vfmw_max val */
            dprint(PRN_ERROR, "hrd->cpb_cnt_minus1 out of range\n");
            return H264_ERR;
        }

        for (sched_se1_idx = 0; sched_se1_idx <= hrd->cpb_cnt_minus1; sched_se1_idx++) {
            hrd->bit_rate_value_minus1[sched_se1_idx] = ue_v(ctx, "VUI: bit_rate_value_minus1");
            hrd->cpb_size_value_minus1[sched_se1_idx] = ue_v(ctx, "VUI: cpb_size_value_minus1");
            hrd->cbr_flag[sched_se1_idx] = (td_u8)u_1(ctx, "VUI: cbr_flag");
        }

        hrd->initial_cpb_removal_delay_length_minus1 = (td_u32)u_v(ctx, 5, "VUI: init delay_len_minus1"); /* 5 bit */
        hrd->cpb_removal_delay_length_minus1 = (td_u32)u_v(ctx, 5, "VUI: cpb_rm_delay_length_minus1"); /* 5 bit num */
        hrd->dpb_output_delay_length_minus1 = (td_u32)u_v(ctx, 5, "VUI: dpb_out_delay_length_minus1"); /* 5 bit num */
        hrd->time_offset_length = (td_u32)u_v(ctx, 5, "VUI: time_offset_length"); /* 5 bit num */
    }

    if ((vui->nal_hrd_parameters_present_flag != 0) || (vui->vcl_hrd_parameters_present_flag != 0)) {
        vui->low_delay_hrd_flag = (td_u8)u_1(ctx, "VUI: low_delay_hrd_flag");
    }

    vui->pic_struct_present_flag = (td_u8)u_1(ctx, "VUI: pic_struct_present_flag");
    vui->bitstream_restriction_flag = (td_u8)u_1(ctx, "VUI: bitstream_restriction_flag");

    if (vui->bitstream_restriction_flag != 0) {
        vui->motion_vectors_over_pic_boundaries_flag = (td_u8)u_1(ctx, "VUI: motion_vectors_over_pic_boundaries_flag");
        vui->max_bytes_per_pic_denom = ue_v(ctx, "VUI: max_bytes_per_pic_denom");
        vui->max_bits_per_mb_denom = ue_v(ctx, "VUI: max_bits_per_mb_denom");
        vui->log2_max_mv_length_horizontal = ue_v(ctx, "VUI: log2_max_mv_length_horizontal");
        vui->log2_max_mv_length_vertical = ue_v(ctx, "VUI: log2_max_mv_length_vertical");
        vui->num_reorder_frames = ue_v(ctx, "VUI: num_reorder_frames");
        vui->max_dec_frame_buffering = ue_v(ctx, "VUI: max_dec_frame_buffering");
        vfmw_check_ret(se_err_check(ctx->se_err_flg) == H264_OK, H264_ERR);
    }
    return H264_OK;
}

STATIC td_s32 vui_parameters_nal_flag(h264_ctx *ctx, h264_uvi_seq_params *vui)
{
    vui->nal_hrd_parameters_present_flag = (td_u8)u_1(ctx, "VUI: nal_hrd_parameters_present_flag");

    if (vui->nal_hrd_parameters_present_flag != 0) {
        td_u32 sched_se1_idx;
        h264_hdr_params *hrd = &(vui->nal_hrd_parameters);

        hrd->cpb_cnt_minus1 = ue_v(ctx, "VUI: cpb_cnt_minus1");
        hrd->bit_rate_scale = (td_u8)u_v(ctx, 4, "VUI: bit_rate_scale"); /* 4 bit num */
        hrd->cpb_size_scale = (td_u8)u_v(ctx, 4, "VUI: cpb_size_scale"); /* 4 bit num */

        if ((hrd->cpb_cnt_minus1 > 31) || (ctx->se_err_flg != 0)) { /* 31 vfmw_max val */
            dprint(PRN_ERROR, "hrd->cpb_cnt_minus1 out of range\n");
            return H264_ERR;
        }

        for (sched_se1_idx = 0; sched_se1_idx <= hrd->cpb_cnt_minus1; sched_se1_idx++) {
            hrd->bit_rate_value_minus1[sched_se1_idx] = ue_v(ctx, "VUI: bit_rate_value_minus1");
            hrd->cpb_size_value_minus1[sched_se1_idx] = ue_v(ctx, "VUI: cpb_size_value_minus1");
            hrd->cbr_flag[sched_se1_idx] = (td_u8)u_1(ctx, "VUI: cbr_flag");
        }

        hrd->initial_cpb_removal_delay_length_minus1 = (td_u32)u_v(ctx, 5, "VUI: init_rm_dly_len_m1"); /* 5 bit num */
        hrd->cpb_removal_delay_length_minus1 = (td_u32)u_v(ctx, 5, "VUI: cpb_rm_delay_len_minus1"); /* 5 bit num */
        hrd->dpb_output_delay_length_minus1 = (td_u32)u_v(ctx, 5, "VUI: dpb_out_delay_len_minus1"); /* 5 bit num */
        hrd->time_offset_length = (td_u32)u_v(ctx, 5, "VUI: time_offset_length"); /* 5 bit num */
    }
    return H264_OK;
}

STATIC td_void vui_parameters_timing_flag(h264_ctx *ctx, h264_uvi_seq_params *vui)
{
    vui->timing_info_present_flag = (td_u8)u_1(ctx, "VUI: timing_info_present_flag");

    if (vui->timing_info_present_flag != 0) {
        vui->num_units_in_tick = (td_u32)u_v(ctx, 32, "VUI: num_units_in_tick"); /* 32 bit num */
        vui->time_scale = (td_u32)u_v(ctx, 32, "VUI: time_scale"); /* 32 bit num */
        vui->fixed_frame_rate_flag = (td_u8)u_1(ctx, "VUI: fixed_frame_rate_flag");

        if (vui->num_units_in_tick != 0) {
            td_u64 result = ((td_u64)vui->time_scale * 1000 * 10) / 2; /* 1000 10 2 times */
            ctx->parse_frame_rate = (td_u32)os_do_div(result, vui->num_units_in_tick);
            ctx->parse_frame_rate = (ctx->parse_frame_rate + 5) / 10; /* 5 10 is num */
        }
    } else {
        ctx->parse_frame_rate = 0xffffffff;
    }
    return;
}

/***********************************************************************************
* Function:    vui_parameters(h264_ctx *ctx)
* Description: 解析VUI语法元素, 并完成容错处理
* Input:       在码流解析时会消耗码流
* Output:      sps        只会影响数组中的一个元素结构体的VUI部分
* Return:      DEC_NORMAL   可以解码此Nalu数据
*              DEC_ERR      不可以解码
* Others:      无
***********************************************************************************/
STATIC td_s32 vui_parameters(h264_ctx *ctx, h264_uvi_seq_params *vui)
{
    td_s32 ret;

    vui->aspect_ratio_info_present_flag = (td_u8)u_1(ctx, "VUI: aspect_ratio_info_present_flag");

    if (vui->aspect_ratio_info_present_flag != 0) {
        vui->aspect_ratio_idc = (td_u8)u_v(ctx, 8, "VUI: aspect_ratio_idc"); /* 8 bit num */

        if (vui->aspect_ratio_idc == 255) { /* 255 is idc */
            vui->sar_width = (td_u32)u_v(ctx, 16, "VUI: sar_width"); /* 16 bit num */
            vui->sar_height = (td_u32)u_v(ctx, 16, "VUI: sar_height"); /* 16 bit num */
        }
    } else {
        vui->aspect_ratio_idc = 0;
    }

    vui->overscan_info_present_flag = (td_u8)u_1(ctx, "VUI: overscan_info_present_flag");

    if (vui->overscan_info_present_flag != 0) {
        vui->overscan_appropriate_flag = (td_u8)u_1(ctx, "VUI: overscan_appropriate_flag");
    }

    vui->video_signal_type_present_flag = (td_u8)u_1(ctx, "VUI: video_signal_type_present_flag");

    if (vui->video_signal_type_present_flag != 0) {
        vui->video_format = (td_u32)u_v(ctx, 3, "VUI: video_format"); /* 3 bit num */
        vui->video_full_range_flag = (td_u8)u_1(ctx, "VUI: video_full_range_flag");
        vui->colour_description_present_flag = (td_u8)u_1(ctx, "VUI: color_description_present_flag");

        if (vui->colour_description_present_flag != 0) {
            vui->colour_primaries = (td_u32)u_v(ctx, 8, "VUI: colour_primaries"); /* 8 bit num */
            vui->transfer_characteristics = (td_u32)u_v(ctx, 8, "VUI: transfer_characteristics"); /* 8 bit num */
            vui->matrix_coefficients = (td_u32)u_v(ctx, 8, "VUI: matrix_coefficients"); /* 8 bit num */

            report_hdr_info(ctx->chan_id, vui->video_full_range_flag, vui->colour_primaries,
                            vui->matrix_coefficients, vui->transfer_characteristics);
        }
    } else {
        vui->video_format = 5; /* 5 unknow type */
    }

    vui->chroma_location_info_present_flag = (td_u8)u_1(ctx, "VUI: chroma_loc_info_present_flag");

    if (vui->chroma_location_info_present_flag != 0) {
        vui->chroma_sample_loc_type_top_field = ue_v(ctx, "VUI: chroma_sample_loc_type_top_field");
        vui->chroma_sample_loc_type_bottom_field = ue_v(ctx, "VUI: chroma_sample_loc_type_bottom_field");
    }

    vui_parameters_timing_flag(ctx, vui);

    ret = vui_parameters_nal_flag(ctx, vui);
    if (ret != H264_OK) {
        return ret;
    }

    ret = vui_parameters_vcl_flag(ctx, vui);
    if (ret != H264_OK) {
        return ret;
    }

    vfmw_check_ret(se_err_check(ctx->se_err_flg) == H264_OK, H264_ERR);

    return H264_OK;
}

STATIC td_void sps_seq_scaling_list_present_flag(const h264_sps *sps1, const h264_sps *sps2, td_u32 *equal, td_u32 i)
{
    td_u32 j;

    vfmw_check(i < 12); /* 12 array size */
    if (sps1->seq_scaling_list_present_flag[i] != 0) {
        if (i < 6) { /* 6 is idx */
            for (j = 0; j < 16; j++) { /* 16 vfmw_max num */
                *equal &= (td_u32)(sps1->scaling_list4x4[i][j] == sps2->scaling_list4x4[i][j]);
            }
        } else {
            vfmw_check(i - 6 < 6); /* 6 array size */
            for (j = 0; j < 64; j++) { /* 64 vfmw_max num */
                *equal &= (td_u32)(sps1->scaling_list8x8[i - 6][j] ==  /* 6 idx */
                                    sps2->scaling_list8x8[i - 6][j]); /* 6 idx */
            }
        }
    }
}

STATIC td_void sps_equal_flag(const h264_sps *sps1, const h264_sps *sps2, td_u32 *equal)
{
    td_u32 i;

    for (i = 0; i < (6 + 2); i++) { /* 6 2 idx */
        *equal &= (td_u32)(sps1->seq_scaling_list_present_flag[i] == sps2->seq_scaling_list_present_flag[i]);

        sps_seq_scaling_list_present_flag(sps1, sps2, equal, i);
    }

    return;
}

STATIC td_void sps_equal_cnt_type(const h264_sps *sps1, const h264_sps *sps2, td_u32 *equal)
{
    td_u32 i;

    switch (sps1->pic_order_cnt_type) {
        case 0: /* 0 is poc type */
            *equal &= (td_u32)(sps1->log2_max_pic_order_cnt_lsb_minus4 == sps2->log2_max_pic_order_cnt_lsb_minus4);
            break;

        case 1: /* 1 is poc type */
            *equal &= (td_u32)(sps1->delta_pic_order_always_zero_flag == sps2->delta_pic_order_always_zero_flag);
            *equal &= (td_u32)(sps1->offset_for_non_ref_pic == sps2->offset_for_non_ref_pic);
            *equal &= (td_u32)(sps1->offset_for_top_to_bottom_field == sps2->offset_for_top_to_bottom_field);
            *equal &=
               (td_u32)(sps1->num_ref_frames_in_pic_order_cnt_cycle == sps2->num_ref_frames_in_pic_order_cnt_cycle);

            for (i = 0; i < sps1->num_ref_frames_in_pic_order_cnt_cycle; i++) {
                *equal &= (td_u32)(sps1->offset_for_ref_frame[i] == sps2->offset_for_ref_frame[i]);
            }
            break;

        default:
            break;
    }
    return;
}

/***********************************************************************************
* Function:    sps_equal(H264_SPS_S *sps1, H264_SPS_S *sps2)
* Description: 比较两个SPS是否相等
* Input:       sps1, sps2  两个待比较的SPS
* Output:      无
* Return:      DEC_TRUE   两个SPS是相等的
*              DEC_FALSE  两个SPS不相等
* Others:      无
***********************************************************************************/
STATIC td_s32 sps_equal(const h264_sps *sps1, const h264_sps *sps2)
{
    td_u32 equal;
    td_s32 ret;

    vfmw_check_ret(sps1 != TD_NULL, H264_ERR);
    vfmw_check_ret(sps2 != TD_NULL, H264_ERR);

    equal = 1;
    equal &= (td_u32)(sps1->profile_idc == sps2->profile_idc);
    equal &= (td_u32)(sps1->constrained_set0_flag == sps2->constrained_set0_flag);
    equal &= (td_u32)(sps1->constrained_set1_flag == sps2->constrained_set1_flag);
    equal &= (td_u32)(sps1->constrained_set2_flag == sps2->constrained_set2_flag);
    equal &= (td_u32)(sps1->level_idc == sps2->level_idc);
    equal &= (td_u32)(sps1->seq_parameter_set_id == sps2->seq_parameter_set_id);
    equal &= (td_u32)(sps1->seq_scaling_matrix_present_flag == sps2->seq_scaling_matrix_present_flag);

    if (sps1->seq_scaling_matrix_present_flag != 0) {
        sps_equal_flag(sps1, sps2, &equal);
    }

    equal &= (td_u32)(sps1->log2_max_frame_num_minus4 == sps2->log2_max_frame_num_minus4);
    equal &= (td_u32)(sps1->pic_order_cnt_type == sps2->pic_order_cnt_type);

    sps_equal_cnt_type(sps1, sps2, &equal);

    equal &= (td_u32)(sps1->num_ref_frames == sps2->num_ref_frames);
    equal &= (td_u32)(sps1->gaps_in_frame_num_value_allowed_flag == sps2->gaps_in_frame_num_value_allowed_flag);
    equal &= (td_u32)(sps1->pic_width_in_mbs_minus1 == sps2->pic_width_in_mbs_minus1);
    equal &= (td_u32)(sps1->pic_height_in_map_units_minus1 == sps2->pic_height_in_map_units_minus1);
    equal &= (td_u32)(sps1->frame_mbs_only_flag == sps2->frame_mbs_only_flag);

    if (sps1->frame_mbs_only_flag == 0) {
        equal &= (td_u32)(sps1->mb_adaptive_frame_field_flag == sps2->mb_adaptive_frame_field_flag);
    }

    equal &= (td_u32)(sps1->direct_8x8_inference_flag == sps2->direct_8x8_inference_flag);

    equal &= (td_u32)(sps1->frame_cropping_flag == sps2->frame_cropping_flag);

    if (sps1->frame_cropping_flag != 0) {
        equal &= (td_u32)(sps1->frame_crop_left_offset == sps2->frame_crop_left_offset);
        equal &= (td_u32)(sps1->frame_crop_right_offset == sps2->frame_crop_right_offset);
        equal &= (td_u32)(sps1->frame_crop_top_offset == sps2->frame_crop_top_offset);
        equal &= (td_u32)(sps1->frame_crop_bottom_offset == sps2->frame_crop_bottom_offset);
    }

    equal &= (td_u32)(sps1->vui_parameters_present_flag == sps2->vui_parameters_present_flag);

    ret = (equal != 0) ? H264_OK : H264_ERR;

    return ret;
}

STATIC td_s32 h264_get_dar_ar_idc1(td_s32 img_width, td_s32 img_height)
{
    if (((img_width == 3840) && (img_height == 2160)) ||
        ((img_width == 1920) && (img_height == 1088)) ||
        ((img_width == 1920) && (img_height == 1080)) ||
        ((img_width == 1280) && (img_height == 720))) {
        return VFMW_DAR_16_9;
    } else if ((img_width == 640) && (img_height == 480)) {
        return VFMW_DAR_4_3;
    } else {
        return VFMW_DAR_IMG_SIZE;
    }
}

/***********************************************************************************
* Function:      td_s32 h264_get_dar(td_s32 ar_idc, td_s32 sar_width, td_s32 sar_height,
                                     td_s32 img_width, td_s32 img_height)
* Description:  计算 display_aspec_ratio
* Input:           sar width, sar height
* Output:
* Return:        aspec_ratio
* Others:      无
***********************************************************************************/
#define CONST_DAR_235_100 (235 * 1024 / 100)
#define CONST_DAR_221_100 (221 * 1024 / 100)
#define CONST_DAR_16_9    (16 * 1024 / 9)
#define CONST_DAR_4_3     (4 * 1024 / 3)

STATIC td_s32 h264_get_dar_other(td_s32 ar_idc, td_s32 sar_width,
    td_s32 sar_height, td_s32 img_width, td_s32 img_height)
{
    td_s32 dar = 0;

    vfmw_unused(ar_idc);
    /* 20131012:  上面的计算方法有可能导致数值(sar_width * img_width * 1024)
           溢出，最终将导致SINT32类型的da值为 -1 */
    if ((sar_height != 0) && (img_height != 0)) {
        dar = (((sar_width * 1024) / (sar_height)) * ((img_width * 1024) / (img_height))) / 1024;
    }

    if (vfmw_abs(dar - CONST_DAR_235_100) < vfmw_abs(dar - CONST_DAR_221_100)) {
        return VFMW_DAR_235_100;
    } else if (vfmw_abs(dar - CONST_DAR_221_100) < vfmw_abs(dar - CONST_DAR_16_9)) {
        return VFMW_DAR_221_100;
    } else if (vfmw_abs(dar - CONST_DAR_16_9) < vfmw_abs(dar - CONST_DAR_4_3)) {
        return VFMW_DAR_16_9;
    } else {
        return VFMW_DAR_4_3;
    }

}

STATIC td_s32 h264_get_dar(td_s32 ar_idc, td_s32 sar_width, td_s32 sar_height, td_s32 img_width, td_s32 img_height)
{
    td_s32 sar_width_tmp = sar_width;
    td_s32 sar_height_tmp = sar_height;
    STATIC td_s32 sar_table[][2] = {
        { 1,  1 }, { 1, 1 }, { 12, 11 }, { 10, 11 }, { 16, 11 },
        { 40, 33 }, { 24, 11 }, { 20, 11 }, { 32, 11 }, { 80, 33 },
        { 18, 11 }, { 15, 11 }, { 64, 33 }, { 160, 99 }, { 4, 3 }, { 3, 2 }, { 2, 1 }
    };
    td_s32 final_dar = 0;

    if (ar_idc == 0 || (ar_idc > 16 && ar_idc != 255) || ((sar_width == 0 || sar_height == 0) && (ar_idc == 255))) {
        final_dar = VFMW_DAR_UNKNOWN;
    }
    /* 2011-11-21.
       sample_aspect_ratio = 1:1本来意味着显示宽高比等于图象的宽度:高度，以下分支本来无错. 但目前VO把1:1理解为无效，
       做法是填充输出窗口，这使得显示效果不可控.
       鉴于各种条件VO不敢做改动，以免使现有效果被改变而引起客户投诉. 未来VO会对宽高比做一次彻底重构，所以暂时把下面
       ar_idc==1的分支去掉，以配合VO把眼下的问题解决掉. */
    else if (ar_idc == 1) {
        final_dar = h264_get_dar_ar_idc1(img_width, img_height);
    } else {
        if (ar_idc != 255) {
            sar_width_tmp = sar_table[ar_idc][0];
            sar_height_tmp = sar_table[ar_idc][1];
        }

        final_dar = h264_get_dar_other(ar_idc, sar_width_tmp, sar_height_tmp, img_width, img_height);
    }

    return final_dar;
}

#define Q8X8_SIZE 64
STATIC td_void h264_process_sps_matrix_8x8(h264_ctx *ctx, h264_sps *sps)
{
    td_u32 i;
    td_u32 q8x8defaultflag[2]; /* 2 is array size */

    for (i = 0; i < 2; i++) { /* 2 vfmw_max num */
        q8x8defaultflag[i] = u_1(ctx, "SPS: seq_scaling_list_present_flag");
        sps->seq_scaling_list_present_flag[6 + i] = (td_s8)q8x8defaultflag[i]; /* 6 idx ofs */

        if (q8x8defaultflag[i] == 1) {
            scaling_list(ctx, sps->scaling_list8x8[i], sps->qmatrix8x8[i],
                         Q8X8_SIZE, &q8x8defaultflag[i]);

            if (q8x8defaultflag[i] == 1) {
                vfmw_check_sec_func(memcpy_s(sps->qmatrix8x8[i], sizeof(sps->qmatrix8x8[i]),
                                             (i == 0) ? g_h264_quant8_intra_default :
                                             g_h264_quant8_inter_default, Q8X8_SIZE));
            }
        } else {
            vfmw_check_sec_func(memcpy_s(sps->qmatrix8x8[i], sizeof(sps->qmatrix8x8[i]),
                                         (i == 0) ? g_h264_quant8_intra_default :
                                         g_h264_quant8_inter_default, Q8X8_SIZE));
        }
    }
    return;
}

#define Q4X4_SIZE 16
STATIC td_void h264_process_sps_matrix_4x4(h264_ctx *ctx, h264_sps *sps)
{
    td_u32 i;
    td_u32 q4x4defaultflag[6]; /* 6 is array size */

    for (i = 0; i < 6; i++) { /* 6 is array size */
        q4x4defaultflag[i] = u_1(ctx, "SPS: seq_scaling_list_present_flag");
        sps->seq_scaling_list_present_flag[i] = (td_s8)q4x4defaultflag[i];

        if (q4x4defaultflag[i] == 1) {
            scaling_list(ctx, sps->scaling_list4x4[i], sps->qmatrix4x4[i],
                         Q4X4_SIZE, &q4x4defaultflag[i]);

            if (q4x4defaultflag[i] == 1) {
                vfmw_check_sec_func(memcpy_s(sps->qmatrix4x4[i], sizeof(sps->qmatrix4x4[i]),
                                             (i < 3) ? g_h264_quant_intra_default : /* 3 idx */
                                             g_h264_quant_inter_default, Q4X4_SIZE));
            }
        } else {
            if (i == 0 || i == 3) { /* 3 idx */
                vfmw_check_sec_func(memcpy_s(sps->qmatrix4x4[i], sizeof(sps->qmatrix4x4[i]),
                                             (i == 0) ? g_h264_quant_intra_default :
                                             g_h264_quant_inter_default, Q4X4_SIZE));
            } else {
                vfmw_check_sec_func(memcpy_s(sps->qmatrix4x4[i], sizeof(sps->qmatrix4x4[i]),
                                             sps->qmatrix4x4[i - 1], Q4X4_SIZE));
            }
        }
    }
    return;
}

STATIC td_void h264_process_sps_by_profile_bit_depth(h264_ctx *ctx, h264_sps *sps)
{
    sps->bit_depth_luma = (td_u16)ue_v(ctx, "SPS: bit_depth_luma_minus8") + 8; /* 8 bit */

    if (sps->bit_depth_luma != 8 || ctx->se_err_flg != 0) { /* 8 is bit depth */
        dprint(PRN_ERROR, "bit_depth_luma(%d) not equal 8.\n", sps->bit_depth_luma);
        if (sps->bit_depth_luma == 0xa || sps->bit_depth_luma == 0xc) {
            ctx->spec_invalid = SPEC_BIT_INVALID;
            ctx->spec_value = sps->bit_depth_luma;
            report_unsupport(ctx->chan_id);
        }
        sps->bit_depth_luma = 8; /* 8 is bit depth */
    }

    sps->bit_depth_chroma = (td_u16)ue_v(ctx, "SPS: bit_depth_chroma_minus8") + 8; /* 8 bit */

    if (sps->bit_depth_chroma != 8 || ctx->se_err_flg != 0) { /* 8 is bit depth */
        dprint(PRN_ERROR, "bit_depth_chroma(%d) not equal 8.\n", sps->bit_depth_chroma);
        if (sps->bit_depth_chroma == 0xa || sps->bit_depth_chroma == 0xc) {
            ctx->spec_invalid = SPEC_BIT_INVALID;
            ctx->spec_value = sps->bit_depth_chroma;
            report_unsupport(ctx->chan_id);
        }
        sps->bit_depth_chroma = 8; /* 8 is bit depth */
    }

    return;
}

STATIC td_s32 h264_process_sps_high_profile(h264_ctx *ctx, h264_sps *sps)
{
    td_u8 flag;
    td_u32 qpprime_y_zero_trans_bypass_flag;

    /* high profile dec chroma_format_idc */
    sps->chroma_format_idc = ue_v(ctx, "SPS: chroma_format_idc");

    flag = (((sps->chroma_format_idc != 0) && (sps->chroma_format_idc != 1)) || (ctx->se_err_flg != 0));
    if (flag != 0) {
        dprint(PRN_ERROR, "sps->chroma_format_idc = %d out of range.\n", sps->chroma_format_idc);
        if (sps->chroma_format_idc == 0x2 || sps->chroma_format_idc == 0x3) {
            report_unsupport(ctx->chan_id);
            ctx->spec_invalid = SPEC_PIX_FMT_INVALID;
        }

        sps->bit_depth_luma = (td_u16)ue_v(ctx, "SPS: bit_depth_luma_minus8") + 8; /* 8 bit */

        if ((sps->bit_depth_luma != 8) || (ctx->se_err_flg != 0)) { /* 8 is bit depth */
            dprint(PRN_ERROR, "bit_depth_luma(%d) not equal 8.\n", sps->bit_depth_luma);
            if (sps->bit_depth_luma == 0xa || sps->bit_depth_luma == 0xc) {
                ctx->spec_invalid = SPEC_BIT_INVALID;
                ctx->spec_value = sps->bit_depth_luma;
                report_unsupport(ctx->chan_id);
            }
        }

        return H264_ERR;
    }

    h264_process_sps_by_profile_bit_depth(ctx, sps);

    qpprime_y_zero_trans_bypass_flag = u_1(ctx, "SPS: qpprime_y_zero_trans_bypass_flag");
    if ((qpprime_y_zero_trans_bypass_flag != 0) || (ctx->se_err_flg != 0)) {
        dprint(PRN_ERROR, "qpprime_y_zero_trans_bypass_flag not equal 0.\n");
        return H264_ERR;
    }

    sps->seq_scaling_matrix_present_flag = (td_u8)u_1(ctx, "SPS: seq_scaling_matrix_present_flag");

    if (sps->seq_scaling_matrix_present_flag == 1) {
        h264_process_sps_matrix_4x4(ctx, sps);
        h264_process_sps_matrix_8x8(ctx, sps);
    }

    return H264_OK;
}

STATIC td_s32 h264_process_sps_by_profile(h264_ctx *ctx, h264_sps *sps)
{
    td_s32 ret;
    td_u8 flag;

    sps->bit_depth_luma = 8; /* 8 is bit depth */
    sps->bit_depth_chroma = 8; /* 8 is bit depth */

    flag = (sps->profile_idc == HIGHPROFILE || sps->profile_idc == HIGH10P || sps->profile_idc == HIGH422P ||
            sps->profile_idc == HIGH444P || sps->profile_idc == SCALABLE_BASELINE_PROFILE ||
            sps->profile_idc == SCALABLE_HIGH_PROFILE);

    if (flag != 0) {
        ret = h264_process_sps_high_profile(ctx, sps);
        vfmw_check_ret(ret == H264_OK, H264_ERR);
    } else {
        sps->chroma_format_idc = 1;
        sps->seq_scaling_matrix_present_flag = 0;
    }

    return H264_OK;
}

STATIC td_s32 h264_process_sps_by_pic_order_cnt_type(h264_ctx *ctx, h264_sps *sps)
{
    td_u8 flag;
    td_u32 i;

    sps->log2_max_frame_num_minus4 = ue_v(ctx, "SPS: log2_max_frame_num_minus4");

    flag = ((sps->log2_max_frame_num_minus4 > 12) || (ctx->se_err_flg != 0)); /* 12 vfmw_min val */
    vfmw_check_ret_print(flag == 0, H264_ERR, "sps->log2_max_frame_num_minus4 out of range.\n");

    sps->pic_order_cnt_type = ue_v(ctx, "SPS: pic_order_cnt_type");

    flag = ((sps->pic_order_cnt_type > 2) || (ctx->se_err_flg != 0)); /* 2 vfmw_min val */
    vfmw_check_ret_print(flag == 0, H264_ERR, "sps->pic_order_cnt_type out of range.\n");

    if (sps->pic_order_cnt_type == 0) {
        sps->log2_max_pic_order_cnt_lsb_minus4 = ue_v(ctx, "SPS: log2_max_pic_order_cnt_lsb_minus4");

        flag = ((sps->log2_max_pic_order_cnt_lsb_minus4 > 12) || (ctx->se_err_flg != 0)); /* 12 vfmw_min val */
        vfmw_check_ret_print(flag == 0, H264_ERR, "sps->log2_max_pic_order_cnt_lsb_minus4 out of range.\n");
    } else if (sps->pic_order_cnt_type == 1) {
        sps->delta_pic_order_always_zero_flag = (td_u8)u_1(ctx, "SPS: delta_pic_order_always_zero_flag");
        vfmw_check_ret(se_err_check(ctx->se_err_flg) == H264_OK, H264_ERR);

        sps->offset_for_non_ref_pic = se_v(ctx, "SPS: offset_for_non_ref_pic");
        vfmw_check_ret(se_err_check(ctx->se_err_flg) == H264_OK, H264_ERR);

        sps->offset_for_top_to_bottom_field = se_v(ctx, "SPS: offset_for_top_to_bottom_field");
        vfmw_check_ret(se_err_check(ctx->se_err_flg) == H264_OK, H264_ERR);

        sps->num_ref_frames_in_pic_order_cnt_cycle = ue_v(ctx, "SPS: num_ref_frames_in_pic_order_cnt_cycle");

        flag = ((sps->num_ref_frames_in_pic_order_cnt_cycle > 255) || (ctx->se_err_flg != 0)); /* 255 vfmw_max cycle */
        vfmw_check_ret_print(flag == 0, H264_ERR, "sps->num_ref_frames_in_pic_order_cnt_cycle out of range.\n");

        for (i = 0; i < sps->num_ref_frames_in_pic_order_cnt_cycle; i++) {
            sps->offset_for_ref_frame[i] = se_v(ctx, "SPS: offset_for_ref_frame[i]");
            vfmw_check_ret(se_err_check(ctx->se_err_flg) == H264_OK, H264_ERR);
        }

        vfmw_check_ret(se_err_check(ctx->se_err_flg) == H264_OK, H264_ERR);
    }
    return H264_OK;
}

STATIC td_s32 h264_process_sps_get_other_info(h264_ctx *ctx, h264_sps *sps)
{
    td_u8 flag;

    sps->num_ref_frames = ue_v(ctx, "SPS: num_ref_frames");
    vfmw_check_ret(se_err_check(ctx->se_err_flg) == H264_OK, H264_ERR);
    if (sps->num_ref_frames > H264_MAX_REF_NUM) {
        report_unsupport_spec(ctx->chan_id, VFMW_SPEC_RESOLUTION, sps->num_ref_frames);
        dprint(PRN_ERROR, "sps->num_ref_frames out of range(%d > %d).\n", sps->num_ref_frames, H264_MAX_REF_NUM);
        return H264_ERR;
    }

    sps->gaps_in_frame_num_value_allowed_flag = (td_u8)u_1(ctx, "SPS: gaps_in_frame_num_value_allowed_flag");
    vfmw_check_ret(se_err_check(ctx->se_err_flg) == H264_OK, H264_ERR);

    sps->pic_width_in_mbs_minus1 = ue_v(ctx, "SPS: pic_width_in_mbs_minus1");

    flag = (((sps->pic_width_in_mbs_minus1 + 1) < H264_MIN_WIDTH_IN_MB) ||
            ((sps->pic_width_in_mbs_minus1 + 1) > H264_MAX_WIDTH_IN_MB) ||
            (ctx->se_err_flg != 0));

    if (flag != 0) {
        report_unsupport_spec(ctx->chan_id, VFMW_SPEC_RESOLUTION, (sps->pic_width_in_mbs_minus1 + 1) * 16); /* 16 mb */

        dprint(PRN_ERROR, "sps->pic_width_in_mbs_minus1 out of range(=%d).\n", sps->pic_width_in_mbs_minus1);
        return H264_ERR;
    }

    sps->pic_height_in_map_units_minus1 = ue_v(ctx, "SPS: pic_height_in_map_units_minus1");
    vfmw_check_ret(se_err_check(ctx->se_err_flg) == H264_OK, H264_ERR);

    return H264_OK;
}

STATIC td_s32 h264_process_sps_by_frame_mbs_only_flag(h264_ctx *ctx, h264_sps *sps, td_u32 *temp)
{
    td_u8 flag;

    sps->frame_mbs_only_flag = (td_u8)u_1(ctx, "SPS: frame_mbs_only_flag");
    vfmw_check_ret(se_err_check(ctx->se_err_flg) == H264_OK, H264_ERR);
    *temp = (1 + (sps->frame_mbs_only_flag == 0));

    vfmw_check_ret_print(!(*temp != 1 && *temp != 2), H264_ERR, "frame_mbs_only_flag err\n"); /* 2 mbaff */

    if (sps->frame_mbs_only_flag == 0) {
        sps->mb_adaptive_frame_field_flag = (td_u8)u_1(ctx, "SPS: mb_adaptive_frame_field_flag");

        flag = (((sps->pic_height_in_map_units_minus1 + 1) > H264_MAX_HEIGHT_IN_MB / 2) || /* 2 half */
                ((sps->pic_width_in_mbs_minus1 + 1) < H264_MIN_WIDTH_IN_MB) ||
                ((sps->pic_height_in_map_units_minus1 + 1) < H264_MIN_HEIGHT_IN_MB / 2)); /* 2 half */
        vfmw_check_ret_print(flag == 0, H264_ERR, "sps->pic_height_in_map_units_minus1 out of range.\n");
    } else {
        sps->mb_adaptive_frame_field_flag = 0; /* default */

        flag = (((sps->pic_height_in_map_units_minus1 + 1) > H264_MAX_HEIGHT_IN_MB) ||
                ((sps->pic_height_in_map_units_minus1 + 1) < H264_MIN_HEIGHT_IN_MB) ||
                ((sps->pic_width_in_mbs_minus1 + 1) > H264_MAX_WIDTH_IN_MB) ||
                ((sps->pic_width_in_mbs_minus1 + 1) < H264_MIN_WIDTH_IN_MB));
        vfmw_check_ret_print(flag == 0, H264_ERR, "sps->pic_height_in_map_units_minus1 out of range.\n");
    }
    return H264_OK;
}

STATIC td_s32 h264_process_sps_cacul_pic_size(h264_ctx *ctx, const h264_sps *sps,
    td_u32 *pic_width_in_smp, td_u32 *pic_height_in_smp, td_u32 *pic_size, td_u32 temp)
{
    /* 计算一副图像的大小 */
    vfmw_check_ret(sps->pic_width_in_mbs_minus1 + 1 < VFMW_INT16_MAX, H264_ERR);
    vfmw_check_ret(sps->pic_height_in_map_units_minus1 + 1 < VFMW_INT16_MAX, H264_ERR);
    vfmw_check_ret(temp < VFMW_INT16_MAX, H264_ERR);
    *pic_width_in_smp = (sps->pic_width_in_mbs_minus1 + 1) * 16; /* 16 is mb size */
    *pic_height_in_smp = (sps->pic_height_in_map_units_minus1 + 1) * temp * 16; /* 16 is mb size */

    vfmw_check_ret(*pic_width_in_smp < VFMW_INT16_MAX, H264_ERR);
    vfmw_check_ret(*pic_height_in_smp < VFMW_INT16_MAX, H264_ERR);
    *pic_size = ((*pic_width_in_smp) * (*pic_height_in_smp) / 2) * 3; /* 2 3 is num */

    if ((*pic_width_in_smp) > H264_MAX_PIX_WIDTH || (*pic_width_in_smp) < H264_MIN_PIX_WIDTH) {
        ctx->spec_invalid = SPEC_WIDTH_INVALID;
        ctx->spec_value = *pic_width_in_smp;
        report_unsupport_spec(ctx->chan_id, VFMW_SPEC_RESOLUTION, *pic_width_in_smp);

        dprint(PRN_ERROR, "PicWidth out of range.\n");
        return H264_ERR;
    }

    if ((*pic_height_in_smp) > H264_MAX_PIX_HEIGHT || (*pic_height_in_smp) < H264_MIN_PIX_HEIGHT) {
        ctx->spec_invalid = SPEC_HEIGHT_INVALID;
        ctx->spec_value = *pic_height_in_smp;
        report_unsupport_spec(ctx->chan_id, VFMW_SPEC_RESOLUTION, *pic_height_in_smp);

        dprint(PRN_ERROR, "PicHeight out of range.\n");
        return H264_ERR;
    }

    if ((*pic_size) > H264_MAX_PIX_WIDTH / 16 * H264_MAX_PIX_HEIGHT / 16 * 384) { /* 16 mb size 384 is num */
        dprint(PRN_ERROR, "pic size too large.\n");
        return H264_ERR;
    }
    return H264_OK;
}

td_u32 g_sps_level_idc_size[][2] = { /* 2, a number */
    {10, 152064},       /* 10 is idc val, 152064 is size */
    {11, 345600},       /* 11 is idc val, 345600 is size */
    {12, 912384},       /* 12 is idc val, 912384 is size */
    {13, 912384},       /* 13 is idc val, 912384 is size */
    {20, 912384},       /* 20 is idc val, 912384 is size */
    {21, 1824768},      /* 21 is idc val, 1824768 is size */
    {22, 3110400},      /* 22 is idc val, 3110400 is size */
    {30, 3110400},      /* 30 is idc val, 3110400 is size */
    {31, 6912000},      /* 31 is idc val, 6912000 is size */
    {32, 7864320},      /* 32 is idc val, 7864320 is size */
    {40, 12582912},     /* 40 is idc val, 12582912 is size */
    {41, 12582912},     /* 41 is idc val, 12582912 is size */
    {42, 13369344},     /* 42 is idc val, 13369344 is size */
    {50, 42393600},     /* 50 is idc val, 42393600 is size */
    {51, 70778880},     /* 51 is idc val, 70778880 is size */
    {52, 70778880},     /* 52 is idc val, 70778880 is size */
#if H264_MAX_PIX_WIDTH > 4096 /* 4096 is 4k */
    {60, 267386880},    /* 60 is idc val, 267386880 is size */
    {61, 267386880},    /* 61 is idc val, 267386880 is size */
    {62, 267386880}     /* 62 is idc val, 267386880 is size */
#endif
};

STATIC td_s32 h264_process_sps_get_num_ref_frames(const h264_ctx *ctx, h264_sps *sps,
    td_u32 pic_size, td_u32 *cacu_frame_size)
{
    td_u32 i;
    td_u32 row_num;
    td_u32 size = 0;

    /* calc row num. */
    row_num = ((td_u32)sizeof(g_sps_level_idc_size) / (td_u32)sizeof(td_u32)) /
        ((td_u32)sizeof(g_sps_level_idc_size[0]) / (td_u32)sizeof(td_u32));

    /* 根据Level分配图像空间大小 */
    for (i = 0; i < row_num; i++) {
        if (sps->level_idc == g_sps_level_idc_size[i][0]) {
            size = g_sps_level_idc_size[i][1];
            break;
        }
    }

    if (size == 0) {
        dprint(PRN_ERROR, "level_idc %d not support.\n", sps->level_idc);
        return H264_ERR; /* LEVEL_ERR */
    }

    if (pic_size != 0) {
        size /= pic_size; /* 计算可放下的图像个数 */
    } else {
        dprint(PRN_ERROR, "error value, pic_size is 0.\n");
    }
    size = (size > 16) ? 16 : size; /* 16 :a number 16 :a number */

    if ((sps->num_ref_frames > size) || (ctx->se_err_flg != 0)) {
        dprint(PRN_WARN, "sps->num_ref_frames(%d) > dpb size(%d), try to select the reasonable one.\n",
               sps->num_ref_frames, size);

        if (sps->num_ref_frames <= 16) { /* 16 is ref num */
            size = vfmw_max(sps->num_ref_frames, size);
        }
        sps->num_ref_frames = size;
    }

    *cacu_frame_size = size;

    return H264_OK;
}

STATIC td_s32 h264_process_sps_by_cropping_flag(h264_ctx *ctx, h264_sps *sps,
    td_u32 temp, td_u32 pic_width_in_smp, td_u32 pic_height_in_smp)
{
    td_u32 frame_cropping_flag;
    td_u32 frame_cropping_rect_left_offset, frame_cropping_rect_right_offset;
    td_u32 frame_cropping_rect_top_offset, frame_cropping_rect_bottom_offset;
    td_s32 crop_w, crop_h;

    sps->direct_8x8_inference_flag = (td_u8)u_1(ctx, "SPS: direct_8x8_inference_flag");
    vfmw_check_ret(se_err_check(ctx->se_err_flg) == H264_OK, H264_ERR);

    sps->frame_cropping_flag = (td_u8)u_1(ctx, "SPS: frame_cropping_flag");
    vfmw_check_ret(se_err_check(ctx->se_err_flg) == H264_OK, H264_ERR);

    frame_cropping_flag = sps->frame_cropping_flag;

    if (frame_cropping_flag != 0) {
        sps->frame_crop_left_offset = ue_v(ctx, "SPS: frame_cropping_rect_left_offset");
        sps->frame_crop_right_offset = ue_v(ctx, "SPS: frame_cropping_rect_right_offset");
        sps->frame_crop_top_offset = ue_v(ctx, "SPS: frame_cropping_rect_top_offset");
        sps->frame_crop_bottom_offset = ue_v(ctx, "SPS: frame_cropping_rect_bottom_offset");

        sps->frame_crop_left_offset = sps->frame_crop_left_offset << 1; /* 1 bit num */
        sps->frame_crop_right_offset = sps->frame_crop_right_offset << 1; /* 1 bit num */
        vfmw_check_ret(temp < VFMW_INT16_MAX, H264_ERR);
        vfmw_check_ret(sps->frame_crop_top_offset < VFMW_INT16_MAX, H264_ERR);
        vfmw_check_ret(sps->frame_crop_bottom_offset < VFMW_INT16_MAX, H264_ERR);
        sps->frame_crop_top_offset = temp * sps->frame_crop_top_offset << 1; /* 1 bit num */
        sps->frame_crop_bottom_offset = temp * sps->frame_crop_bottom_offset << 1; /* 1 bit num */

        frame_cropping_rect_left_offset = sps->frame_crop_left_offset;
        frame_cropping_rect_right_offset = sps->frame_crop_right_offset;
        frame_cropping_rect_top_offset = sps->frame_crop_top_offset;
        frame_cropping_rect_bottom_offset = sps->frame_crop_bottom_offset;

        crop_w = (td_s32)(pic_width_in_smp - frame_cropping_rect_left_offset - frame_cropping_rect_right_offset);
        crop_h = (td_s32)(pic_height_in_smp - frame_cropping_rect_top_offset - frame_cropping_rect_bottom_offset);

        if (crop_w <= 0 || crop_h <= 0) {
            /* dec err cropping para, not cropping */
            sps->disp_width = pic_width_in_smp;
            sps->disp_height = pic_height_in_smp;
            sps->disp_center_x = pic_width_in_smp / 2; /* 2 is half */
            sps->disp_center_y = pic_height_in_smp / 2; /* 2 is half */
        } else {
            sps->disp_width = (td_u32)crop_w;
            sps->disp_height = (td_u32)crop_h;
            sps->disp_center_x = frame_cropping_rect_left_offset + sps->disp_width / 2; /* 2 is half */
            sps->disp_center_y = frame_cropping_rect_top_offset + sps->disp_height / 2; /* 2 is half */
        }
    } else {
        sps->disp_width = pic_width_in_smp;
        sps->disp_height = pic_height_in_smp;
        sps->disp_center_x = pic_width_in_smp / 2; /* 2 is half */
        sps->disp_center_y = pic_height_in_smp / 2; /* 2 is half */
    }
    return H264_OK;
}

STATIC td_void h264_process_sps_get_max_refnum_width_height(const h264_ctx *ctx, td_s32 *max_ref_num,
    td_s32 *max_width, td_s32 *max_height)
{
    *max_ref_num = ctx->extra_data->max_ref_num;
    *max_width = ctx->extra_data->max_width;
    *max_height = ctx->extra_data->max_height;
}

STATIC td_s32 h264_process_sps_get_vui_info(h264_ctx *ctx, h264_sps *sps,
    td_u32 pic_width_in_smp, td_u32 pic_height_in_smp, td_u32 size)
{
    td_u8 flag;
    td_u32 size_tmp = size;

    sps->vui_parameters_present_flag = (td_u8)u_1(ctx, "SPS: vui_parameters_present_flag");
    vfmw_check_ret(se_err_check(ctx->se_err_flg) == H264_OK, H264_ERR);

    sps->vui_seq_parameters.matrix_coefficients = 2; /* 2 is def coeff */

    if (sps->vui_parameters_present_flag != 0) {
        if (vui_parameters(ctx, &(sps->vui_seq_parameters)) == H264_OK && /* 解析VUI信息 */
            (sps->vui_seq_parameters.bitstream_restriction_flag != 0)) {
            if (sps->vui_seq_parameters.max_dec_frame_buffering > size_tmp ||
                sps->vui_seq_parameters.max_dec_frame_buffering < 1) {
                dprint(PRN_ERROR, "max_dec_frame_buffering(%d) > MaxDpbSize(%d)\n",
                        sps->vui_seq_parameters.max_dec_frame_buffering, size_tmp);
            } else {
                size_tmp = vfmw_max(sps->vui_seq_parameters.max_dec_frame_buffering, sps->num_ref_frames);
            }
        }
    } else {
        /* add some default value */
        sps->vui_seq_parameters.aspect_ratio_idc = 0;
        sps->vui_seq_parameters.video_format = 5; /* 5 is def val */
    }

    sps->vui_seq_parameters.aspect_ratio = (td_u32)h264_get_dar((td_s32)sps->vui_seq_parameters.aspect_ratio_idc,
        (td_s32)sps->vui_seq_parameters.sar_width, (td_s32)sps->vui_seq_parameters.sar_height,
        (td_s32)pic_width_in_smp, (td_s32)pic_height_in_smp);

    flag = (((sps->num_ref_frames > 0) && (sps->vui_seq_parameters.max_dec_frame_buffering < sps->num_ref_frames)) ||
             (ctx->extra_data->output_order == VFMW_DEC_ORDER_SIMPLE_DPB) ||
             (ctx->extra_data->output_order == VFMW_DEC_ORDER_SKYPLAY));

    if (flag != 0) {
        size_tmp = vfmw_min(size_tmp, sps->num_ref_frames);
    }

    size_tmp = vfmw_min(size_tmp, 16); /* 16 is ref num */
    ctx->true_dpb_size = (td_s32)size_tmp;

    {
        td_s32 max_ref_num, max_width, max_height;

        h264_process_sps_get_max_refnum_width_height(ctx, &max_ref_num, &max_width, &max_height);

        if (size_tmp > (td_u32)max_ref_num) {
            return H264_ERR;
        }

        if (pic_width_in_smp > (td_u32)max_width || pic_height_in_smp > (td_u32)max_height) {
            /* 此处直接返回error可以避免后续码流如果分辨率超大或者参考帧个数过多时重复初始化DPB导致不必要的麻烦 */
            return H264_ERR;
        }
    }

    sps->dpb_size_plus1 = size_tmp + 1;

    return H264_OK;
}

/***********************************************************************************
* Function:    process_sps(h264_ctx *ctx, H264_SPS_S *sps)
* Description: 解析SPS及VUI信息, 并完成容错处理
* Input:       在码流解析时会消耗码流
* Output:      sps         暂存解析的SPS及VUI参数到一个结构体
* Return:      DEC_NORMAL   可以解码此Nalu数据
*              DEC_ERR      不可以解码
* Others:      无
***********************************************************************************/
STATIC td_s32 process_sps(h264_ctx *ctx, h264_sps *sps)
{
    td_u32 size = 0;
    td_u32 pic_size;
    td_u32 pic_width_in_smp, pic_height_in_smp;
    td_u32 temp;

    vfmw_check_ret(sps != TD_NULL, H264_ERR);

    if (h264_process_sps_by_profile(ctx, sps) != H264_OK) {
        return H264_ERR;
    }

    if (h264_process_sps_by_pic_order_cnt_type(ctx, sps) != H264_OK) {
        return H264_ERR;
    }

    if (h264_process_sps_get_other_info(ctx, sps) != H264_OK) {
        return H264_ERR;
    }

    if (h264_process_sps_by_frame_mbs_only_flag(ctx, sps, &temp) != H264_OK) {
        return H264_ERR;
    }

    if (h264_process_sps_cacul_pic_size(ctx, sps, &pic_width_in_smp, &pic_height_in_smp, &pic_size, temp) != H264_OK) {
        return H264_ERR;
    }

    if (h264_process_sps_get_num_ref_frames(ctx, sps, pic_size, &size) != H264_OK) {
        return H264_ERR;
    }

    if (h264_process_sps_by_cropping_flag(ctx, sps, temp, pic_width_in_smp, pic_height_in_smp) != H264_OK) {
        return H264_ERR;
    }

    if (h264_process_sps_get_vui_info(ctx, sps,  pic_width_in_smp, pic_height_in_smp, size) != H264_OK) {
        return H264_ERR;
    }

    return H264_OK;
}

STATIC td_void sps_svc_ext_print(h264_ctx *ctx, h264_sps *sps, td_u32 test_code)
{
    td_u32 chroma_phase_x_plus1_flag;
    td_u32 chroma_phase_y_plus1;
    td_u32 seq_tcoeff_level_prediction_flag;
    td_u32 adaptive_tcoeff_level_prediction_flag;
    td_u32 slice_header_restriction_flag;

    (td_void)(test_code); /* avoid to build err when printing is not supported. */
    (td_void)(chroma_phase_x_plus1_flag);
    (td_void)(chroma_phase_y_plus1);
    (td_void)(adaptive_tcoeff_level_prediction_flag);

    if (sps->chroma_format_idc == 1 || sps->chroma_format_idc == 2) { /* 2 is idc val */
        chroma_phase_x_plus1_flag = u_1(ctx, "SPS_SVC_EXT: chroma_phase_x_plus1_flag");
    }

    if (sps->chroma_format_idc == 1) {
        chroma_phase_y_plus1 = (td_u32)u_v(ctx, 2, "SPS_SVC_EXT: chroma_phase_y_plus1"); /* 2 is bit num */
    }

    seq_tcoeff_level_prediction_flag = u_1(ctx, "SPS_SVC_EXT: seq_tcoeff_level_prediction_flag");
    if (seq_tcoeff_level_prediction_flag != 0) {
        adaptive_tcoeff_level_prediction_flag = u_1(ctx, "SPS_SVC_EXT: adaptive_tcoeff_level_prediction_flag");
    }

    slice_header_restriction_flag = u_1(ctx, "SPS_SVC_EXT: slice_header_restriction_flag");
    /* fix me here */
    if (slice_header_restriction_flag != 1) {
        dprint(PRN_ERROR, "%s error:%d,%x,%d\n", __func__,
               slice_header_restriction_flag, test_code, sps->chroma_format_idc);
    }
}

/* add for subsps in svc, 解析这些码字用来判断该码流是否可以正常解码 */
STATIC td_s32 sps_svc_ext(h264_ctx *ctx)
{
    h264_pps *pps = TD_NULL;
    h264_sps *sps = TD_NULL;
    td_u32 inter_layer_present_flag;
    td_u32 extended_spatial_scal_idc;
    td_u32 test_code;

    (td_void)(inter_layer_present_flag);

    if (ctx->pps[ctx->cur_slice.pic_parameter_set_id] == TD_NULL) {
        ctx->pps[ctx->cur_slice.pic_parameter_set_id] =
            (h264_pps *)h264_get_dyn_mem(ctx, sizeof(h264_pps), "pps", ctx->cur_slice.pic_parameter_set_id);
        if (ctx->pps[ctx->cur_slice.pic_parameter_set_id] == TD_NULL) {
            dprint(PRN_ERROR, "%s, %d, alloc dyn mem error\n", __func__, __LINE__);
            return H264_ERR;
        }
        ctx->pps[ctx->cur_slice.pic_parameter_set_id]->seq_parameter_set_id = 0;
    }
    pps = (ctx->pps[ctx->cur_slice.pic_parameter_set_id]);
    if (ctx->sps[pps->seq_parameter_set_id] == TD_NULL) {
        ctx->sps[pps->seq_parameter_set_id] =
            (h264_sps *)h264_get_dyn_mem(ctx, sizeof(h264_sps), "sps", pps->seq_parameter_set_id);
        if (ctx->sps[pps->seq_parameter_set_id] == TD_NULL) {
            dprint(PRN_ERROR, "%s, %d, alloc dyn mem error\n", __func__, __LINE__);
            return H264_ERR;
        }
    }
    sps = (ctx->sps[pps->seq_parameter_set_id]);
    test_code = (td_u32)bs_get(&(ctx->bs), 32); /* 32 is bit num */
    inter_layer_present_flag = u_1(ctx, "SPS_SVC_EXT: inter_layer_deblocking_filter_control_present_flag");
    extended_spatial_scal_idc = (td_u32)u_v(ctx, 2, "extended_spatial_scalability_idc"); /* 2 is bit num */
    if (extended_spatial_scal_idc == 1) {
        dprint(PRN_ERROR, "spatial scalability not support!!\n");
        return H264_ERR;
    }

    sps_svc_ext_print(ctx, sps, test_code);

    return H264_OK;
}

STATIC td_void h264_dec_sps_profile_idc(const h264_ctx *ctx, h264_dec_sps_info *sps_parse)
{
    switch (sps_parse->profile_idc) {
        case BASELINE:
            dprint(PRN_ERROR, "Baseline stream, try to decode, exit when FMO occurred.\n");
            break;

        case EXTENDED:
            dprint(PRN_ERROR, "EXTENDED stream, try to decode, exit when datapartition occurred.\n");
            break;

        case MAINPROFILE:
            break;

        case HIGHPROFILE:
            break;

        /* add svc base profile and high profile */
        case SCALABLE_BASELINE_PROFILE:
        case SCALABLE_HIGH_PROFILE:
            break;

        case HIGH10P:
        case HIGH422P:
        case HIGH444P:
        case HIGH444PP:
            report_unsupport(ctx->chan_id);
            dprint(PRN_ERROR, "others High profile stream, try to decode. \n");
            break;

        default:
            dprint(PRN_ERROR, "profile_idc = %5d error, try to decode as main profile.\n", sps_parse->profile_idc);
            sps_parse->profile_idc = HIGHPROFILE;
            break;
    }
    return;
}

STATIC td_s32 h264_dec_sps_set_id(h264_ctx *ctx, td_u32 seq_parameter_set_id, const h264_dec_sps_info *sps_parse)
{
    h264_sps *sps_tmp = &ctx->sps_tmp; /* 定义成静态，防止占用堆栈太多 */

    if (ctx->sps[seq_parameter_set_id]->is_valid != 0) { /* 曾经解对过该id对应的SPS */
        vfmw_check_sec_func(memset_s(sps_tmp, sizeof(h264_sps), 0, sizeof(h264_sps)));
        sps_tmp->profile_idc = sps_parse->profile_idc;
        sps_tmp->constrained_set0_flag = (td_u8)sps_parse->constrained_set0_flag;
        sps_tmp->constrained_set1_flag = (td_u8)sps_parse->constrained_set1_flag;
        sps_tmp->constrained_set2_flag = (td_u8)sps_parse->constrained_set2_flag;
        sps_tmp->constrained_set3_flag = (td_u8)sps_parse->constrained_set3_flag;
        sps_tmp->level_idc = sps_parse->level_idc;
        sps_tmp->seq_parameter_set_id = seq_parameter_set_id;

        if (process_sps(ctx, sps_tmp) != H264_OK) {
            dprint(PRN_ERROR, "Line %d: SPS[%d] decode error.\n", __LINE__, seq_parameter_set_id);
            return H264_ERR;
        }

        if (sps_equal(sps_tmp, (ctx->sps[seq_parameter_set_id])) != H264_OK) {
            sps_tmp->is_refreshed = 1;
            sps_tmp->is_valid = 1;

            vfmw_check_sec_func(memmove_s((ctx->sps[seq_parameter_set_id]),
                                          sizeof(h264_sps), sps_tmp, sizeof(h264_sps)));
        }
    } else {
        ctx->sps[seq_parameter_set_id]->profile_idc = sps_parse->profile_idc;
        ctx->sps[seq_parameter_set_id]->constrained_set0_flag = (td_u8)sps_parse->constrained_set0_flag;
        ctx->sps[seq_parameter_set_id]->constrained_set1_flag = (td_u8)sps_parse->constrained_set1_flag;
        ctx->sps[seq_parameter_set_id]->constrained_set2_flag = (td_u8)sps_parse->constrained_set2_flag;
        ctx->sps[seq_parameter_set_id]->constrained_set3_flag = (td_u8)sps_parse->constrained_set3_flag;
        ctx->sps[seq_parameter_set_id]->level_idc = sps_parse->level_idc;
        ctx->sps[seq_parameter_set_id]->seq_parameter_set_id = seq_parameter_set_id;

        if (process_sps(ctx, (ctx->sps[seq_parameter_set_id])) != H264_OK) {
            dprint(PRN_ERROR, "Line %d: SPS[%d] decode error.\n", __LINE__, seq_parameter_set_id);
            ctx->sps[seq_parameter_set_id]->is_refreshed = 1;
            ctx->sps[seq_parameter_set_id]->is_valid = 0;
            return H264_ERR;
        }

        ctx->sps[seq_parameter_set_id]->is_refreshed = 1;
        ctx->sps[seq_parameter_set_id]->is_valid = 1;
    }
    return H264_OK;
}

/***********************************************************************************
* Function:    h264_dec_sps(h264_ctx *ctx)
* Description: 解析SPS, 并完成容错处理
* Input:       在码流解析时会消耗码流
* Output:      g_SPS        只会影响数组中的一个元素结构体
* Return:      DEC_NORMAL   可以解码此Nalu数据
*              DEC_ERR      不可以解码
* Others:      无
***********************************************************************************/
STATIC td_s32 h264_dec_sps(h264_ctx *ctx)
{
    td_s32 ret;
    td_u32 seq_parameter_set_id;
    h264_dec_sps_info sps_parse = {0};

    ctx->sps_update_flag = 1;
    sps_parse.profile_idc = (td_u32)u_v(ctx, 8, "SPS: profile_idc"); /* 8 is bit num */
    sps_parse.constrained_set0_flag = u_1(ctx, "SPS: constrained_set0_flag");
    sps_parse.constrained_set1_flag = u_1(ctx, "SPS: constrained_set1_flag");
    sps_parse.constrained_set2_flag = u_1(ctx, "SPS: constrained_set2_flag");
    sps_parse.constrained_set3_flag = u_1(ctx, "SPS: constrained_set3_flag");

    u_v(ctx, 4, "SPS: reserved_zero_4bits"); /* 4 is bit num */
    sps_parse.level_idc = (td_u32)u_v(ctx, 8, "SPS: level_idc"); /* 8 is bit num */
    seq_parameter_set_id = ue_v(ctx, "SPS: seq_parameter_set_id");

    if (ctx->se_err_flg != 0) {
        return H264_ERR;
    }

    if (seq_parameter_set_id > (td_u32)(ctx->max_sps_num - 1) || ctx->max_sps_num <= 0) {
        dprint(PRN_ERROR, "seq_parameter_set_id(%u) out of range[0, %d).\n", seq_parameter_set_id, ctx->max_sps_num);

        if (seq_parameter_set_id > (td_u32)(ctx->max_sps_num - 1)) {
        }

        /* 可以考虑钳位强解 */
        return H264_ERR;
    }

    h264_dec_sps_profile_idc(ctx, &sps_parse);

    if (sps_parse.level_idc < H264_MIN_LEVEL) {
        dprint(PRN_WARN, "level_idc = %5d error, try to decode as level_idc %d.\n",
            sps_parse.level_idc, H264_MAX_LEVEL);
        sps_parse.level_idc = H264_MAX_LEVEL;
    }

    if (sps_parse.level_idc > H264_MAX_LEVEL) {
        dprint(PRN_WARN, "level_idc = %5d error, try to decode as level_idc %d.\n",
            sps_parse.level_idc, H264_MAX_LEVEL);
        sps_parse.level_idc = H264_MAX_LEVEL;
    }

    if ((seq_parameter_set_id < H264_MAX_SPS_NUM) &&
        (ctx->sps[seq_parameter_set_id] == TD_NULL)) {
        ctx->sps[seq_parameter_set_id] =
            (h264_sps *)h264_get_dyn_mem(ctx, sizeof(h264_sps), "sps", seq_parameter_set_id);
    }

    if (seq_parameter_set_id < H264_MAX_SPS_NUM) {
        ret = h264_dec_sps_set_id(ctx, seq_parameter_set_id, &sps_parse);
        vfmw_check_ret_no_print(ret == H264_OK, ret);
    }

    /* add for svc base profile and high profile */
    if (sps_parse.profile_idc == SCALABLE_BASELINE_PROFILE || sps_parse.profile_idc == SCALABLE_HIGH_PROFILE) {
        if (sps_svc_ext(ctx) != H264_OK) {
            dprint(PRN_ERROR, "svc ext error:\n");
            return H264_ERR;
        }
    }

    return H264_OK;
}

/***********************************************************************************
* td_s32    pass_bytes(h264_ctx *ctx, td_u32 len)
* Description: 解析SEI
* Input:       pStream             在码流解析时会消耗码流
*              size                SEI Nalu大小
* Output:      g_RecoverPointFlg   解码恢复点标志
*              pSEI                输出的SEI信息结构体
* Return:      DEC_NORMAL          可以解码此Nalu数据
*              DEC_ERR             不可以解码
* Others:      无
***********************************************************************************/
STATIC td_s32 pass_bytes(h264_ctx *ctx, td_u32 len)
{
    td_u32 len_tmp = len;
    td_u32 segment_idx;

    if (ctx->cur_nal == TD_NULL || len_tmp <= 0) {
        return -1;
    }

    if (ctx->cur_nal->stream[ctx->cur_nal->nal_used_segment].stream_buffer == TD_NULL) {
        return -1;
    }

    if (ctx->cur_nal->nal_segment > 2 || ctx->cur_nal->nal_used_segment > 1 ||
        (td_u32)ctx->cur_nal->nal_used_segment >= ctx->cur_nal->nal_segment) {
        return -1;
    }

    vfmw_check_ret(len_tmp < (td_u32)VFMW_INT_MAX, -1);
    len_tmp *= 8;

    for (segment_idx = ctx->cur_nal->nal_used_segment; segment_idx < ctx->cur_nal->nal_segment; segment_idx++) {
        if ((len_tmp + ctx->cur_nal->stream[segment_idx].bits_offset) <=
            (ctx->cur_nal->stream[segment_idx].bitstream_length * 8)) {
            ctx->cur_nal->stream[segment_idx].bits_offset += len_tmp;

            /* pass stream succeed */
            return 1;
        } else {
            len_tmp = len_tmp + ctx->cur_nal->stream[segment_idx].bits_offset -
                  ctx->cur_nal->stream[segment_idx].bitstream_length * 8;
            ctx->cur_nal->stream[segment_idx].bits_offset = ctx->cur_nal->stream[segment_idx].bitstream_length * 8;
            ctx->cur_nal->nal_used_segment++;
        }
    }

    /* no stream */
    return 0;
}

/***********************************************************************************
* Function:    td_s32    get_bytes(h264_ctx *ctx, td_u8 * des, td_s32 len)
* Description: 解析SEI
* Input:       pStream             在码流解析时会消耗码流
*              size                SEI Nalu大小
* Output:      g_RecoverPointFlg   解码恢复点标志
*              pSEI                输出的SEI信息结构体
* Return:      DEC_NORMAL          可以解码此Nalu数据
*              DEC_ERR             不可以解码
* Others:      无
***********************************************************************************/
STATIC td_s32 get_bytes(h264_ctx *ctx, td_u8 *des, td_s32 len)
{
    td_u32 segment_idx;
    td_u8 *byte_pos = TD_NULL;
    td_u8 *end_byte = TD_NULL;
    td_s32 cnt;

    if (des == TD_NULL || ctx->cur_nal == TD_NULL || len <= 0) {
        return -1;
    }

    if (ctx->cur_nal->nal_segment > 2 || ctx->cur_nal->nal_used_segment > 1 ||
        (td_u32)ctx->cur_nal->nal_used_segment >= ctx->cur_nal->nal_segment) {
        return -1;
    }

    if (ctx->cur_nal->stream[ctx->cur_nal->nal_used_segment].stream_buffer == TD_NULL) {
        return -1;
    }

    cnt = 0;

    for (segment_idx = ctx->cur_nal->nal_used_segment; segment_idx < ctx->cur_nal->nal_segment; segment_idx++) {
        byte_pos = ctx->cur_nal->stream[segment_idx].stream_buffer +
                   ((ctx->cur_nal->stream[segment_idx].bits_offset + 7) / 8);
        end_byte = ctx->cur_nal->stream[segment_idx].stream_buffer + ctx->cur_nal->stream[segment_idx].bitstream_length;

        while (byte_pos < end_byte) {
            *(des++) = *(byte_pos);
            cnt++;
            byte_pos++;
            ctx->cur_nal->stream[segment_idx].bits_offset += 8;

            if (cnt >= len) {
                return len;
            }
        }

        ctx->cur_nal->nal_used_segment++;
    }

    return cnt;
}

STATIC td_s32 skip_bytes(h264_ctx *ctx, td_s32 len)
{
    td_s32 i = 0;
    td_s32 count = 0;

    for (i = 0; i < len; i++) {
        if ((bs_pos(&ctx->bs) + 8) > (ctx->bs.bs_len * 8)) {
            break;
        }

        bs_skip(&(ctx->bs), 8);
        count++;
    }

    return count;
}

STATIC td_s32 h264_dec_frame_packing_sei(h264_ctx *ctx, td_s32 pay_load_size)
{
    h264_frame_packing_arrangement *frame_packing = &ctx->frame_pack_sei;

    vfmw_unused(pay_load_size);
    frame_packing->frame_packing_arrangement_id = (td_s32)ue_v(ctx, "SEI: frame_packing_arrangement_id");
    frame_packing->frame_packing_arrangement_cancel_flag = (td_s8)u_v(ctx, 1, "SEI: frame_packing_cancel_flag");

    if (frame_packing->frame_packing_arrangement_cancel_flag == 0) {
        frame_packing->frame_packing_arrangement_type = u_v(ctx, 7, "SEI: frame_packing_arrangement_type");
        frame_packing->quincunx_sampling_flag = (td_s8)u_v(ctx, 1, "SEI: quincunx_sampling_flag");
        frame_packing->content_interpretation_type = u_v(ctx, 6, "SEI: content_interpretation_type");
        frame_packing->spatial_flipping_flag = (td_s8)u_v(ctx, 1, "SEI: spatial_flipping_flag");
        frame_packing->frame0_flipped_flag = (td_s8)u_v(ctx, 1, "SEI: frame0_flipped_flag");
        frame_packing->field_views_flag = (td_s8)u_v(ctx, 1, "SEI: field_views_flag");
        frame_packing->current_frame_is_frame0_flag = (td_s8)u_v(ctx, 1, "SEI: current_frame_is_frame0_flag");
        frame_packing->frame0_self_contained_flag = (td_s8)u_v(ctx, 1, "SEI: frame0_self_contained_flag");
        frame_packing->frame1_self_contained_flag = (td_s8)u_v(ctx, 1, "SEI: frame1_self_contained_flag");

        if ((frame_packing->quincunx_sampling_flag == 0) && frame_packing->frame_packing_arrangement_type != 5) {
            frame_packing->frame0_grid_position_x = (td_s8)u_v(ctx, 1, "SEI: frame0_grid_position_x");
            frame_packing->frame0_grid_position_y = (td_s8)u_v(ctx, 1, "SEI: frame0_grid_position_y");
            frame_packing->frame1_grid_position_x = (td_s8)u_v(ctx, 1, "SEI: frame1_grid_position_x");
            frame_packing->frame1_grid_position_y = (td_s8)u_v(ctx, 1, "SEI: frame1_grid_position_y");
        }

        frame_packing->frame_packing_arrangement_reserved_byte =
            u_v(ctx, 8, "SEI: frame_packing_arrangement_reserved_byte");
        frame_packing->frame_packing_arrangement_repetition_period =
            (td_s32)ue_v(ctx, "SEI: frame_packing_arrangement_repetition_period");
    }

    frame_packing->frame_packing_arrangement_extension_flag =
        (td_s8)u_v(ctx, 1, "SEI: frame_packing_arrangement_extension_flag");

    frame_packing->frame_packing_dec_ok = 1;
    return H264_OK;
}

STATIC td_s32 h264_dec_pic_timing_sei_sps(const h264_ctx *ctx, h264_sps **sps)
{
    td_s32 i;

    if (*sps != TD_NULL) {
        if ((*sps)->is_valid == 0) {
            *sps = TD_NULL;
        }
    }

    if (*sps != TD_NULL) {
        return H264_OK;
    }

    for (i = 0; i < ctx->max_sps_num; i++) {
        if (ctx->sps[i] != TD_NULL) {
            if (ctx->sps[i]->is_valid != 0) {
                *sps = ctx->sps[i];
                break;
            }
        }
    }

    if (*sps == TD_NULL) {
        dprint(PRN_WARN, "DecPicTimingSEI but SPS is invalid.\n");
        return H264_ERR;
    }

    return H264_OK;
}

STATIC td_s32 h264_dec_pic_timing_sei(h264_ctx *ctx, td_s32 pay_load_size)
{
    h264_pic_timing_sei *pic_timing = &ctx->pic_timing_sei;
    h264_sps *sps = TD_NULL;
    td_s32 cpb_removal_len = 0;
    td_s32 dpb_output_len = 0;
    td_s32 ret;

    vfmw_unused(pay_load_size);
    sps = ctx->sps[ctx->cur_sps.seq_parameter_set_id];
    ret = h264_dec_pic_timing_sei_sps(ctx, &sps);
    if (ret != H264_OK) {
        dprint(PRN_WARN, "h264_dec_pic_timing_sei_sps error\n");
        return ret;
    }

    if ((sps->vui_parameters_present_flag != 0) && ((sps->vui_seq_parameters.nal_hrd_parameters_present_flag != 0) ||
                                             (sps->vui_seq_parameters.vcl_hrd_parameters_present_flag != 0))) {
        if (sps->vui_seq_parameters.nal_hrd_parameters_present_flag != 0) {
            cpb_removal_len = (td_s32)(sps->vui_seq_parameters.nal_hrd_parameters.cpb_removal_delay_length_minus1 + 1);
            dpb_output_len = (td_s32)(sps->vui_seq_parameters.nal_hrd_parameters.dpb_output_delay_length_minus1 + 1);
        } else if (sps->vui_seq_parameters.vcl_hrd_parameters_present_flag != 0) {
            cpb_removal_len = (td_s32)(sps->vui_seq_parameters.vcl_hrd_parameters.cpb_removal_delay_length_minus1 + 1);
            dpb_output_len = (td_s32)(sps->vui_seq_parameters.vcl_hrd_parameters.dpb_output_delay_length_minus1 + 1);
        }

        pic_timing->cpb_removal_delay = (td_u32)u_v(ctx, (td_u32)cpb_removal_len, "SEI: cpb_removal_delay");
        pic_timing->dpb_output_delay = (td_u32)u_v(ctx, (td_u32)dpb_output_len, "SEI: dpb_output_delay");
    }

    if (sps->vui_seq_parameters.pic_struct_present_flag != 0) {
        pic_timing->pic_struct = (td_u8)u_v(ctx, 4, "pic_struct");
    }

    return H264_OK;
}

STATIC td_void h264_sei_payload_get_bit_offset(const h264_ctx *ctx, td_u32 *total_bit_offset)
{
    if (ctx->cur_nal->nal_segment == 2) { /* 2, a number */
        /* 8, a number */
        *total_bit_offset = (ctx->cur_nal->stream[0].bits_offset + ctx->cur_nal->stream[1].bits_offset) / 8;
    } else {
        *total_bit_offset = ctx->cur_nal->stream[0].bits_offset;
    }
    return;
}

STATIC td_void h264_sei_payload_buffering_period(h264_ctx *ctx, td_s32 pay_load_size)
{
    td_s32 ret;

    ret = pass_bytes(ctx, (td_u32)pay_load_size);
    if (ret < 0) {
        dprint(PRN_ERROR, "pass_bytes err [%s][%d]\n", __func__, __LINE__);
    }

    skip_bytes(ctx, pay_load_size);

    return;
}

STATIC td_s32 h264_sei_payload_pic_timing(h264_ctx *ctx, td_s32 pay_load_size)
{
    td_s32 ret;
    td_s32 bitpos;
    td_s32 bits;

    bitpos = bs_pos(&ctx->bs);
    ret = h264_dec_pic_timing_sei(ctx, pay_load_size);
    if (ret != H264_OK) {
        return ret;
    }
    bitpos = bs_pos(&ctx->bs) - bitpos;

    if (8 * pay_load_size > bitpos) {
        skip_bytes(ctx, ((8 * pay_load_size - bitpos) / 8));

        vfmw_check_ret(pay_load_size < VFMW_INT_MAX, H264_ERR);
        bits = (8 * pay_load_size - bitpos) % 8;

        if ((bs_pos(&ctx->bs) + bits) <= (ctx->bs.bs_len * 8)) {
            bs_skip(&ctx->bs, bits);
        }
    }

    ret = pass_bytes(ctx, (td_u32)pay_load_size);
    if (ret < 0) {
        dprint(PRN_ERROR, "pass_bytes err [%s][%d]\n", __func__, __LINE__);
    }
    return H264_OK;
}
#ifdef H264_USRDATA_SUPPORT
STATIC td_s32 h264_sei_payload_parse_userdata_len(h264_ctx *ctx, td_s32 pay_load_size,
                                                   td_u32 *min_len, td_u32 *min_len_tmp, vfmw_usrdat **usr_dat)
{
    td_u32 segment_idx;
    td_u32 lenth_not_parse;

    if (ctx->total_usrdat_num > (MAX_USERDATA_NUM - 1)) {
        usd_release(ctx->chan_id, ctx->usr_dat_array[0]);
        usd_release(ctx->chan_id, ctx->usr_dat_array[1]);
        usd_release(ctx->chan_id, ctx->usr_dat_array[2]);
        usd_release(ctx->chan_id, ctx->usr_dat_array[3]);
        ctx->usr_dat_array[0] = TD_NULL;
        ctx->usr_dat_array[1] = TD_NULL;
        ctx->usr_dat_array[2] = TD_NULL;
        ctx->usr_dat_array[3] = TD_NULL;
        ctx->total_usrdat_num = 0;
    }
    *usr_dat = usd_get(ctx->chan_id);
    ctx->usr_dat_array[ctx->total_usrdat_num] = *usr_dat;
    if (*usr_dat == TD_NULL) {
        return H264_ERR;
    }
    *min_len = (td_u32)(vfmw_min(VFMW_MAX_USD_SIZE, pay_load_size));
    lenth_not_parse = 0;

    for (segment_idx = ctx->cur_nal->nal_used_segment; segment_idx < ctx->cur_nal->nal_segment; segment_idx++) {
        lenth_not_parse += ctx->cur_nal->stream[segment_idx].bitstream_length -
                           ((ctx->cur_nal->stream[segment_idx].bits_offset + 7) / 8);
    }

    if ((td_s32)(*min_len) > (td_s32)lenth_not_parse) {
        *min_len_tmp = lenth_not_parse;
    } else {
        *min_len_tmp = *min_len;
    }
    return H264_OK;
}

STATIC td_void h264_sei_payload_parse_userdata_itu_t_t35(h264_ctx *ctx, td_s32 *used_byte, vfmw_usrdat *usr_dat)
{
    td_s32 ret;

    usr_dat->is_registered = 1;
    usr_dat->itu_t_t35_country_code = (td_u8)u_v(ctx, 8, "SEI: itu_t_t35_country_code"); /* 8, a number */
    if (usr_dat->itu_t_t35_country_code != 0xFF) {
        usr_dat->itu_t_t35_country_code_extension_byte = 0;
        *used_byte = 1;
    } else {
        usr_dat->itu_t_t35_country_code_extension_byte =
            (td_u8)u_v(ctx, 8, "SEI: itu_t_t35_country_code_extension_byte"); /* 8, a number */
        *used_byte = 2; /* 2, a number */
    }
    usr_dat->itu_t_t35_provider_code = (td_u16)u_v(ctx, 16, "SEI: itu_t_t35_provider_code"); /* 16, a number */
    *used_byte += 2; /* 2, a number */
    ret = pass_bytes(ctx, (td_u32)*used_byte);
    if (ret == -1) {
        return;
    }

    return;
}

STATIC td_bool h264_sei_usr_dat_unreg_unsupport_uuid(td_u8 *uuid_iso_iec_11578)
{
    const td_u8 unsupport_uuid1[UUID_ISO_IEC_11578_LEN]  = {0xfa, 0x5e, 0xfe, 0x40, 0xa6, 0xaf, 0x11, 0xdd,
        0xa6, 0x59, 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b};
    const td_u8 unsupport_uuid2[UUID_ISO_IEC_11578_LEN]  = {0x68, 0x55, 0x98, 0x4e, 0x49, 0x9c, 0x45, 0xc5,
        0x8e, 0x5b, 0xf2, 0x7b, 0xd1, 0xd4, 0xac, 0xe6};

    if (os_memcmp(uuid_iso_iec_11578, unsupport_uuid1, UUID_ISO_IEC_11578_LEN) == 0) {
        return TD_TRUE;
    }

    if (os_memcmp(uuid_iso_iec_11578, unsupport_uuid2, UUID_ISO_IEC_11578_LEN) == 0) {
        return TD_TRUE;
    }
    return TD_FALSE;
}

STATIC td_bool h264_sei_usr_dat_unreg_unsupport_userdata(td_u8 *user_data)
{
    const td_u8 unsupport_userdata1[9]  = {0x44, 0x69, 0x76, 0x58, 0x20, 0x50, 0x6c, 0x75, 0x73}; /* 9 datalen */
    const td_u8 unsupport_userdata2[8]  = {0x44, 0x69, 0x76, 0x58, 0x50, 0x6c, 0x75, 0x73};       /* 8 datalen */
    const td_u8 unsupport_userdata3[9]  = {0x44, 0x69, 0x76, 0x58, 0x20, 0x48, 0x45, 0x56, 0x43}; /* 9 datalen */

    if (os_memcmp(user_data, unsupport_userdata1, sizeof(unsupport_userdata1)) == 0) {
        dprint(PRN_ERROR, "cann't dec usrdata  2\n");
        return TD_TRUE;
    }

    if (os_memcmp(user_data, unsupport_userdata2, sizeof(unsupport_userdata2)) == 0) {
        dprint(PRN_ERROR, "cann't dec usrdata  3\n");
        return TD_TRUE;
    }

    if (os_memcmp(user_data, unsupport_userdata3, sizeof(unsupport_userdata3)) == 0) {
        dprint(PRN_ERROR, "cann't dec usrdata  4\n");
        return TD_TRUE;
    }

    return TD_FALSE;
}

STATIC td_s32 h264_sei_payload_parse_userdata(h264_ctx *ctx, td_s32 pay_load_size, vfmw_usrdat *usr_dat,
                                             td_s32 used_byte, td_u32 min_len, td_s32 len_tmp)
{
    td_s32 ret;
    td_u32 user_data_type_id;
    td_u32 unsupport = 0;
    (td_void)memset_s(usr_dat->data, VFMW_MAX_USD_SIZE, 0, 20); /* 20: uuid len 16 and codec prefix len 4 */
    ret = get_bytes(ctx, usr_dat->data, len_tmp);
    if (ret <= 0) {
        dprint(PRN_ERROR, "cann't dec usrdata\n");
        usd_release(ctx->chan_id, usr_dat);
        ctx->usr_dat_array[ctx->total_usrdat_num] = TD_NULL;
        return H264_ERR;
    }
    if (h264_sei_usr_dat_unreg_unsupport_uuid(usr_dat->data) == TD_TRUE) {
        dprint(PRN_ERROR, "unsupported  codec\n");
        report_unsupport(ctx->chan_id);
        unsupport = 1;
    }

    if (unsupport == 0) {
        if (h264_sei_usr_dat_unreg_unsupport_userdata(usr_dat->data + UUID_ISO_IEC_11578_LEN) == TD_TRUE) {
            dprint(PRN_ERROR, "unsupported  codec\n");
            report_unsupport(ctx->chan_id);
        }
    }

    if (ret >= AFD_INFO_MIN_LENGTH_IN_BYTES) {
        user_data_type_id = *((td_u32 *)usr_dat->data);
        if (user_data_type_id == AFD_IDENTIFIER) {
            ctx->cur_afd_info.active_format_flag =
                (usr_dat->data[4] == AFD_ACTIVE_FORMAT_FLAG_BYTE_VALUE ? 1 : 0);
            if (ctx->cur_afd_info.active_format_flag) {
                ctx->cur_afd_info.active_format = (usr_dat->data[5] & 0x0F);
            }
            /* transfer afd info by usrdata */
            skip_bytes(ctx, ret);
        }
    }
    if (ret < ((td_s32)min_len - used_byte)) {
        /* return len smaller than the len would read, must no stream */
        usr_dat->data_size = ret;
        skip_bytes(ctx, ret);
    } else {
        usr_dat->data_size = pay_load_size - used_byte;

        /* need pass the stream which don't write to usrdata but is apart of usrdata; */
        if ((td_u32)pay_load_size > min_len) {
            ret = pass_bytes(ctx, (td_u32)pay_load_size - min_len);
            if (ret < 0) {
                dprint(PRN_ERROR, "pass_bytes err [%s][%d]\n", __func__, __LINE__);
            }
        }
        skip_bytes(ctx, pay_load_size - used_byte);
    }
    return H264_OK;
}

STATIC td_s32 h264_sei_payload_type_user_data(h264_ctx *ctx, td_u32 pay_load_type, td_s32 pay_load_size)
{
    td_s8 registered_flag = 0;
    td_s32 used_byte;
    td_s32 len_tmp;
    td_u32 min_len = VFMW_MAX_USD_SIZE;
    td_u32 min_len_tmp = VFMW_MAX_USD_SIZE;
    vfmw_usrdat *usr_dat = TD_NULL;

    if (pay_load_type == SEI_USER_DATA_REGISTERED_ITU_T_T35) {
        registered_flag = 1;
    }

    if (h264_sei_payload_parse_userdata_len(ctx, pay_load_size, &min_len, &min_len_tmp, &usr_dat) != H264_OK) {
        return H264_ERR;
    }

    used_byte = 0;
    if (registered_flag == 1) { /* SEI_USER_DATA_REGISTERED_ITU_T_T35 解析相关参数 */

        h264_sei_payload_parse_userdata_itu_t_t35(ctx, &used_byte, usr_dat);
        if ((td_u32)used_byte == min_len_tmp) {
            usr_dat->pts = ctx->extra_data->pts;
            usr_dat->from = VFMW_USD_H264_REG;
            usr_dat->seq_cnt = ctx->seq_cnt;
            report_usrdat(ctx->chan_id, usr_dat);
            ctx->total_usrdat_num++;
            return H264_OK;
        }
    }

    len_tmp = (td_s32)(min_len_tmp - (td_u32)used_byte);
    if (len_tmp <= 0) {
        dprint(PRN_ERROR, "len_tmp = %d cann't dec usrdata\n", len_tmp);
        usd_release(ctx->chan_id, usr_dat);
        ctx->usr_dat_array[ctx->total_usrdat_num] = TD_NULL;
        return H264_ERR;
    }

    h264_sei_payload_parse_userdata(ctx, pay_load_size, usr_dat, used_byte, min_len, len_tmp);
    usr_dat->pts = ctx->extra_data->pts;
    usr_dat->from = 8;
    usr_dat->seq_cnt = ctx->seq_cnt;
    report_usrdat(ctx->chan_id, usr_dat);
    ctx->total_usrdat_num++; /* add for h264 cc usrdata must add this! */

    return H264_OK;
}
#endif
STATIC td_s32 h264_sei_payload_parse_part1(h264_ctx *ctx, td_u32 pay_load_type,
                                           td_s32 pay_load_size, td_s32 *find_flag)
{
    td_s32 ret;

    switch (pay_load_type) {
        case SEI_BUFFERING_PERIOD:
            h264_sei_payload_buffering_period(ctx, pay_load_size);
            break;

        case SEI_PIC_TIMING:
            ret = h264_sei_payload_pic_timing(ctx, pay_load_size);
            if (ret != H264_OK) {
                return ret;
            }
            break;

        case SEI_PAN_SCAN_RECT:
        case SEI_FILLER_PAYLOAD:
            ret = pass_bytes(ctx, (td_u32)pay_load_size);
            if (ret < 0) {
                dprint(PRN_ERROR, "pass_bytes err [%s][%d]\n", __func__, __LINE__);
            }

            skip_bytes(ctx, pay_load_size);
            break;

        case SEI_USER_DATA_REGISTERED_ITU_T_T35:
        case SEI_USER_DATA_UNREGISTERED:
#ifdef H264_USRDATA_SUPPORT
            return h264_sei_payload_type_user_data(ctx, pay_load_type, pay_load_size);
#endif
        default:
             *find_flag = 0;
            break;
    }

    return H264_OK;
}

STATIC td_void h264_sei_pay_load_type_frame_packing_arrangement(h264_ctx *ctx, td_s32 pay_load_size)
{
    td_s32 ret;
    td_s32 bitpos;
    td_s32 bits;

    bitpos = bs_pos(&ctx->bs);
    h264_dec_frame_packing_sei(ctx, pay_load_size);
    bitpos = bs_pos(&ctx->bs) - bitpos;

    if (8 * pay_load_size > bitpos) {
        skip_bytes(ctx, (8 * pay_load_size - bitpos) / 8);
        vfmw_check(pay_load_size < VFMW_INT_MAX);
        bits = (8 * pay_load_size - bitpos) % 8;

        if ((bs_pos(&ctx->bs) + bits) <= (ctx->bs.bs_len * 8)) {
            bs_skip(&ctx->bs, bits);
        }
    }

    ret = pass_bytes(ctx, (td_u32)pay_load_size);
    if (ret < 0) {
        dprint(PRN_ERROR, "pass_bytes err [%s][%d]\n", __func__, __LINE__);
    }
}

STATIC td_s32 h264_parse_splice_info(h264_ctx *ctx, td_u8 *bytes, td_u8 version)
{
    td_u32 data;

    ctx->splice_info.version = version;
    if (version == VFMW_SPLICE_VERSION_V1) {
        ctx->splice_info.format_type = ((bytes[0] & 0x3) == 0) ? VFMW_SPLICE_FORMAT_AYUV : VFMW_SPLICE_FORMAT_MAX;
        ctx->splice_info.packing_type =
            (((bytes[0] >> 2) & 0x3) == 0) ? VFMW_SPLICE_PACKING_TAB : VFMW_SPLICE_PACKING_MAX;
        data = bytes[2] & 0xf;
        data = data << 8 | bytes[1];
        ctx->splice_info.ori_width = data;
        data = bytes[3];
        data = data << 4 | ((bytes[2] & 0xf0) >> 4);
        ctx->splice_info.ori_height = data;
    } else {
        dprint(PRN_ERROR, "[%s][%d] version %d is not support\n", __func__, __LINE__, version);
        return H264_ERR;
    }

    if (ctx->splice_info.ori_width == 0 ||
        ctx->splice_info.ori_width > H264_MAX_PIX_WIDTH ||
        ctx->splice_info.ori_height == 0 ||
        ctx->splice_info.ori_height > H264_MAX_PIX_HEIGHT) {
        dprint(PRN_ERROR, "[%s][%d] ori_width %d max:%d | ori_height %d max:%d\n", __func__, __LINE__,
            ctx->splice_info.ori_width, H264_MAX_PIX_WIDTH, ctx->splice_info.ori_height, H264_MAX_PIX_HEIGHT);
        return H264_ERR;
    }

    if (ctx->splice_info.format_type == VFMW_SPLICE_FORMAT_AYUV &&
        ctx->splice_info.packing_type == VFMW_SPLICE_PACKING_TAB) {
        ctx->splice_info.type = VFMW_SPLICE_AYUV_TAB;
    } else {
        ctx->splice_info.type = VFMW_SPLICE_NONE;
        dprint(PRN_ERROR, "[%s][%d] format_type %d packing_type %d is not support!\n", __func__, __LINE__,
            ctx->splice_info.format_type, ctx->splice_info.packing_type);
        return H264_ERR;
    }

    return H264_OK;
}

/* This flag is related to the video dial tool and cannot be modified at will. */
STATIC td_s32 h264_sei_pay_load_type_splice(h264_ctx *ctx, td_s32 pay_load_size)
{
    td_s32 i;
    td_s32 ret;
    td_u8 version;
    td_u8 *bytes = TD_NULL;
    td_s32 splice_info_len;

    ret = get_bytes(ctx, &version, 1);
    vfmw_check_ret_print(ret > 0, H264_ERR, "SEI nal parse payload splice get_bytes error\n");

    if (version == VFMW_SPLICE_VERSION_V1) {
        splice_info_len = (td_s32)sizeof(h264_unregistered_splice_sei_v1);
    } else {
        dprint(PRN_ERROR, "[%s][%d] version %d is not support\n", __func__, __LINE__, version);
        return H264_ERR;
    }

    if (pay_load_size != splice_info_len) {
        dprint(PRN_ERROR, "[%s][%d] pay_load_size %d splice_info_len %d err\n", __func__, __LINE__, pay_load_size,
            splice_info_len);
        return H264_ERR;
    }

    bytes = (td_u8 *)os_alloc_vir(ctx->chan_id, "splice_info", splice_info_len - 1);
    if (bytes == TD_NULL) {
        dprint(PRN_ERROR, "[%s][%d] os_alloc_vir %d err\n", __func__, __LINE__, splice_info_len - 1);
        return H264_ERR;
    }

    for (i = 0; i < splice_info_len - 1; i++) {
        ret = get_bytes(ctx, &bytes[i], 1);
        vfmw_check_goto_print(ret > 0, err0, "SEI nal parse payload splice get_bytes error\n");
    }
    ret = skip_bytes(ctx, pay_load_size);
    vfmw_check_goto_print(ret > 0, err0, "SEI nal parse payload type skip_bytes error\n");

    ret = h264_parse_splice_info(ctx, bytes, version);
    vfmw_check_goto_print(ret == H264_OK, err0, "h264_parse_splice_info error\n");

    os_free_vir(bytes);
    return H264_OK;
err0:
    os_free_vir(bytes);
    return H264_ERR;
}

STATIC td_void h264_sei_payload_parse_part2(h264_ctx *ctx, td_u32 pay_load_type, td_s32 pay_load_size)
{
    td_s32 ret;

    switch (pay_load_type) {
        case SEI_RECOVERY_POINT:
            skip_bytes(ctx, pay_load_size);

            ret = pass_bytes(ctx, (td_u32)pay_load_size);
            if (ret < 0) {
                dprint(PRN_ERROR, "pass_bytes err [%s][%d]\n", __func__, __LINE__);
            }
            break;

        case SEI_DEC_REF_PIC_MARKING_REPETITION:
        case SEI_SPARE_PIC:
        case SEI_SCENE_INFO:
        case SEI_SUB_SEQ_INFO:
        case SEI_SUB_SEQ_LAYER_CHARACTERISTICS:
        case SEI_SUB_SEQ_CHARACTERISTICS:
        case SEI_FULL_FRAME_FREEZE:
        case SEI_FULL_FRAME_FREEZE_RELEASE:
        case SEI_FULL_FRAME_SNAPSHOT:
        case SEI_PROGRESSIVE_REFINEMENT_SEGMENT_START:
        case SEI_PROGRESSIVE_REFINEMENT_SEGMENT_END:
        case SEI_MOTION_CONSTRAINED_SLICE_GROUP_SET:
        case SEI_FILM_GRAIN_CHARACTERISTICS:
        case SEI_DEBLOCKING_FILTER_DISPLAY_PREFERENCE:
        case SEI_STEREO_VIDEO_INFO:
            ret = pass_bytes(ctx, (td_u32)pay_load_size);
            if (ret < 0) {
                dprint(PRN_ERROR, "pass_bytes err [%s][%d]\n", __func__, __LINE__);
            }

            skip_bytes(ctx, pay_load_size);

            break;

        case SEI_FRAME_PACKING_ARRANGEMENT:
            h264_sei_pay_load_type_frame_packing_arrangement(ctx, pay_load_size);
            break;
        case SEI_CUSTOMIZED_SPLICE_INFO:
            ret = h264_sei_pay_load_type_splice(ctx, pay_load_size);
            if (ret < 0) {
                dprint(PRN_ERROR, "h264_sei_pay_load_type_splice err [%s][%d]\n", __func__, __LINE__);
                ctx->splice_info.type = VFMW_SPLICE_NONE;
            }
            break;

        default:
            ret = pass_bytes(ctx, (td_u32)pay_load_size);
            if (ret < 0) {
                dprint(PRN_ERROR, "pass_bytes err [%s][%d]\n", __func__, __LINE__);
            }

            skip_bytes(ctx, pay_load_size);

            break;
    }
}

STATIC td_s32 h264_sei_pay_load(h264_ctx *ctx, td_u32 pay_load_type, td_s32 pay_load_size)
{
    td_u32 total_bit_offset; /* just for dprint */
    td_s32 find_flag = 1;
    td_s32 ret;

    h264_sei_payload_get_bit_offset(ctx, &total_bit_offset);

    ret = h264_sei_payload_parse_part1(ctx, pay_load_type, pay_load_size, &find_flag);
    if (ret != H264_OK) {
        return ret;
    }

    if (find_flag == 0) {
        h264_sei_payload_parse_part2(ctx, pay_load_type, pay_load_size);
    }

    return H264_OK;
}

#define MAX_PAYLOAD_SIZE (400 * 1024)

STATIC td_void h264_dec_sei_nal_type(h264_ctx *ctx)
{
    td_bool option = 0;
    option = 1;

    if (option == 1) {
        ctx->cur_nal->stream[0].bits_offset = 32; /* 32 is bit num */
    }
    return;
}

/***********************************************************************************
* Function:    h264_dec_sei(h264_ctx *ctx)
* Description: 解析SEI
* Input:       pStream             在码流解析时会消耗码流
*              size                SEI Nalu大小
* Output:      g_RecoverPointFlg   解码恢复点标志
*              pSEI                输出的SEI信息结构体
* Return:      DEC_NORMAL          可以解码此Nalu数据
*              DEC_ERR             不可以解码
* Others:      无
***********************************************************************************/
STATIC td_s32 h264_dec_sei(h264_ctx *ctx)
{
    td_u32 pay_load_type;
    td_s32 pay_load_size;
    td_s32 ret;
    td_u8 byte_tmp;

    /* skip nal type */
    h264_dec_sei_nal_type(ctx);

    byte_tmp = 0;
    pay_load_type = 0;

    do {
        ret = get_bytes(ctx, &byte_tmp, 1);
        vfmw_check_ret_print(ret > 0, H264_ERR, "SEI nal parse payload type error\n");

        pay_load_type += ((byte_tmp == 0xff) ? 0xff : byte_tmp);
        ret = skip_bytes(ctx, 1);
        vfmw_check_ret_print(ret > 0, H264_ERR, "SEI nal parse payload type error\n");
    } while (byte_tmp == 0xFF);

    vfmw_check_ret_no_print(ret > 0, H264_ERR);

    do {
        pay_load_size = 0;

        do {
            ret = get_bytes(ctx, &byte_tmp, 1);
            vfmw_check_ret_print(ret > 0, H264_ERR, "SEI nal dec payload type err\n");

            pay_load_size += (td_s32)((byte_tmp == 0xff) ? 0xff : byte_tmp);
            ret = skip_bytes(ctx, 1);
            vfmw_check_ret_print(ret > 0, H264_ERR, "SEI nal dec payload type err\n");
        } while (byte_tmp == 0xff);

        if (pay_load_size > MAX_PAYLOAD_SIZE) {
            dprint(PRN_ERROR, "SEI nal dec pay_load_size err\n");
            return H264_ERR;
        }

        if (h264_sei_pay_load(ctx, pay_load_type, pay_load_size) != H264_OK) {
            return H264_ERR;
        }

        if (ctx->total_usrdat_num >= 4) { /* 4 is vfmw_max num */
            dprint(PRN_ERROR, "too much usrdat, cann't dec SEI\n");
            return H264_ERR;
        }

        pay_load_type = 0;

        do {
            ret = get_bytes(ctx, &byte_tmp, 1);
            vfmw_check_ret_print(ret > 0, H264_ERR, "SEI nal dec payload type err\n");

            pay_load_type += ((byte_tmp == 0xff) ? 0xff : byte_tmp);
            ret = skip_bytes(ctx, 1);
            vfmw_check_ret_print(ret > 0, H264_ERR, "SEI nal dec payload type err\n");
        } while (byte_tmp == 0xff);
    } while ((ret > 0) && (byte_tmp != 0x80));

    return H264_OK;
}

/*************************************************************************
*   函数原型 :  init_old_slice(h264_ctx  *ctx)
*   功能描述 :  初始化g_OldSlice, 目的是为了使第一slice可以判断new_pic_flag
*   参数说明 :  无
*   返回值   ： 无
*************************************************************************/
STATIC td_void init_old_slice(h264_ctx *ctx)
{
    ctx->prev_slice.pic_parameter_set_id = (td_u32)ctx->max_pps_num;
    ctx->prev_slice.frame_num = 88888; /* 88888 is init val */
    ctx->prev_slice.field_pic_flag = 0;
    ctx->prev_slice.bottom_field_flag = 2; /* 2 is invalid val */
    ctx->prev_slice.nal_ref_idc = 7; /* 7 is init val */
    ctx->prev_slice.nal_unit_type = 32; /* 32 is nal type */
    ctx->prev_slice.pic_oder_cnt_lsb = 3; /* 3 is poc */
    ctx->prev_slice.delta_pic_oder_cnt_bottom = -1;
    ctx->prev_slice.delta_pic_order_cnt[0] = 0;
    ctx->prev_slice.delta_pic_order_cnt[1] = 0;
    ctx->prev_slice.first_mb_in_slice = H264_MAX_MB_NUM_IN_PIC;

    return;
}

STATIC td_void h264_dec_one_nal_init(h264_ctx *ctx, td_u32 nal_header)
{
    ctx->cur_nal->nal_unit_type = nal_header & 0x1f;
    ctx->cur_nal->nal_ref_idc = (nal_header >> 5) & 3;
    ctx->cur_nal->forbidden_bit = (nal_header >> 7) & 1;
    ctx->se_err_flg = 0; /* clear g_SeErrFlg */
    if (ctx->is_insert_eopic_nal != 0) {
        ctx->cur_nal->nal_unit_type = NALU_TYPE_EOPIC;
    }

    ctx->total_nal++;

    return;
}

STATIC td_s32 h264_dec_one_nal_unit_type(h264_ctx *ctx, td_u32 *nal_header)
{
    td_u8 flag;

    /* 如果nal_unit_type == 20我们需要解一个24bit的头 */
    if (ctx->cur_nal->nal_unit_type == NALU_TYPE_SLC_EXT) {
        *nal_header = (td_u32)bs_get(&(ctx->bs), 24); /* 24, a number */
        ctx->cur_nal->svc_extension_flag = (*nal_header >> 23) & 1; /* 23, a number */

        if (ctx->cur_nal->svc_extension_flag != 0) {
            ctx->nalu_svc_ext.idr_flag = (*nal_header >> 22) & 1; /* 22, a number */
            ctx->nalu_svc_ext.priority_id = (*nal_header >> 16) & 0x3f; /* 16, a number */
            ctx->nalu_svc_ext.no_inter_layer_pred_flag = (*nal_header >> 15) & 1; /* 15, a number */
            ctx->nalu_svc_ext.dependency_id = (*nal_header >> 12) & 7; /* 12, 7, a number */
            ctx->nalu_svc_ext.quality_id = (*nal_header >> 8) & 0xf; /* 8, a number */
            ctx->nalu_svc_ext.temporal_id = (*nal_header >> 5) & 7; /* 5, 7, a number */
            ctx->nalu_svc_ext.use_ref_base_pic_flag = (*nal_header >> 4) & 1; /* 4, a number */
            ctx->nalu_svc_ext.discardable_flag = (*nal_header >> 3) & 1; /* 3, a number */
            ctx->nalu_svc_ext.output_flag = (*nal_header >> 2) & 1; /* 2, a number */
            ctx->nalu_svc_ext.reserved_three_2bits = *nal_header & 3; /* 3, a number */
        }
        ctx->cur_slice.slice_output_flag = ctx->nalu_svc_ext.output_flag;

        flag = (ctx->nalu_svc_ext.quality_id != 0 ||
                ctx->nalu_svc_ext.no_inter_layer_pred_flag != 1);
        if (flag != 0) {
            dprint(PRN_WARN, "svc_extension_flag error:priority_id:%d, pred_flag%d\n",
                   ctx->nalu_svc_ext.quality_id, ctx->nalu_svc_ext.no_inter_layer_pred_flag);
            clear_curr_nal(ctx);
            return H264_ERR;
        }

        if (ctx->nalu_svc_ext.idr_flag == 1) {
            ctx->cur_nal->nal_unit_type = NALU_TYPE_IDR; /* 此处用来判断是否为idr帧 */
        }

        ctx->cur_nal->nal_bitoffset += 24; /* used bits when decode nal svc info, 24 , a number. */
    } else {
        ctx->cur_slice.slice_output_flag = 1;
    }
    return H264_OK;
}

STATIC td_s32 h264_dec_one_nal_scl_etx(h264_ctx *ctx)
{
    td_s32 ret;

    ctx->cur_slice.nal_unit_type = ctx->cur_nal->nal_unit_type;
    ctx->cur_slice.nal_ref_idc = ctx->cur_nal->nal_ref_idc;
    ret = slice_check(ctx);
    if (ret != H264_OK) {
        dprint(PRN_ERROR, "slice_check failed, clear current slice.\n");
        clear_curr_slice(ctx);
        report_decsyntax_err(ctx->chan_id);
        report_dec_err(ctx->chan_id, ctx->extra_data->pts, SYNTAX_ERR_SLICE_HDR);
        return H264_ERR;
    }

    ret = h264_dec_slice(ctx);
    if (ret == SYN_PARSE_RESOURCE_INVALID) {
        ctx->have_nal_not_decoded = 1;

        return SYN_PARSE_RESOURCE_INVALID;
    } else if (ret != H264_OK) {
        clear_curr_slice(ctx);
        report_decsyntax_err(ctx->chan_id);
        report_dec_err(ctx->chan_id, ctx->extra_data->pts, SYNTAX_ERR_SLICE_HDR);
        return H264_ERR;
    }

    return H264_OK;
}

STATIC td_s32 h264_dec_one_nal_pps(h264_ctx *ctx)
{
    td_s32 ret;

    ret = h264_dec_pps(ctx);
    clear_curr_nal(ctx);

    if (ret != H264_OK) {
        dprint(PRN_ERROR, "PPS decode error.\n");
        report_decsyntax_err(ctx->chan_id);
        report_dec_err(ctx->chan_id, ctx->extra_data->pts, SYNTAX_ERR_PPS);
        return H264_ERR;
    }

    ctx->total_pps++;

    return H264_OK;
}

STATIC td_s32 h264_dec_one_nal_sub_sps(h264_ctx *ctx)
{
    td_s32 ret;

    ret = h264_dec_sps(ctx);
    clear_curr_nal(ctx);

    if (ret != H264_OK) {
        dprint(PRN_ERROR, "SPS decode error.\n");
        report_decsyntax_err(ctx->chan_id);
        report_dec_err(ctx->chan_id, ctx->extra_data->pts, SYNTAX_ERR_SPS);
        return H264_ERR;
    }

    ctx->total_sps++;

    return H264_OK;
}

STATIC td_s32 h264_dec_one_nal_sei(h264_ctx *ctx)
{
    td_s32 ret;

    ret = h264_dec_sei(ctx);
    clear_curr_nal(ctx);

    if (ret != H264_OK) {
        dprint(PRN_WARN, "SEI decode error.\n");
        report_decsyntax_err(ctx->chan_id);
    }
    return H264_OK;
}

STATIC td_s32 h264_dec_one_nal_eostream(h264_ctx *ctx)
{
    td_s32 ret;

    if (h264_is_eos_nal(ctx) == 1) {
        ctx->is_stream_end_flag = 1;
        clear_curr_nal(ctx);
        ctx->start_mode = 0;
        ret = dec_check(ctx); /* 解码前检查 */
        if (ret != H264_OK) {
            return SYN_PARSE_ERR;
        }

        ctx->cur_dps->sed_state = SED_IDLE;

        ret = write_pic_msg(ctx);
        if (ret != H264_OK) {
            return SYN_PARSE_ERR;
        }

        ret = h264_dec_post_proc(ctx);
        if (ret != H264_OK) {
            return SYN_PARSE_ERR;
        }

        return SYN_PARSE_CURR_FRMDONE;
    }

    clear_curr_nal(ctx);

    return H264_OK;
}

STATIC td_s32 h264_dec_one_nal_eopic(h264_ctx *ctx, td_u32 nal_mode)
{
    td_s32 ret;
    td_u32 code0 = 0;
    td_u32 code1 = 0;

    if (ctx->is_insert_eopic_nal == 0) {
        code0 = (td_u32)bs_get(&(ctx->bs), 32);
        code1 = (td_u32)bs_get(&(ctx->bs), 32);
    }

    if (nal_mode == 0) {
        clear_curr_nal(ctx);
    }

    if ((code0 == VFMW_STREAM_END_CODE1 && code1 == VFMW_STREAM_END_CODE2) || (ctx->is_insert_eopic_nal != 0)) {
        report_meet_newfrm(ctx->chan_id);
        ctx->start_mode = 0;
        ctx->is_insert_eopic_nal = 0;
        ret = dec_check(ctx); /* 解码前检查 */

        if (ret != H264_OK) {
            return SYN_PARSE_ERR;
        }

        ctx->cur_dps->sed_state = SED_IDLE;

        ret = write_pic_msg(ctx);
        if (ret != H264_OK) {
            return SYN_PARSE_ERR;
        }

        ret = h264_dec_post_proc(ctx);
        if (ret != H264_OK) {
            return SYN_PARSE_ERR;
        }
        return SYN_PARSE_CURR_FRMDONE;
    }
    return H264_OK;
}

STATIC td_void h264_dec_one_nal_default(h264_ctx *ctx, td_u32 nal_header)
{
    (td_void)(nal_header); /* avoid to build err when printing is not supported. */
    dprint(PRN_ERROR, "***** NAL: UNSUPPORT, nal_unit_type=%d,nalu header:%x\n",
           ctx->cur_nal->nal_unit_type, nal_header);

    clear_curr_nal(ctx);
    report_decsyntax_err(ctx->chan_id);

    return;
}

STATIC td_void h264_dec_one_nal_dolby(h264_ctx *ctx, td_u32 nal_mode, td_u32 *nal_header)
{
    td_bool option = 0;

    vfmw_unused(nal_mode);
    option = 1;

    if (option == 1) {
        *nal_header = (td_u32)bs_get(&(ctx->bs), 32); /* 取第一个32bit, 0x00 0x00 0x01 0xxx */
        ctx->cur_nal->nal_bitoffset += 32;   /* used bits when decode nal, 使用了0x000001 + 8bit, 共32bit */
    }
    return;
}

STATIC td_s32 h264_dec_one_nal_type(h264_ctx *ctx, td_u32 nal_mode, td_u32 nal_header)
{
    td_s32 ret = H264_OK;

    switch (ctx->cur_nal->nal_unit_type) {
        case NALU_TYPE_SLICE:
        case NALU_TYPE_IDR:
        case NALU_TYPE_SLC_EXT:
            ret = h264_dec_one_nal_scl_etx(ctx);
            break;

        case NALU_TYPE_PPS:
            ret = h264_dec_one_nal_pps(ctx);
            break;

        case NALU_TYPE_SPS:
        case NALU_TYPE_SUB_SPS:
            ret = h264_dec_one_nal_sub_sps(ctx);
            break;

        case NALU_TYPE_SEI:
            ret = h264_dec_one_nal_sei(ctx);
            break;

        case NALU_TYPE_AUD:
        /* NAL: AUD */
        case NALU_TYPE_EOSEQ:
        /* NAL: EOSEQ */
        case NALU_TYPE_FILL:
            clear_curr_nal(ctx);
            break;

        case NALU_TYPE_EOSTREAM:
            ret = h264_dec_one_nal_eostream(ctx);
            break;

        case NALU_TYPE_SPSEXT:
        /* NAL: SPSEXT */
        case NALU_TYPE_AUX:
            clear_curr_nal(ctx);
            break;

        case NALU_TYPE_EOPIC:
            ret = h264_dec_one_nal_eopic(ctx, nal_mode);
            break;

        default:
            h264_dec_one_nal_default(ctx, nal_header);
            break;
    }

    return ret;
}

STATIC td_s32 h264_dec_one_nal(h264_ctx *ctx, td_u32 nal_mode)
{
    td_u32 nal_header;
    td_s32 ret = H264_OK;

    ctx->cur_nal->nal_used_segment = 0; /* decode from first segment */

    h264_dec_one_nal_dolby(ctx, nal_mode, &nal_header);
    h264_dec_one_nal_init(ctx, nal_header);

    if (h264_dec_one_nal_unit_type(ctx, &nal_header) != H264_OK) {
        return H264_ERR;
    }

    ret = h264_dec_one_nal_type(ctx, nal_mode, nal_header);

    return ret;
}

/* 在一个seg内部搜索0比特的个数
   返回值: 0比特个数，如果返回值等于len*8，则表明此seg完全由0字节组成.
 */
#define TZ_BUF_SIZE 64
STATIC td_u8 g_tmp_buf[TZ_BUF_SIZE + 4];
STATIC td_s32 find_zero_bits_in_seg(const td_u8 *ptr, td_s32 len)
{
    td_u8 *last_byte = TD_NULL;
    td_u8 byte;
    const td_u8 *this_rount_ptr = TD_NULL;
    td_s32 zero_byte = 0, zero_bit = 0;
    td_s32 round, i, r;
    td_u32 total_left_size, this_round_size;

    if (ptr == TD_NULL || len <= 0) {
        dprint(PRN_ERROR, "\nthe stream is error,len:%d\n", len);
        return 0;
    }

    round = (len / TZ_BUF_SIZE) + ((len % TZ_BUF_SIZE) != 0);

    for (r = round; r > 0; r--) {
        vfmw_check_ret((round - r) < VFMW_INT_MAX, 0);
        total_left_size = (td_u32)(len - (round - r) * TZ_BUF_SIZE);

        this_round_size = (total_left_size >= TZ_BUF_SIZE) ? TZ_BUF_SIZE : total_left_size;
        this_rount_ptr = ptr + total_left_size - this_round_size;

        vfmw_check_sec_func(memcpy_s(g_tmp_buf, sizeof(g_tmp_buf), this_rount_ptr, this_round_size));

        last_byte = g_tmp_buf + this_round_size - 1;

        while (*last_byte == 0x00) {
            zero_byte++;

            if (last_byte == g_tmp_buf) {
                break;
            }

            last_byte--;
        }

        if (*last_byte != 0) {
            break;
        }
    }

    /* 在非零字节中搜索0比特个数 */
    if (zero_byte < len) {
        byte = *(ptr + len - zero_byte - 1);

        for (i = 0; i < 8; i++) {
            if ((byte & 1) != 0) {
                break;
            }

            byte >>= 1;
            zero_bit++;
        }
    }

    return (zero_byte * 8 + zero_bit);
}

/*************************************************************************
*      函数原型 :  td_s32 find_trail_zeros(h264_ctx *ctx)
*      功能描述 :  找拖尾零的函数
*      参数说明 :  无
*      返回值   ： DEC_NORMAL; DEC_ERR;
*************************************************************************/
STATIC td_s32 find_trail_zeros(h264_ctx *ctx)
{
    td_s32 zero_bit;

    if (ctx == TD_NULL || ctx->cur_nal == TD_NULL) {
        return H264_ERR;
    }

    zero_bit = -1;

    if (ctx->cur_nal->nal_segment > 1) {
        zero_bit = find_zero_bits_in_seg(ctx->cur_nal->stream[1].stream_buffer,
                                         (td_s32)ctx->cur_nal->stream[1].bitstream_length);

        if ((td_u32)zero_bit >= ctx->cur_nal->stream[1].bitstream_length * 8) {
            /* 该nal由两个seg组成，而且第二个seg全为0，释放掉第二个seg */
            stm_release_seg(ctx->chan_id, ctx->cur_nal->stream[1].stream_id);
            ctx->cur_nal->stream[1].stream_buffer = TD_NULL;
            ctx->cur_nal->nal_segment = 1;
            zero_bit = -1;
        }
    }

    /* 如果nal只有一个seg，或者虽有2个seg但第二个seg全0，搜索第一个seg的拖尾0 */
    if (zero_bit == -1) {
        zero_bit = find_zero_bits_in_seg(ctx->cur_nal->stream[0].stream_buffer,
                                         (td_s32)ctx->cur_nal->stream[0].bitstream_length);
    }

    /* padding = 1000000..., 所以需要去掉的比特数 = zero_bit + 1 */
    ctx->cur_nal->nal_trail_zero_bit_num = (td_u32)(zero_bit + 1);

    if (ctx->cur_nal->nal_segment > 1) {
        if (ctx->cur_nal->nal_trail_zero_bit_num >= ctx->cur_nal->stream[1].bitstream_length * 8) {
            /* SDK-2268: 拖尾1不能去掉，否则VDH报错，cabac码流拖尾1也是slice的结束 */
            /* 有两个seg, 但第二个seg只包含拖尾1+若干拖尾0，所以第二个seg应该被丢弃 */
            stm_release_seg(ctx->chan_id, ctx->cur_nal->stream[1].stream_id);
            ctx->cur_nal->stream[1].stream_buffer = TD_NULL;
            ctx->cur_nal->nal_segment = 1;
            ctx->cur_nal->nal_trail_zero_bit_num = 0;
        }
    }

    return H264_OK;
}

/*************************************************************************
*      函数原型 :  td_void combine_packet(td_void)
*      功能描述 :  拼包功能函数
*      参数说明 :  无
*      返回值   ： td_void
*************************************************************************/
STATIC td_void combine_packet(h264_ctx *ctx)
{
    td_s32 stream_id;

    /* 表示肯定分包了，也就是说肯定Buf触底了 */
    if (ctx->cur_nal->stream[0].bitstream_length < GET_ONE_NALU_SIZE) { /* 需要copy本包到buf的头部 */
        /* copy到最前面 */
        vfmw_check_sec_func(memcpy_s(ctx->cur_nal->stream[1].stream_buffer - ctx->cur_nal->stream[0].bitstream_length,
                  GET_ONE_NALU_SIZE, ctx->cur_nal->stream[0].stream_buffer, ctx->cur_nal->stream[0].bitstream_length));

        /* 原来的位置释放掉 */
        stream_id = ctx->cur_nal->stream[0].stream_id;
        ctx->cur_nal->stream[0].stream_id = ctx->cur_nal->stream[1].stream_id;
        ctx->cur_nal->stream[0].bits_offset = 0;
        ctx->cur_nal->stream[0].stream_buffer = ctx->cur_nal->stream[1].stream_buffer -
                                                ctx->cur_nal->stream[0].bitstream_length;
        ctx->cur_nal->stream[0].stream_phy_addr = ctx->cur_nal->stream[1].stream_phy_addr -
                                                  ctx->cur_nal->stream[0].bitstream_length;
        ctx->cur_nal->stream[0].bitstream_length = ctx->cur_nal->stream[0].bitstream_length +
                                                   ctx->cur_nal->stream[1].bitstream_length;
        ctx->cur_nal->nal_segment = 1;

        stm_release_seg(ctx->chan_id, stream_id);
        ctx->cur_nal->stream[1].stream_buffer = TD_NULL;
        ctx->cur_nal->stream[1].bits_offset = 0;
        ctx->cur_nal->stream[1].bitstream_length = 0;
    }
}

STATIC td_s32 receive_packet_first(h264_ctx *ctx, const h264_stream_packet *packet)
{
    if ((packet->stream_len > 0) && (packet->stream != TD_NULL) && (packet->strm_phy_addr != 0)) {
        ctx->cur_nal = &(ctx->nal_array[0]);
        ctx->cur_nal->stream[0].stream_buffer = packet->stream;
        ctx->cur_nal->stream[0].bitstream_length = (td_u32)packet->stream_len; /* include nal header */
        ctx->cur_nal->stream[0].stream_phy_addr = packet->strm_phy_addr;
        ctx->cur_nal->stream[0].bits_offset = 0;
        ctx->cur_nal->stream[0].stream_id = packet->stream_id;
        ctx->cur_nal->nal_segment = 1;
        ctx->cur_nal->nal_used_segment = 0;
        ctx->cur_nal->nal_integ = packet->last_pack_in_nal;
        ctx->cur_nal->raw_stream_len = packet->raw_stream_len; /* 当前nal包的原始码流长度 */
        ctx->seg_cut_state = (td_u32)packet->seg_cut_state;

        if (ctx->extra_data->stop_record != 1) {
            ctx->cur_pic.frame_stream_size += (td_u32)packet->stream_len;
            ctx->cur_pic.cur_nal_size += (td_u32)packet->stream_len;
        }
    } else {
        dprint(PRN_ERROR, "receive a zero packet\n");
        stm_release_seg(ctx->chan_id, packet->stream_id);
        report_decsyntax_err(ctx->chan_id);
        return H264_ERR;
    }
    return H264_OK;
}

STATIC td_void receive_packet_second(h264_ctx *ctx, const h264_stream_packet *packet)
{
    ctx->cur_nal->nal_integ = packet->last_pack_in_nal;

    if ((ctx->extra_data->dynamic_fs_en != 1) || (ctx->extra_data->wait_fs_flag == 0)) {
        if ((packet->stream_len > 0) && (packet->stream != TD_NULL) && (packet->strm_phy_addr != 0) &&
            (ctx->cur_nal->nal_segment <= 1)) {
            ctx->cur_nal->stream[ctx->cur_nal->nal_segment].stream_buffer = packet->stream;
            ctx->cur_nal->stream[ctx->cur_nal->nal_segment].bitstream_length = (td_u32)packet->stream_len;
            ctx->cur_nal->stream[ctx->cur_nal->nal_segment].stream_phy_addr = packet->strm_phy_addr;
            ctx->cur_nal->stream[ctx->cur_nal->nal_segment].bits_offset = 0;
            ctx->cur_nal->stream[ctx->cur_nal->nal_segment].stream_id = packet->stream_id;
            ctx->cur_nal->nal_segment++;

            if (ctx->extra_data->stop_record != 1) {
                ctx->cur_pic.frame_stream_size += (td_u32)packet->stream_len;
                ctx->cur_pic.cur_nal_size += (td_u32)packet->stream_len;
            }

            /* 拼包功能函数 */
            combine_packet(ctx);
            ctx->scd_wrap_around = 1;
        } else {
            /* 第二包长度为0或码流地址为空,那么就只解第一包数据 */
            ctx->cur_nal->nal_integ = 1;
            /* 第二包只需要release */
            stm_release_seg(ctx->chan_id, packet->stream_id);
            report_decsyntax_err(ctx->chan_id);
        }
    }
    return;
}

/*************************************************************************
*      函数原型 :  td_s32 receive_packet(h264_stream_packet *packet)
*      功能描述 :  获取一片码流，并判断该片码流是否是完整的nal
*      参数说明 :  无
*      返回值   ： 1:一个完整的Nalu;0不是一个完整的Nalu;DEC_ERR:解码错误
*************************************************************************/
/* 错误处理可以提出来做,但有两种错误处理需要处理,所以暂时先放在这里 */
STATIC td_s32 receive_packet(h264_ctx *ctx, const h264_stream_packet *packet)
{
    td_s32 ret;

    ctx->scd_wrap_around = 0;

    /* 检查pCurrNal是否正常: 如果pCurrNal中已经存放了至少一包而且当前包不是LastSeg，或者pCurrNal已经是完整的nal，
       则说明pCurrNal状态已经出错，需要把它清除，以保证可以正常接收当前seg. */
    if ((ctx->extra_data->dynamic_fs_en != 1) || (ctx->extra_data->wait_fs_flag == 0)) {
        if ((ctx->cur_nal != TD_NULL) &&
            ((packet->last_pack_in_nal != 1) ||
             (ctx->cur_nal->nal_segment >= 2) ||
             (ctx->cur_nal->nal_integ == 1))) {
            dprint(PRN_ERROR, "nal_release_err\n");
            clear_curr_nal(ctx);
        }
    }

    /* 第一次收到包 */
    if (ctx->cur_nal == TD_NULL) {
        /* 收到非零的包 */
        ret = receive_packet_first(ctx, packet);
        if (ret != H264_OK) {
            dprint(PRN_ERROR, "%s error\n", __func__);
            return ret;
        }
    } else {
        /* 收到第二包数据 */
        receive_packet_second(ctx, packet);
    }

    vfmw_check_ret(ctx->cur_nal != TD_NULL, H264_ERR);

    ret = (ctx->cur_nal->nal_integ != 0) ? H264_OK : H264_ERR;

    return ret;
}

STATIC td_s32 inquire_slice_ext_property(h264_ctx *ctx)
{
    td_u32 nal_header;
    nal_header = (td_u32)bs_get(&(ctx->bs), 24); /* parse 24 bit */
    ctx->cur_nal->svc_extension_flag = (nal_header >> 23) & 1;

    if (ctx->cur_nal->svc_extension_flag != 0) {
        ctx->nalu_svc_ext.idr_flag = (nal_header >> 22) & 1;
        ctx->nalu_svc_ext.priority_id = (nal_header >> 16) & 0x3f;
        ctx->nalu_svc_ext.no_inter_layer_pred_flag = (nal_header >> 15) & 1;
        ctx->nalu_svc_ext.dependency_id = (nal_header >> 12) & 7;
        ctx->nalu_svc_ext.quality_id = (nal_header >> 8) & 0xf;
        ctx->nalu_svc_ext.temporal_id = (nal_header >> 5) & 7;
        ctx->nalu_svc_ext.use_ref_base_pic_flag = (nal_header >> 4) & 1;
        ctx->nalu_svc_ext.discardable_flag = (nal_header >> 3) & 1;
        ctx->nalu_svc_ext.output_flag = (nal_header >> 2) & 1;
        ctx->nalu_svc_ext.reserved_three_2bits = nal_header & 3;
    }

    if (ctx->nalu_svc_ext.idr_flag == 1) {
        ctx->cur_nal->nal_unit_type = NALU_TYPE_IDR;
    }

    return H264_OK;
}

STATIC td_s32 inquire_slice_property(h264_ctx *ctx, td_s32 *is_new_pic, td_s32 *is_size_changed)
{
    td_s32 ret;
    h264_sps *sps = TD_NULL;
    h264_pps *pps = TD_NULL;
    td_s32 oldw, oldh, neww, newh;

    *is_new_pic = *is_size_changed = 0;
    ctx->is_resolution_changed = 0;

    ctx->cur_slice.nal_unit_type = ctx->cur_nal->nal_unit_type;
    ctx->cur_slice.nal_ref_idc = ctx->cur_nal->nal_ref_idc;

    if (slice_check(ctx) != H264_OK) {
        return H264_ERR;
    }

    ret = process_slice_header_first_part(ctx);

    if (ret != H264_OK) {
        dprint(PRN_WARN, "sliceheader dec err\n");
        return H264_ERR;
    }

    *is_new_pic = (ctx->cur_slice.new_pic_flag == 1) ? 1 : 0;

    /* 判断该slice的图象尺寸是否较之前发生了变化 */
    pps = (ctx->pps[ctx->cur_slice.pic_parameter_set_id]);
    sps = (ctx->sps[pps->seq_parameter_set_id]);

    oldw = ctx->cur_width_in_mb;
    oldh = ctx->cur_height_in_mb;
    neww = (td_s32)(sps->pic_width_in_mbs_minus1 + 1);
    vfmw_check_ret(sps->pic_height_in_map_units_minus1 + 1 < VFMW_INT16_MAX, H264_ERR);
    vfmw_check_ret(2 - sps->frame_mbs_only_flag < VFMW_INT16_MAX, H264_ERR);
    newh = (td_s32)((sps->pic_height_in_map_units_minus1 + 1) * (2 - sps->frame_mbs_only_flag));

    if (h264_check_stm_len(ctx, 64) != H264_OK) {
        return H264_ERR;
    }

    if ((oldw != neww) || (oldh != newh)) {
        *is_size_changed = 1;
        ctx->is_resolution_changed = 1;
    }

    if (sps->dpb_size_plus1 > (ctx->dpb.size + 1)) {
        *is_size_changed = 1;
    }

    return H264_OK;
}

STATIC td_s32 have_slice_to_dec(const h264_ctx *ctx)
{
    if (ctx->slice_para_num > 0) {
        return H264_OK;
    } else {
        return H264_ERR;
    }
}

/* 检查参考列表是否有错误 */
STATIC td_s32 is_ref_list_wrong(const h264_ctx *ctx)
{
    td_u32 i;
    td_s32 ret = 0;
    fsp_logic *logic_fs = TD_NULL;

    /* 检查list 0 */
    if (ctx->cur_slice.listx_size[0] > 0) {
        for (i = 0; i < ctx->cur_slice.listx_size[0]; i++) {
            logic_fs = fsp_get_logic(ctx->chan_id, ctx->listx[0][i]->frame_store->logic_fs_id);

            if (logic_fs == TD_NULL) {
                ret = 1;
                break;
            }
        }
    }

    /* 若list 0检查无误，而且list1非空，继续检查list 1 */
    if ((ret == 0) && (ctx->cur_slice.listx_size[0] > 0)) {
        for (i = 0; i < ctx->cur_slice.listx_size[1]; i++) {
            logic_fs = fsp_get_logic(ctx->chan_id, ctx->listx[1][i]->frame_store->logic_fs_id);
            if (logic_fs == TD_NULL) {
                ret = 1;
                break;
            }
        }
    }

    ret = (ret != 0) ? H264_ERR : H264_OK;

    return ret;
}

STATIC td_void h264_process_pts(h264_ctx *ctx, const stm_seg_info *seg)
{
    vfmw_check(ctx != TD_NULL);
    vfmw_check(seg != TD_NULL);
    vfmw_check(ctx->extra_data != TD_NULL);

    if (seg->pts != VFMW_INVALID_PTS) {
        ctx->extra_data->pts = seg->pts;
    }

    ctx->extra_data->user_tag = seg->user_tag;
    ctx->extra_data->disp_time = seg->disp_time;
    ctx->extra_data->disp_enable_flag = seg->disp_enable_flag;
    ctx->extra_data->disp_frame_distance = seg->disp_frame_distance;
    ctx->extra_data->distance_before_1st_frame = seg->distance_beforce_first_frame;
    ctx->extra_data->gop_num = seg->gop_num;
    ctx->extra_data->dv_dual_layers = seg->dv_dual_layer;
}

STATIC td_s32 h264_alloc_ctx_mem(td_s32 chan_id, h264_ctx **ctx)
{
    h264_ctx *tmp_ctx = TD_NULL;
    mem_record mem = { 0 };

    mem.mem_name = "h264_ctx";
    mem.length = vfmw_align_up((td_u32)sizeof(h264_ctx), VFMW_CTX_ALIGN);
    mem.is_cached = 0;
    mem.type = CACHE_MEM_TYPE;
    mem.unid = (td_u32)chan_id;
    os_malloc(&mem); /* "H264_CTX" */

    tmp_ctx = (h264_ctx *)(mem.vir_addr);
    if (tmp_ctx == TD_NULL) {
        dprint(PRN_ERROR, "OS_ALLOC_VIR_MEM h264_ctx fail %d\n", __LINE__);
        return H264_ERR;
    }
    vfmw_check_sec_func(memset_s(tmp_ctx, sizeof(h264_ctx), 0, sizeof(h264_ctx)));
    tmp_ctx->ctx_mem_size = (td_u32)sizeof(h264_ctx);

    *ctx = tmp_ctx;

    return H264_OK;
}

STATIC td_void h264_config_max_param(syn_cfg_info *extra_data)
{
    if (extra_data->max_width < 0) {
        extra_data->max_width = H264_MIN_PIX_WIDTH;
    } else if ((extra_data->max_width == 0) || (extra_data->max_width > H264_MAX_PIX_WIDTH)) {
        extra_data->max_width = H264_MAX_PIX_WIDTH;
    }

    if (extra_data->max_height < 0) {
        extra_data->max_height = H264_MIN_PIX_HEIGHT;
    } else if ((extra_data->max_height == 0) || (extra_data->max_height > H264_MAX_PIX_HEIGHT)) {
        extra_data->max_height = H264_MAX_PIX_HEIGHT;
    }

    extra_data->max_slice_num = ((extra_data->max_slice_num <= 0) ||
        (extra_data->max_slice_num > H264_MAX_SLICE_NUM)) ? H264_MAX_SLICE_NUM : extra_data->max_slice_num;
    extra_data->max_sps_num = ((extra_data->max_sps_num <= 0) ||
        (extra_data->max_sps_num > H264_MAX_SPS_NUM)) ? H264_MAX_SPS_NUM : extra_data->max_sps_num;
    extra_data->max_pps_num = ((extra_data->max_pps_num <= 0) ||
        (extra_data->max_pps_num > H264_MAX_PPS_NUM)) ? H264_MAX_PPS_NUM : extra_data->max_pps_num;
    extra_data->max_ref_num = ((extra_data->max_ref_num <= 0) ||
        (extra_data->max_ref_num > H264_MAX_NUM_REF_PICS)) ? H264_MAX_NUM_REF_PICS : extra_data->max_ref_num;
}

#if defined(VFMW_H264_SLC_LDY_SUPPORT)
STATIC td_void h264_init_ldy_info(h264_ctx *ctx)
{
    vfmw_check(ctx != TD_NULL);
    vfmw_check(ctx->extra_data != TD_NULL);

    ctx->half_nal_try = 0;
    ctx->half_nal_succ = 0;
    ctx->post_pic_count = 0;
    ctx->ldy_cfg = LDY_IDLE;
    if (ctx->extra_data->slc_ldy_en) {
        ctx->ldy_cfg = LDY_SLC;
    } else if (ctx->extra_data->sed_only_en) {
        ctx->ldy_cfg = LDY_SED;
    }

    ctx->ldy_mode = ctx->ldy_cfg;
    ctx->half_nal_en = (ctx->ldy_cfg != LDY_IDLE);
}
#endif

td_s32 h264_init(td_s32 chan_id, td_void **ctx, const syn_cfg_info *extra_data)
{
    h264_ctx *tmp_ctx = TD_NULL;

    vfmw_check_ret(ctx != TD_NULL, SYN_ERR);
    vfmw_check_ret(extra_data != TD_NULL, SYN_ERR);
    vfmw_check_ret(h264_alloc_ctx_mem(chan_id, (h264_ctx **)ctx) == H264_OK, SYN_ERR);

    tmp_ctx = (h264_ctx *)(*ctx);
    tmp_ctx->chan_id = chan_id;

    vfmw_check_sec_func(memcpy_s(&tmp_ctx->extra_cfg, sizeof(tmp_ctx->extra_cfg), extra_data, sizeof(syn_cfg_info)));
    tmp_ctx->extra_data = &tmp_ctx->extra_cfg;
    tmp_ctx->is_sec = tmp_ctx->extra_data->sec_flag;
    tmp_ctx->bs.ops.is_sec = tmp_ctx->is_sec;
    h264_config_max_param(tmp_ctx->extra_data);
    tmp_ctx->max_sps_num = tmp_ctx->extra_data->max_sps_num;
    tmp_ctx->max_pps_num = tmp_ctx->extra_data->max_pps_num;
    tmp_ctx->max_slice_num = tmp_ctx->extra_data->max_slice_num;

    tmp_ctx->cur_pps.pic_parameter_set_id = (td_u32)tmp_ctx->max_pps_num;
    tmp_ctx->cur_sps.seq_parameter_set_id = (td_u32)tmp_ctx->max_sps_num;

    tmp_ctx->start_mode = 0;
    tmp_ctx->total_nal = 0;
    tmp_ctx->total_sps = 0;
    tmp_ctx->total_pps = 0;
    tmp_ctx->total_slice = 0;
    tmp_ctx->seq_cnt = 0;
    tmp_ctx->total_pic_num = 0;
    tmp_ctx->total_frame_num = 0;
    tmp_ctx->total_field_num = 0;
    tmp_ctx->total_fs_num = MAX_FRAME_STORE; /* change while g_DPB.size change */
    tmp_ctx->total_pmv_num = MAX_PMV_STORE;  /* change while g_DPB.size change */
    tmp_ctx->have_nal_not_decoded = 0;
    tmp_ctx->mutex_hold = 0;
    tmp_ctx->total_usrdat_num = 0;

    tmp_ctx->allow_start_dec = 0;
    tmp_ctx->idr_found = 0;
    tmp_ctx->first_frame_displayed = FIRST_DISP_INIT;
    tmp_ctx->ppic_found = 0;
    tmp_ctx->parse_frame_rate = 0;

    vfmw_check_ret(tmp_ctx->chan_id != -1, H264_ERR);
    clear_all(tmp_ctx, 0);
#if defined(VFMW_H264_SLC_LDY_SUPPORT)
    h264_init_ldy_info(tmp_ctx);
#endif

    return SYN_OK;
}

td_s32 h264_reset(td_void *ctx, td_void *args)
{
    td_s32 cnt;
    h264_ctx *tmp_ctx = (h264_ctx *)ctx;
    vfmw_chan_reset_option *rst_opt = (vfmw_chan_reset_option *)args;

    vfmw_check_ret(tmp_ctx != TD_NULL && tmp_ctx->extra_data != TD_NULL, H264_ERR);
    tmp_ctx->start_mode = 0;

    if (rst_opt == TD_NULL || rst_opt->keep_sps_pps == 0) {
        for (cnt = 0; cnt < H264_MEM_NUM; cnt++) {
            if (tmp_ctx->mem_list[cnt].mem_addr != TD_NULL) {
                vfmw_check_sec_func(memset_s(tmp_ctx->mem_list[cnt].mem_addr,
                                             tmp_ctx->mem_list[cnt].mem_size, 0, tmp_ctx->mem_list[cnt].mem_size));
            }
        }
    }

    tmp_ctx->cur_pps.pic_parameter_set_id = (td_u32)tmp_ctx->max_pps_num;
    tmp_ctx->cur_sps.seq_parameter_set_id = (td_u32)tmp_ctx->max_sps_num;

    tmp_ctx->total_nal = 0;
    tmp_ctx->total_sps = 0;
    tmp_ctx->total_pps = 0;
    tmp_ctx->total_slice = 0;
    tmp_ctx->seq_cnt = 0;
    tmp_ctx->total_pic_num = 0;
    tmp_ctx->total_frame_num = 0;
    tmp_ctx->total_field_num = 0;
    tmp_ctx->total_fs_num = MAX_FRAME_STORE; /* change while g_DPB.size change */
    tmp_ctx->total_pmv_num = MAX_PMV_STORE;  /* change while g_DPB.size change */
    tmp_ctx->have_nal_not_decoded = 0;
    tmp_ctx->mutex_hold = 0;
#ifdef H264_USRDATA_SUPPORT
    tmp_ctx->total_usrdat_num = 0;
    tmp_ctx->usr_dat_array[0] = TD_NULL;
    tmp_ctx->usr_dat_array[1] = TD_NULL;
    tmp_ctx->usr_dat_array[2] = TD_NULL;
    tmp_ctx->usr_dat_array[3] = TD_NULL;
#endif
    tmp_ctx->allow_start_dec = 0;
    tmp_ctx->idr_found = 0;
    tmp_ctx->first_frame_displayed = FIRST_DISP_INIT;
    tmp_ctx->ppic_found = 0;
    tmp_ctx->parse_frame_rate = 0;

    clear_all(tmp_ctx, 0);
#if defined(VFMW_H264_SLC_LDY_SUPPORT)
    h264_init_ldy_info(tmp_ctx);
#endif

    return SYN_OK;
}

td_s32 h264_config(td_void *ctx, const syn_cfg_info *extra_data)
{
    h264_ctx *tmp_ctx = (h264_ctx *)ctx;

    vfmw_check_ret(ctx != TD_NULL, SYN_ERR);
    vfmw_check_ret(extra_data != TD_NULL, SYN_ERR);

    vfmw_check_sec_func(memcpy_s(&tmp_ctx->extra_cfg, sizeof(tmp_ctx->extra_cfg), extra_data, sizeof(syn_cfg_info)));
    h264_config_max_param(&tmp_ctx->extra_cfg);
    tmp_ctx->max_sps_num = (tmp_ctx->extra_data->max_sps_num == 0) ?
                           H264_MAX_SPS_NUM : tmp_ctx->extra_data->max_sps_num;
    tmp_ctx->max_pps_num = (tmp_ctx->extra_data->max_pps_num == 0) ?
                           H264_MAX_PPS_NUM : tmp_ctx->extra_data->max_pps_num;
    tmp_ctx->max_slice_num = (tmp_ctx->extra_data->max_slice_num == 0) ?
                             H264_MAX_SLICE_NUM : tmp_ctx->extra_data->max_slice_num;

#ifdef VFMW_H264_EXT_SLC_NUM
    tmp_ctx->max_slice_num = vfmw_max(tmp_ctx->max_slice_num, pdt_get_product_info()->h264_ext_slc_num);
    tmp_ctx->max_slice_num = vfmw_min(tmp_ctx->max_slice_num, VFMW_H264_EXT_SLC_NUM);
#endif

    return SYN_OK;
}

/*************************************************************************
*   函数原型 :  h264_deinit(h264_ctx *ctx)
*   功能描述 :  销毁h264解码器
*   参数说明 :  无
*   返回值   ： 无
*************************************************************************/
td_s32 h264_deinit(td_void *ctx, td_void *extra_data)
{
    td_u32 i;
#ifdef H264_USRDATA_SUPPORT
    td_u32 usrdat_idx = 0;
#endif
    h264_ctx *tmp_ctx = (h264_ctx *)ctx;
    fsp_logic *logic_fs = TD_NULL;
    td_s32 cnt;
    mem_record mem = { 0 };

    vfmw_unused(extra_data);
    vfmw_check_ret(tmp_ctx != TD_NULL, SYN_ERR);
    if (tmp_ctx->mutex_hold == 1) {
        tmp_ctx->mutex_hold = 0;
    }

    for (cnt = 0; cnt < H264_MEM_NUM; cnt++) {
        if (tmp_ctx->mem_list[cnt].mem_addr != TD_NULL) {
            mem.vir_addr = (td_u8 *)(tmp_ctx->mem_list[cnt].mem_addr);
            mem.is_cached = 0;
            os_free(&mem);
            tmp_ctx->mem_list[cnt].mem_addr = TD_NULL;
        }
    }
#ifdef H264_USRDATA_SUPPORT
    for (i = 0; i < tmp_ctx->total_usrdat_num; i++) {
        if (tmp_ctx->usr_dat_array[i] != TD_NULL) {
            usd_release(tmp_ctx->chan_id, tmp_ctx->usr_dat_array[i]);
            tmp_ctx->usr_dat_array[i] = TD_NULL;
        }
    }
#endif

    for (i = 0; i < MAX_FRAME_STORE; i++) {
        logic_fs = fsp_get_logic(tmp_ctx->chan_id, tmp_ctx->frame_store[i].logic_fs_id);
        if (logic_fs == TD_NULL) {
            continue;
        }
#ifdef H264_USRDATA_SUPPORT
        for (usrdat_idx = 0; usrdat_idx < 4; usrdat_idx++) {
            if (uint64_ptr(logic_fs->image.usrdat[usrdat_idx]) != TD_NULL) {
                usd_release(tmp_ctx->chan_id, uint64_ptr(logic_fs->image.usrdat[usrdat_idx]));
                logic_fs->image.usrdat[usrdat_idx] = ptr_uint64(TD_NULL);
            }
        }
#endif
    }
    mem.vir_addr = (td_u8 *)(tmp_ctx);
    mem.is_cached = 0;
    os_free(&mem);

    return SYN_OK;
}

STATIC td_s32 h264_check_stm_len(const h264_ctx *ctx, td_s32 bit_len)
{
#if defined(VFMW_H264_SLC_LDY_SUPPORT)
    vfmw_check_ret(ctx != TD_NULL, H264_ERR);
    if (ctx->half_nal_try == TRY_HALF) {
        td_s32 width_mb;
        td_s32 height_mb;
        td_s32 cur_bits_len;
        td_s32 min_bits_len;
        h264_sps *sps = TD_NULL;
        h264_pps *pps = TD_NULL;

        pps = (ctx->pps[ctx->cur_slice.pic_parameter_set_id]);
        sps = (ctx->sps[pps->seq_parameter_set_id]);

        width_mb = (td_s32)(sps->pic_width_in_mbs_minus1 + 1);
        vfmw_check_ret((sps->pic_height_in_map_units_minus1 + 1) < VFMW_INT16_MAX, H264_ERR);
        vfmw_check_ret((2 - sps->frame_mbs_only_flag) < VFMW_INT16_MAX, H264_ERR);
        height_mb = (td_s32)((sps->pic_height_in_map_units_minus1 + 1) * (2 - sps->frame_mbs_only_flag));

        cur_bits_len = bs_residual_bits(&ctx->bs);
        vfmw_check_ret(width_mb < VFMW_INT16_MAX, H264_ERR);
        vfmw_check_ret(height_mb < VFMW_INT16_MAX, H264_ERR);
        min_bits_len = (width_mb * height_mb) / (ctx->slice_para_num + 1) + bit_len;

        if ((cur_bits_len < 0) || (cur_bits_len < min_bits_len)) {
            return H264_ERR;
        }
    }
#else
    vfmw_unused(ctx);
    vfmw_unused(bit_len);
#endif

    return H264_OK;
}

#if defined(VFMW_H264_SLC_LDY_SUPPORT)
STATIC td_s32 h264_sed_ldy_succ(h264_ctx *ctx);

STATIC td_s32 h264_is_need_sed_only(const h264_ctx *ctx)
{
    vfmw_check_ret(ctx != TD_NULL, H264_ERR);
    vfmw_check_ret(ctx->extra_data != TD_NULL, H264_ERR);

    if (ctx->slice_para_num == 0 ||
        ctx->try_slice_num == ctx->slice_para_num) {
        return 0;
    }

    if (ctx->cur_dps != TD_NULL && ctx->cur_dps->ldy_mode == LDY_SLC &&
        ctx->slc_ldy_cnt >= H264_MAX_LDY_NUM) {
        return 0;
    }

    if (ctx->half_nal_try == TRY_HALF && ctx->cur_nal != TD_NULL) {
        return 1;
    }

    if (ctx->half_nal_try == TRY_IDLE &&
        stm_control(ctx->chan_id, STM_CMD_GET_SEG_NUM, TD_NULL) <= 0) {
        return 1;
    }

    if (ctx->extra_data->crc_mode != 0) {
        if (os_get_rand() % 3 == 0) {
            return 1;
        }
    }

    return 0;
}

STATIC td_s32 h264_dec_set_dec_param(h264_ctx *ctx)
{
    td_s32 chan_id = ctx->chan_id;

    if (ctx->half_nal_en == 0) {
        return H264_ERR;
    }

    if (h264_is_need_sed_only(ctx) == 1) {
        if (dec_check(ctx) != H264_OK) {
            goto out;
        }

        ctx->cur_dps->sed_state = SED_IDLE;
        ctx->cur_dps->ldy_mode = (td_s32)ctx->ldy_mode;
        ctx->try_slice_num = ctx->slice_para_num;

        if (ctx->ldy_mode == LDY_SED) {
            ctx->cur_dps->sed_state = SED_TRY;
            if (ctx->half_nal_try == TRY_IDLE) {
                ctx->half_nal_try = TRY_NORM;
            } else {
                ctx->half_nal_try = TRY_SLICE;
                ctx->have_nal_not_decoded = 1;
            }
        }

        if (write_pic_msg(ctx) != H264_OK) {
            goto out;
        }

        return SYN_PARSE_PRE_FRMDONE;
    } else if (ctx->half_nal_try == TRY_HALF) {
        if (ctx->slice_para_num > 0) {
            ctx->slice_para_num--;
        }
        clear_curr_nal(ctx);
        dps_delete_slice(chan_id, ctx, ctx->cur_dps);
    }

out:
    ctx->half_nal_try = TRY_IDLE;
    ctx->have_nal_not_decoded = 0;

    return H264_ERR;
}

STATIC td_s32 h264_dec_is_slice_unit(td_u32 NalType)
{
    if (NalType == NALU_TYPE_IDR || NalType == NALU_TYPE_SLICE || NalType == NALU_TYPE_SLC_EXT) {
        return 1;
    }

    return 0;
}

STATIC td_s32 h264_dec_is_frame_end_nal(td_u32 NalType)
{
    return ((NalType == NALU_TYPE_SPS) || (NalType == NALU_TYPE_SUB_SPS) || (NalType == NALU_TYPE_PPS) ||
            (NalType == NALU_TYPE_EOPIC) || (NalType == NALU_TYPE_EOSTREAM));
}

STATIC td_s32 h264_dec_check_sed_only_bs(h264_ctx *ctx)
{
    td_u8 *bs_buffer = TD_NULL;
    td_u32 bs_length = 0;

    bs_buffer = ctx->cur_nal->stream[0].stream_buffer;
    bs_length = ctx->cur_nal->stream[0].bitstream_length;
    if (bs_length < 4) {
        return H264_ERR;
    }

    bs_init(&ctx->bs, bs_buffer, bs_length);

    return H264_OK;
}

STATIC td_void h264_dec_sed_only_nal_header(h264_ctx *ctx, td_u32 *next_nal_type)
{
    td_u32 svc_extension_flag = 0;
    td_u32 svc_idr_flag = 0;
    td_u32 extra_nal_header = 0;
    td_u32 no_inter_pred;
    td_u32 quality_id;

    extra_nal_header = (td_u32)bs_get(&ctx->bs, 24);
    svc_extension_flag = (extra_nal_header >> 23) & 0x1;
    if (svc_extension_flag == 1) {
        svc_idr_flag = (extra_nal_header >> 22) & 0x1;
        no_inter_pred = (extra_nal_header >> 15) & 0x1;
        quality_id = (extra_nal_header >> 8) & 0xf;
        if (quality_id != 0 || no_inter_pred != 1) {
            /* Allegro_FUNNY_CAVLC_B01_L20_5x6_10.2.26l err slc_ext in f62 */
            *next_nal_type = NALU_TYPE_AUD;
            return;
        }
    }

    if (svc_idr_flag == 1) {
        *next_nal_type = NALU_TYPE_IDR;
    }

    return;
}

STATIC td_s32 h264_dec_sed_only_slice_flag(h264_ctx *ctx, td_u32 next_nal_type)
{
    td_u32 first_slice_flag = 0;
    td_s32 ret;

    if (h264_dec_is_slice_unit(next_nal_type) == 1
        && bs_residual_bits(&ctx->bs) > 0) {
        first_slice_flag = (td_u32)bs_get(&ctx->bs, 1);
    }

    if ((ctx->slice_para_num > 0)
        &&  (h264_dec_is_frame_end_nal(next_nal_type) == 1 || first_slice_flag)) {
        ctx->half_nal_succ = 0;
        ctx->have_nal_not_decoded = 1;

        ret = h264_sed_ldy_succ(ctx);
        if (ret == H264_OK) {
            return SYN_PARSE_PRE_FRMDONE;
        }
    } else if (h264_dec_is_slice_unit(next_nal_type) != 1) {
        return H264_ERR;
    }

    /* slc ldy do not try half */
    if (ctx->ldy_mode == LDY_SLC) {
        return H264_ERR;
    }

    ctx->half_nal_succ = 0;
    ctx->half_nal_try = TRY_HALF;
    return H264_OK;
}

STATIC td_s32 h264_dec_check_sed_only(h264_ctx *ctx)
{
    td_s32 ret;
    td_u32 next_nal_type = 0;

    td_s32 chan_id = ctx->chan_id;

    ctx->half_nal_try = TRY_IDLE;

    if (ctx->seg_cut_state == SEG_CUT_HALF) {
        if (ctx->half_nal_en == 0 ||
            /* slice end may have invalid bytes */
            ctx->half_nal_succ == 1) {
            return H264_ERR;
        }

        if (ctx->extra_data->crc_mode == 0 &&
            stm_control(chan_id, STM_CMD_GET_SEG_NUM, TD_NULL) > 0) {
            return H264_ERR;
        } else {

            ret = h264_dec_check_sed_only_bs(ctx);
            if (ret != H264_OK) {
                return ret;
            }

            bs_skip(&ctx->bs, 24);

            if (bs_residual_bits(&ctx->bs) < 8) {
                return H264_ERR;
            }

            bs_skip(&ctx->bs, 3);
            next_nal_type = (td_u32)bs_get(&ctx->bs, 5);
            if (next_nal_type == NALU_TYPE_SLC_EXT && bs_residual_bits(&ctx->bs) < 24) {
                return H264_ERR;
            }
            if (next_nal_type == NALU_TYPE_SLC_EXT && bs_residual_bits(&ctx->bs) >= 24) {
                h264_dec_sed_only_nal_header(ctx, &next_nal_type);
            }

            ret = h264_dec_sed_only_slice_flag(ctx, next_nal_type);
            if (ret != H264_OK) {
                dprint(PRN_WARN, "%s, h264_dec_sed_only_slice_flag failed\n", __func__);
                return ret;
            }
        }
    } else if (ctx->seg_cut_state == SEG_CUT_REPEAT) {
        if (ctx->half_nal_succ == 1) {
            ctx->half_nal_succ = 0;
            return H264_ERR;
        }
    }

    return H264_OK;
}
#endif

#ifdef VFMW_H264_SLC_LDY_SUPPORT
STATIC td_s32 h264_sed_ldy_succ(h264_ctx *ctx)
{
    td_s32 ret = SYN_ERR;

    vfmw_check_ret(ctx != TD_NULL, H264_ERR);
    vfmw_check_ret(ctx->cur_dps != TD_NULL, H264_ERR);

    ret = dec_check(ctx);
    if (ret != H264_OK) {
        dprint(PRN_ERROR, "%s %d dec_check Failed!\n", __func__, __LINE__);
        return H264_ERR;
    }

    ctx->cur_dps->sed_state = SED_IDLE;
    ctx->half_nal_try = TRY_IDLE;
    ctx->try_slice_num = 0;

    ret = write_pic_msg(ctx);
    if (ret != H264_OK) {
        return H264_ERR;
    }

    ret = h264_dec_post_proc(ctx);
    if (ret != H264_OK) {
        return H264_ERR;
    }

    return H264_OK;
}

STATIC td_s32 h264_dec_proc_pre_nal(h264_ctx *ctx)
{
    td_s32 ret;
    td_s32 chan_id;

    vfmw_check_ret(ctx != TD_NULL, H264_ERR);
    chan_id = ctx->chan_id;
    vfmw_check_ret(ctx->cur_dps != TD_NULL, H264_ERR);

    if (ctx->half_nal_try == TRY_SLICE) {
        /* prev half slice sed try succ */
        if (ctx->cur_dps->sed_state == SED_SUCC) {
            ctx->half_nal_succ = 1;
            ret = h264_dec_post_proc(ctx);
            if (ret != H264_OK) {
                return H264_ERR;
            }

            ctx->cur_dps->sed_state = SED_IDLE;
            ctx->half_nal_try = TRY_IDLE;
            ctx->try_slice_num = 0;

            return SYN_PARSE_NOT_FRMDONE;
        }

        if (ctx->try_slice_num > 0) {
            ctx->try_slice_num--;
        }
        if (ctx->slice_para_num > 0) {
            ctx->slice_para_num--;
        }
        clear_curr_nal(ctx);
        dps_delete_slice(chan_id, ctx, ctx->cur_dps);

        return SYN_PARSE_NOT_FRMDONE;
    } else if (ctx->half_nal_try == TRY_NORM) {
        /* sed try succ */
        if (ctx->cur_dps->sed_state == SED_SUCC) {
            ret = h264_dec_post_proc(ctx);
            if (ret != H264_OK) {
                return H264_ERR;
            }

            ctx->cur_dps->sed_state = SED_IDLE;
            ctx->half_nal_try = TRY_IDLE;
            ctx->try_slice_num = 0;
        }

        return SYN_PARSE_NOT_FRMDONE;
    }

    return H264_OK;
}
#endif

STATIC td_void h264_report_fake_frame(h264_ctx *ctx)
{
    if ((ctx->total_pic_num != 0) &&
        (ctx->is_resolution_changed == 1) &&
        ((td_u32)(ctx->cur_width_in_mb) != ctx->fake_frm_width_in_mb ||
         (td_u32)(ctx->cur_height_in_mb) != ctx->fake_frm_height_in_mb)) {
        ctx->fake_frm_width_in_mb = (td_u32)ctx->cur_width_in_mb;
        ctx->fake_frm_height_in_mb = (td_u32)ctx->cur_height_in_mb;
        report_fake_frame(ctx->chan_id);
    }
}

STATIC td_s32 h264_dec_decode_packet_naul(h264_ctx *ctx, td_u8 *eos_flag, td_u8 *nal_unit_type)
{
    td_u8 nal_header;

    vfmw_unused(eos_flag);
    if (ctx->cur_nal == TD_NULL) {
        dprint(PRN_ERROR, "%s:%d cur_nal is null!\n", __func__, __LINE__);
        return H264_ERR;
    }

    /* 取一个NAL头部，判断是否new pic */
        ctx->cur_nal->nal_bitoffset = 0;
        bs_init(&(ctx->bs), (td_u8 *)((ctx->cur_nal->stream[0].stream_buffer)),
                (td_s32)ctx->cur_nal->stream[0].bitstream_length);

        nal_header = (td_u8)bs_get(&(ctx->bs), 32); /* 取第一个32bit, 0x00 0x00 0x01 0xxx */
        ctx->cur_nal->nal_unit_type = *nal_unit_type = (td_u8)(nal_header & 0x1F);
        ctx->cur_nal->nal_ref_idc = (td_u32)((nal_header >> 5) & 3);

    return H264_OK;
}

STATIC td_s32 h264_dec_decode_packet_add_svc(h264_ctx *ctx)
{
    td_s32 is_new_pic = 0;
    td_s32 is_size_changed = 0;
    td_u8 flag;
    td_s32 ret;

    if (inquire_slice_property(ctx, &is_new_pic, &is_size_changed) != H264_OK) {
        dprint(PRN_WARN, "H264DEC inquire_slice_property error.\n");
        clear_curr_nal(ctx);
        report_decsyntax_err(ctx->chan_id);
        return H264_ERR;
    }

    ctx->pic_counter += (td_u32)(is_new_pic == 1); /* 32位数30fps可播4.4年，因此不考虑32位溢出问题 */

    if (is_new_pic == 1) {
        flag = (ctx->cur_slice.field_pic_flag == 1);
        ctx->new_pic_counter = (flag != 0) ? (ctx->new_pic_counter + 1) : (ctx->new_pic_counter + 2);
    }

    flag = ((is_new_pic == 1) && (have_slice_to_dec(ctx) == H264_OK));
    if (flag != 0) { /* 如果还有Slice未解码，说明还未遇到NALU_TYPE_EOPIC，就主动插入一个NALU_TYPE_EOPIC */
        ctx->cur_nal->nal_bitoffset = 0;
        ctx->is_insert_eopic_nal = 1;
        ret = h264_dec_one_nal(ctx, 1);

        /* dec fail may clear have flag */
        ctx->have_nal_not_decoded = 1;
        if (ret == SYN_PARSE_ERR) {
            return SYN_PARSE_PRE_FRMERR;
        }

        flag = (ctx->cur_slice.field_pic_flag == 1);
        ctx->new_pic_counter = (flag != 0) ? (ctx->new_pic_counter - 1) : (ctx->new_pic_counter - 2);

        return SYN_PARSE_PRE_FRMDONE;
    }

    /*  CNcomment: 如果图象尺寸在此slice发生了变化，应该等之前已解码的图象全部进入DNR后，再继续新尺寸图象的解码，否则
        老尺寸的图象可能会有较多的丢弃，导致播放效果不佳。2011-11-1 */
    if (is_size_changed == 1) {
        ret = flush_dpb(ctx);

        if (ret != H264_OK) {
            dprint(PRN_ERROR, "line %d: flush dpb return %d\n", __LINE__, ret);
        }
        /* inquire voque new image num */
        if (fsp_get_frame_buf_mode(ctx->chan_id) == VFMW_FRM_ALLOC_OUTER) {
            td_u32 readnum = 0;
            td_u32 newnum = 0;
            voq_get_queue_img_num(ctx->chan_id, &readnum, &newnum);
            if (newnum > 0) {
                ctx->have_nal_not_decoded = 1;
                return SYN_PARSE_RESOURCE_INVALID;
            }
        }

        h264_report_fake_frame(ctx);
    }

    return H264_OK;
}

STATIC td_s32 h264_dec_decode_packet_bs(h264_ctx *ctx, td_u8 eos_flag)
{
    td_s32 ret;
    td_u8 flag;

    /* 去拖尾零函数 */
    ret = find_trail_zeros(ctx);

    if (ret != H264_OK) {
        dprint(PRN_ERROR, "find_trail_zeros ERR\n");
        return H264_ERR;
    }

    /* 赋g_BS中的值, BS的初始化需要从word对齐的位置开始 */
    ctx->cur_nal->nal_bitoffset = 0;
    bs_init(&(ctx->bs), (td_u8 *)((ctx->cur_nal->stream[0].stream_buffer)),
            (td_s32)ctx->cur_nal->stream[0].bitstream_length);
    /* If is EOS nal, should not do bs_skip */
    ret = h264_dec_one_nal(ctx, eos_flag);

    flag = ((ret == SYN_PARSE_CURR_FRMDONE) ||
            (ret == SYN_PARSE_RESOURCE_INVALID) ||
            (ret == SYN_PARSE_ERR));
    if (flag != 0) {
        return ret;
    } else if (ret != H264_OK) {
        return SYN_PARSE_NOT_FRMDONE;
    }

    ctx->cur_pic.cur_nal_size = 0;

    return H264_OK;
}

STATIC td_s32 h264_dec_decode_packet_spelled_nalu(h264_ctx *ctx, const h264_stream_packet *packet, td_s32 *ret)
{
    td_u8 flag;

    vfmw_check_ret(packet != TD_NULL, H264_ERR);

    ctx->advanced_cfg = ((td_u32)ctx->extra_data->dec_mode << 16) & 0x70000;
    ctx->ref_error_thr = (td_u32)ctx->extra_data->ref_err_thr;
    ctx->out_error_thr = (td_u32)ctx->extra_data->out_err_thr;
    ctx->se_err_flg = 0;

    /* 接收到一个包并将其拼成一个完整的Nalu, 注意RecivePacket有三个返回值 */
    if (ctx->have_nal_not_decoded != 1) {
        flag = ((ctx->extra_data->dynamic_fs_en == 1) &&
                (ctx->extra_data->wait_fs_flag == 1) &&
                (ctx->scd_wrap_around == 1));
        if (flag != 0) {
            ctx->cur_nal = ctx->last_nal;
            ctx->scd_wrap_around = 0;
        } else {
            *ret = receive_packet(ctx, packet);
            ctx->last_nal = ctx->cur_nal;
        }
    } else {
        *ret = (ctx->cur_nal != TD_NULL) ? H264_OK : H264_ERR;
    }

    ctx->have_nal_not_decoded = 0; /* 认为该NAL一定会被用掉，除非遇到new_pic_flag时则另行处理 */

    return H264_OK;
}
#if defined(VFMW_H264_SLC_LDY_SUPPORT)
STATIC td_s32 h264_dec_decode_packet_sed_slc(h264_ctx *ctx)
{
    td_s32 ret;

    ret = h264_dec_proc_pre_nal(ctx);
    if (ret != H264_OK) {
        ctx->cur_nal = TD_NULL;
        ctx->half_nal_try = TRY_IDLE;
        return ret;
    }

    ret = h264_dec_check_sed_only(ctx);
    if (ret == SYN_PARSE_PRE_FRMDONE) {
        return SYN_PARSE_PRE_FRMDONE;
    } else if (ret != H264_OK) {
        clear_curr_nal(ctx);

        /* slc ldy do not try half */
        if (ctx->ldy_mode == LDY_SLC) {
            return H264_ERR;
        }

        ret = h264_dec_set_dec_param(ctx);
        if (ret == SYN_PARSE_PRE_FRMDONE) {
            return SYN_PARSE_PRE_FRMDONE;
        }

        return H264_ERR;
    }
    return H264_OK;
}
#endif
/*************************************************************************
*      函数原型 :  td_s32 h264_dec_decode_packet(h264_stream_packet *packet)
*      功能描述 :  获取一片码流，并判断该片码流是否是完整的nal
*      参数说明 :  无
*      返回值   ： 无
*************************************************************************/
STATIC td_s32 h264_dec_decode_packet(h264_ctx *ctx, h264_stream_packet *packet)
{
    td_s32 ret = 0;
    td_u8 nal_unit_type = 0;
    td_u8 eos_flag = 0;
    td_u8 flag = 0;
    td_s32 ret_1;

    ret_1 = h264_dec_decode_packet_spelled_nalu(ctx, packet, &ret);
    vfmw_check_ret(ret_1 == H264_OK, ret_1);

    /* 收到一个完整的Nalu开始解码 */
    if (ret == H264_OK) {
#if defined(VFMW_H264_SLC_LDY_SUPPORT)
    ret = h264_dec_decode_packet_sed_slc(ctx);
    if (ret != H264_OK) {
        return ret;
    }
#endif

    ret = h264_dec_decode_packet_naul(ctx, &eos_flag, &nal_unit_type);
    if (ret != H264_OK) {
        return ret;
    }

    flag = ((nal_unit_type == NALU_TYPE_SLICE) ||
            (nal_unit_type == NALU_TYPE_IDR) ||
            (nal_unit_type == NALU_TYPE_SLC_EXT));
    if (flag != 0) { /* add svc */
        if (nal_unit_type == NALU_TYPE_SLC_EXT) {
            inquire_slice_ext_property(ctx); /* 此处要解析svc相关的24个bit，两个原因:解析idr 和后续码字偏移正确 */
        }

        ret = h264_dec_decode_packet_add_svc(ctx);
        if (ret != H264_OK) {
            return ret;
        }
    }

    ret = h264_dec_decode_packet_bs(ctx, eos_flag);
    if (ret != H264_OK) {
        return ret;
    }

#if defined(VFMW_H264_SLC_LDY_SUPPORT)
    if (ctx->half_nal_en == 1) {
        ret = h264_dec_set_dec_param(ctx);
        if (ret == SYN_PARSE_PRE_FRMDONE) {
            return SYN_PARSE_PRE_FRMDONE;
        }
    }
#endif
    }

    return SYN_PARSE_NOT_FRMDONE;
}

STATIC td_void h264_proc_err_nal(h264_ctx *ctx, const stm_seg_info *seg, td_s32 dec_ret)
{
    td_u32 err_hold;

    vfmw_check(ctx != TD_NULL);
    vfmw_check(seg != TD_NULL);

    /* for error stm/cfg cause seglist full */
    if (dec_ret == SYN_PARSE_PRE_FRMDONE ||
        dec_ret == SYN_PARSE_CURR_FRMDONE) {
        ctx->err_nal_cnt = 0;
    } else if (ctx->last_seg_id != (td_u32)(seg->stream_id)) {
        ctx->err_nal_cnt++;

        err_hold = pdt_get_product_info()->h264_ext_slc_num > 1024 ? pdt_get_product_info()->h264_ext_slc_num : 1024;

        if (ctx->slice_para_num > 0 && ctx->err_nal_cnt > err_hold) {
            dprint(PRN_ERROR, "H264 ErrNalCnt(%d) ClearCurPic\n", ctx->err_nal_cnt);
            ctx->err_nal_cnt = 0;
            h264_clear_curr_pic(ctx);
        }
    }

    ctx->last_seg_id = (td_u32)seg->stream_id;
}

td_s32 h264_get_parse_info(const td_void *ctx_ext, td_void *args_ext)
{
    h264_ctx *ctx = (h264_ctx *)ctx_ext;
    syn_syntax_info *info = (syn_syntax_info *)args_ext;

    vfmw_check_ret(info != TD_NULL, SYN_ERR);
    vfmw_check_ret(ctx != TD_NULL, SYN_ERR);

    info->profile = ctx->profile;
    info->level = ctx->level;
    info->spec_invalid = ctx->spec_invalid;
    info->spec_value = ctx->spec_value;

    return SYN_OK;
}

syn_parse_state h264_decode(stm_seg_info *seg, dps_info *dps, td_void *arg_ctx)
{
    td_s32 decode_result = 0;
    syn_parse_state ret = SYN_PARSE_RESOURCE_INVALID;
    h264_ctx *ctx = (h264_ctx *)arg_ctx;
    h264_stream_packet h264_packet;

    vfmw_check_ret((seg != TD_NULL && dps != TD_NULL && arg_ctx != TD_NULL), SYN_PARSE_RESOURCE_INVALID);

    h264_packet.stream = seg->vir_addr;
    h264_packet.strm_phy_addr = seg->phy_addr;
    h264_packet.stream_len = (td_s32)seg->length;
    h264_packet.last_pack_in_nal = seg->is_last_seg;
    h264_packet.stream_id = seg->stream_id;
    h264_packet.pack_is_pic_over_flag = seg->dsp_check_pic_over_flag;
    h264_packet.raw_stream_len = (td_s32)seg->raw_len;
    h264_packet.seg_cut_state = seg->cut_result;

    h264_process_pts(ctx, seg);

    ctx->cur_dps = dps;

    decode_result = h264_dec_decode_packet(ctx, &h264_packet);

    if (decode_result == H264_ERR) {
        ret = SYN_PARSE_NOT_FRMDONE;
    } else {
        ret = decode_result;
    }

    if (ctx->is_stream_end_flag == 1) {
        proc_single_field(ctx);
        h264_dec_get_remain_img(ctx);
        voq_last_frm(ctx->chan_id);
        ctx->is_stream_end_flag = 0;
    }

    h264_proc_err_nal(ctx, seg, ret);

    return ret;
}

/*************************************************************************
*   函数原型 :  td_void h264_dec_recycle_image(h264_ctx *ctx, td_u32 pic_id)
*   功能描述 :  h264回收VO已经显示的图像槽位
*   参数说明 :
*   返回值   ：
*************************************************************************/
STATIC td_s32 h264_dec_recycle_image(const h264_ctx *ctx, td_u32 pic_id)
{
#ifdef H264_USRDATA_SUPPORT
    td_u32 i = 0;
#endif
    fsp_logic *logic_fs = TD_NULL;

    /* 找到PicID所对应的逻辑帧存，从而找到image结构体，这样才能释放image所附带的userdata */
    logic_fs = fsp_get_logic(ctx->chan_id, (td_s32)pic_id);

    if (logic_fs != TD_NULL) {
        /* 检查这个逻辑帧存是否正在等待显示释放 */
        if (logic_fs->disp_state == FSP_DISP_STATE_IDLE) {
            return H264_ERR;
        }
#ifdef H264_USRDATA_SUPPORT
        /* 释放userdata */
        for (i = 0; i < 4; i++) {
            if (uint64_ptr(logic_fs->image.usrdat[i]) != TD_NULL) {
                usd_release(ctx->chan_id, uint64_ptr(logic_fs->image.usrdat[i]));
                logic_fs->image.usrdat[i] = ptr_uint64(TD_NULL);
            }
        }
#endif
        /* 将图像'pic_id'标记为"显示结束" */
        fsp_set_disp(ctx->chan_id, (td_s32)pic_id, 0);
        return H264_OK;
    }

    return H264_ERR;
}

STATIC td_s32 h264_dec_strat_mode(h264_ctx *ctx)
{
    td_s32 ret;
    luma_info *luma = TD_NULL;
    td_s32 logic_id = ctx->cur_pic.frame_store->logic_fs_id;
    fsp_logic *logic_fs = TD_NULL;

    if (ctx->start_mode == 0) {
        ctx->cur_pic.err_level = 0;

        clear_all_slice(ctx);

        ret = store_pic_in_dpb(ctx, luma);

        if (ret != H264_OK && ret != H264_ALREADY_OUT) {
            dprint(PRN_ERROR, "pH264Ctx_StorePicInDPB failed, ret = %d\n", ret);
            logic_fs = fsp_get_logic(ctx->chan_id, logic_id);
            if (logic_fs != TD_NULL) {
                voq_delete(ctx->chan_id, &logic_fs->image);
            }
            fsp_set_disp(ctx->chan_id, logic_id, 0);
            fsp_clear_logic(ctx->chan_id, logic_id, 1);
            h264_clear_curr_pic(ctx);
            dps_release_seg(ctx->chan_id, ctx->cur_dps);
            clear_dpb(ctx);
            ctx->cur_pic.frame_store->is_used = 0;
            ctx->cur_pic.frame_store->is_in_dpb = 0;
            ctx->cur_pic.frame_store = TD_NULL;
            return H264_ERR;
        }
        ctx->allow_start_dec = 1;
    } else {
        /* 统计当前Pic错误宏块比例，判断是否需要置g_CurrPic.err_level? */
        ctx->cur_pic.err_level = 0;
        clear_all_slice(ctx);

        ret = store_pic_in_dpb(ctx, luma);

        if (ret != H264_OK) {
            dprint(PRN_ERROR, "imode nal store pic err, ret = %d\n", ret);
        }

        ret = flush_dpb(ctx);

        if (ret != H264_OK) {
            dprint(PRN_ERROR, "imode nal flush dpb err, ret = %d\n", ret);
        }

        ctx->allow_start_dec = 0;
        init_old_slice(ctx);
    }
    return H264_OK;
}

STATIC td_s32 h264_dec_post_proc(h264_ctx *ctx)
{
    td_ulong lock_flag;
    td_s32 logic_id;

    vfmw_check_ret(ctx->extra_data != TD_NULL, H264_ERR);
    vfmw_check_ret(ctx->cur_dps != TD_NULL, H264_ERR);
    vfmw_check_ret(ctx->cur_pic.frame_store != TD_NULL, H264_ERR);

#if defined(VFMW_H264_SLC_LDY_SUPPORT)
    if ((ctx->post_pic_count < 2) ||
        (ctx->extra_data->dec_mode == VFMW_DEC_MODE_I) ||
        (ctx->cur_pic.pic_width_in_mb * 16 <= 1920)) {
        ctx->half_nal_en = ctx->extra_data->sed_only_en;
        ctx->half_nal_en |= ctx->extra_data->slc_ldy_en;
    } else {
        ctx->half_nal_en = 0;
    }

    if (ctx->post_pic_count < 2) {
        ctx->post_pic_count++;
    }
#endif
    if (ctx->dpb.size == 0 && ctx->cur_pic.nal_ref_idc != 0) {
        ctx->cur_pic.nal_ref_idc = 0;
    }

    if (h264_dec_strat_mode(ctx) != H264_OK) {
        return H264_ERR;
    }

    fsp_img_format_lock(&lock_flag);
    if (ctx->cur_pic.frame_store->is_used == 3) {
        fsp_set_fld_flag(ctx->chan_id, ctx->cur_pic.frame_store->logic_fs_id, 3);
    }
    fsp_img_format_unlock(&lock_flag);

    ctx->profile = ctx->cur_sps.profile_idc;
    ctx->level = ctx->cur_sps.level_idc;

#ifdef VFMW_H264_SLC_LDY_SUPPORT
    if (ctx->cur_dps->sed_state == SED_SUCC) {
        return H264_OK;
    }
#endif

    logic_id = ctx->cur_pic.frame_store->logic_fs_id;
    fsp_set_wait(ctx->chan_id, logic_id, ctx->cur_pic.structure);

    return H264_OK;
}

td_s32 h264_dec_get_image_height(const h264_ctx *ctx)
{
    td_u32 image_height = 0;
    td_u32 tmp1, tmp2;

    vfmw_check_ret(ctx != TD_NULL, 0);
    tmp1 = ctx->cur_sps.pic_height_in_map_units_minus1 + 1;
    tmp2 = 2 - ctx->cur_sps.frame_mbs_only_flag;
    vfmw_check_ret(tmp1 < VFMW_INT16_MAX, 0);
    vfmw_check_ret(tmp2 < VFMW_INT16_MAX, 0);
    image_height = tmp1 * tmp2 * 16; /* 16 mb size */

    return (td_s32)image_height;
}

td_u32 h264_get_ctx_size(const td_void *arg_ctx)
{
    const h264_ctx *ctx = (h264_ctx *)arg_ctx;
    td_u32 ret = 0;

    if (ctx == TD_NULL) {
        dprint(PRN_ERROR, "H264 is not init\n");
        return ret;
    }

    ret = (td_u32)ctx->ctx_mem_size;

    return ret;
}

td_s32 h264_get_max_size(td_void)
{
    td_u32 size = 0;

    size = vfmw_align_up((td_u32)sizeof(h264_ctx), VFMW_CTX_ALIGN);

    /* + sps pps */
    size += H264_MAX_SPS_NUM * vfmw_align_up((td_u32)sizeof(h264_sps), VFMW_CTX_ALIGN);
    size += H264_MAX_PPS_NUM * vfmw_align_up((td_u32)sizeof(h264_pps), VFMW_CTX_ALIGN);
    size = vfmw_align_up((td_u32)size, VFMW_CTX_ALIGN);

    return (td_s32)size;
}

td_void h264_read_log(const td_void *args, td_void *buf, td_s32 len, td_s32 *used)
{
    h264_ctx *ctx = (h264_ctx *)args;

    if (ctx == TD_NULL) {
        os_dump_proc(buf, len, used, 0, "syn context not exists.%-56s|\r\n", " ");
        return;
    }

    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "profile", ctx->cur_sps.profile_idc, "Level",
                 ctx->cur_sps.level_idc);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "frame_mbs_only", ctx->cur_sps.frame_mbs_only_flag, "mb_adaptive",
                 ctx->cur_sps.mb_adaptive_frame_field_flag);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "chroma_format", ctx->cur_sps.chroma_format_idc, "bit_depth_luma",
                 ctx->cur_sps.bit_depth_luma);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "dpb_size_plus1", ctx->cur_sps.dpb_size_plus1, "bit_depth_chroma",
                 ctx->cur_sps.bit_depth_chroma);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "is_stream_end", ctx->is_stream_end_flag, "dpb_size",
                 ctx->dpb.size);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "parse_frame_rate", ctx->parse_frame_rate,
                 "dpb_used_size", ctx->dpb.used_size);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "aspect_width", ctx->cur_pic.fs_image.aspect_width,
                 "aspect_height", ctx->cur_pic.fs_image.aspect_height);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "afd_format_flag", ctx->cur_pic.fs_image.afd.active_format_flag,
                 "afd_format", ctx->cur_pic.fs_image.afd.active_format);
}
#ifdef H264_PVR_SUPPORT
td_s32 h264_dec_cfg_fffb_speed(td_void *arg_ctx, const td_void *args)
{
    td_s32 ret = SYN_OK;
    const td_s32 *fffb_speed = (td_s32 *)args;
    h264_ctx *ctx = (h264_ctx *)arg_ctx;

    vfmw_check_ret(ctx != TD_NULL, SYN_ERR);
    vfmw_check_ret_print(fffb_speed != TD_NULL, SYN_ERR, "%s pFFFBSpeed is TD_NULL\n", __func__);

    ctx->extra_cfg.fast_forward_backward_speed = *fffb_speed;

    return ret;
}
td_s32 h264_dec_cfg_pvr_info(td_void *arg_ctx, const td_void *args)
{
    td_s32 ret = SYN_OK;
    h264_ctx *ctx = (h264_ctx *)arg_ctx;
    const vfmw_control_info *pvr_info = (vfmw_control_info *)args;

    vfmw_check_ret(ctx != TD_NULL, SYN_ERR);
    vfmw_check_ret_print(pvr_info != TD_NULL, SYN_ERR, "%s pPvrInfo is TD_NULL\n", __func__);

    vfmw_check_sec_func(memcpy_s(&ctx->extra_cfg.pvr_info,
                                 sizeof(vfmw_control_info), pvr_info, sizeof(*pvr_info)));

    return ret;
}
#endif
td_s32 h264_control(td_void *ctx, td_s32 cmd_id, const td_void *args)
{
    td_s32 ret = SYN_ERR;

    vfmw_unused(ctx);
    vfmw_unused(args);
    switch (cmd_id) {
#ifdef H264_PVR_SUPPORT
        case SYNCTX_CMD_CFG_FFFB_SPEED:
            ret = h264_dec_cfg_fffb_speed(ctx, args);
            break;

        case SYNCTX_CMD_CFG_PVR_INFO:
            ret = h264_dec_cfg_pvr_info(ctx, args);
            break;
#endif
        default:
            dprint(PRN_ERROR, "Unsupport cmd:%d\n", cmd_id);
            break;
    }

    return ret;
}
