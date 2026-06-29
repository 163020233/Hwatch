/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#include "dec_hal.h"
#include "dec_dev.h"
#include "dec_chn.h"
#include "fsp.h"
#include "vfmw_osal.h"
#include "vfmw_define.h"

#ifdef VFMW_HEVC_SUPPORT
#include "vdh_h265.h"
#endif
#ifdef VFMW_H264_SUPPORT
#include "vdh_h264.h"
#endif

#define DEC_IRQ_NUM 2
#define UHD_PIX_WIDTH_4096  4096

typedef struct {
    td_s32 irq_handle[DEC_IRQ_NUM];
    dec_dev_vdh vdh[VDH_NUM];
    os_event serial_event;
} dec_hal_ctx;

STATIC dec_hal_ctx *g_dec_hal_ctx = TD_NULL;

STATIC dec_hal_ops g_dec_hal_ops[] = {
#ifdef VFMW_H264_SUPPORT
    {
        .write_msg = hal_h264_write_msg,
        .process_slice = hal_h264_process_slice,
        .make_slice_ref = hal_h264_make_slice_ref,
        .get_rep_param = hal_h264_get_rep_param,
        .fill_ext_mem = hal_h264_fill_ext_mem,
        .vid_std = VFMW_H264,
    },
#endif

#ifdef VFMW_HEVC_SUPPORT
    {
        .write_msg = hal_h265_write_msg,
        .process_slice = hal_h265_process_slice,
        .make_slice_ref = hal_h265_make_slice_ref,
        .get_rep_param = hal_h265_get_rep_param,
        .fill_ext_mem = hal_h265_fill_ext_mem,
        .vid_std = VFMW_HEVC,
    },
#endif
};

STATIC td_u32 dec_hal_calc_vdh_up_msg_mem_size(td_u32 up_msg_size)
{
    td_u32 size;

    up_msg_size += FRM_UP_MSG_LEN; /*  for logic bug  */
    size = vfmw_align_up(up_msg_size, DEC_DEV_VDH_UP_MSG_ALIGN);
    size += DEC_DEV_VDH_UP_MSG_ALIGN * (VDH_BSP_NUM - 1);

    return size;
}

/*  open and close  */
STATIC td_u32 dec_hal_calc_vdh_msg_mem_size(td_void)
{
    td_u32 size = 0;

    size += VDH_PUB_MSG_SLOT_LEN;
    size += VDH_CORE_MSG_SLOT_LEN;
    size += VDH_PIC_MSG_SLOT_LEN;
    size += HEVC_MAX_SLICE_NUM * HEVC_SLICE_MSG_SLOT_LEN;
    size += dec_hal_calc_vdh_up_msg_mem_size(HEVC_MAX_SLICE_NUM * DEC_BASE_UPMSG_SLOT_LEN);

#ifdef VFMW_HEVC_SUPPORT
    size += HEVC_TILE_INFO_LEN;
#endif

    size = vfmw_max(size, sizeof(h265_tile));
    size = vfmw_align_up(size, 128); /* 128 is for alignment */

    return size;
}

STATIC td_u32 dec_hal_calc_base_ext_mem(vdh_ext_mem_info *vdh_mem, UADDR base_addr, dec_hal_ext_mem_type type)
{
    UADDR phy_addr = base_addr;

    if (type == DEC_HAL_EXT_MEM_DYNAMIC) {
        arrange_ptr(phy_addr, vfmw_align_up(OLD_SED_TOP_ADDR_LEN, VDH_ADDR_ALIGN_16), vdh_mem->old_sed_top_addr);
    } else {
        arrange_ptr(phy_addr, vfmw_align_up(OLD_PMV_TOP_ADDR_LEN, VDH_ADDR_ALIGN_16), vdh_mem->old_pmv_top_addr);
        arrange_ptr(phy_addr, vfmw_align_up(OLD_RCN_TOP_ADDR_LEN, VDH_ADDR_ALIGN_16), vdh_mem->old_rcn_top_addr);
        arrange_ptr(phy_addr, vfmw_align_up(OLD_DBLK_TOP_ADDR_LEN, VDH_ADDR_ALIGN_16), vdh_mem->old_dblk_top_addr);
    }

    return (td_u32)(phy_addr - base_addr);
}

STATIC void dec_hal_free_vdh_msg_mem(vdh_msg_info *msg)
{
    mem_record mem = { 0 };

    if (msg->base_vir_addr != TD_NULL) {
        mem.dma_buf = msg->dma_buf;
        mem.phy_addr = msg->base_phy_addr;
        mem.vir_addr = msg->base_vir_addr;
        mem.length = msg->base_mem_len;
        mem.is_cached = 0;
        os_free(&mem);
    }

    vfmw_check_sec_func(memset_s(msg, sizeof(vdh_msg_info), 0, sizeof(vdh_msg_info)));

    return;
}

STATIC td_s32 dec_hal_alloc_vdh_msg_mem_protocol(UADDR phy_addr, td_u8 *vir_addr, vdh_msg_info *msg,
    const mem_record *mem)
{
#ifdef VFMW_HEVC_SUPPORT
    arrange_ptr(phy_addr, HEVC_TILE_INFO_LEN, msg->tile_info_phy_addr);
    arrange_ptr(vir_addr, HEVC_TILE_INFO_LEN, msg->tile_info_vir_addr);
#endif

    if (phy_addr - mem->phy_addr > (UADDR)mem->length || (td_u32)(vir_addr - mem->vir_addr) > mem->length) {
        return DEC_ERR;
    }

    return DEC_OK;
}

STATIC td_s32 dec_hal_alloc_vdh_msg_mem(dec_dev_info *dev, vdh_msg_info *msg)
{
    mem_record mem = { 0 };
    UADDR phy_addr;
    td_u8 *vir_addr = TD_NULL;
    td_u32 size;
    td_u32 up_msg_size;
    td_u32 slc_msg_size;

    size = dec_hal_calc_vdh_msg_mem_size();

    /*  vfmw_vdh_msg  */
    mem.length = vfmw_align_up(size, 128); /* 128 is for alignment */
    mem.is_cached = 0;
    mem.type = DEV_HAL_MEM_TYPE;
    mem.unid = dev->dev_id;
    mem.mem_name = "vfmw_vdh_msg";
    if (os_malloc(&mem) != OSAL_OK) {
        goto exit;
    }
    phy_addr = mem.phy_addr;
    vir_addr = mem.vir_addr;
    msg->dma_buf = mem.dma_buf;
    msg->base_phy_addr = phy_addr;
    msg->base_vir_addr = vir_addr;
    msg->base_mem_len = mem.length;

    arrange_ptr(phy_addr, VDH_PUB_MSG_SLOT_LEN, msg->pub_msg_phy_addr);
    arrange_ptr(vir_addr, VDH_PUB_MSG_SLOT_LEN, msg->pub_msg_vir_addr);

    arrange_ptr(phy_addr, VDH_CORE_MSG_SLOT_LEN, msg->core_msg_phy_addr);
    arrange_ptr(vir_addr, VDH_CORE_MSG_SLOT_LEN, msg->core_msg_vir_addr);

    arrange_ptr(phy_addr, VDH_PIC_MSG_SLOT_LEN, msg->pic_msg_phy_addr);
    arrange_ptr(vir_addr, VDH_PIC_MSG_SLOT_LEN, msg->pic_msg_vir_addr);

    slc_msg_size = HEVC_SLICE_MSG_SLOT_LEN * HEVC_MAX_SLICE_NUM;
    arrange_ptr(phy_addr, slc_msg_size, msg->slice_msg_phy_addr);
    arrange_ptr(vir_addr, slc_msg_size, msg->slice_msg_vir_addr);
    msg->slice_msg_all_size = slc_msg_size;
    msg->slice_msg_one_size = HEVC_SLICE_MSG_SLOT_LEN;

    up_msg_size = dec_hal_calc_vdh_up_msg_mem_size(HEVC_MAX_SLICE_NUM * DEC_BASE_UPMSG_SLOT_LEN);
    arrange_ptr(phy_addr, up_msg_size, msg->up_msg_phy_addr);
    arrange_ptr(vir_addr, up_msg_size, msg->up_msg_vir_addr);
    msg->slice_up_msg_all_size = up_msg_size;

    if (dec_hal_alloc_vdh_msg_mem_protocol(phy_addr, vir_addr, msg, &mem) == DEC_ERR) {
        goto exit;
    }

    return DEC_OK;
exit:
    dprint(PRN_ERROR, "Malloc VDH message memory error!\n");
    dec_hal_free_vdh_msg_mem(msg);

    return DEC_ERR;
}

STATIC void dec_hal_free_vdh_ext_mem(vdh_ext_mem_info *vdh_mem)
{
    mem_record mem = { 0 };

    if (vdh_mem->base_vir_addr != TD_NULL) {
        mem.phy_addr = vdh_mem->base_phy_addr;
        mem.vir_addr = vdh_mem->base_vir_addr;
        mem.length = vdh_mem->base_mem_len;
        mem.is_cached = 0;
        os_free(&mem);
    }

    if (vdh_mem->dyn_ram_vir_addr != TD_NULL) {
        mem.phy_addr = vdh_mem->dyn_ram_phy_addr;
        mem.vir_addr = vdh_mem->dyn_ram_vir_addr;
        mem.length = vdh_mem->dyn_ram_mem_len;
        mem.is_cached = 0;
        os_free(&mem);
    }

    vfmw_check_sec_func(memset_s(vdh_mem, sizeof(vdh_ext_mem_info), 0, sizeof(vdh_ext_mem_info)));

    return;
}
STATIC td_s32 dec_hal_get_basic_size(td_void)
{
    td_s32 basic_used = 0;

#ifdef VFMW_H264_SUPPORT
    basic_used += vfmw_align_up(H264_CA_MN_ADDR_LEN, VDH_ADDR_ALIGN_16);
#endif

#ifdef VFMW_HEVC_SUPPORT
    basic_used += vfmw_align_up(HEVC_CA_MN_ADDR_LEN, VDH_ADDR_ALIGN_16);
#endif

    return basic_used;
}

STATIC td_void dec_hal_get_align_phy(vdh_ext_mem_info *vdh_mem, UADDR *phy_addr, dec_hal_ext_mem_type type)
{
    vfmw_check(vdh_mem != TD_NULL);
    vfmw_check(phy_addr != TD_NULL);

    if (type == DEC_HAL_EXT_MEM_DYNAMIC) {
        return;
    }
#ifdef VFMW_H264_SUPPORT
    arrange_ptr(*phy_addr, vfmw_align_up(H264_CA_MN_ADDR_LEN, VDH_ADDR_ALIGN_16), vdh_mem->h264_mn_phy_addr);
#endif

#ifdef VFMW_HEVC_SUPPORT
    arrange_ptr(*phy_addr, vfmw_align_up(HEVC_CA_MN_ADDR_LEN, VDH_ADDR_ALIGN_16), vdh_mem->h265_mn_phy_addr);
#endif
}

STATIC td_void dec_hal_alloc_vdh_set_vdh_mem(const dec_dev_info *dev,
    vdh_ext_mem_info *vdh_mem, const mem_record *mem, dec_hal_ext_mem_type type)
{
    vdh_mem->dev_id = dev->dev_id;
    if (type == DEC_HAL_EXT_MEM_STATIC) {
        vdh_mem->base_phy_addr = mem->phy_addr;
        vdh_mem->base_vir_addr = mem->vir_addr;
        vdh_mem->base_mem_len = mem->length;
    } else {
        vdh_mem->dyn_ram_phy_addr = mem->phy_addr;
        vdh_mem->dyn_ram_vir_addr = mem->vir_addr;
        vdh_mem->dyn_ram_mem_len = mem->length;
    }
}

STATIC td_s32 dec_hal_alloc_vdh_ext_mem(dec_dev_info *dev, vdh_ext_mem_info *vdh_mem,
    dec_hal_ext_mem_type type)
{
    UADDR phy_addr;
    td_s32 i;
    td_s32 std_num;
    td_s32 mem_used;
    td_s32 mem_total;
    td_s32 max_used = 0;
    td_s32 basic_used = 0;
    dec_hal_ops *ops = TD_NULL;
    mem_record mem = { 0 };

    vfmw_check_ret(dev != TD_NULL, DEC_ERR);
    vfmw_check_ret(vdh_mem != TD_NULL, DEC_ERR);

    if (type == DEC_HAL_EXT_MEM_STATIC) {
        basic_used = dec_hal_get_basic_size();
        vfmw_check_sec_func(memset_s(vdh_mem, sizeof(vdh_ext_mem_info), 0, sizeof(vdh_ext_mem_info)));
    }

    max_used = (td_s32)dec_hal_calc_base_ext_mem(vdh_mem, 1, type);
    std_num = sizeof(g_dec_hal_ops) / sizeof(dec_hal_ops);
    for (i = 0; i < std_num; i++) {
        ops = g_dec_hal_ops + i;
        if (ops->fill_ext_mem != TD_NULL) {
            mem_used = ops->fill_ext_mem(vdh_mem, 1, type);
            max_used = vfmw_max(mem_used, max_used);
        }
    }
    mem_total = basic_used + max_used;
    /*  vfmw vdh ext mem  */
    mem.length = (td_u32)vfmw_align_up((td_u32)mem_total, 128); /* 128 is for alignment */
    mem.is_cached = 0;
    mem.type = DEV_HAL_MEM_TYPE;
    mem.unid = dev->dev_id;
    if (type == DEC_HAL_EXT_MEM_STATIC) {
        mem.mem_name = "vfmw_vdh_ext";
    } else {
        mem.mem_name = "vfmw_vdh_ext_dyn";
    }
    if (os_malloc(&mem) != OSAL_OK) {
        return DEC_ERR;
    }

    phy_addr = mem.phy_addr;
    dec_hal_alloc_vdh_set_vdh_mem(dev, vdh_mem, &mem, type);
    dec_hal_get_align_phy(vdh_mem, &phy_addr, type);
    dec_hal_calc_base_ext_mem(vdh_mem, phy_addr, type);
    for (i = 0; i < std_num; i++) {
        ops = g_dec_hal_ops + i;
        if (ops->fill_ext_mem != TD_NULL) {
            ops->fill_ext_mem(vdh_mem, phy_addr, type);
        }
    }

    return DEC_OK;
}

STATIC td_s32 dec_hal_alloc_vdh_mem(dec_dev_info *dev)
{
    dec_dev_vdh *vdh = (dec_dev_vdh *)dev->handle;
    vdh_msg_pool_info *msg_pool = &vdh->vdh_msg_pool;
    vdh_ext_mem_info *vdh_mem = &vdh->vdh_ext_mem;
    vdh_msg_info *msg = TD_NULL;
    td_u16 msg_id;

    for (msg_id = 0; msg_id < msg_pool->msg_num; msg_id++) {
        msg = &msg_pool->vdh_msg[msg_id];
        if (dec_hal_alloc_vdh_msg_mem(dev, msg) != DEC_OK) {
            goto err;
        }
    }

    if (dec_hal_alloc_vdh_ext_mem(dev, vdh_mem, DEC_HAL_EXT_MEM_STATIC) != DEC_OK) {
        goto err;
    }

    return DEC_OK;

err:
    for (msg_id = 0; msg_id < msg_pool->msg_num; msg_id++) {
        msg = &msg_pool->vdh_msg[msg_id];
        dec_hal_free_vdh_msg_mem(msg);
    }

    return DEC_ERR;
}

STATIC void dec_hal_free_vdh_mem(dec_dev_info *dev)
{
    dec_dev_vdh *vdh = (dec_dev_vdh *)dev->handle;
    vdh_msg_pool_info *msg_pool = &vdh->vdh_msg_pool;
    vdh_ext_mem_info *vdh_mem = &vdh->vdh_ext_mem;
    vdh_msg_info *msg = TD_NULL;
    td_u16 msg_id;

    for (msg_id = 0; msg_id < msg_pool->msg_num; msg_id++) {
        msg = &msg_pool->vdh_msg[msg_id];
        dec_hal_free_vdh_msg_mem(msg);
    }

    dec_hal_free_vdh_ext_mem(vdh_mem);

    return;
}

STATIC td_s32 dec_hal_create_vdh_inst(dec_dev_info *dev)
{
    td_void *vir = TD_NULL;
    mem_record slc_bin;

    if (os_event_init(&g_dec_hal_ctx->serial_event, 0) != DEC_OK) {
        dprint(PRN_FATAL, "%s %d os_event_init err!\n", __func__, __LINE__);
        return DEC_ERR;
    }

    vir = &g_dec_hal_ctx->vdh[dev->dev_id];

    dev->handle = (td_void *)vir;

    slc_bin = g_dec_hal_ctx->vdh[dev->dev_id].slc_bin;

    vfmw_check_sec_func(memset_s(vir, sizeof(dec_dev_vdh), 0, sizeof(dec_dev_vdh)));

    g_dec_hal_ctx->vdh[dev->dev_id].slc_bin = slc_bin;

    return DEC_OK;
}

STATIC void dec_hal_destroy_vdh_inst(dec_dev_info *dev)
{
    dev->handle = TD_NULL;
    os_event_exit(g_dec_hal_ctx->serial_event);

    return;
}

STATIC td_s32 dec_hal_config_vdh(dec_dev_info *dev)
{
    dec_dev_vdh *vdh = (dec_dev_vdh *)dev->handle;
    td_u8 *base_vir_addr = TD_NULL;
    td_u8 *mmu_vir_addr = TD_NULL;
    td_u32 i;
    UADDR phy_addr;
    const td_s32 is_sec = 0;

    phy_addr = VDM_REG_PHY_ADDR;
    base_vir_addr = os_kmap_reg(phy_addr, VDH_REGISTER_LEN);
    if (base_vir_addr == TD_NULL) {
        return DEC_ERR;
    }

    vdh->vdh_reg_pool.reg_num = VDH_REG_NUM;
    vdh->vdh_msg_pool.msg_num = VDH_MSG_NUM;

    for (i = 0; i < vdh->vdh_reg_pool.reg_num; i++) {
        vdh->vdh_reg_pool.vdh_reg[i].reg_id = i;
        vdh->vdh_reg_pool.vdh_reg[i].is_sec = is_sec;
        vdh->vdh_reg_pool.vdh_reg[i].reg_phy_addr = phy_addr;
        vdh->vdh_reg_pool.vdh_reg[i].reg_vir_addr = base_vir_addr;
        vdh->vdh_reg_pool.vdh_reg[i].base_ofs[0] = OFS_BASE1;
        vdh->vdh_reg_pool.vdh_reg[i].base_ofs[1] = OFS_BASE2;
        vdh->vdh_reg_pool.vdh_reg[i].mmu_vir_addr = mmu_vir_addr;
    }

    return DEC_OK;
}

STATIC void dec_hal_clear_vdh(dec_dev_info *dev)
{
    dec_dev_vdh *vdh = (dec_dev_vdh *)dev->handle;
    td_u8 *base_vir_addr = TD_NULL;
    td_u8 *mmu_vir_addr = TD_NULL;
    td_u32 i;

    base_vir_addr = vdh->vdh_reg_pool.vdh_reg[0].reg_vir_addr;
    if (base_vir_addr != TD_NULL) {
        os_kunmap_reg(base_vir_addr);
    }

    mmu_vir_addr = vdh->vdh_reg_pool.vdh_reg[0].mmu_vir_addr;
    if (mmu_vir_addr != TD_NULL) {
        os_kunmap_reg(mmu_vir_addr);
    }

    for (i = 0; i < vdh->vdh_reg_pool.reg_num; i++) {
        vdh->vdh_reg_pool.vdh_reg[i].reg_phy_addr = 0;
        vdh->vdh_reg_pool.vdh_reg[i].reg_vir_addr = TD_NULL;
        vdh->vdh_reg_pool.vdh_reg[i].crg_phy_addr = 0;
        vdh->vdh_reg_pool.vdh_reg[i].crg_vir_addr = TD_NULL;
        vdh->vdh_reg_pool.vdh_reg[i].mmu_vir_addr = TD_NULL;
    }

    vdh->vdh_reg_pool.reg_num = 0;
    vdh->vdh_msg_pool.msg_num = 0;

    return;
}

STATIC td_void dec_hal_open_vdh_clock(const dec_dev_info *dev)
{
    vfmw_unused(dev);

    return;
}

STATIC td_void dec_hal_close_vdh_clock(const dec_dev_info *dev)
{
    vfmw_unused(dev);

    return;
}

STATIC void dec_hal_enable_vdh(const dec_dev_info *dev)
{
    td_u32 pxp_reg_id = 0;
    vdh_reg_info *reg = TD_NULL;
    dec_dev_vdh *vdh = (dec_dev_vdh *)dev->handle;
    vdh_reg_pool_info *reg_pool = &vdh->vdh_reg_pool;

    dev->ops->get_reg(dev, 0, &reg);
    rd_vreg(reg->reg_vir_addr + reg->base_ofs[0], VREG_OFS_BSP_CUR_REG, pxp_reg_id);

    if (pxp_reg_id < VDH_REG_NUM) {
        reg_pool->next_cfg_reg = pxp_reg_id;
        reg_pool->next_bsp_reg = pxp_reg_id;
        reg_pool->next_pxp_reg = pxp_reg_id;
    }

    return;
}

STATIC void dec_hal_disable_vdh(dec_dev_info *dev)
{
    vfmw_unused(dev);

    return;
}

STATIC td_s32 dec_hal_get_vdh(td_void)
{
    return DEC_OK;
}

STATIC td_s32 dec_hal_put_vdh(td_void)
{
    return DEC_OK;
}

STATIC td_bool dec_hal_check_std_is_conflict(dec_dev_info *dev, const td_u16 cur_reg_id)
{
    dec_dev_vdh *vdh = TD_NULL;
    dec_chan *cur_chan = TD_NULL;

    vdh = (dec_dev_vdh *)dev->handle;
    cur_chan = dec_chan_get_chan(vdh->vdh_reg_pool.vdh_reg[cur_reg_id].chan_id);
    if (cur_chan == TD_NULL) {
        dprint(PRN_ERROR, "%s %d chan is null!\n", __func__, __LINE__);
        return TD_TRUE;
    }

    if ((dev->prev_reg_vid_std == VFMW_HEVC) && (cur_chan->chan_cfg.vid_std != VFMW_HEVC)) {
        dev->prev_reg_vid_std = cur_chan->chan_cfg.vid_std;
        return TD_TRUE;
    }

    dev->prev_reg_vid_std = cur_chan->chan_cfg.vid_std;
    return TD_FALSE;
}

/* logic error for serial, reset VDH if std conflict */
STATIC td_s32 dec_hal_reset_vdh_in_multichan(dec_dev_info *dev, const dec_hal_ops *ops, dec_chan_task *task)
{
    td_s32 ret;
    td_u32 ofs, chan_id, d32, retry_cnt;
    td_u32 *vir = TD_NULL;
    td_u16 prev_reg_id;
    dec_dev_vdh *vdh = TD_NULL;
    vdh_reg_pool_info *reg_pool = TD_NULL;
    vdh_reg_info *prev_reg = TD_NULL;

    vfmw_unused(ops);
    vfmw_check_ret(dev != TD_NULL, DEC_ERR);
    vdh = (dec_dev_vdh *)dev->handle;
    vfmw_check_ret(vdh != TD_NULL, DEC_ERR);
    vfmw_check_ret(task != TD_NULL, DEC_ERR);
    vfmw_check_ret(dev->ops != TD_NULL, DEC_ERR);

    if (dec_hal_check_std_is_conflict(dev, task->reg_id) == TD_FALSE) {
        return DEC_OK;
    }

    reg_pool = &vdh->vdh_reg_pool;
    prev_reg_id = (task->reg_id + reg_pool->reg_num - 1) % reg_pool->reg_num;
    if ((VREG_OFS_BETWEEN_REG * prev_reg_id) < VFMW_MAX_SINT32) {
        ofs = (td_s32)(VREG_OFS_BETWEEN_REG * prev_reg_id);
    } else {
        dprint(PRN_ERROR, "%s %d overflow!\n", __func__, __LINE__);
        return DEC_ERR;
    }

    dev->ops->get_reg(dev, prev_reg_id, &prev_reg);
    retry_cnt = 0;
    while (retry_cnt < 0xff) {  // max wait 512ms
        vir = (td_u32 *)(prev_reg->reg_vir_addr + prev_reg->base_ofs[1]);
        rd_vreg(vir, VREG_OFS_INFO_STATE + ofs, d32);
        if (((vreg_info_state *)(&d32))->vdec_busy_state != 1) {
            chan_id = vdh->vdh_reg_pool.vdh_reg[task->reg_id].chan_id;
            ret = dev->ops->reset(dev);
            vfmw_check_ret(ret == DEC_OK, DEC_ERR);
            dec_dev_release_reg(dev->dev_id, task->reg_id);
            dec_dev_acquire_reg(chan_id, dev->dev_id, &task->reg_id);
            break;
        }
        os_event_wait(g_dec_hal_ctx->serial_event, 2); // wait 2ms
        retry_cnt++;
    }
    if (retry_cnt == 0xff) {
        dprint(PRN_ALWS, "%s %d wait prev reg:%d finish to reset VDH failed!\n", __func__, __LINE__, prev_reg_id);
        return DEC_ERR;
    }

    return DEC_OK;
}

STATIC td_s32 dec_hal_init_ctx(td_void)
{
    g_dec_hal_ctx = (dec_hal_ctx *)os_alloc_vir(0, "dec_hal", sizeof(dec_hal_ctx));
    if (g_dec_hal_ctx == TD_NULL) {
        dprint(PRN_FATAL, "alloc g_dec_hal_ctx fail!\n");
        return DEC_ERR;
    }
    (td_void)memset_s(g_dec_hal_ctx, sizeof(dec_hal_ctx), 0, sizeof(dec_hal_ctx));
    return DEC_OK;
}

STATIC td_void dec_hal_exit_ctx(td_void)
{
    if (g_dec_hal_ctx != TD_NULL) {
        os_free_vir(g_dec_hal_ctx);
        g_dec_hal_ctx = TD_NULL;
    }
}

/*  interface  */
td_s32 dec_hal_get_ops(vfmw_vid_std vid_std_info, dec_hal_ops **ops)
{
    td_u32 ops_ldx;

    if (ops == TD_NULL) {
        return DEC_ERR;
    }

    for (ops_ldx = 0; ops_ldx < (sizeof(g_dec_hal_ops) / sizeof(g_dec_hal_ops[0])); ops_ldx++) {
        if (g_dec_hal_ops[ops_ldx].vid_std == vid_std_info) {
            *ops = &g_dec_hal_ops[ops_ldx];
            return DEC_OK;
        }
    }
    dprint(PRN_ERROR, "%s the vid_std_info %d not Support!\n", __func__, vid_std_info);

    return DEC_ERR;
}

td_s32 dec_hal_open_vdh(td_void *dec_dev)
{
    dec_dev_info *dev = (dec_dev_info *)dec_dev;

    if (dec_hal_init_ctx() != DEC_OK) {
        return DEC_ERR;
    }

    if (dec_hal_create_vdh_inst(dev) != DEC_OK) {
        goto exit;
    }

    if (dec_hal_config_vdh(dev) != DEC_OK) {
        goto exit;
    }

    if (dec_hal_alloc_vdh_mem(dev) != DEC_OK) {
        goto exit;
    }

    dec_hal_open_vdh_clock(dev);

    dec_hal_enable_vdh(dev);

    if (dec_hal_put_vdh() != DEC_OK) {
        dprint(PRN_ERROR, "%s free vdh lock err\n", __func__);
    }

    return DEC_OK;

exit:
    dec_hal_exit_ctx();
    dprint(PRN_ERROR, "%s failed!\n", __func__);

    return DEC_ERR;
}

td_s32 dec_hal_close_vdh(td_void *dec_dev)
{
    td_s32 ret;
    dec_dev_info *dev = (dec_dev_info *)dec_dev;

    ret = DEC_OK;

    if (dec_hal_put_vdh() != DEC_OK) {
        dprint(PRN_ERROR, "%s free vdh lock err\n", __func__);
    }

    dec_hal_disable_vdh(dev);

    dec_hal_close_vdh_clock(dev);

    dec_hal_free_vdh_mem(dev);

    dec_hal_clear_vdh(dev);

    dec_hal_destroy_vdh_inst(dev);

    dec_hal_exit_ctx();

    return ret;
}

td_s32 dec_hal_reset_vdh(const td_void *dec_dev)
{
    const dec_dev_info *dev = (dec_dev_info *)dec_dev;
    vdh_reg_info *reg = TD_NULL;
    dec_dev_vdh *vdh = TD_NULL;
    vdh_reg_pool_info *reg_pool = TD_NULL;
    td_u32 *vir = TD_NULL;
    td_u32 d32, int_mask[2]; /* 2 is length */
    td_u32 i;

    vfmw_check_ret(dev != TD_NULL, DEC_ERR);
    vfmw_check_ret(dev->ops != TD_NULL, DEC_ERR);
    dev->ops->get_reg(dev, 0, &reg);

    vir = (td_u32 *)(reg->reg_vir_addr + reg->base_ofs[0]);

    vfmw_check_ret(pdt_add_sleep_veto(VFMW_ID_VDH) == EXT_SUCCESS, DEC_ERR);
    vfmw_check_goto(pdt_add_sleep_apb_veto(VFMW_ID_VDH) == EXT_SUCCESS, err1);

    rd_vreg(vir, VREG_OFS_BSP_INTR_MASK, int_mask[0]);
    rd_vreg(vir, VREG_OFS_PXP_INTR_MASK, int_mask[1]);

    rd_vreg(reg->reg_vir_addr, VREG_OFS_VDH_CLKSEL, d32);

    if (((vcrg_vdh_clksel *)(&d32))->vdh_all_clken == 0) {
        goto err2;
    }

    rd_vreg(reg->reg_vir_addr, VREG_OFS_VDH_SRST_REQ, d32);
    ((vcrg_vdh_srst_req *)(&d32))->vdh_mfde_srst_req = 1;
    wr_vreg(reg->reg_vir_addr, VREG_OFS_VDH_SRST_REQ, d32);

    for (i = 0; i < 1000; i++) { /* 1000 :for loop count */
        os_udelay(30); /* 30 is designed for delay */

        rd_vreg(reg->reg_vir_addr, VREG_OFS_VDH_SRST_OK, d32);
        if (((vcrg_vdh_srst_req_ok *)(&d32))->vdh_mfde_srst_ok == 1) {
            break;
        }
    }

    if (i == 1000) { /* 1000 is the count of loop */
        dprint(PRN_ERROR, "reset VDH ERROR!!!\n");
    }

    rd_vreg(reg->reg_vir_addr, VREG_OFS_VDH_SRST_REQ, d32);
    ((vcrg_vdh_srst_req *)(&d32))->vdh_mfde_srst_req = 0;
    wr_vreg(reg->reg_vir_addr, VREG_OFS_VDH_SRST_REQ, d32);

    os_mb();

    wr_vreg(vir, VREG_OFS_BSP_INTR_MASK, int_mask[0]);
    wr_vreg(vir, VREG_OFS_PXP_INTR_MASK, int_mask[1]);

    rd_vreg(vir, VREG_OFS_BSP_CUR_REG, d32);
    if (d32 != 0) {
        dprint(PRN_ERROR, "reset But Cur Reg Not 0\n");
    }

    vdh = (dec_dev_vdh *)dev->handle;
    reg_pool = &vdh->vdh_reg_pool;

    reg_pool->next_cfg_reg = 0;
    reg_pool->next_bsp_reg = 0;
    reg_pool->next_pxp_reg = 0;

    pdt_remove_sleep_apb_veto(VFMW_ID_VDH);
    pdt_remove_sleep_veto(VFMW_ID_VDH);

    return DEC_OK;

err2:
    pdt_remove_sleep_apb_veto(VFMW_ID_VDH);
err1:
    pdt_remove_sleep_veto(VFMW_ID_VDH);
    return DEC_ERR;
}

td_s32 dec_hal_resume_vdh(const td_void *dev)
{
    td_u8 *vir_addr = TD_NULL;

    dec_hal_open_vdh_clock(dev);

    pdt_init_mmu_reg_addr(vir_addr);

    return EXT_SUCCESS;
}

td_s32 dec_hal_suspend_vdh(const td_void *dev)
{
    td_s32 ret;

    ret = DEC_OK;

    dec_hal_close_vdh_clock(dev);

    return ret;
}

typedef os_irqret (*fn_dec_dev_isr)(td_s32, td_void *);

td_s32 dec_hal_init_irq_vdh(const td_void *dev)
{
    td_s32 ret;
    const td_s32 irq_flag = 0x80;
    td_s32 pxp_irq_num = VDM_PXP_INT_NUM;
    STATIC const td_char irq_name[1][16] = { /* 2 and 16 are length */
        "vdh_norm",
    };

    fn_dec_dev_isr pxp_func = dec_dev_isr_pxp0;

    vfmw_unused(dev);

    ret = os_request_irq(pxp_irq_num, pxp_func, irq_flag, irq_name[0], /* 0 vdh id */
                         &g_dec_hal_ctx->irq_handle[0]); /* 0 vdh id */
    if (ret != OSAL_OK) {
        return DEC_ERR;
    }

    return DEC_OK;
}

td_s32 dec_hal_exit_irq_vdh(const td_void *dev)
{
    const td_char irq_name[1][16] = { /* 2 and 16 are length */
        "vdec_vdh",
    };
    td_s32 pxp_irq_num = VDM_PXP_INT_NUM;

    vfmw_unused(dev);

    os_free_irq(pxp_irq_num, irq_name[0], /* 0 vdh id */
        &g_dec_hal_ctx->irq_handle[0]); /* 0 vdh id */

    return DEC_OK;
}

td_void dec_hal_write_pub_msg(const td_void *vdh_msg, const td_void *dec_task, const td_void *dev_vdh)
{
    td_u32 d32;
    td_u32 *slot = TD_NULL;

    const vdh_msg_info *msg = (vdh_msg_info *)vdh_msg;
    const dec_dev_vdh *vdh = (dec_dev_vdh *)dev_vdh;
    const dec_chan_task *task = (dec_chan_task *)dec_task;
    const vdh_ext_mem_info *vdh_mem = TD_NULL;
    const dec_chan_mem_param *task_mem = TD_NULL;

    vfmw_check(msg != TD_NULL && vdh != TD_NULL && task != TD_NULL);
    vdh_mem = &(vdh->vdh_ext_mem);
    task_mem = &(task->mem_param);
    slot = (td_u32 *)(msg->pub_msg_vir_addr);

    /*  D2 Cfg By Std  */
    /*  D3 Cfg By Std  */
    d32 = (td_u32)hw_addr_shift(msg->pic_msg_phy_addr);
    wr_msgword(slot + 4, d32); /* 4 :deviation */

    d32 = (td_u32)hw_addr_shift(msg->up_msg_phy_addr);
    wr_msgword(slot + 5, d32); /* 5 :a number */

    /*  D6 Cfg By Std  */
    /*  part_dec_over_int_level  */
    wr_msgword(slot + 8, task_mem->tunl_line_num); /* 8 :deviation */

    d32 = (td_u32)hw_addr_shift(task_mem->luma_phy_addr);
    wr_msgword(slot + 9, d32); /* 9 :deviation */

    d32 = task_mem->y_stride;
    wr_msgword(slot + 10, d32); /* 10 :deviation */

    wr_msgword(slot + 11, task_mem->uv_stride); /* 11 :deviation */

    d32 = task_mem->uv_offset;
    wr_msgword(slot + 12, d32); /* 12 :deviation */

    d32 = task_mem->cmp_head_size;
    wr_msgword(slot + 13, d32); /* 13 :deviation */

    d32 = task_mem->nbit_y_stride;
    wr_msgword(slot + 14, d32); /* 14 :deviation */

    d32 = task_mem->nbit_y_offset;
    wr_msgword(slot + 15, d32); /* 15 :deviation */

    d32 = task_mem->nbit_uv_offset;
    wr_msgword(slot + 16, d32); /* 16 :deviation */

    d32 = 1 << 2;               /* 2 : check_sum */
    wr_msgword(slot + 17, d32); /* 17 :deviation */

    d32 = (td_u32)hw_addr_shift(task_mem->luma_phy_addr + task_mem->line_num_ofs);
    wr_msgword(slot + 20, d32); /* 20 :deviation */

    /*  D21 Cfg By New Std  */
    d32 = (task_mem->pxp_in_ram) ? 0 : (td_u32)hw_addr_shift(vdh_mem->old_sed_top_addr);
    wr_msgword(slot + 21, d32); /* 21 :deviation */

    /*  D22 Cfg By New Std  */
    d32 = (td_u32)hw_addr_shift(vdh_mem->old_pmv_top_addr);
    wr_msgword(slot + 22, d32); /* 22 :deviation */

    d32 = (td_u32)hw_addr_shift(task_mem->pmv_phy_addr);
    wr_msgword(slot + 23, d32); /* 23 :deviation */

    /*  D24 Cfg By New Std  */
    d32 = (td_u32)hw_addr_shift(vdh_mem->old_rcn_top_addr);
    wr_msgword(slot + 24, d32); /* 24 :deviation */

    /*  D25 Cfg By New Std  */
    /*  D26 Cfg By New Std  */
    /*  D27 Cfg By New Std  */
    /*  D28 Cfg By New Std  */
    /*  D29 Cfg By New Std  */
    /*  D30 Cfg By New Std  */
    d32 = (td_u32)hw_addr_shift(vdh_mem->old_dblk_top_addr);
    wr_msgword(slot + 30, d32); /* 30 is a number */

    /*  D31 Cfg By Std  */
    /*  D32 Cfg By Std  */
    /*  D34 Cfg By Std  */
    /*  D35 Cfg By Std  */
    return;
}

td_s32 dec_hal_config_msg_vdh(const td_void *dec_dev, const dec_hal_ops *ops, const td_void *dec_task)
{
    dec_dev_info *dev = (dec_dev_info *)dec_dev;
    dec_dev_vdh *vdh = TD_NULL;
    dec_chan_task *task = (dec_chan_task *)dec_task;
    vdh_msg_info *msg = TD_NULL;
    td_s32 ret;
    td_u32 d32 = 0;
    td_u32 *slot = TD_NULL;

    vfmw_check_ret(ops != TD_NULL, DEC_ERR);
    vfmw_check_ret(dec_dev != TD_NULL, DEC_ERR);
    vfmw_check_ret(task != TD_NULL, DEC_ERR);
    vfmw_check_ret(dev->ops != TD_NULL, DEC_ERR);
    vdh = (dec_dev_vdh *)dev->handle;
    ret = dev->ops->get_msg(dev, task->msg_id, &msg);
    if (ret != DEC_OK) {
        return DEC_ERR;
    }

    vfmw_check_ret(task->dps != TD_NULL, DEC_ERR);
    if (task->dps->is_frm_cpy == 1) {
        slot = (td_u32 *)msg->pub_msg_vir_addr;
        ((vmsg_com_b0_d3 *)(&d32))->vdh_mdma_sel = 1;  // 0:vdh  1:mdma
        wr_msgword(slot + 3, d32);      // 3 : D3

        d32 = hw_addr_shift(msg->up_msg_phy_addr);
        wr_msgword(slot + 5, d32);      // 5 : D5

        slot = (td_u32 *)msg->core_msg_vir_addr;
        d32 = hw_addr_shift(msg->slice_msg_phy_addr);
        wr_msgword(slot + 28, d32);     // 28 : D28
        vfmw_check_ret(ops->write_mdma_msg != TD_NULL, DEC_ERR);
        return ops->write_mdma_msg(msg->slice_msg_vir_addr, task->dps->pic_param, &task->mem_param, msg);
    }

    vfmw_check_ret(ops->write_msg != TD_NULL, DEC_ERR);
    ret = ops->write_msg(msg, task, vdh);
    if (ret != DEC_OK) {
        return DEC_ERR;
    }

    return DEC_OK;
}

STATIC void dec_hal_write_reg(vdh_reg_info *reg, dec_chan_task *task, const vdh_msg_info *msg)
{
    td_u32 d32;
    td_u32 *vir = TD_NULL;
    td_s32 ofs = 0;

    vfmw_check(reg != TD_NULL);
    vfmw_check(task != TD_NULL);
    vfmw_check(msg != TD_NULL);
    vfmw_check(reg->reg_vir_addr != TD_NULL);

    if ((VREG_OFS_BETWEEN_REG * reg->reg_id) < VFMW_MAX_SINT32) {
        ofs = (td_s32)(VREG_OFS_BETWEEN_REG * reg->reg_id);
    } else {
        dprint(PRN_ERROR, "%s %d overflow!\n", __func__, __LINE__);
    }
    vir = (td_u32 *)(reg->reg_vir_addr + reg->base_ofs[0]);

    d32 = task->mem_param.vdh_mmu_en << 6; /* move 6 bits left */
    wr_vreg(reg->reg_vir_addr, VREG_OFS_RST_EN, d32);

    wr_vreg(reg->reg_vir_addr, VREG_OFS_RFS_RAS_EMA, 0xb000b);

    wr_vreg(vir, VREG_OFS_BSP_INTR_MSKSTATE, 1);

    wr_vreg(vir, VREG_OFS_BSP_INTR_MASK, 1);

    wr_vreg(vir, VREG_OFS_PXP_INTR_MASK,  0xfffffffe);

    vir = (td_u32 *)(reg->reg_vir_addr + reg->base_ofs[1]);

    d32 = 0;
    ((vreg_info_state *)(&d32))->low_latency_en = (td_u32)(task->dps->ldy_mode == LDY_SED);
    ((vreg_info_state *)(&d32))->bsp_internal_ram = task->mem_param.bsp_in_ram;
    ((vreg_info_state *)(&d32))->pxp_internal_ram = task->mem_param.pxp_in_ram;
    ((vreg_info_state *)(&d32))->frame_indicator = task->cfg_id;
    ((vreg_info_state *)(&d32))->vdh_safe_flag = (reg->is_sec != 0) ? 1 : 0;

    wr_vreg(vir, VREG_OFS_INFO_STATE + ofs, d32);

    d32 = (td_u32)hw_addr_shift(msg->pub_msg_phy_addr);
    wr_vreg(vir, VREG_OFS_AVM_ADDR_CNT + ofs, d32);
}

td_void dec_hal_write_core_msg(const vdh_msg_info *msg, const td_void *task, const td_void *vdh)
{
    td_u32 i;
    td_u32 d32;
    td_u32 *slot = TD_NULL;
    dec_chan_task *dec_task = (dec_chan_task *)task;
    const dec_chan_slice_list *slice_list = TD_NULL;

    vfmw_unused(vdh);

    vfmw_check(dec_task != TD_NULL);
    vfmw_check(msg != TD_NULL);
    slice_list = &(dec_task->slice_list);
    slot = (td_u32 *)(msg->core_msg_vir_addr);
    vfmw_check(slot != TD_NULL);

    d32 = (td_u32)hw_addr_shift(msg->slice_bin_phy_addr);
    wr_msgword(slot + 0, d32);

    d32 = (td_u32)hw_addr_shift(msg->ctb_head_phy_addr);
    wr_msgword(slot + 5, d32); /* 5 :deviation */

    d32 = 0; // only vp9 av1
    wr_msgword(slot + 24, d32); /* 24 :a number */

    for (i = 0; i < slice_list->list_num; i++) {
        d32 = (td_u32)hw_addr_shift(slice_list->dn_msg_phy_addr[i]);
        wr_msgword(slot + 28 + i, d32); /* 28 :deviation */

        d32 = (td_u32)hw_addr_shift(slice_list->up_msg_phy_addr[i] + FRM_UP_MSG_LEN);
        wr_msgword(slot + 32 + i, d32); /* 32 :deviation */
    }

    d32 = slice_list->list_len[0];
    wr_msgword(slot + 36, d32); /* 36 :deviation */

    d32 = msg->slice_bin_length;
    wr_msgword(slot + 48, d32); /* 48 :deviation */

    d32 = msg->ctb_head_length;
    wr_msgword(slot + 52, d32); /* 52 :deviation */
}

td_s32 dec_hal_config_reg_vdh(const td_void *dec_dev, const dec_hal_ops *ops, const td_void *dec_task)
{
    dec_dev_info *dev = (dec_dev_info *)dec_dev;
    dec_dev_vdh *vdh = TD_NULL;
    dec_chan_task *task = (dec_chan_task *)dec_task;
    vdh_reg_info *reg = TD_NULL;
    vdh_msg_info *msg = TD_NULL;
    td_s32 ret;

    vfmw_unused(ops);
    vfmw_check_ret(dev != TD_NULL, DEC_ERR);
    vdh = (dec_dev_vdh *)dev->handle;
    vfmw_check_ret(vdh != TD_NULL, DEC_ERR);
    vfmw_check_ret(task != TD_NULL, DEC_ERR);
    vfmw_check_ret(dev->ops != TD_NULL, DEC_ERR);

    /* logic error for serial start */
    ret = dec_hal_reset_vdh_in_multichan(dev, ops, task);
    if (ret != DEC_OK) {
        return DEC_ERR;
    }
    /* logic error for serial end */
    ret = dev->ops->get_reg(dev, task->reg_id, &reg);
    if (ret != DEC_OK) {
        return DEC_ERR;
    }

    ret = dev->ops->get_msg(dev, task->msg_id, &msg);
    if (ret != DEC_OK) {
        return DEC_ERR;
    }

    reg->msg_id = task->msg_id;

    dec_hal_write_reg(reg, task, msg);

    return DEC_OK;
}

td_s32 dec_hal_start_dec(const td_void *dec_dev, td_u16 reg_id)
{
    dec_dev_info *dev = (dec_dev_info *)dec_dev;
    vdh_reg_info *reg = TD_NULL;
    td_u32 ofs;

    vfmw_check_ret(dev != TD_NULL, DEC_ERR);
    vfmw_check_ret(dev->ops != TD_NULL, DEC_ERR);
    dev->ops->get_reg(dev, reg_id, &reg);

    if ((reg_id * VREG_OFS_BETWEEN_REG) < VFMW_MAX_UINT32) {
        ofs = (td_u32)(VREG_OFS_DEC_START + reg_id * VREG_OFS_BETWEEN_REG);
        vfmw_check_ret(reg != TD_NULL, DEC_ERR);
        wr_vreg(reg->reg_vir_addr + reg->base_ofs[1], ofs, 1);

        ofs = (td_u32)(VREG_OFS_PXP_START + reg_id * VREG_OFS_BETWEEN_REG);
        wr_vreg(reg->reg_vir_addr + reg->base_ofs[1], ofs, 1);
    } else {
        dprint(PRN_ERROR, "%s %d overflow!\n", __func__, __LINE__);
    }

    return DEC_OK;
}

td_u16 dec_hal_read_bsp_indicator(td_void *dec_dev, td_u16 reg_id)
{
    dec_dev_info *dev = (dec_dev_info *)dec_dev;
    vdh_reg_info *reg = TD_NULL;
    td_u32 data = 0;
    td_u16 indicator;

    vfmw_check_ret(dec_dev != TD_NULL, 0);

    dev->ops->get_reg(dev, reg_id, &reg);

    rd_vreg(reg->reg_vir_addr + reg->base_ofs[0], VREG_OFS_BSP_INTR_STATE, data);

    indicator = ((vreg_bsp_intr_state *)(&data))->frame_indicator;

    return indicator;
}

td_s32 dec_hal_read_int_info(td_void *dec_dev, td_s32 reg_id)
{
    td_u32 data = 0;
    dec_dev_info *dev = TD_NULL;
    vdh_reg_info *reg = TD_NULL;
    td_u32 *vir = TD_NULL;
    dec_dev_vdh *p_vdh = TD_NULL;

    dev = (dec_dev_info *)dec_dev;
    vfmw_check_ret(dev != TD_NULL, DEC_ERR);
    vfmw_check_ret(dev->ops != TD_NULL, DEC_ERR);
    vfmw_check_sec_func(memset_s(&dev->back_up, sizeof(dev->back_up), 0, sizeof(dec_back_up)));
    p_vdh = (dec_dev_vdh *)dev->handle;
    vfmw_check_ret(p_vdh != TD_NULL, DEC_ERR);

    dev->ops->get_reg(dev, reg_id, &reg);
    vir = (td_u32 *)(reg->reg_vir_addr + reg->base_ofs[0]);
    rd_vreg(vir, VREG_OFS_PXP_INTR_STATE, data);
    dev->back_up.int_flag = (data >> 17) & 0x1; /* move 17 bits right */
    dev->back_up.int_flag |= ((data >> 19) & 0x1) << 1; /* 19 is bit count */
    dev->back_up.int_unid = data & 0xffff;
    dev->back_up.err_flag = (data >> 18) & 0x1; /* move 18 bits right */
    dev->back_up.int_reg = (data >> 24) & 0x7; /* move 24 bits right */

    rd_vreg(vir, VREG_OFS_PXP_SLICE_NUM, data);
    dev->back_up.slice_num = data & 0xffff;

    if (dev->back_up.err_flag != 0) {
        dev->back_up.rd_slc_msg = 1;
    }

    rd_vreg(vir, VREG_OFS_BSP_CYCLES, data);
    dev->back_up.bsp_cycle = data;
    rd_vreg(vir, VREG_OFS_PXP_CYCLES, data);
    dev->back_up.pxp_cycle = data;

    data = 0;
    rd_vreg(reg->reg_vir_addr, VREG_OFS_CORRECT_CNT, data);
    p_vdh->dec_report.correct_cnt = data;

    return DEC_OK;
}

td_s32 dec_hal_clear_bsp_int_state(td_void *dec_dev, td_u16 reg_id)
{
    dec_dev_info *dev = (dec_dev_info *)dec_dev;
    vdh_reg_info *reg = TD_NULL;
    const td_u32 data = 0xFFFFFFFF;

    vfmw_check_ret(dev != TD_NULL, DEC_ERR);
    vfmw_check_ret(dev->ops != TD_NULL, DEC_ERR);
    dev->ops->get_reg(dev, reg_id, &reg);

    vfmw_check_ret(reg != TD_NULL, DEC_ERR);
    wr_vreg(reg->reg_vir_addr + reg->base_ofs[0], VREG_OFS_BSP_INTR_MSKSTATE, data);

    return DEC_OK;
}

td_s32 dec_hal_clear_int_state(td_void *dec_dev, td_u16 reg_id)
{
    dec_dev_info *dev = (dec_dev_info *)dec_dev;
    vdh_reg_info *reg = TD_NULL;
    td_u32 data = 0;

    vfmw_check_ret(dev != TD_NULL, DEC_ERR);

    vfmw_check_ret(dev->ops->get_reg != TD_NULL, DEC_ERR);
    dev->ops->get_reg(dev, reg_id, &reg);

    if ((dev->back_up.int_flag & 1) != 0) {
        data |= 0x1;
    }

    if ((dev->back_up.int_flag & 2) != 0) { /* 2:for alignment */
        data |= 0x1 << 2; /* move 2 bits left */
    }

    wr_vreg(reg->reg_vir_addr + reg->base_ofs[0], VREG_OFS_PXP_INTR_MSKSTATE, data);

    os_event_give(g_dec_hal_ctx->serial_event);

    return DEC_OK;
}

VFMW_ACC_BASE STATIC td_void dec_hal_calc_dec_mem_param(const mem_req_info *mem_req, mem_need_info *mem_need,
    dec_chan_task *task)
{
    dec_chan_mem_param *mem = TD_NULL;
    td_u8 bit_depth;

    vfmw_check(mem_req != TD_NULL && mem_need != TD_NULL && task != TD_NULL);
    mem = &task->mem_param;
    bit_depth = mem_need->dec_info.bit_depth;

    vfmw_check(mem != TD_NULL);
    mem->vdh_2d_en = mem_need->linear_en;
    mem->compress_en = mem_need->dec_info.compress_en;
    mem->cmp_head_stride = mem_need->dec_info.head_stride;
    mem->cmp_head_size = mem_need->dec_info.head_size;

    mem->y_stride = mem_need->dec_info.dec_stride;
    mem->uv_stride = mem_need->dec_info.dec_stride;
    mem->image_stride = mem_need->dec_info.dec_stride;
    mem->bit_depth = bit_depth;

    if (mem->vdh_2d_en == 0) {
        dprint(PRN_ALWS, "%s, %d, not support tile now!\r\n", __func__, __LINE__);
        return;
    }

    if (mem->linear_no_gap_en == TD_TRUE) {
        mem->uv_offset = mem_need->dec_info.dec_stride * mem_req->disp_height + mem->cmp_head_size;
        mem->uv_offset_dis = mem_need->dec_info.dec_stride *
            (mem_req->slot_height - mem_req->disp_height) + mem->cmp_head_size;
    } else {
        mem->uv_offset = mem_need->dec_info.dec_stride * mem_req->slot_height + mem->cmp_head_size;
        mem->uv_offset_dis = mem_need->dec_info.dec_stride *
            (mem_req->dec_height / 2 + mem_req->slot_height - mem_req->dec_height) + mem->cmp_head_size; /* 2 is half */
    }

    if (bit_depth > 8) { /* 8 is standard strength */
        dprint(PRN_ALWS, "%s, %d, not support 10 bit!\r\n", __func__, __LINE__);
    } else {
        mem->nbit_image_stride = 0; /* 2 :a number */
        mem->nbit_y_stride = 0;
        mem->nbit_y_offset = 0;
        mem->nbit_uv_offset = 0;
    }

    if (mem->low_delay_en == 1) {
        mem->line_num_ofs = (mem->uv_offset + mem->nbit_uv_offset) * 3 / 2; /* 3 is multiplier 2 :for half */
    } else {
        mem->line_num_ofs = 0;
    }

    return;
}

STATIC VFMW_ACC_BASE td_s32 dec_hal_set_mem_info(dps_info *dps, dec_dev_info *dev, dec_chan_mem_param *mem,
    dec_chan_cfg *cfg, td_u32 dec_pic_wid)
{
    const vfmw_product_info *pdt_info = pdt_get_product_info();
    dec_pic_param_h264 *h264_pic_param = TD_NULL;
    dec_dev_vdh *vdh = (dec_dev_vdh *)dev->handle;
    vdh_ext_mem_info *vdh_mem = &vdh->vdh_ext_mem;

    vfmw_check_ret(pdt_info != TD_NULL, DEC_ERR);
    vfmw_check_ret(mem != TD_NULL, DEC_ERR);
    vfmw_check_ret(cfg != TD_NULL, DEC_ERR);

    mem->vdh_mmu_en = 0;
    mem->mask_mmu_irq = 0;
    mem->vdh_safe_flag = cfg->sec_flag;
    switch (cfg->vid_std) {
        case VFMW_HEVC:
            mem->pxp_in_ram = dec_pic_wid > VDH_IN_RAM_MAX_WIDTH ? 0 : 1;
            mem->bsp_in_ram = dec_pic_wid > VDH_IN_RAM_MAX_WIDTH ? 0 : 1;
            break;

        case VFMW_H264:
            h264_pic_param = (dec_pic_param_h264 *)dps->pic_param;
            if (h264_pic_param->mbaff_frame_flag) {
                mem->pxp_in_ram = dec_pic_wid > VDH_IN_RAM_MAX_WIDTH / 2 ? 0 : 1; /* 2 is half */
            } else {
                mem->pxp_in_ram = dec_pic_wid > VDH_IN_RAM_MAX_WIDTH ? 0 : 1;
            }
            mem->bsp_in_ram = dec_pic_wid > VDH_IN_RAM_MAX_WIDTH ? 0 : 1;
            break;
        default:
            break;
    }
    if (mem->pxp_in_ram == 0 && vdh_mem->is_dyn_ram_alloc_ok == TD_FALSE) {
        vfmw_check_ret(dec_hal_alloc_vdh_ext_mem(dev, vdh_mem, DEC_HAL_EXT_MEM_DYNAMIC) == DEC_OK, DEC_ERR);
        vdh_mem->is_dyn_ram_alloc_ok = TD_TRUE;
    }

    mem->low_delay_en = 0;
    mem->uv_order_en = cfg->uv_order_en;
    mem->tunl_line_num = 1;
#ifdef VFMW_MODULE_LOWDLY_SUPPORT
    mem->low_delay_en = cfg->low_delay_en;
    mem->tunl_line_num = cfg->tunl_line_num;
#endif
    mem->linear_no_gap_en = cfg->linear_no_gap_en;

    return DEC_OK;
}

STATIC td_void dec_hal_calc_disp_mem_param_set(dec_chan_mem_param *mem)
{
    mem->disp_mem.vdh_2d_en = mem->vdh_2d_en;
    mem->disp_mem.bit_depth = mem->bit_depth;
    mem->disp_mem.tile_format = 0;
    mem->disp_mem.compress_en = mem->compress_en;
    mem->disp_mem.cmp_head_stride = mem->cmp_head_stride;
    mem->disp_mem.cmp_head_size = mem->cmp_head_size;
    mem->disp_mem.image_stride = mem->image_stride;
    mem->disp_mem.y_stride = mem->y_stride;
    mem->disp_mem.uv_offset = mem->uv_offset;
    mem->disp_mem.nbit_image_stride = mem->nbit_image_stride;
    mem->disp_mem.nbit_y_stride = mem->nbit_y_stride;
    mem->disp_mem.nbit_y_offset = mem->nbit_y_offset;
    mem->disp_mem.nbit_uv_offset = mem->nbit_uv_offset;
    mem->disp_mem.line_num_ofs = mem->line_num_ofs;
}

VFMW_ACC_BASE static td_void dec_hal_calc_disp_mem_param(const mem_req_info *mem_req,
    const mem_need_info *mem_need, dec_chan_cfg *cfg, dec_chan_task *task)
{
    dec_chan_mem_param *mem = &task->mem_param;
    mem->disp_mem.is_disp_need = 0;

    vfmw_unused(mem_req);
    vfmw_unused(mem_need);
    vfmw_unused(cfg);

    dec_hal_calc_disp_mem_param_set(mem);

    return;
}

VFMW_ACC_BASE td_s32 dec_hal_calc_mem_param(td_void *dec_dev, dec_hal_ops *ops, td_void *chan_cfg,
    td_void *dec_task)
{
    dec_chan_task *task = (dec_chan_task *)dec_task;
    dec_chan_mem_param *mem = TD_NULL;
    dec_chan_cfg *cfg = (dec_chan_cfg *)chan_cfg;
    td_u32 dec_pic_wid;
    td_u32 dec_pic_heig;
    td_u32 disp_pic_width;
    td_u32 disp_pic_heig;
    mem_req_info mem_req = { 0 };
    mem_need_info mem_need = { 0 };

    vfmw_unused(ops);

    vfmw_check_ret(task != TD_NULL, DEC_ERR);
    vfmw_check_ret(cfg != TD_NULL, DEC_ERR);
    vfmw_check_ret(task->dps != TD_NULL, DEC_ERR);
    mem = &task->mem_param;
    vfmw_check_ret(mem != TD_NULL, DEC_ERR);
    dec_pic_wid = task->dps->frm_width;
    dec_pic_heig = task->dps->frm_height;
    disp_pic_width = task->dps->disp_width;
    disp_pic_heig = task->dps->disp_height;

    if ((cfg->alloc_type == VFMW_FRM_ALLOC_OUTER) &&
        ((task->dps->top_offset + task->dps->bottom_offset) > 64)) { /* 64 is the vfmw_max offset */
        dprint(PRN_ERROR, "%s offset is not support!  top:%d bot:%d\n", __func__,
            task->dps->top_offset, task->dps->bottom_offset);
        return DEC_ERR;
    }

    vfmw_check_ret(dec_hal_set_mem_info(task->dps, dec_dev, mem, cfg, dec_pic_wid) == DEC_OK, DEC_ERR);

    mem_req.vid_std = cfg->vid_std;
    mem_req.cmp_mode = cfg->vcmp_mode;
    mem_req.frm_layout = cfg->layout;
    mem_req.dec_width = dec_pic_wid;
    mem_req.dec_height = dec_pic_heig;
    mem_req.disp_height = disp_pic_heig;
    mem_req.disp_width = disp_pic_width;
    mem_req.bit_depth = task->dps->bit_depth;
    mem_req.alloc_type = cfg->alloc_type;
    mem_req.frame_mbs_only = task->dps->frame_mbs_only;
    mem_req.is_max_buf_size_en = cfg->is_max_buf_size_en;
    mem_req.max_dec_width = cfg->max_dec_width;
    mem_req.max_dec_height = cfg->max_dec_height;
    pdt_get_mem_req(&mem_req, &mem_need);

    dec_hal_calc_dec_mem_param(&mem_req, &mem_need, task);
    dec_hal_calc_disp_mem_param(&mem_req, &mem_need, cfg, task);

    return DEC_OK;
}

td_s32 dec_hal_read_frm_msg(td_void *dec_dev, td_void *chan_arg, td_void *dec_task)
{
    td_u32 *slot = TD_NULL;
    dec_dev_info *dev = TD_NULL;
    dec_dev_vdh *vdh = TD_NULL;
    dec_chan *chan = (dec_chan *)chan_arg;
    dec_chan_task *task = (dec_chan_task *)dec_task;
    dec_report_info *dec_report = TD_NULL;

    vfmw_check_ret(dec_dev != TD_NULL && chan_arg != TD_NULL && dec_task != TD_NULL, DEC_ERR);
    dev = (dec_dev_info *)dec_dev;
    vdh = (dec_dev_vdh *)dev->handle;
    vfmw_check_ret(vdh != TD_NULL, DEC_ERR);
    dec_report = &vdh->dec_report;

    if (chan->chan_cfg.vid_std != VFMW_H264 &&
        chan->chan_cfg.vid_std != VFMW_HEVC) {
        return DEC_OK;
    }

    slot = (td_u32 *)(task->slice_list.up_msg_vir_addr[0]);
    vfmw_check_ret(slot != TD_NULL, DEC_ERR);
    dec_report->err_flag = slot[0] >> 31; /* 31 is bit count */

    return DEC_OK;
}

td_s32 dec_hal_read_slc_msg(td_void *dec_dev, td_u16 reg_id, td_void *list)
{
    dec_chan_slice_list *slice_list = (dec_chan_slice_list *)list;
    dec_dev_info *dev = (dec_dev_info *)dec_dev;
    dec_dev_vdh *vdh = TD_NULL;
    dec_report_info *dec_report = TD_NULL;
    td_u32 *slot = TD_NULL;
    td_u32 slice_cnt = 0;
    td_u32 slice_num = 0;
    td_u32 total_slice;
    td_u32 i, j, step;
    td_u32 msg_idx = 0;

    vfmw_unused(reg_id);
    vfmw_check_ret(slice_list != TD_NULL, DEC_ERR);
    vfmw_check_ret(dev != TD_NULL, DEC_ERR);
    vdh = (dec_dev_vdh *)dev->handle;
    vfmw_check_ret(vdh != TD_NULL, DEC_ERR);
    dec_report = &vdh->dec_report;

    total_slice = dev->back_up.slice_num;

    if (total_slice <= MAX_UP_MSG_SLICE_NUM) {
        dec_report->over_flow = 0;
    } else {
        dprint(PRN_ERROR, "UpMsg SlcNum %d > %d\n", total_slice, MAX_UP_MSG_SLICE_NUM);
        dec_report->over_flow = 1;
        total_slice = 0;
    }

    dec_report->slice_num = total_slice;
    step = DEC_BASE_UPMSG_SLOT_LEN >> 2; /* 2 is bit count */

    for (i = 0; i < slice_list->list_num && slice_cnt < total_slice; i++) {
        slot = (td_u32 *)(slice_list->up_msg_vir_addr[i] + FRM_UP_MSG_LEN);
        slice_num = vfmw_min(slice_list->list_len[i], total_slice - slice_cnt);
        slice_cnt += slice_num;
        for (j = 0; j < slice_num; j++, slot += step) {
            if (msg_idx < (MAX_UP_MSG_SLICE_NUM + 1)) {
                dec_report->slice_mb_range[msg_idx][0] = slot[1] & 0xFFFFF;
                dec_report->slice_mb_range[msg_idx][1] = slot[2] & 0xFFFFF; /* 2 is an index */
                msg_idx++;
            }
        }
    }

    return DEC_OK;
}

td_s32 dec_hal_read_dec_reg(td_void *dec_dev, td_u16 reg_id)
{
    dec_dev_info *p_dev = TD_NULL;
    dec_dev_vdh *p_vdh = TD_NULL;
    dec_report_info *p_dec_report = TD_NULL;
    vdh_reg_info *p_vdh_reg = TD_NULL;
    td_u32 d32 = 0;

    p_dev = (dec_dev_info *)dec_dev;
    vfmw_check_ret(p_dev != TD_NULL, DEC_ERR);
    p_vdh = (dec_dev_vdh *)p_dev->handle;
    vfmw_check_ret(p_vdh != TD_NULL, DEC_ERR);
    p_dec_report = &p_vdh->dec_report;

    vfmw_check_ret(p_dev->ops != TD_NULL, DEC_ERR);
    p_dev->ops->get_reg(dec_dev, reg_id, &p_vdh_reg);

    if (p_dev->type == DEC_DEV_TYPE_VDH) {
        rd_vreg(p_vdh_reg->reg_vir_addr, VREG_OFS_MB0_QP_IN_CURR_PIC, d32);
        p_dec_report->prev_pic_mb0_qp = d32;

        d32 = 0;
        rd_vreg(p_vdh_reg->reg_vir_addr, VREG_OFS_SWITCH_ROUNDING, d32);
        p_dec_report->switch_rounding = d32;

        p_dec_report->is_read_from_reg = TD_TRUE;
    }

    return DEC_OK;
}

STATIC td_s32 dec_hal_cancel_vdh(td_void *dec_dev, td_u16 reg_id)
{
    dec_dev_info *dev = (dec_dev_info *)dec_dev;
    vdh_reg_info *reg = TD_NULL;
    td_u32 ofs = 0;
    td_u32 d32 = 0;
    td_u32 i;
    td_u32 time;

    vfmw_check_ret(dev != TD_NULL, DEC_ERR);
    vfmw_check_ret(dev->ops != TD_NULL, DEC_ERR);
    dev->ops->get_reg(dev, reg_id, &reg);

    os_mb();

    if ((reg_id * VREG_OFS_BETWEEN_REG) < VFMW_MAX_UINT32) {
        ofs = (td_u32)(VREG_OFS_INFO_STATE + reg_id * VREG_OFS_BETWEEN_REG);
    } else {
        dprint(PRN_ERROR, "%s %d overflow!\n", __func__, __LINE__);
    }
    ofs += reg->base_ofs[1];

    rd_vreg(reg->reg_vir_addr, ofs, d32);

    ((vreg_info_state *)(&d32))->dec_cancel = 1;
    wr_vreg(reg->reg_vir_addr, ofs, d32);

    time = os_get_time_us();

    for (i = 0; i < 10000; i++) { /* 10000 :for loop count */
        rd_vreg(reg->reg_vir_addr, ofs, d32);

        if (((vreg_info_state *)(&d32))->bsp_state == 1 &&
            ((vreg_info_state *)(&d32))->pxp_state == 1) {
            time = os_get_time_us() - time;
            if (time > 50) { /* 50: for test  */
                os_print("Time %d \n", time);
            }
            return DEC_OK;
        } else {
            os_udelay(30); /* 30 is designed for delay */
        }
    }

    dprint(PRN_ERROR, "%s Fail!\n", __func__);

    return DEC_ERR;
}

td_s32 dec_hal_cancel_dec(td_void *dec_dev, td_u16 reg_id)
{
    dec_dev_info *dev = (dec_dev_info *)dec_dev;

    vfmw_check_ret(dev != TD_NULL, DEC_ERR);
    return dec_hal_cancel_vdh(dev, reg_id);
}

td_s32 dec_hal_get_dev(td_s32 is_sec, td_void *chan_task, td_u16 *dev_id)
{
    dec_chan_task *task = (dec_chan_task *)chan_task;

    vfmw_unused(is_sec);
    vfmw_check_ret(dev_id != TD_NULL, DEC_ERR);

    if (task == TD_NULL) {
        *dev_id = 0;
        return DEC_OK;
    }

    vfmw_check_ret(task->dps != TD_NULL, DEC_ERR);
    *dev_id = 0;

    return DEC_OK;
}

STATIC td_s32 dec_hal_alloc_frm_bin(dec_dev_info *dev, dec_dev_vdh *vdh, vdh_msg_info *msg)
{
    td_s32 id;
    td_s32 size = 0;
    mem_record mem = { 0 };
    UADDR phy_addr;

    /*  alloc frm bin  */
    size += VDH_MSG_NUM * SLICE_BIN_LEN;
    size += VDH_MSG_NUM * CTB_HEAD_LEN;

    /* vfmw_slc_bin */
    mem.length = (td_u32)size;
    mem.is_cached = 0;
    mem.type = DEV_HAL_MEM_TYPE;
    mem.unid = dev->dev_id;
    mem.mem_name = "vfmw_slc_bin";
    if (os_malloc(&mem) != OSAL_OK) {
        return DEC_ERR;
    }

    phy_addr = mem.phy_addr;
    vdh->slc_bin = mem;

    phy_addr = vfmw_align_up(phy_addr, VFMW_DEV_ALIGN);

    /* slc bin */
    for (id = 0; id < VDH_MSG_NUM; id++) {
        arrange_ptr(phy_addr, SLICE_BIN_LEN, msg[id].slice_bin_phy_addr);
        msg[id].slice_bin_length = SLICE_BIN_LEN;

        arrange_ptr(phy_addr, CTB_HEAD_LEN, msg[id].ctb_head_phy_addr);
        msg[id].ctb_head_length = CTB_HEAD_LEN;
    }

    return DEC_OK;
}

STATIC td_s32 dec_hal_refresh_bin(td_s32 dev_id)
{
    td_s32 id;
    td_s32 need = 0;
    td_ulong lock_flag = 0;
    dec_chan *chan = TD_NULL;
    dec_dev_info *dev = TD_NULL;
    dec_dev_vdh *vdh = TD_NULL;
    vdh_msg_info *msg = TD_NULL;

    dec_dev_get_dev(dev_id, &dev);

    if (dev == TD_NULL ||
        dev->handle == TD_NULL ||
        dev->type != DEC_DEV_TYPE_VDH) {
        return DEC_OK;
    }

    for (id = 0; id < VFMW_CHAN_NUM && need == 0; id++) {
        dec_chan_lock_chan(id, &lock_flag);
        chan = dec_chan_get_chan(id);
        if (chan != TD_NULL &&
            (chan->chan_cfg.vid_std == VFMW_VP9 ||
             chan->chan_cfg.vid_std == VFMW_AVS2)) {
            need = 1;
        }
        dec_chan_unlock_chan(id, &lock_flag);
    }

    vdh = &g_dec_hal_ctx->vdh[dev_id];
    msg = vdh->vdh_msg_pool.vdh_msg;
    vfmw_check_ret(msg != TD_NULL, DEC_ERR);

    if (need == 1 && vdh->slc_bin.phy_addr == 0) {
        if (dec_hal_alloc_frm_bin(dev, vdh, msg) != DEC_OK) {
            return DEC_ERR;
        }
    }

    if (need == 0 && vdh->slc_bin.phy_addr != 0) {
        /*  free frm bin  */
        for (id = 0; id < VDH_MSG_NUM; id++) {
            msg[id].slice_bin_length = 0;
            msg[id].slice_bin_phy_addr = 0;

            msg[id].ctb_head_length = 0;
            msg[id].ctb_head_phy_addr = 0;
        }
        os_free(&vdh->slc_bin);
        vdh->slc_bin.phy_addr = 0;
    }

    return DEC_OK;
}

STATIC td_s32 dec_hal_dev_idle(const dec_dev_info *dev)
{
    td_s32 cnt;
    dec_dev_vdh *vdh = TD_NULL;

    vdh = (dec_dev_vdh *)dev->handle;
    for (cnt = 0; cnt < vdh->vdh_reg_pool.reg_num; cnt++) {
        if (vdh->vdh_reg_pool.vdh_reg[cnt].used == 1) {
            return DEC_ERR;
        }
    }

    return DEC_OK;
}

STATIC td_s32 dec_hal_wait_vdh(dec_chan *chan, const dec_chan_task *task)
{
    td_s32 ret, dev_id;
    dec_dev_info *dev = TD_NULL;

    vfmw_unused(chan);
    vfmw_unused(task);

    for (dev_id = 0; dev_id < DEC_DEV_NUM; dev_id++) {
        dec_dev_get_dev(dev_id, &dev);
        if (dev == TD_NULL ||
            dev->handle == TD_NULL) {
            return DEC_OK;
        }

        ret = dec_hal_dev_idle(dev);
        if (ret != DEC_OK) {
            return DEC_ERR;
        }
    }

    return DEC_OK;
}

STATIC td_s32 dec_hal_is_vdh_int(td_s32 dev_id, const td_s32 *reg_id)
{
    dec_dev_info *dev = TD_NULL;
    vdh_reg_info *reg = TD_NULL;
    td_u32 *vir = TD_NULL;
    td_u32 data, int_flag;

    dec_dev_get_dev(dev_id, &dev);
    if (dev == TD_NULL || dev->ops == TD_NULL || dev->ops->get_reg == TD_NULL) {
        dprint(PRN_ERROR, "get int_flag error!\n");
        return DEC_ERR;
    }

    dev->ops->get_reg(dev, (*reg_id), &reg);
    vir = (td_u32 *)(reg->reg_vir_addr + reg->base_ofs[0]);
    rd_vreg(vir, VREG_OFS_PXP_INTR_STATE, data);
    int_flag = (data >> 17) & 0x1; /* 17:dec_over */
    if (int_flag == 0) {
        return DEC_ERR;
    }

    return DEC_OK;
}

static td_s32 dec_hal_read_frm_msg_cycle(td_s32 dev_id, td_void *chan_arg, td_void *task_arg)
{
    td_s32 logic_id = 0;
    dec_dev_info *dev = TD_NULL;
    fsp_logic *logic_fs = TD_NULL;
    vfmw_image *image = TD_NULL;
    dec_chan *chan = (dec_chan *)chan_arg;
    dec_chan_task *task = (dec_chan_task *)task_arg;

    dec_dev_get_dev(dev_id, &dev);
    if (dev == TD_NULL) {
        dprint(PRN_ERROR, "read_frm_msg_cycle error!\n");
        return DEC_ERR;
    }

    logic_id = dps_get_fsp_logic_id(chan->chan_id, task->dps);
    logic_fs = fsp_get_logic(chan->chan_id, logic_id);
    vfmw_alert_ret(logic_fs != TD_NULL, DEC_ERR);
    image = fsp_get_image(chan->chan_id, logic_id);
    vfmw_alert_ret(image != TD_NULL, DEC_ERR);

    image->cycle = dev->back_up.pxp_cycle;
    dprint(PRN_CYCLE, "\r\n cycle: %d\r\n", image->cycle);

    return DEC_OK;
}

static td_s32 dec_hal_read_frm_msg_pix_sum(td_s32 dev_id, td_void *chan_arg, td_void *task_arg)
{
    td_u64 tmp64;
    td_u32 *slot = TD_NULL;
    td_s32 logic_id = 0;
    fsp_logic *logic_fs = TD_NULL;
    vfmw_image *image = TD_NULL;
    dec_chan *chan = (dec_chan *)chan_arg;
    dec_chan_task *task = (dec_chan_task *)task_arg;

    vfmw_unused(dev_id);
    logic_id = dps_get_fsp_logic_id(chan->chan_id, task->dps);
    logic_fs = fsp_get_logic(chan->chan_id, logic_id);
    vfmw_alert_ret(logic_fs != TD_NULL, DEC_ERR);
    image = fsp_get_image(chan->chan_id, logic_id);
    vfmw_alert_ret(image != TD_NULL, DEC_ERR);

    slot = (td_u32 *)(task->slice_list.up_msg_vir_addr[0]);
    if (fsp_is_dec_over(logic_fs) == TD_TRUE) { /* top&&bot valid */
        if (image->is_fld_flag == 1) {
            tmp64 = slot[VREG_OFS_VDH_LUMA_PIXEL_SUM_HIGH];
            tmp64 = (((tmp64 & 0xF0000000) << 4) | slot[VREG_OFS_VDH_LUMA_PIXEL_SUM_LOW]) >> 2; /* 2 4 is offset */
            image->pix_info.pix_sum_luma += (td_u32)tmp64;

            tmp64 = slot[VREG_OFS_VDH_CHROMA_PIXEL_SUM_HIGH];
            tmp64 = (((tmp64 & 0xF000000) << 8) | slot[VREG_OFS_VDH_CHROMA_PIXEL_SUM_LOW]) >> 2; /* 2 8 is offset */
            image->pix_info.pix_sum_chroma += (td_u32)tmp64;
        } else {
            tmp64 = slot[VREG_OFS_VDH_LUMA_PIXEL_SUM_HIGH];
            tmp64 = (((tmp64 & 0xF0000000) << 4) | slot[VREG_OFS_VDH_LUMA_PIXEL_SUM_LOW]) >> 2; /* 2 4 is offset */
            image->pix_info.pix_sum_luma = (td_u32)tmp64;

            tmp64 = slot[VREG_OFS_VDH_CHROMA_PIXEL_SUM_HIGH];
            tmp64 = (((tmp64 & 0xF000000) << 8) | slot[VREG_OFS_VDH_CHROMA_PIXEL_SUM_LOW]) >> 2; /* 2 8 is offset */
            image->pix_info.pix_sum_chroma += (td_u32)tmp64;
        }
        dprint(PRN_PIX_SUM, "\r\n ==pix sum== [0x%x, 0x%x]\r\n", image->pix_info.pix_sum_luma,
            image->pix_info.pix_sum_chroma);
    } else { /* top or bot valid */
        tmp64 = slot[VREG_OFS_VDH_LUMA_PIXEL_SUM_HIGH];
        tmp64 = (((tmp64 & 0xF0000000) << 4) | slot[VREG_OFS_VDH_LUMA_PIXEL_SUM_LOW]) >> 2; /* 2 4 is offset */
        image->pix_info.pix_sum_luma = (td_u32)tmp64;

        tmp64 = slot[VREG_OFS_VDH_CHROMA_PIXEL_SUM_HIGH];
        tmp64 = (((tmp64 & 0xF000000) << 8) | slot[VREG_OFS_VDH_CHROMA_PIXEL_SUM_LOW]) >> 2; /* 2 8 is offset */
        image->pix_info.pix_sum_chroma += (td_u32)tmp64;
    }

    return DEC_OK;
}

td_s32 dec_hal_control(td_s32 dev_id, td_s32 cid, td_void *arg1, td_void *arg2)
{
    td_s32 ret;
    dec_chan *chan = TD_NULL;
    dec_chan_task *task = TD_NULL;

    switch (cid) {
        case DHAL_CID_REFRESH_BIN:
            ret = dec_hal_refresh_bin(dev_id);
            break;
        case DHAL_CID_WAIT_DEC:
            chan = (dec_chan *)arg1;
            task = (dec_chan_task *)arg2;
            ret = dec_hal_wait_vdh(chan, task);
            break;
        case DHAL_CID_GET_VDH:
            ret = dec_hal_get_vdh();
            break;
        case DHAL_CID_PUT_VDH:
            ret = dec_hal_put_vdh();
            break;
        case DHAL_CID_IS_VDH_INT:
            ret = dec_hal_is_vdh_int(dev_id, (td_s32*)arg1);
            break;
        case DHAL_CID_GET_PIX_SUM:
            chan = (dec_chan *)arg1;
            task = (dec_chan_task *)arg2;
            ret = dec_hal_read_frm_msg_pix_sum(dev_id, chan, task);
            break;
        case DHAL_CID_GET_CYCLE:
            chan = (dec_chan *)arg1;
            task = (dec_chan_task *)arg2;
            ret = dec_hal_read_frm_msg_cycle(dev_id, chan, task);
            break;
        default:
            ret = DEC_OK;
            break;
    }

    return ret;
}

td_s32 dec_hal_get_mem_need_size(const td_void *dev, vfmw_svr_mem_need *mem)
{
    td_s32 size;

    vfmw_unused(dev);
    vfmw_unused(mem);

    size = vfmw_align_up(sizeof(dec_dev_vdh), DEC_DEV_VDH_UP_MSG_ALIGN);
    size += 0; /* calc mem for MDC */

    return size;
}

td_s32 mdma_hal_get_mem_need_size(const td_void *dev, vfmw_svr_mem_need *mem)
{
    td_s32 size;

    vfmw_unused(dev);
    vfmw_unused(mem);

    size = vfmw_align_up(sizeof(dec_dev_mdma), DEC_DEV_VDH_UP_MSG_ALIGN);
    size += 0; /* calc mem for MDC */

    return size;
}
