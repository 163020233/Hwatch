/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#include "dec_chn.h"
#include "dec_dev.h"
#include "dec_ref.h"
#include "fsp.h"
#include "voque.h"
#include "dps.h"
#include "dec_ctrl.h"
#include "dbg.h"
#include "vfmw_define.h"
#ifdef VFMW_IPT_SUPPORT
#include "ip_test.h"
#endif
#include "vfmw_pdt.h"
#include "vdec_chr.h"

/* lock and unlock */
VFMW_ACC_BASE STATIC td_void dec_chan_lock_chan_pool(dec_chan_pool *chan_pool, td_ulong *lock_flag)
{
    os_spin_lock(chan_pool->spin_lock, lock_flag);
}

VFMW_ACC_BASE STATIC td_void dec_chan_unlock_chan_pool(dec_chan_pool *chan_pool, td_ulong *lock_flag)
{
    os_spin_unlock(chan_pool->spin_lock, lock_flag);
}

VFMW_ACC_BASE td_void dec_chan_lock_chan(td_s32 chan_id, td_ulong *lock_flag)
{
#ifdef VFMW_IPT_SUPPORT
    vfmw_unused(chan_id);
    vfmw_unused(lock_flag);
#else
    os_spin_lock(dec_ctrl_get_chan_pool()->chan_lock[chan_id], lock_flag);
#endif
}

VFMW_ACC_BASE td_void dec_chan_unlock_chan(td_s32 chan_id, td_ulong *lock_flag)
{
#ifdef VFMW_IPT_SUPPORT
    vfmw_unused(chan_id);
    vfmw_unused(lock_flag);
#else
    os_spin_unlock(dec_ctrl_get_chan_pool()->chan_lock[chan_id], lock_flag);
#endif
}

STATIC td_void dec_chan_print_block(td_s32 chan_id, const td_char *info)
{
    td_u32 cur_time;
    dec_chan *chan = dec_ctrl_get_chan(chan_id);

    (td_void)(info); /* avoid to build err when printing is not supported. */
    if (chan == TD_NULL) {
        dprint(PRN_ERROR, "Get chan %d failed!\n", chan_id);
        return;
    }

    vfmw_check(os_get_time_ms != TD_NULL);
    cur_time = os_get_time_ms();
    if (cur_time - chan->block_time < 300) { /* 300 :a number */
        return;
    }

    chan->block_time = cur_time;

    dprint(PRN_DEC_BLK, "Chan %d DEC block of %s\n", chan->chan_id, info);
}

/* init and deinit */
STATIC td_void dec_chan_init_task(dec_chan_task *task)
{
    task->dps = TD_NULL;
    task->state = DEC_TASK_IDLE;
    task->dev_id = 0;
    task->msg_id = 0;
    task->reg_id = 0;
    task->cfg_id = 0;
    task->seg_rlsd_flag = 0;

    vfmw_check_sec_func(memset_s(&task->slice_list, sizeof(dec_chan_slice_list), 0, sizeof(dec_chan_slice_list)));

    vfmw_check_sec_func(memset_s(&task->mem_param, sizeof(dec_chan_mem_param), 0, sizeof(dec_chan_mem_param)));

    return;
}

STATIC td_void dec_chan_init_task_pool(dec_chan_task_pool *task_pool)
{
    td_u32 i;
    dec_chan_task *task = TD_NULL;

    task_pool->cfg_task_num = 0;
    task_pool->dec_task_num = 0;
    task_pool->next_cfg_task = 0;
    task_pool->next_bsp_task = 0;
    task_pool->next_pxp_task = 0;

    task = &task_pool->task[0];
    for (i = 0; i < DEC_MAX_TASK_NUM; i++, task++) {
        dec_chan_init_task(task);
    }

    return;
}

STATIC td_void dec_chan_init_chan(dec_chan *chan)
{
    chan->state = DEC_CHAN_STATE_NULL;
    chan->dec_ops = TD_NULL;
    chan->run_cnt = 0;
    chan->dps_try_cnt = 0;
    chan->dps_succ_cnt = 0;
    chan->dps_fail_cnt = 0;
    chan->mem_try_cnt = 0;
    chan->mem_succ_cnt = 0;
    chan->mem_fail_cnt = 0;
    chan->rcv_dps_num = 0;
    chan->dec_dps_num = 0;
    chan->err_frame_num = 0;
    chan->wait_dev = 0;
    chan->block_time = 0;

    vfmw_check_sec_func(memset_s(&chan->chan_cfg, sizeof(dec_chan_cfg), 0, sizeof(dec_chan_cfg)));

    dec_chan_init_task_pool(&chan->chan_task_pool);

    return;
}

STATIC td_void dec_chan_init_chan_pool(dec_chan_pool *chan_pool)
{
    td_u32 i;

    vfmw_check_sec_func(memset_s(chan_pool, sizeof(dec_chan_pool), 0, sizeof(dec_chan_pool)));

    os_spin_lock_init(&chan_pool->spin_lock);

    for (i = 0; i < DEC_MAX_CHAN_NUM; i++) {
        os_spin_lock_init(&chan_pool->chan_lock[i]);
    }

    return;
}

STATIC td_void dec_chan_deinit_chan_pool(dec_chan_pool *chan_pool)
{
    td_u32 i;

    for (i = 0; i < DEC_MAX_CHAN_NUM; i++) {
        os_spin_lock_exit(chan_pool->chan_lock[i]);
    }

    os_spin_lock_exit(chan_pool->spin_lock);

    vfmw_check_sec_func(memset_s(chan_pool, sizeof(dec_chan_pool), 0, sizeof(dec_chan_pool)));

    return;
}

/* check state */
VFMW_ACC_BASE td_s32 dec_chan_check_chan_valid(const dec_chan *chan)
{
    if (chan == TD_NULL ||
        chan->state == DEC_CHAN_STATE_NULL ||
        chan->state == DEC_CHAN_STATE_CANCEL) {
        return DEC_ERR;
    }

    return DEC_OK;
}

VFMW_ACC_BASE STATIC td_void dec_chan_set_task_state(dec_chan_task *task, dec_task_state state)
{
    /* lock ? */
    task->state = state;

    /* unlock ? */
    return;
}

/* manage task */
STATIC td_s32 dec_chan_get_cfg_task(dec_chan *chan, dec_chan_task **task)
{
    dec_chan_task *temp = TD_NULL;

    temp = &chan->chan_task_pool.task[chan->chan_task_pool.next_cfg_task];

    if (temp->state != DEC_TASK_IDLE) {
        return DEC_ERR;
    }

    *task = temp;

    return DEC_OK;
}

STATIC dec_chan_task *dec_chan_acquire_task(dec_chan *chan)
{
    dec_chan_task *task = TD_NULL;

    task = &chan->chan_task_pool.task[chan->chan_task_pool.next_cfg_task];

    if (task->state < DEC_TASK_START) {
        return TD_NULL;
    }

    return task;
}

STATIC td_s32 dec_chan_get_cfg_task_num(const dec_chan *chan)
{
    td_s32 task_num;

    /* lock ? */
    task_num = chan->chan_task_pool.cfg_task_num;

    /* unlock ? */
    return task_num;
}

STATIC td_u16 dec_chan_upd_next_task(td_u16 task)
{
    return (td_u16)((task + 1) % DEC_MAX_TASK_NUM);
}

VFMW_ACC_BASE STATIC td_void dec_chan_upd_next_cfg_task(dec_chan *chan)
{
    dec_chan_task_pool *task_pool = &chan->chan_task_pool;

    task_pool->next_cfg_task = dec_chan_upd_next_task(task_pool->next_cfg_task);

    return;
}

STATIC td_s32 dec_chan_upd_next_bsp_task(dec_chan *chan)
{
    dec_chan_task_pool *task_pool = &chan->chan_task_pool;

    task_pool->next_bsp_task = dec_chan_upd_next_task(task_pool->next_bsp_task);

    return DEC_OK;
}

STATIC td_s32 dec_chan_upd_next_pxp_task(dec_chan *chan)
{
    dec_chan_task_pool *task_pool = &chan->chan_task_pool;

    task_pool->next_pxp_task = dec_chan_upd_next_task(task_pool->next_pxp_task);

    return DEC_OK;
}

td_s32 dec_chan_get_bsp_task(dec_chan *chan, dec_chan_task **task)
{
    td_s32 ret = DEC_ERR;

    vfmw_check_ret(chan != TD_NULL, DEC_ERR);

    if (chan->chan_task_pool.dec_task_num > 0) {
        vfmw_check_ret(task != TD_NULL, DEC_ERR);
        *task = &chan->chan_task_pool.task[chan->chan_task_pool.next_bsp_task];
        dec_chan_upd_next_bsp_task(chan);
        ret = DEC_OK;
    }

    return ret;
}

td_s32 dec_chan_get_pxp_task(dec_chan *chan, dec_chan_task **task)
{
    td_s32 ret = DEC_ERR;

    vfmw_check_ret(chan != TD_NULL, DEC_ERR);

    if (chan->chan_task_pool.dec_task_num > 0) {
        vfmw_check_ret(task != TD_NULL, DEC_ERR);
        *task = &chan->chan_task_pool.task[chan->chan_task_pool.next_pxp_task];
        vfmw_check_ret((*task)->dps != TD_NULL, DEC_ERR);
        ret = DEC_OK;
    }

    return ret;
}

td_s32 dec_chan_update_pxp_task(dec_chan *chan)
{
    td_s32 ret = DEC_ERR;

    vfmw_check_ret(chan != TD_NULL, DEC_ERR);

    if (chan->chan_task_pool.dec_task_num > 0) {
        dec_chan_upd_next_pxp_task(chan);
        ret = DEC_OK;
    }

    return ret;
}

td_s32 dec_chan_clear_ref_info(const dec_chan *chan, const dec_chan_task *task)
{
    dps_ref_info dps_ref = { 0 };

    vfmw_check_ret(chan != TD_NULL, DEC_ERR);
    vfmw_check_ret(task != TD_NULL, DEC_ERR);
    vfmw_check_ret(task->dps != TD_NULL, DEC_ERR);

    dps_get_ref_info(chan->chan_id, task->dps, &dps_ref);
    dec_ref_update_ref_cnt(chan->chan_id, &dps_ref, -1);

    return DEC_OK;
}

/* dps and seg stream */
STATIC td_s32 dec_chan_acquire_dps(dec_chan *chan)
{
    dec_chan_task *task = TD_NULL;
    dps_info *dps = TD_NULL;
    td_s32 ret;

    vfmw_check_ret(chan != TD_NULL, DEC_ERR);

    ret = dec_chan_get_cfg_task(chan, &task);
    if (ret != DEC_OK) {
        return DEC_ERR;
    }

    chan->dps_try_cnt++;
    ret = dps_receive(chan->chan_id, &dps);
    if (ret != DPS_OK) {
        chan->dps_fail_cnt++;
        return DEC_ERR;
    }

    if (dps_check_valid(chan->chan_id, dps) == DPS_OK) {
        task->state = DEC_TASK_START;
        ret = DEC_OK;
    } else {
        (td_void)dps_release(chan->chan_id, dps);
        return DEC_ERR;
    }

    task->dps = dps;
    task->seg_rlsd_flag = 0;
    chan->chan_task_pool.cfg_task_num++;
    chan->rcv_dps_num++;
    chan->dps_succ_cnt++;

    return ret;
}

STATIC td_s32 dec_chan_release_dps(td_u16 chan_id, dps_info *dps)
{
    td_s32 ret;

    vfmw_check_ret(dps != TD_NULL, DEC_ERR);

    ret = dps_release(chan_id, dps);
    if (ret == DPS_OK) {
        ret = DEC_OK;
    } else {
        dprint(PRN_ERROR, "DEC Release chan %d failed\n", chan_id);
        ret = DEC_ERR;
    }

    return ret;
}

td_s32 dec_chan_release_task_dps(const dec_chan *chan, dec_chan_task *task)
{
    td_s32 ret;

    vfmw_check_ret(chan != TD_NULL, DEC_ERR);
    vfmw_check_ret(task != TD_NULL, DEC_ERR);
    vfmw_check_ret(task->dps != TD_NULL, DEC_ERR);

    ret = dec_chan_release_dps((td_u16)chan->chan_id, task->dps);
    if (ret == DEC_OK) {
        task->dps = TD_NULL;
        task->state = DEC_TASK_IDLE;
    }

    return ret;
}

td_s32 dec_chan_release_task_seg(const dec_chan *chan, dec_chan_task *task)
{
    vfmw_check_ret(chan != TD_NULL, DEC_ERR);
    vfmw_check_ret(task != TD_NULL, DEC_ERR);
    vfmw_check_ret(task->dps != TD_NULL, DEC_ERR);

    if (task->seg_rlsd_flag == 0) {
        dps_release_seg((td_s32)chan->chan_id, task->dps);

        task->seg_rlsd_flag = 1;
    }

    return DEC_OK;
}

VFMW_ACC_BASE STATIC td_void dec_chan_decrease_cfg_task_num(dec_chan *chan)
{
    vfmw_check(chan != TD_NULL);

    chan->chan_task_pool.cfg_task_num--;
}

VFMW_ACC_BASE STATIC td_void dec_chan_increase_dec_task_num(dec_chan *chan)
{
    vfmw_check(chan != TD_NULL);

    chan->chan_task_pool.dec_task_num++;
}

td_void dec_chan_decrease_dec_task_num(dec_chan *chan)
{
    vfmw_check(chan != TD_NULL);
    chan->chan_task_pool.dec_task_num--;
}

td_void dec_chan_increase_dec_dps_num(dec_chan *chan)
{
    vfmw_check(chan != TD_NULL);
    chan->dec_dps_num++;
}

STATIC dec_chan_task *dec_chan_acquire_cfg_task(dec_chan *chan)
{
    td_s32 ret;

    if (dec_chan_get_cfg_task_num(chan) == 0) {
        ret = dec_chan_acquire_dps(chan);
        if (ret != DEC_OK) {
            return TD_NULL;
        }
    }

    return dec_chan_acquire_task(chan);
}

VFMW_ACC_BASE STATIC td_s32 dec_chan_process_dec_slice(const dec_chan *chan,
    dec_chan_task *task)
{
    td_s32 ret = DEC_OK;
    if (task->dps->is_frm_cpy != 0) {
        return ret;
    }

    vfmw_check_ret(chan->dec_ops != TD_NULL, DEC_ERR);
    vfmw_check_ret(chan->dec_ops->process_slice != TD_NULL, DEC_ERR);

    ret = chan->dec_ops->process_slice(task->dps->pic_param, task->dps->slice_head, (td_u16)task->dps->slice_num,
                                       &task->slice_list);

    return ret;
}

VFMW_ACC_BASE STATIC td_s32 dec_chan_process_ref_info(const dec_chan *chan, const dec_chan_task *task)
{
    dps_ref_info dps_ref = { 0 };

    dps_get_ref_info(chan->chan_id, task->dps, &dps_ref);

    (td_void)dec_ref_mark_pic_ref_state(chan->chan_id, &dps_ref);

    /* convert ref pic info from FSP logic id to FSP phy_addr, ops to diff vid_std */
    (td_void)dec_ref_make_pic_ref_info(chan->chan_id, &dps_ref, chan->chan_cfg.poc_threshold);

    (td_void)dec_ref_get_pic_store_type(chan->chan_id, &dps_ref);

    /* convert RefIdx2FspLogicId to RefIdx2ApcIdx for each slice */
    (td_void)dec_ref_make_slice_ref_info(chan->chan_id, task->dps, chan->dec_ops);

    (td_void)dec_ref_update_ref_cnt(chan->chan_id, &dps_ref, 1);

    return DEC_OK;
}

VFMW_ACC_BASE STATIC td_void dec_chan_set_task_mem(dec_chan *chan,
    const fsp_logic *logic_fs, dec_chan_task *task)
{
    if (logic_fs->dec_fs != TD_NULL) {
        task->mem_param.frm_mem_fd = logic_fs->dec_fs->mem_fd;
        task->mem_param.frm_dma_buf = logic_fs->dec_fs->dma_buf;
        task->mem_param.luma_phy_addr = logic_fs->dec_fs->phy_addr;
        task->mem_param.luma_vir_addr = uint64_ptr(logic_fs->dec_fs->vir_addr);

        task->mem_param.ext_att_handle = logic_fs->dec_fs->info_handle;
        task->mem_param.ext_att_phy_addr = logic_fs->dec_fs->info_phy_addr;
        task->mem_param.ext_att_vir_addr = logic_fs->dec_fs->info_vir_addr;
        task->mem_param.ext_att_buf_size = logic_fs->dec_fs->info_buf_size;
        task->mem_param.ext_attr_offset = logic_fs->dec_fs->info_offset;

        task->mem_param.ext_hfbc_handle = logic_fs->dec_fs->hfbc_handle;
        task->mem_param.ext_hfbc_vir_addr = logic_fs->dec_fs->hfbc_vir_addr;
    }

    if (logic_fs->disp_fs != TD_NULL) {
        task->mem_param.disp_mem.frm_mem_fd = logic_fs->disp_fs->mem_fd;
        task->mem_param.disp_mem.frm_dma_buf = logic_fs->disp_fs->dma_buf;
        task->mem_param.disp_mem.luma_phy_addr = logic_fs->disp_fs->phy_addr;
        task->mem_param.disp_mem.luma_vir_addr = uint64_ptr(logic_fs->disp_fs->vir_addr);
        task->mem_param.disp_mem.sec_info = logic_fs->disp_fs->sec_info;

        task->mem_param.ext_att_handle = logic_fs->disp_fs->info_handle;
        task->mem_param.ext_att_phy_addr = logic_fs->disp_fs->info_phy_addr;
        task->mem_param.ext_att_vir_addr = logic_fs->disp_fs->info_vir_addr;
        task->mem_param.ext_att_buf_size = logic_fs->disp_fs->info_buf_size;
        task->mem_param.ext_attr_offset = logic_fs->disp_fs->info_offset;
    }

    task->mem_param.pmv_phy_addr = 0;
    if (logic_fs->pmv_fs != TD_NULL) {
        task->mem_param.pmv_phy_addr = logic_fs->pmv_fs->phy_addr;
        task->mem_param.half_pmvblk_offset = logic_fs->pmv_fs->buf_size / 2; /* 2 :a number */
    }

    task->mem_param.mtdt_phy_addr = 0;
    task->mem_param.mtdt_vir_addr = TD_NULL;
    task->mem_param.mtdt_size = 0;
    if (logic_fs->nor_fs != TD_NULL) {
        task->mem_param.mtdt_phy_addr = logic_fs->nor_fs->phy_addr;
        task->mem_param.mtdt_vir_addr = uint64_ptr(logic_fs->nor_fs->vir_addr);
        task->mem_param.meta_data_dma_buf = logic_fs->nor_fs->dma_buf;
        task->mem_param.mtdt_size = logic_fs->nor_fs->buf_size;
        task->mem_param.mtdt_fd = logic_fs->nor_fs->mem_fd;
        task->mem_param.mtdt_fd_offset = logic_fs->nor_fs->fd_offset;
    }

    task->mem_param.chan_mem = chan->chn_hal_mem;
}

VFMW_ACC_BASE STATIC td_s32 dec_chan_acquire_mem(dec_chan *chan, dec_chan_task *task)
{
    td_s32 fsp_logic_id;
    td_s32 air_logic_id;
    fsp_logic *logic_fs = TD_NULL;

    fsp_clear_useless_dec_fs(chan->chan_id);

    fsp_logic_id = dps_get_fsp_logic_id(chan->chan_id, task->dps);

    logic_fs = fsp_get_logic(chan->chan_id, fsp_logic_id);
    if (logic_fs == TD_NULL) {
        dprint(PRN_ERROR, "DecChan %d AcquireMem Fail\n", chan->chan_id);
        return DEC_ERR;
    }

    if (logic_fs->dec_fs == TD_NULL) {
        chan->mem_try_cnt++;
        /* New Fs buffer */
        if (task->dps->air_dec_info.air_version == AIR_DEC_FRAME) {
            air_logic_id = (td_s32)task->dps->air_dec_info.air_logic_id;
            if (fsp_new_phy(chan->chan_id, fsp_logic_id, air_logic_id) != FSP_OK) {
                chan->mem_fail_cnt++;
                return DEC_ERR;
            }
        } else {
            if (fsp_new_phy(chan->chan_id, fsp_logic_id, -1) != FSP_OK) {
                chan->mem_fail_cnt++;
                return DEC_ERR;
            }
        }

        chan->mem_succ_cnt++;
        fsp_set_dec_ref(chan->chan_id, fsp_logic_id);
    }

    dec_chan_set_task_mem(chan, logic_fs, task);

    return DEC_OK;
}

VFMW_ACC_BASE STATIC td_void dec_chan_set_image_ext_param(const dec_chan_mem_param *task_mem,
    image_buf *img_buf)
{
    img_buf->ext_att_handle = task_mem->ext_att_handle;
    img_buf->ext_attr_offset = task_mem->ext_attr_offset;
    img_buf->ext_att_phy_addr = task_mem->ext_att_phy_addr;
    img_buf->ext_att_vir_addr = task_mem->ext_att_vir_addr;
    img_buf->ext_att_buf_size = (td_s32)task_mem->ext_att_buf_size;

    img_buf->ext_hfbc_handle = task_mem->ext_hfbc_handle;
    img_buf->ext_hfbc_vir_addr = task_mem->ext_hfbc_vir_addr;
}

VFMW_ACC_BASE STATIC td_s32 dec_chan_set_image_param(dec_chan *chan,
    dec_chan_task *task)
{
    dec_chan_mem_param *task_mem = &task->mem_param;
    image_buf img_buf = { 0 };
    td_s32 fsp_logic_id;
    td_s32 ret;

    fsp_logic_id = dps_get_fsp_logic_id(chan->chan_id, task->dps);

    img_buf.mem_fd = task_mem->disp_mem.frm_mem_fd;
    img_buf.dma_buf = task_mem->disp_mem.frm_dma_buf;
    img_buf.luma_phy_addr = task_mem->disp_mem.luma_phy_addr;
    img_buf.luma_vir_addr = ptr_uint64(task_mem->disp_mem.luma_vir_addr);
    img_buf.chrom_phy_addr = img_buf.luma_phy_addr + task_mem->disp_mem.uv_offset;
    img_buf.chrom_vir_addr = img_buf.luma_vir_addr + task_mem->disp_mem.uv_offset;
    img_buf.image_stride = task_mem->disp_mem.image_stride;

    img_buf.luma_outdisp_phy_addr = img_buf.luma_phy_addr +
        task_mem->disp_mem.uv_offset * 3 / 2; /* 3 :a number 2 :a number */
    img_buf.luma_outdisp_vir_addr = img_buf.luma_vir_addr +
        task_mem->disp_mem.uv_offset * 3 / 2; /* 3 :a number 2 :a number */
    img_buf.chrom_outdisp_phy_addr = img_buf.luma_outdisp_phy_addr + task_mem->uv_offset_dis;
    img_buf.chrom_outdisp_vir_addr = img_buf.luma_outdisp_vir_addr + task_mem->uv_offset_dis;

    img_buf.luma_2bit_phy_addr = img_buf.luma_phy_addr + task_mem->disp_mem.nbit_y_offset;
    img_buf.luma_2bit_vir_addr = ptr_uint64(img_buf.luma_vir_addr + task_mem->disp_mem.nbit_y_offset);
    img_buf.chrom_2bit_phy_addr = img_buf.luma_2bit_phy_addr + task_mem->disp_mem.nbit_uv_offset;
    img_buf.chrom_2bit_vir_addr = ptr_uint64(img_buf.luma_2bit_vir_addr + task_mem->disp_mem.nbit_uv_offset);
    img_buf.image_stride_2bit = task_mem->disp_mem.nbit_image_stride;

    img_buf.head_stride = task_mem->disp_mem.cmp_head_stride;
    img_buf.head_info_size = task_mem->disp_mem.cmp_head_size;

    img_buf.top_luma_phy_addr = img_buf.luma_phy_addr;
    img_buf.top_chrom_phy_addr = img_buf.top_luma_phy_addr + task_mem->disp_mem.uv_offset;
    img_buf.btm_luma_phy_addr = img_buf.top_luma_phy_addr + img_buf.image_stride;
    img_buf.btm_chrom_phy_addr = img_buf.top_chrom_phy_addr + img_buf.image_stride;
    img_buf.line_num_phy_addr = task_mem->disp_mem.luma_phy_addr + task_mem->disp_mem.line_num_ofs;

    img_buf.vdh_2d_en = task_mem->disp_mem.vdh_2d_en;
    img_buf.tile_format = task_mem->disp_mem.tile_format;
    img_buf.compress_en = task_mem->disp_mem.compress_en;
    img_buf.bit_depth = task_mem->disp_mem.bit_depth;

    img_buf.metadata_phy_addr = task_mem->mtdt_phy_addr;
    img_buf.metadata_dma_buf = task_mem->meta_data_dma_buf;
    img_buf.metadata_vir_addr = ptr_uint64(task_mem->mtdt_vir_addr);
    img_buf.metadata_size = task_mem->mtdt_size;
    img_buf.mtdt_fd = task_mem->mtdt_fd;
    img_buf.mtdt_fd_offset = task_mem->mtdt_fd_offset;
    img_buf.sec_info = task_mem->disp_mem.sec_info;
    img_buf.av1_film_tab_offset = task_mem->av1_film_tab_offset;
    img_buf.buf_size = task_mem->disp_mem.buf_size;

    dec_chan_set_image_ext_param(task_mem, &img_buf);

    ret = fsp_set_image(chan->chan_id, fsp_logic_id, &img_buf);
    ret = (ret == FSP_OK) ? DEC_OK : DEC_ERR;

    return ret;
}

VFMW_ACC_BASE STATIC td_s32 dec_chan_calc_mem_param(dec_chan *chan, dec_chan_task *task)
{
    return dec_dev_calc_mem_param(task->dev_id, chan->dec_ops, &chan->chan_cfg, task);
}

STATIC td_void dec_chan_pass_reg_info_to_task(const dec_chan *chan, dec_chan_task *task)
{
    task->mem_param.prev_pic_mb0_qp = chan->prev_pic_mb0_qp;
    task->mem_param.switch_rounding = chan->switch_rounding;
}

VFMW_ACC_BASE STATIC td_s32 dec_chan_split_task(dec_chan *chan, dec_chan_task *task)
{
    if (dec_chan_process_dec_slice(chan, task) != DEC_OK) {
        dec_chan_print_block(chan->chan_id, "split slice failed");
        return DEC_ERR;
    }

    dec_chan_set_task_state(task, DEC_TASK_SPLITOK);

    return DEC_OK;
}

VFMW_ACC_BASE STATIC td_s32 dec_chan_convref(const dec_chan *chan, dec_chan_task *task)
{
    if (dec_chan_process_ref_info(chan, task) != DEC_OK) {
        dec_chan_print_block(chan->chan_id, "no ref");
        return DEC_ERR;
    }

    dec_chan_set_task_state(task, DEC_TASK_REFOK);

    return DEC_OK;
}

VFMW_ACC_BASE STATIC td_s32 dec_chan_aqrmem(dec_chan *chan, dec_chan_task *task)
{
    if (dec_chan_acquire_mem(chan, task) != DEC_OK) {
        dec_chan_print_block(chan->chan_id, "no mem");
        return DEC_ERR;
    }

    dec_chan_set_task_state(task, DEC_TASK_MEMOK);

    return DEC_OK;
}

VFMW_ACC_BASE STATIC td_s32 dec_chan_cfgmsg(dec_chan *chan,
    dec_chan_task *task)
{
    td_s32 is_sec = chan->is_sec;
    dec_dev_acquire_dev(is_sec, task, &task->dev_id);

    if (dec_dev_acquire_msg(chan->chan_id, task->dev_id, &task->msg_id) != DEC_OK) {
        dec_chan_print_block(chan->chan_id, "no msg");
        return DEC_ERR;
    }

    vfmw_check_ret(dec_chan_calc_mem_param(chan, task) == DEC_OK, DEC_ERR);

    dec_chan_pass_reg_info_to_task(chan, task);

    if (dec_dev_config_msg(task->dev_id, task->msg_id, chan->dec_ops, task) != DEC_OK) {
        dec_dev_release_msg(task->dev_id, task->msg_id);
        dprint(PRN_ERROR, "Chan %d config dev %d Msg %d fail!\n", chan->chan_id, task->dev_id, task->msg_id);
        return DEC_ERR;
    }

    dec_chan_set_image_param(chan, task);

    dec_chan_set_task_state(task, DEC_TASK_CFGMSGOK);

    return DEC_OK;
}

VFMW_ACC_BASE STATIC td_s32 dec_chan_pre_process_task(dec_chan *chan,
    dec_chan_task *task)
{
    switch (task->state) {
        case DEC_TASK_IDLE:
            return DEC_ERR;
        case DEC_TASK_START:
            goto split;
        case DEC_TASK_SPLITOK:
            goto convref;
        case DEC_TASK_REFOK:
            goto aqrmem;
        case DEC_TASK_MEMOK:
            goto cfgmsg;
        case DEC_TASK_CFGMSGOK:
            return DEC_OK;
        default:
            return DEC_ERR;
    };

split:
    if (dec_chan_split_task(chan, task) != DEC_OK) {
        return DEC_ERR;
    }
convref:
    if (dec_chan_convref(chan, task) != DEC_OK) {
        return DEC_ERR;
    }
aqrmem:
    if (dec_chan_aqrmem(chan, task) != DEC_OK) {
        return DEC_ERR;
    }
cfgmsg:

    return dec_chan_cfgmsg(chan, task);
}

STATIC td_s32 dec_chan_adjust_task_ref_info(const dec_chan *chan, dec_chan_task *task)
{
    td_s32 ret;
    dps_ref_info dps_ref = { 0 };

    dps_get_ref_info((td_s32)chan->chan_id, task->dps, &dps_ref);
    ret = dec_ref_make_pic_ref_info(chan->chan_id, &dps_ref, chan->chan_cfg.poc_threshold);
    if (ret != DEC_OK) {
        dprint(PRN_ERROR, "Adjust task Refence Info Error!\n");
        return DEC_ERR;
    }

    if (task->state == DEC_TASK_CFGMSGOK) {
        dec_dev_release_msg(task->dev_id, task->msg_id);
        dec_chan_set_task_state(task, DEC_TASK_MEMOK);
    }

    return DEC_OK;
}

td_void dec_chan_adjust_ref_info(dec_chan *chan, const dec_chan_task *curr_task)
{
    td_s32 i;
    dec_chan_task *task = TD_NULL;

    vfmw_check(chan != TD_NULL);

    for (i = 0; i < DEC_MAX_TASK_NUM; i++) {
        task = &chan->chan_task_pool.task[i];
        if (task != curr_task) {
            if (task->state >= DEC_TASK_REFOK &&
                task->state < DEC_TASK_DECODING &&
                task->dps != TD_NULL) {
                dec_chan_adjust_task_ref_info(chan, task);
            }
        }
    }
}

/* manage chan */
STATIC VFMW_ACC_BASE dec_chan_task *dec_chan_get_dec_task(dec_chan *chan)
{
    vfmw_check_ret(chan != TD_NULL, TD_NULL);

    if (chan->state != DEC_CHAN_STATE_RUN) {
        return TD_NULL;
    }

    chan->run_cnt++;

    return dec_chan_acquire_cfg_task(chan);
}

/* manage chan */
td_u32 dec_chan_scdeduling(dec_chan **chan, dec_chan_task **task)
{
    dec_chan_pool *chan_pool = TD_NULL;
    dec_chan *chan_tmp = TD_NULL;
    dec_chan_task *task_tmp = TD_NULL;
    td_u32 next_chan;
    td_u16 next_try = 1;
    td_u16 i;
    td_ulong pool_lock_flag;
    td_ulong lock_flag;

    vfmw_check_ret(chan != TD_NULL, 0);
    vfmw_check_ret(task != TD_NULL, 0);

    chan_pool = dec_ctrl_get_chan_pool();
    dec_chan_lock_chan_pool(chan_pool, &pool_lock_flag);

    next_chan = chan_pool->next_chan;

    for (i = 0; i < DEC_MAX_CHAN_NUM; i++) {
        if (next_chan < DEC_MAX_CHAN_NUM) {
            chan_tmp = chan_pool->chan[next_chan];
        }

        if (next_chan < DEC_MAX_CHAN_NUM) {
            dec_chan_lock_chan((td_s32)next_chan, &lock_flag);
        }

        if (chan_tmp != TD_NULL && chan_tmp->state == DEC_CHAN_STATE_RUN) {
            task_tmp = dec_chan_acquire_cfg_task(chan_tmp);
            if (task_tmp != TD_NULL) {
                *chan = chan_tmp;
                *task = task_tmp;
                chan_tmp->run_cnt++;
                next_try = 0;
            }
        }

        if (next_chan < DEC_MAX_CHAN_NUM) {
            dec_chan_unlock_chan((td_s32)next_chan, &lock_flag);
        }

        next_chan = (next_chan + 1) % DEC_MAX_CHAN_NUM;

        if (next_try == 0) {
            break;
        }
    }

    dec_chan_unlock_chan_pool(chan_pool, &pool_lock_flag);

    return next_chan;
}

td_void dec_chan_upd_scheduling(dec_chan *chan)
{
    dec_chan_pool *chan_pool = TD_NULL;
    td_ulong lock_flag;

    vfmw_unused(chan);
    chan_pool = dec_ctrl_get_chan_pool();

    dec_chan_lock_chan_pool(chan_pool, &lock_flag);
    chan_pool->next_chan = (chan_pool->next_chan + 1) % DEC_MAX_CHAN_NUM;
    dec_chan_unlock_chan_pool(chan_pool, &lock_flag);

    return;
}

/* cancel decode */
td_void dec_chan_cancel_task(dec_chan *chan, dec_chan_task *task, td_u32 perm_flag)
{
    vfmw_check(task != TD_NULL);

    dec_dev_cancel_reg(task->dev_id, task->reg_id);

    if (perm_flag == 1) {
        fsp_clear_dec(chan->chan_id, task->dps->logic_id);
        fsp_set_disp(chan->chan_id, task->dps->logic_id, 0);
        dec_dev_release_msg(task->dev_id, task->msg_id);
        dec_chan_release_task_seg(chan, task);
        dec_chan_clear_ref_info(chan, task);
        dec_chan_release_task_dps(chan, task);
    }

    task->state = DEC_TASK_TEMPCANCEL + perm_flag;

    return;
}

STATIC td_s32 dec_chan_cancel_chan_task(dec_chan *chan, td_u32 perm_flag, td_s32 *can_cel_num)
{
    dec_chan_task *task = TD_NULL;
    td_s32 cancel_index;
    td_s32 i;
    /* from last to first */
    for (i = 0; i < chan->chan_task_pool.dec_task_num; i++) {
        cancel_index = (td_s32)((chan->chan_task_pool.next_pxp_task + i) % DEC_MAX_TASK_NUM);
        task = &chan->chan_task_pool.task[cancel_index];

        dec_chan_cancel_task(chan, task, perm_flag);
    }

    if (can_cel_num != TD_NULL) {
        *can_cel_num = i;
    }

    return DEC_OK;
}

/* interface */
dec_chan *dec_chan_get_chan(td_s32 chan_id)
{
    dec_chan_pool *chan_pool = TD_NULL;

    if (chan_id > DEC_MAX_CHAN_NUM - 1) {
        dprint(PRN_ERROR, "DecChan chan_id error!\n");
        return TD_NULL;
    }

    chan_pool = dec_ctrl_get_chan_pool();

    return chan_pool->chan[chan_id];
}

td_void dec_chan_get_chan_num(dec_chan_type_num *num)
{
    dec_chan_pool *chan_pool = TD_NULL;
    td_ulong lock_flag;

    vfmw_check(num != TD_NULL);

    chan_pool = dec_ctrl_get_chan_pool();
    dec_chan_lock_chan_pool(chan_pool, &lock_flag);

    num->sec = (td_s32)chan_pool->chan_num_sec;
    num->no_sec = (td_s32)chan_pool->chan_num_no_sec;

    dec_chan_unlock_chan_pool(chan_pool, &lock_flag);

    return;
}

td_void dec_chan_init(td_void)
{
    dec_chan_pool *chan_pool = TD_NULL;

    chan_pool = dec_ctrl_get_chan_pool();

    dec_chan_init_chan_pool(chan_pool);
}

td_void dec_chan_deinit(td_void)
{
    dec_chan_pool *chan_pool = TD_NULL;

    chan_pool = dec_ctrl_get_chan_pool();

    dec_chan_deinit_chan_pool(chan_pool);
}

td_s32 dec_chan_create(td_s32 chan_id, const td_void *args)
{
    dec_chan_pool *chan_pool = TD_NULL;
    dec_chan *chan = TD_NULL;
    td_s32 ret = DEC_ERR;
    td_ulong pool_lock_flag = 0;
    td_ulong lock_flag = 0;
    td_void *vir = TD_NULL;
    mem_record mem = { 0 };
    dec_create_info *param = (dec_create_info *)args;

    vfmw_check_ret((chan_id < DEC_MAX_CHAN_NUM), DEC_ERR);

    mem.mem_name = "dec_chan";
    mem.length = vfmw_align_up((td_u32)sizeof(dec_chan), VFMW_CTX_ALIGN);
    mem.type = CACHE_MEM_TYPE;
    mem.is_cached = 0;
    mem.unid = (td_u32)chan_id;
    vfmw_check_ret(os_malloc != TD_NULL, DEC_ERR);
    os_malloc(&mem);
    vir = (td_void *)(mem.vir_addr);
    vfmw_check_ret(vir != TD_NULL, DEC_ERR);

    chan_pool = dec_ctrl_get_chan_pool();

    dec_chan_lock_chan_pool(chan_pool, &pool_lock_flag);

    dec_chan_lock_chan(chan_id, &lock_flag);

    chan = dec_chan_get_chan(chan_id);
    vfmw_check_goto((chan == TD_NULL), exit);

    chan = (dec_chan *)vir;
    chan_pool->chan[chan_id] = chan;
    dec_chan_init_chan(chan);
    chan->chan_id = chan_id;
    vfmw_check_goto(os_get_unid != TD_NULL, exit);
    chan->unid = (td_u32)os_get_unid();
    chan->state = DEC_CHAN_STATE_SUSPEND;

    chan->prev_pic_mb0_qp = 0x0F;
    chan->switch_rounding = 1;
    chan->is_sec = param->is_sec;
    chan->adpt_type = param->adpt_type;

    vfmw_check_sec_func(memset_s(chan->chn_hal_mem, sizeof(chan->chn_hal_mem), 0, sizeof(chan->chn_hal_mem)));

    (chan->is_sec == 0) ? chan_pool->chan_num_no_sec++ : chan_pool->chan_num_sec++;

    ret = DEC_OK;

exit:

    dec_chan_unlock_chan(chan_id, &lock_flag);

    dec_chan_unlock_chan_pool(chan_pool, &pool_lock_flag);

    if (ret != DEC_OK) {
        os_free(&mem);
    }

    return DEC_OK;
}

STATIC td_void dec_chan_free_memory(dec_chan *chan)
{
    td_s32 cnt, mem_num;
    mem_record mem = { 0 };

    vfmw_check(chan != TD_NULL);

    mem_num = (td_s32)(sizeof(chan->chn_hal_mem) / sizeof(vfmw_mem_desc));
    for (cnt = 0; cnt < mem_num; cnt++) {
        if (chan->chn_hal_mem[cnt].phy_addr != 0) {
            mem.dma_buf = chan->chn_hal_mem[cnt].dma_buf;
            mem.length = chan->chn_hal_mem[cnt].length;
            mem.phy_addr = chan->chn_hal_mem[cnt].phy_addr;
            mem.vir_addr = uint64_ptr(chan->chn_hal_mem[cnt].vir_addr);
            mem.is_cached = 0;
            os_free(&mem);
            chan->chn_hal_mem[cnt].phy_addr = 0;
        }
    }

    mem.vir_addr = (td_u8 *)(chan);
    mem.is_cached = 0;
    os_free(&mem);
}

td_s32 dec_chan_destroy(td_s32 chan_id)
{
    dec_chan_pool *chan_pool = TD_NULL;
    dec_chan *chan = TD_NULL;
    td_s32 ret = DEC_ERR;
    td_ulong pool_lock_flag = 0;
    td_ulong lock_flag = 0;

    vfmw_check_ret((chan_id < DEC_MAX_CHAN_NUM), DEC_ERR);

    chan_pool = dec_ctrl_get_chan_pool();

    if (dec_chan_stop(chan_id) != DEC_OK) {
        dprint(PRN_ERROR, "Dec stop channel %d err!\n", chan_id);
    }

    dec_chan_lock_chan_pool(chan_pool, &pool_lock_flag);

    dec_chan_lock_chan(chan_id, &lock_flag);

    chan = dec_ctrl_get_chan(chan_id);
    vfmw_check_goto((chan != TD_NULL), exit);

    if (dec_chan_cancel_chan_task(chan, 1, TD_NULL) != DEC_OK) {
        goto exit;
    }

    chan->state = DEC_CHAN_STATE_NULL;
    chan_pool->chan[chan_id] = TD_NULL;

    if (chan->is_sec == 0) {
        chan_pool->chan_num_no_sec--;
    } else {
        chan_pool->chan_num_sec--;
    }

    ret = DEC_OK;

exit:

    dec_chan_unlock_chan(chan_id, &lock_flag);
    dec_chan_unlock_chan_pool(chan_pool, &pool_lock_flag);

    if (ret == DEC_OK) {
        dec_chan_free_memory(chan);
    }
    vdec_record_chr_event(chan_id, MT_VDEC_DESTROY, MT_VDEC_STORE_CLOSE, MT_VDEC_VFMW_DEC_DESTROY_OK);

    return ret;
}

td_s32 dec_chan_config(td_s32 chan_id, const td_void *chan_cfg)
{
    td_s32 ret = DEC_ERR;
    dec_chan *chan = TD_NULL;
    vfmw_vid_std vid_std;
    td_ulong lock_flag;
    dec_hal_ops *ops = TD_NULL;

    vfmw_check_ret(chan_cfg != TD_NULL, DEC_ERR);

    dec_chan_lock_chan(chan_id, &lock_flag);

    chan = dec_ctrl_get_chan(chan_id);
    vid_std = ((dec_chan_cfg *)chan_cfg)->vid_std;

    if (dec_chan_check_chan_valid(chan) != DEC_OK) {
        goto exit;
    }

    if (dec_chan_check_chan_valid(chan) != DEC_OK) {
        dprint(PRN_ERROR, "%s %d Dec Chan %d not valid\n",
               __func__, __LINE__, chan->chan_id);
        goto exit;
    }

    vfmw_check_sec_func(memcpy_s(&chan->chan_cfg, sizeof(chan->chan_cfg), chan_cfg, sizeof(dec_chan_cfg)));

    dec_hal_get_ops(vid_std, &chan->dec_ops);
    ops = chan->dec_ops;

    ret = DEC_OK;

exit:

    dec_chan_unlock_chan(chan_id, &lock_flag);

    if (ops != TD_NULL &&
        ops->alloc_chan_mem != TD_NULL &&
        chan->chn_hal_mem[0].phy_addr == 0 &&
        ops->alloc_chan_mem(chan_id, chan->chn_hal_mem, 2) != DEC_OK) { /* 2 is the len of array size */
        ret = DEC_ERR;
    }

    return ret;
}

STATIC td_void dec_chan_update_chan_prior_tab(td_void);

td_s32 dec_chan_start(td_s32 chan_id)
{
    dec_chan *chan = dec_ctrl_get_chan(chan_id);

    td_s32 ret = DEC_ERR;
    td_ulong lock_flag;

    if (chan == TD_NULL) {
        dprint(PRN_ERROR, "Get chan %d failed!\n", chan_id);
        return ret;
    }

    dec_chan_lock_chan(chan_id, &lock_flag);

    if (dec_chan_check_chan_valid(chan) != DEC_OK) {
        dprint(PRN_ERROR, "%s %d Error, Dec Chan%d is not valid\n", __func__, __LINE__, chan->chan_id);
        goto exit;
    }

    chan->state = DEC_CHAN_STATE_RUN;

    ret = DEC_OK;

exit:

    dec_chan_unlock_chan(chan_id, &lock_flag);
    dec_chan_update_chan_prior_tab();

    return ret;
}

td_s32 dec_chan_stop(td_s32 chan_id)
{
    td_s32 ret = DEC_ERR;
    dec_chan *chan = TD_NULL;
    td_ulong lock_flag;

    dec_chan_lock_chan(chan_id, &lock_flag);

    chan = dec_chan_get_chan(chan_id);
    if (chan == TD_NULL) {
        dprint(PRN_ERROR, "Get chan %d failed!\n", chan_id);
        goto exit;
    }

    chan->state = DEC_CHAN_STATE_SUSPEND;

    ret = DEC_OK;

exit:

    dec_chan_unlock_chan(chan_id, &lock_flag);

    return ret;
}

td_s32 dec_chan_reset(td_s32 chan_id)
{
    td_s32 ret = DEC_ERR;
    dec_chan *chan = TD_NULL;
    td_ulong lock_flag;

    dec_chan_lock_chan(chan_id, &lock_flag);

    chan = dec_chan_get_chan(chan_id);
    if (chan == TD_NULL) {
        dprint(PRN_ERROR, "Get chan %d failed!\n", chan_id);
        goto exit;
    }

    if (dec_chan_check_chan_valid(chan) != DEC_OK) {
        dprint(PRN_ERROR, "%s %d Error, Dec Chan%d is not valid\n", __func__, __LINE__, chan_id);
        goto exit;
    }

    if (dec_chan_cancel_chan_task(chan, 1, TD_NULL) != DEC_OK) {
        dprint(PRN_ERROR, "%s %d Error, Dec Chan%d cancel_chan_task fail\n", __func__, __LINE__, chan_id);
        goto exit;
    }

    chan->state = DEC_CHAN_STATE_SUSPEND;
    chan->run_cnt = 0;
    chan->dps_try_cnt = 0;
    chan->dps_succ_cnt = 0;
    chan->dps_fail_cnt = 0;
    chan->rcv_dps_num = 0;
    chan->dec_dps_num = 0;
    chan->err_frame_num = 0;

    dec_chan_init_task_pool(&chan->chan_task_pool);

    ret = DEC_OK;

exit:

    dec_chan_unlock_chan(chan_id, &lock_flag);

    return ret;
}

STATIC td_void dec_chan_mark_display_air_dec(dec_chan *chan, dec_chan_task *task, td_s32 pic_structure,
    td_s32 fsp_logic_id)
{
    td_s32 air_logic_id = (td_s32)task->dps->air_dec_info.air_logic_id;
    if (task->dps->air_dec_info.air_sta == AIR_STATE_OVER) {
        fsp_set_air_state(chan->chan_id, fsp_logic_id, FS_AIR_STATE_USE);
        fsp_set_dec_state(chan->chan_id, fsp_logic_id, pic_structure, task->err_ratio);
        fsp_set_air_state(chan->chan_id, air_logic_id, FS_AIR_STATE_OVER);
        fsp_set_dec_state(chan->chan_id, air_logic_id, pic_structure, task->err_ratio);
    } else if (task->dps->air_dec_info.air_sta != AIR_STATE_FIRST) {
        fsp_set_dec_state(chan->chan_id, fsp_logic_id, pic_structure, task->err_ratio);
    }
}

td_s32 dec_chan_mark_display_state(dec_chan *chan, dec_chan_task *task, td_u16 state)
{
    td_s32 i, cancel_cnt, fsp_logic_id;
    td_s32 pic_structure;
    fsp_logic *logic = TD_NULL;
    td_u32 ref_fsp_id;
    dps_ref_info dps_ref = { 0 };

    vfmw_check_ret(chan != TD_NULL, DEC_ERR);
    vfmw_check_ret(task != TD_NULL, DEC_ERR);
    vfmw_check_ret(task->dps != TD_NULL, DEC_ERR);
    vfmw_check_ret(os_get_time_us != TD_NULL, DEC_ERR);

    pic_structure = dps_get_pic_struct(chan->chan_id, task->dps);
    fsp_logic_id = dps_get_fsp_logic_id(chan->chan_id, task->dps);

    if (task->mem_param.low_delay_en == 1) {
        task->err_ratio = 0;
    } else if (state == 0) {
        task->err_ratio = 101; /* 101 :a number */
    }

    dps_get_ref_info(chan->chan_id, task->dps, &dps_ref);
    for (i = 0; i < (td_s32)dps_ref.cur_ref_size; i++) {
        ref_fsp_id = *(dps_ref.cur_ref_fsp_id + i);
        logic = fsp_get_logic(chan->chan_id, (td_s32)ref_fsp_id);
        if (logic != TD_NULL && (logic->err_flag == 1 || logic->ref_err == 1)) {
            logic = fsp_get_logic(chan->chan_id, fsp_logic_id);
            if (logic != TD_NULL) {
                logic->ref_err = 1;
            }
        }
    }

    if (task->dps->air_dec_info.air_version == AIR_DEC_FRAME) {
        dec_chan_mark_display_air_dec(chan, task, pic_structure, fsp_logic_id);
    } else {
        fsp_set_dec_state(chan->chan_id, fsp_logic_id, pic_structure, task->err_ratio);
    }

    logic = fsp_get_logic(chan->chan_id, fsp_logic_id);
    if (logic != TD_NULL && logic->err_flag == 1) {
        dec_chan_cancel_chan_task(chan, 0, &cancel_cnt);
        if (cancel_cnt == 0) {
            dec_chan_adjust_ref_info(chan, task);
        } else {
            chan->state = DEC_CHAN_STATE_CANCEL;
        }
    }

    if (dcheck(PRN_TIME)) {
        td_s32 cid = chan->chan_id;
        td_u32 fid = (td_u32)task->dps->unid;
        dbg_add_time(cid, fid, TPOS_DEQ_DEC, os_get_time_us());
    }

    voq_report(chan->chan_id);

    return DEC_OK;
}

STATIC dec_chan_task *dec_chn_get_running_task(td_s32 chan_id)
{
    td_u32 i;
    dec_chan *chan = TD_NULL;

    chan = dec_ctrl_get_chan(chan_id);
    if (chan == TD_NULL) {
        return TD_NULL;
    }

    if (chan->state != DEC_CHAN_STATE_RUN) {
        return TD_NULL;
    }

    if (dec_chan_get_cfg_task_num(chan) == 0) {
        return TD_NULL;
    }

    for (i = 0; i < DEC_MAX_TASK_NUM; i++) {
        dec_chan_task *task = &chan->chan_task_pool.task[i];
        if (task->state >= DEC_TASK_START) {
            return task;
        }
    }

    return TD_NULL;
}

td_s32 dec_chan_get_info(td_s32 chan_id, const td_void *args)
{
    td_u32 i;
    td_ulong lock_flag;
    td_u32 max_ratio = 0;
    dec_entry *entry = TD_NULL;
    dec_chan *chan = TD_NULL;
    dec_info *info = (dec_info *)args;
    dec_chan_task *task = TD_NULL;
    dec_dev_info *dev = TD_NULL;

    vfmw_check_ret(info != TD_NULL, DEC_ERR);

    entry = dec_ctrl_get_entry();
    vfmw_check_ret(entry != TD_NULL, DEC_ERR);

    dec_chan_lock_chan(chan_id, &lock_flag);

    chan = dec_ctrl_get_chan(chan_id);
    if (dec_chan_check_chan_valid(chan) != DEC_OK) {
        goto exit;
    }

    task = dec_chn_get_running_task(chan_id);
    if (task == TD_NULL) {
        info->is_task_run = TD_FALSE;
    } else {
        info->is_task_run = TD_TRUE;
        info->task_state = (td_u32)task->state;
    }
    for (i = 0; i < DEC_DEV_NUM; i++) {
        dev = dec_ctrl_get_dev(i);
        if (dev == TD_NULL || dev->handle == TD_NULL) {
            continue;
        }
        max_ratio = dev->ratio > max_ratio ? dev->ratio : max_ratio;
    }
    info->dec_dps_num = (td_u32)chan->dec_dps_num;
    info->err_frame_num = (td_s32)chan->err_frame_num;
    info->load = entry->is_over_load == 1 ? 1000 : max_ratio; /* 1000 :the max of dec load */
exit:

    dec_chan_unlock_chan(chan_id, &lock_flag);

    return DEC_OK;
}

td_void dec_chan_update_chan_prior_tab(td_void)
{
    dec_chan_pool *chan_pool = TD_NULL;
    dec_chan *chan = TD_NULL;
    td_s32 i, j, tmp_pri_chan[2]; /* 2 :a number */
    td_ulong pool_lock_flag;
    td_s32 soft_flag;

    chan_pool = dec_ctrl_get_chan_pool();
    dec_chan_lock_chan_pool(chan_pool, &pool_lock_flag);
    chan_pool->chan_pri_tab_index = 0;

    /* snap current prior status */
    chan_pool->started_chan_num = 0;
    for (i = 0; i < DEC_MAX_CHAN_NUM; i++) {
        chan = chan_pool->chan[i];

        if (chan != TD_NULL &&
            chan->state == DEC_CHAN_STATE_RUN) {
            if (chan_pool->started_chan_num < DEC_MAX_CHAN_NUM) {
                /* 0 is invalid */
                chan_pool->chan_pri_tab[chan_pool->started_chan_num][1] = (td_s32)(1 + chan->chan_cfg.priority);
                chan_pool->chan_pri_tab[chan_pool->started_chan_num][0] = i;
                chan_pool->started_chan_num++;
            }
        }
    }

    for (i = (td_s32)chan_pool->started_chan_num; i < VFMW_CHAN_NUM; i++) {
        chan_pool->chan_pri_tab[i][1] = 0;
        chan_pool->chan_pri_tab[i][0] = 0;
    }

    /* sort prio */
    for (i = 0; i < DEC_MAX_CHAN_NUM - 1; i++) {
        soft_flag = DEC_OK;
        for (j = i + 1; j < DEC_MAX_CHAN_NUM; j++) {
            if (chan_pool->chan_pri_tab[i][1] < chan_pool->chan_pri_tab[j][1]) {
                tmp_pri_chan[0] = chan_pool->chan_pri_tab[i][0];
                tmp_pri_chan[1] = chan_pool->chan_pri_tab[i][1];

                chan_pool->chan_pri_tab[i][0] = chan_pool->chan_pri_tab[j][0];
                chan_pool->chan_pri_tab[i][1] = chan_pool->chan_pri_tab[j][1];

                chan_pool->chan_pri_tab[j][0] = tmp_pri_chan[0];
                chan_pool->chan_pri_tab[j][1] = tmp_pri_chan[1];
                soft_flag = DEC_ERR;
            }
        }
        if (soft_flag == DEC_OK) {
            break;
        }
    }
    dec_chan_unlock_chan_pool(chan_pool, &pool_lock_flag);

    return;
}

STATIC VFMW_ACC_BASE td_s32 dec_chan_update_tab_after_dec(td_s32 curr_chan)
{
    dec_chan_pool *chan_pool = TD_NULL;
    td_s32 i;
    td_ulong pool_lock_flag;
    td_s32 tmp_index = 0;

    chan_pool = dec_ctrl_get_chan_pool();
    dec_chan_lock_chan_pool(chan_pool, &pool_lock_flag);
    chan_pool->chan_pri_tab_index = 0;

    /* snap current prior status */
    for (i = 0; i < (td_s32)chan_pool->started_chan_num; i++) {
        if (chan_pool->chan_pri_tab[i][0] == curr_chan) {
            tmp_index = i;
            break;
        }
    }

    for (i = tmp_index; i < (td_s32)chan_pool->started_chan_num - 1; i++) {
        chan_pool->chan_pri_tab[i][0] = chan_pool->chan_pri_tab[i + 1][0];
    }
    chan_pool->chan_pri_tab[chan_pool->started_chan_num - 1][0] = curr_chan;

    dec_chan_unlock_chan_pool(chan_pool, &pool_lock_flag);

    return DEC_OK;
}

STATIC VFMW_ACC_BASE td_void dec_chan_get_high_prior_chan(dec_chan_pool *chan_pool)
{
    td_ulong pool_lock_flag;

    vfmw_check(chan_pool != TD_NULL);

    dec_chan_lock_chan_pool(chan_pool, &pool_lock_flag);
    chan_pool->curr_high_prior_chan = chan_pool->chan_pri_tab[chan_pool->chan_pri_tab_index][0];
    chan_pool->chan_pri_tab_index++;
    dec_chan_unlock_chan_pool(chan_pool, &pool_lock_flag);

    return;
}

STATIC VFMW_ACC_BASE td_s32 dec_chan_handle_task(dec_chan *chan, dec_chan_task *task)
{
    vfmw_check_ret(chan != TD_NULL, DEC_ERR);
    vfmw_check_ret(task != TD_NULL, DEC_ERR);

    chan->wait_dev = 0;

    /* handle task */
    if (dec_dev_wait_hardware(chan, task) != DEC_OK) {
        dec_chan_print_block(chan->chan_id, "wait mdma");
        chan->wait_dev = 1;
        return DEC_ERR;
    }

    if (dec_chan_pre_process_task(chan, task) != DEC_OK) {
        return DEC_ERR;
    }

    if (dec_dev_acquire_reg(chan->chan_id, task->dev_id, &task->reg_id) != DEC_OK) {
        dec_chan_print_block(chan->chan_id, "no reg");
        return DEC_ERR;
    }

    return DEC_OK;
}

STATIC td_void dec_chan_start_dec_task(dec_chan *chan, dec_chan_task *task)
{
    dec_entry *entry = TD_NULL;

    entry = dec_ctrl_get_entry();

    vfmw_check(pdt_add_sleep_veto(VFMW_ID_VDH) == EXT_SUCCESS);
    vfmw_check_goto(pdt_add_sleep_apb_veto(VFMW_ID_VDH) == EXT_SUCCESS, err);
    dec_dev_acquire_cfg_id(task->dev_id, &task->cfg_id);
    dec_dev_config_reg(task->dev_id, task->reg_id, chan->dec_ops, task);
    dec_chan_upd_next_cfg_task(chan);
    dec_chan_decrease_cfg_task_num(chan);
    dec_chan_increase_dec_task_num(chan);
    dec_chan_set_task_state(task, DEC_TASK_DECODING);
#ifdef VFMW_IPT_SUPPORT
    ipt_sr_routine(chan, task);
#endif

    if (entry->bypass_chan == chan->chan_id) {
        dec_dev_bypass_process(task->dev_id);
    } else {
        dec_dev_start_reg(task->dev_id, task->reg_id); /* may move to just after config_reg to start VDH as early */
    }
    pdt_remove_sleep_apb_veto(VFMW_ID_VDH);
    return;
err:
    pdt_remove_sleep_veto(VFMW_ID_VDH);
    return;
}

STATIC VFMW_ACC_BASE td_s32 dec_chan_process_single_chan(td_s32 next_chan)
{
    td_u32 ret = DEC_ERR;
    td_ulong lock_flag;
    dec_chan *chan = TD_NULL;
    dec_chan_task *task = TD_NULL;
    dec_task_state task_state;

    vfmw_check_ret(os_get_time_us != TD_NULL, DEC_ERR);
    dec_chan_lock_chan(next_chan, &lock_flag);
    chan = dec_chan_get_chan(next_chan);
    if (dec_chan_check_chan_valid(chan) != DEC_OK) {
        dec_chan_unlock_chan(next_chan, &lock_flag);
        return DEC_OK;
    }

    /* get task */
    task = dec_chan_get_dec_task(chan);
    if (task == TD_NULL) {
        td_u16 dev_id = 0;
        dec_dev_acquire_dev(chan->is_sec, TD_NULL, &dev_id);
        if (dec_dev_time_out(dev_id) != DEC_OK) { /* no task to handle, check device time out */
            dprint(PRN_ERROR, "%s Error, Dec Chan %d TimeOut handle failed.\n", __func__, chan->chan_id);
        }
        dec_chan_print_block(chan->chan_id, "no task");
        dec_chan_unlock_chan(next_chan, &lock_flag);
        return DEC_OK;
    }

    task_state = task->state;

    if (dec_chan_handle_task(chan, task) != DEC_OK) {
        goto round_over;
    }

    dec_chan_start_dec_task(chan, task);

    if (dcheck(PRN_TIME)) {
        td_s32 cid = chan->chan_id;
        td_u32 fid = (td_u32)task->dps->unid;
        dbg_add_time(cid, fid, TPOS_QUE_DEC, os_get_time_us());
    }

    ret = DEC_OK;

round_over:
    if (dec_dev_time_out(task->dev_id) != DEC_OK) {
        dprint(PRN_ERROR, "%s Error, Dec Chan %d TimeOut handle failed 2\n", __func__, chan->chan_id);
    }

    dec_chan_unlock_chan(next_chan, &lock_flag);

    if (ret == DEC_OK) {
        dec_chan_update_tab_after_dec(next_chan);
        return DEC_ERR;
    } else if (task_state == DEC_TASK_MEMOK || task_state == DEC_TASK_CFGMSGOK) {
        return DEC_ERR; /* if vdh is busy(can't get reg or msg),service the same channel */
    }

    return DEC_OK;
}

VFMW_ACC_BASE td_void dec_chan_try_open_dev(td_void)
{
    td_s32 ret;
    dec_chan_type_num num = { 0 };
    dec_chan_get_chan_num(&num);

    /* for ree + tee */
    if (num.sec > 0 ||
        num.no_sec > 0) {
        ret = dec_dev_open(0);
        if (ret != DEC_OK) {
            vdec_record_chr_event(-1, MT_VDEC_KEY_ERROR, MT_VDEC_VFMW_KEY_ERROR, MT_VDEC_VFMW_VDH_OPEN_ERR);
        }
    }
}

VFMW_ACC_BASE td_void dec_chan_try_close_dev(td_void)
{
    dec_chan_type_num num = { 0 };

    dec_chan_get_chan_num(&num);

    if (num.no_sec == 0 && num.sec == 0) {
        dec_dev_close(0);
    }
}

VFMW_ACC_BASE td_void dec_chan_try_refresh_bin(td_void)
{
    if (dec_dev_refresh_bin() != DEC_OK) {
        dprint(PRN_ERROR, "refresh bin fail!\n");
    }
}

STATIC td_s32 dec_chan_get_next_chan_id(dec_chan_pool *chan_pool)
{
    dec_chan *prev_chan = TD_NULL;
    td_s32 next_chan_id;
    td_ulong lock_flag;

    if (chan_pool->curr_high_prior_chan >= DEC_MAX_CHAN_NUM) {
        dprint(PRN_ERROR, "DecChan chan_id error!\n");
        return DEC_MAX_CHAN_NUM;
    }

    dec_chan_lock_chan(chan_pool->curr_high_prior_chan, &lock_flag);

    prev_chan = dec_chan_get_chan(chan_pool->curr_high_prior_chan);
    if (prev_chan != TD_NULL && prev_chan->wait_dev != 0) {
        next_chan_id = prev_chan->chan_id;
    } else {
        next_chan_id = DEC_MAX_CHAN_NUM;
    }

    dec_chan_unlock_chan(chan_pool->curr_high_prior_chan, &lock_flag);

    return next_chan_id;
}

VFMW_ACC_BASE td_void dec_chan_routine(td_void)
{
    td_ulong pool_lock_flag;
    td_s32 next_chan;
    td_u32 started_chan_num;
    dec_chan_pool *chan_pool = TD_NULL;

    chan_pool = dec_ctrl_get_chan_pool();
    dec_chan_lock_chan_pool(chan_pool, &pool_lock_flag);
    started_chan_num = chan_pool->started_chan_num;
    chan_pool->chan_pri_tab_index = 0;
    dec_chan_unlock_chan_pool(chan_pool, &pool_lock_flag);

    dec_chan_try_open_dev();
    dec_chan_try_refresh_bin();

    while ((started_chan_num--) != 0) {
        next_chan = dec_chan_get_next_chan_id(chan_pool);
        if (next_chan >= DEC_MAX_CHAN_NUM) {
            /* get high prior chan id */
            dec_chan_get_high_prior_chan(chan_pool);
            next_chan = chan_pool->curr_high_prior_chan;
        }

        if (next_chan >= DEC_MAX_CHAN_NUM) {
            continue;
        }

        (td_void)dec_chan_process_single_chan(next_chan);
    }

    dec_chan_try_close_dev();
}
