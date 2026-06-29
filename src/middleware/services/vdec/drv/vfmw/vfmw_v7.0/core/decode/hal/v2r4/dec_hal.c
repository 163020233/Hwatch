/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2006-2024. All rights reserved.
 * Description: dec hal
 * Author: AuthorNameMagicTag
 * Create: 2006-01-01
 */

#include "dec_dev.h"
#include "dec_chn.h"
#include "dec_ref.h"
#include "dec_post.h"
#include "dbg.h"
#include "vdh_h264.h"
#include "vfmw_pdt.h"
#include "fsp.h"
#include "vfmw_define.h"
#include "vdec_chr.h"
#include "dec_hal.h"

static td_s32 g_dec_irq_handle[2] = {0, 0};
static td_u32 g_pxp_num = FPGA_PXP_NUM;

static dec_hal_ops g_dec_hal_ops[] = {
#ifdef VFMW_H264_SUPPORT
    {
        .write_vdh_reg         = hal_h264_write_reg,
        .write_vdh_pic_msg     = hal_h264_write_pic_msg,
        .write_vdh_slice_lst   = hal_h264_write_slice_lst,
        .process_slice         = hal_h264_process_slice,
        .calc_mem_param        = hal_h264_calc_mem_param,
        .make_slice_ref        = hal_h264_make_slice_ref,
        .get_rep_param         = hal_h264_get_rep_param,
        .write_mdma_reg        = TD_NULL,
        .write_mdma_msg        = TD_NULL,
        .vid_std               = VFMW_H264,
    },
#endif
};

static td_u32 dec_hal_calc_vdh_up_msg_mem_size(td_u32 up_msg_size)
{
    td_u32 size = 0;

    size = vfmw_align_up(up_msg_size, DEC_DEV_VDH_UP_MSG_ALIGN);

    return size;
}

// open and close
static td_u32 dec_hal_calc_msg_size(td_void)
{
    td_u32 size = 0;

    size += H264_PIC_MSG_SLOT_LEN;
    size += H264_MAX_SLICE_NUM * H264_SLICE_MSG_SLOT_LEN;
    size += dec_hal_calc_vdh_up_msg_mem_size(H264_MAX_SLICE_NUM * H264_UP_MSG_SLOT_LEN);

    return size;
}

static td_u32 dec_hal_calc_vdh_ext_mem_size(td_void)
{
    td_u32 size = 0;

    size += H264_CA_MN_ADDR_LEN;
    size += H264_SED_TOP_ADDR_LEN;
    size += H264_PMV_TOP_ADDR_LEN;
    size += H264_RCN_TOP_ADDR_LEN;
    size += H264_DBLK_TOP_ADDR_LEN;

    return size;
}

static td_s32 dec_hal_free_vdh_msg_mem(vdh_msg_info *msg)
{
    mem_record mem = {0};

    if (msg->base_vir_addr != TD_NULL) {
        mem.phy_addr = msg->base_phy_addr;
        mem.vir_addr = msg->base_vir_addr;
        mem.length = dec_hal_calc_msg_size();
        mem.is_cached = 0;
        os_free(&mem);
    }

    if (msg->slice_msg_phy_addr != TD_NULL) {
        os_free_vir(msg->slice_msg_phy_addr);
    }

    if (msg->slice_msg_vir_addr != TD_NULL) {
        os_free_vir(msg->slice_msg_vir_addr);
    }

    vfmw_check_sec_func(memset_s(msg, sizeof(vdh_msg_info), 0, sizeof(vdh_msg_info)));

    return DEC_OK;
}

static td_s32 dec_hal_alloc_slice_mem(vdh_msg_info *msg)
{
    td_u32 size = 0;

    size = H264_MAX_SLICE_NUM * (td_u32)sizeof(UADDR);
    msg->slice_msg_phy_addr = (UADDR *)os_alloc_vir(msg->chan_id, "dec_slc_phy", (td_s32)size);
    if (msg->slice_msg_phy_addr == TD_NULL) {
        dprint(PRN_ERROR, "%s, %d error!\n", __func__, __LINE__);
        return DEC_ERR;
    }
    vfmw_check_sec_func(memset_s(msg->slice_msg_phy_addr, size, 0, size));

    size = H264_MAX_SLICE_NUM * (td_u32)sizeof(td_u8 *);
    msg->slice_msg_vir_addr = (td_u8 **)os_alloc_vir(msg->chan_id, "dec_slc_vir", (td_s32)size);
    if (msg->slice_msg_vir_addr == TD_NULL) {
        dprint(PRN_ERROR, "%s, %d error!\n", __func__, __LINE__);
        goto exit;
    }
    vfmw_check_sec_func(memset_s(msg->slice_msg_vir_addr, size, 0, size));

    return DEC_OK;
exit:
    os_free_vir(msg->slice_msg_phy_addr);
    return DEC_ERR;
}

static td_s32 dec_hal_alloc_vdh_msg_mem(const dec_dev_info *dec_dev, vdh_msg_info *msg)
{
    mem_record mem = { 0 };
    UADDR  phy_addr = 0;
    td_u8 *vir_addr = TD_NULL;
    td_u32 size = 0;
    td_u32 up_msg_size = 0;
    td_u32 idx = 0;
    td_s32 ret;

    ret = dec_hal_alloc_slice_mem(msg);
    vfmw_check_ret(ret == DEC_OK, DEC_ERR);

    size = dec_hal_calc_msg_size();
    /* vfmw_vdh_msg */
    mem.length = size;
    mem.is_cached = 0;
    mem.type = DEV_HAL_MEM_TYPE;
    mem.unid = dec_dev->dev_id;
    mem.mem_name = "vfmw_vdh_msg";
    vfmw_check_goto(os_malloc != TD_NULL, exit);
    if (os_malloc(&mem) != OSAL_OK) {
        goto exit;
    }
    vfmw_check_sec_func(memset_s(mem.vir_addr, mem.length, 0, mem.length));

    phy_addr = mem.phy_addr;
    vir_addr = mem.vir_addr;
    msg->base_phy_addr = phy_addr;
    msg->base_vir_addr = vir_addr;

    ARRANGE_PTR(phy_addr, H264_PIC_MSG_SLOT_LEN, msg->pic_msg_phy_addr);
    ARRANGE_PTR(vir_addr, H264_PIC_MSG_SLOT_LEN, msg->pic_msg_vir_addr);

    for (idx = 0; idx < H264_MAX_SLICE_NUM; idx++) {
        ARRANGE_PTR(phy_addr, H264_SLICE_MSG_SLOT_LEN, msg->slice_msg_phy_addr[idx]);
        ARRANGE_PTR(vir_addr, H264_SLICE_MSG_SLOT_LEN, msg->slice_msg_vir_addr[idx]);
    }

    up_msg_size = dec_hal_calc_vdh_up_msg_mem_size(H264_MAX_SLICE_NUM * H264_UP_MSG_SLOT_LEN);
    ARRANGE_PTR(phy_addr, up_msg_size, msg->up_msg_phy_addr);
    ARRANGE_PTR(vir_addr, up_msg_size, msg->up_msg_vir_addr);

    if ((td_u32)(phy_addr - mem.phy_addr) > mem.length || (td_u32)(vir_addr - mem.vir_addr) > mem.length) {
        goto exit;
    }

    return DEC_OK;

exit:
    dprint(PRN_ERROR, "Malloc VDH message memory error!\n");
    dec_hal_free_vdh_msg_mem(msg);

    return DEC_ERR;
}

static td_s32 dec_hal_free_vdh_ext_mem(vdh_ext_mem_info *vdh_mem)
{
    mem_record mem = { 0 };

    if (vdh_mem->base_vir_addr != TD_NULL) {
        mem.phy_addr = vdh_mem->base_phy_addr;
        mem.vir_addr = vdh_mem->base_vir_addr;
        mem.length  = dec_hal_calc_vdh_ext_mem_size();
        mem.is_cached = 0;
        os_free(&mem);
    }

    vfmw_check_sec_func(memset_s(vdh_mem, sizeof(vdh_ext_mem_info), 0, sizeof(vdh_ext_mem_info)));

    return DEC_OK;
}

static td_s32 dec_hal_alloc_vdh_ext_mem(const dec_dev_info *dec_dev, vdh_ext_mem_info *vdh_mem)
{
    mem_record mem = { 0 };
    UADDR  phy_addr = 0;
    td_u8 *vir_addr = TD_NULL;
    td_u32 size = 0;

    size = dec_hal_calc_vdh_ext_mem_size();

    /* vfmw_vdh_ext */
    mem.length = size;
    mem.is_cached = 0;
    mem.type = DEV_HAL_MEM_TYPE;
    mem.unid = dec_dev->dev_id;
    mem.mem_name = "vfmw_vdh_ext";
    vfmw_check_ret(os_malloc != TD_NULL, DEC_ERR);
    vfmw_check_ret(os_malloc(&mem) == OSAL_OK, DEC_ERR);
    vfmw_check_sec_func(memset_s(mem.vir_addr, mem.length, 0, mem.length));

    phy_addr = mem.phy_addr;
    vir_addr = mem.vir_addr;
    vdh_mem->base_phy_addr = phy_addr;
    vdh_mem->base_vir_addr = vir_addr;

    ARRANGE_PTR(phy_addr, H264_CA_MN_ADDR_LEN, vdh_mem->h264_mn_phy_addr);
    ARRANGE_PTR(vir_addr, H264_CA_MN_ADDR_LEN, vdh_mem->h264_mn_vir_addr);

    ARRANGE_PTR(phy_addr, H264_SED_TOP_ADDR_LEN, vdh_mem->sed_top_phy_addr);
    ARRANGE_ADD(vir_addr, H264_SED_TOP_ADDR_LEN);

    ARRANGE_PTR(phy_addr, H264_PMV_TOP_ADDR_LEN, vdh_mem->pmv_top_phy_addr);
    ARRANGE_ADD(vir_addr, H264_PMV_TOP_ADDR_LEN);

    ARRANGE_PTR(phy_addr, H264_RCN_TOP_ADDR_LEN, vdh_mem->rcn_top_phy_addr);
    ARRANGE_ADD(vir_addr, H264_RCN_TOP_ADDR_LEN);

    ARRANGE_PTR(phy_addr, H264_DBLK_TOP_ADDR_LEN, vdh_mem->dblk_top_phy_addr);
    ARRANGE_ADD(vir_addr, H264_DBLK_TOP_ADDR_LEN);

    if ((td_u32)(phy_addr - mem.phy_addr) > mem.length || (td_u32)(vir_addr - mem.vir_addr) > mem.length) {
        goto exit;
    }

    hal_h264_init_hal(vdh_mem);

    return DEC_OK;

exit:

    dprint(PRN_ERROR, "Malloc VDH external memory error!\n");
    dec_hal_free_vdh_ext_mem(vdh_mem);

    return DEC_ERR;
}

static td_s32 dec_hal_alloc_vdh_mem(dec_dev_info *dec_dev)
{
    dec_dev_vdh *vdh = (dec_dev_vdh *)dec_dev->handle;
    vdh_msg_pool_info *msg_pool = &vdh->vdh_msg_pool;
    vdh_ext_mem_info  *vdh_mem  = &vdh->vdh_ext_mem;
    vdh_msg_info *msg = TD_NULL;
    td_u16 msg_id = 0;

    for (msg_id = 0; msg_id < msg_pool->msg_num; msg_id++) {
        msg = &msg_pool->vdh_msg[msg_id];
        if (dec_hal_alloc_vdh_msg_mem(dec_dev, msg) != DEC_OK) {
            goto err1;
        }
    }

    if (dec_hal_alloc_vdh_ext_mem(dec_dev, vdh_mem) != DEC_OK) {
        goto err1;
    }

    return DEC_OK;

err1:
    for (msg_id = 0; msg_id < msg_pool->msg_num; msg_id++) {
        msg = &msg_pool->vdh_msg[msg_id];
        dec_hal_free_vdh_msg_mem(msg);
    }
    return DEC_ERR;
}

static td_s32 dec_hal_free_vdh_mem(dec_dev_info *dec_dev)
{
    dec_dev_vdh *vdh = (dec_dev_vdh *)dec_dev->handle;
    vdh_msg_pool_info *msg_pool = &vdh->vdh_msg_pool;
    vdh_ext_mem_info  *vdh_mem  = &vdh->vdh_ext_mem;
    vdh_msg_info *msg = TD_NULL;
    td_u16 msg_id = 0;

    for (msg_id = 0; msg_id < msg_pool->msg_num; msg_id++) {
        msg = &msg_pool->vdh_msg[msg_id];
        dec_hal_free_vdh_msg_mem(msg);
    }

    dec_hal_free_vdh_ext_mem(vdh_mem);

    return DEC_OK;
}

static td_s32 dec_hal_create_vdh_inst(dec_dev_info *dec_dev)
{
    dec_dev_vdh *vdh = TD_NULL;

    vdh = (dec_dev_vdh *)os_alloc_vir(dec_dev->dev_id, "mfde_inst", sizeof(dec_dev_vdh));
    if (vdh == TD_NULL) {
        return DEC_ERR;
    }

    dec_dev->handle = (td_void *)vdh;

    vfmw_check_sec_func(memset_s(vdh, sizeof(dec_dev_vdh), 0, sizeof(dec_dev_vdh)));

    return DEC_OK;
}

static td_s32 dec_hal_destroy_vdh_inst(dec_dev_info *dec_dev)
{
    if (dec_dev->handle != TD_NULL) {
        os_free_vir(dec_dev->handle);
        dec_dev->handle = TD_NULL;
    }

    return DEC_OK;
}

static td_s32 dec_hal_config_vdh(dec_dev_info *dec_dev)
{
    dec_dev_vdh *vdh = (dec_dev_vdh *)dec_dev->handle;
    UADDR  base_phy_addr = 0;
    td_u8 *base_vir_addr = 0;
    td_u32 i = 0;

    vfmw_check_ret(os_kmap_reg != TD_NULL, DEC_ERR);

    base_phy_addr = VDH_BASE_PHY_ADDR;
    base_vir_addr = os_kmap_reg(base_phy_addr, VDH_REGISTER_LEN);
    if (base_vir_addr == TD_NULL) {
        dprint(PRN_ALWS, "%s, %d base_vir_addr = NULL\n", __func__, __LINE__);
        return DEC_ERR;
    }

    vdh->vdh_reg_pool.reg_num = VDH_REG_NUM;
    vdh->vdh_msg_pool.msg_num = VDH_MSG_NUM;

    for (i = 0; i < vdh->vdh_reg_pool.reg_num; i++) {
        vdh->vdh_reg_pool.vdh_reg[i].reg_phy_addr = VDM_REG_PHY_ADDR;
        vdh->vdh_reg_pool.vdh_reg[i].reg_vir_addr = base_vir_addr + VDM_REG_PHY_ADDR - base_phy_addr;
    }

    return DEC_OK;
}

static td_s32 dec_hal_clear_vdh(dec_dev_info *dec_dev)
{
    dec_dev_vdh *vdh = (dec_dev_vdh *)dec_dev->handle;
    UADDR  base_phy_addr = 0;
    td_u8 *base_vir_addr = 0;
    td_u32 i = 0;

    base_phy_addr  = VDH_BASE_PHY_ADDR;
    base_vir_addr  = vdh->vdh_reg_pool.vdh_reg[0].reg_vir_addr;
    base_vir_addr -= VDM_REG_PHY_ADDR - base_phy_addr;

    os_kunmap_reg(base_vir_addr);

    for (i = 0; i < vdh->vdh_reg_pool.reg_num; i++) {
        vdh->vdh_reg_pool.vdh_reg[i].reg_phy_addr = 0;
        vdh->vdh_reg_pool.vdh_reg[i].reg_vir_addr = TD_NULL;
    }

    vdh->vdh_reg_pool.reg_num = 0;
    vdh->vdh_msg_pool.msg_num = 0;

    return DEC_OK;
}

static td_s32 dec_hal_open_vdh_clock(const dec_dev_info *dec_dev)
{
    vfmw_unused(dec_dev);

    return DEC_OK;
}

static td_s32 dec_hal_close_vdh_clock(const dec_dev_info *dec_dev)
{
    vfmw_unused(dec_dev);

    return DEC_OK;
}

static td_s32 dec_hal_enable_vdh(dec_dev_info *dec_dev)
{
    vfmw_unused(dec_dev);

    return DEC_OK;
}

static td_s32 dec_hal_disable_vdh(dec_dev_info *dec_dev)
{
    vfmw_unused(dec_dev);
    return DEC_OK;
}

static td_u32 dec_hal_calc_mdma_msg_mem_size(td_void)
{
    td_u32 size = 0;

    size += MDMA_PIC_MSG_SLOT_LEN;
    size += MDMA_MBG_MSG_SLOT_LEN;

    return size;
}

static td_s32 dec_hal_alloc_mdma_msg_mem(const dec_dev_info *dec_dev, mdma_msg_info *msg)
{
    td_s32 ret = DEC_ERR;
    mem_record mem = { 0 };
    td_u32 size = 0;
    UADDR  phy_addr = 0;
    td_u8 *vir_addr = TD_NULL;

    size = dec_hal_calc_mdma_msg_mem_size();

    /* VFMW_MdmaMsg */
    mem.length = size;
    mem.is_cached = 0;
    mem.type = DEV_HAL_MEM_TYPE;
    mem.unid = dec_dev->dev_id;
    mem.mem_name = "vfmw_mdma_msg";
    vfmw_check_goto(os_malloc != TD_NULL, exit);
    if (os_malloc(&mem) != OSAL_OK) {
        ret = DEC_ERR;
        goto exit;
    }

    phy_addr = mem.phy_addr;
    vir_addr = mem.vir_addr;
    msg->base_phy_addr = phy_addr;
    msg->base_vir_addr = vir_addr;
    msg->length      = size;

    ARRANGE_PTR(phy_addr, MDMA_PIC_MSG_SLOT_LEN, msg->pic_msg_phy_addr);
    ARRANGE_PTR(vir_addr, MDMA_PIC_MSG_SLOT_LEN, msg->pic_msg_vir_addr);

    ARRANGE_PTR(phy_addr, MDMA_MBG_MSG_SLOT_LEN, msg->mbg_msg_phy_addr);
    ARRANGE_PTR(vir_addr, MDMA_MBG_MSG_SLOT_LEN, msg->mbg_msg_vir_addr);

    ret = DEC_OK;
exit:
    return ret;
}

static td_s32 dec_hal_free_mdma_msg_mem(mdma_msg_info *msg)
{
    mem_record mem = { 0 };

    if (msg->base_vir_addr != TD_NULL) {
        mem.phy_addr = msg->base_phy_addr;
        mem.vir_addr = msg->base_vir_addr;
        mem.length  = msg->length;
        mem.is_cached = 0;
        os_free(&mem);
    }

    vfmw_check_sec_func(memset_s(msg, sizeof(mdma_msg_info), 0, sizeof(mdma_msg_info)));

    return DEC_OK;
}

static td_s32 dec_hal_create_mdma_inst(dec_dev_info *dec_dev)
{
    dec_dev_mdma *mdma = TD_NULL;

    mdma = (dec_dev_mdma *)os_alloc_vir(dec_dev->dev_id, "mdma_inst", sizeof(dec_dev_mdma));
    if (mdma == TD_NULL) {
        return DEC_ERR;
    }

    dec_dev->handle = (td_void *)mdma;

    vfmw_check_sec_func(memset_s(mdma, sizeof(dec_dev_mdma), 0, sizeof(dec_dev_mdma)));

    return DEC_OK;
}

static td_s32 dec_hal_destroy_mdma_inst(dec_dev_info *dec_dev)
{
    if (dec_dev->handle != TD_NULL) {
        os_free_vir(dec_dev->handle);
        dec_dev->handle = TD_NULL;
    }

    return DEC_OK;
}

static td_s32 dec_hal_config_mdma(dec_dev_info *dec_dev)
{
    dec_dev_mdma *mdma = (dec_dev_mdma *)dec_dev->handle;
    mdma_reg_info *reg = &mdma->reg;
    UADDR  base_phy_addr = 0;
    td_u8 *base_vir_addr = 0;

    base_phy_addr = VDH_BASE_PHY_ADDR;
    base_vir_addr = os_kmap_reg(base_phy_addr, VDH_REGISTER_LEN);
    if (base_vir_addr == TD_NULL) {
        return DEC_ERR;
    }

    reg->reg_phy_addr = base_phy_addr;
    reg->reg_vir_addr = base_vir_addr;

    return DEC_OK;
}

static td_s32 dec_hal_clear_mdma(dec_dev_info *dec_dev)
{
    dec_dev_mdma *mdma = (dec_dev_mdma *)dec_dev->handle;
    mdma_reg_info *reg = &mdma->reg;

    os_kunmap_reg(reg->reg_vir_addr);

    reg->reg_phy_addr = 0;
    reg->reg_vir_addr = TD_NULL;

    return DEC_OK;
}

static td_s32 dec_hal_alloc_mdma_mem(dec_dev_info *dec_dev)
{
    dec_dev_mdma *mdma = (dec_dev_mdma *)dec_dev->handle;
    mdma_msg_info *msg = &mdma->msg;
    td_s32 ret = DEC_ERR;

    ret = dec_hal_alloc_mdma_msg_mem(dec_dev, msg);

    return ret;
}

static td_s32 dec_hal_free_mdma_mem(dec_dev_info *dec_dev)
{
    dec_dev_mdma *mdma = (dec_dev_mdma *)dec_dev->handle;
    mdma_msg_info *msg = &mdma->msg;

    dec_hal_free_mdma_msg_mem(msg);

    return DEC_OK;
}

// interface
td_s32 dec_hal_get_ops(vfmw_vid_std vid_std, dec_hal_ops **ops)
{
    td_u16 ops_idx = 0;

    if (ops == TD_NULL) {
        return DEC_ERR;
    }

    for (ops_idx = 0; ops_idx < sizeof(g_dec_hal_ops) / sizeof(g_dec_hal_ops[0]); ops_idx++) {
        if (g_dec_hal_ops[ops_idx].vid_std == vid_std) {
            *ops = &g_dec_hal_ops[ops_idx];
            return DEC_OK;
        }
    }

    return DEC_OK;
}

td_s32 dec_hal_open_vdh(td_void *dec_dev)
{
    dec_dev_info *dev = (dec_dev_info *)dec_dev;

    if (dec_hal_create_vdh_inst(dev) != DEC_OK) {
        dprint(PRN_ERROR, "%s dec_hal_create_vdh_inst failed!\n", __func__);
        return DEC_ERR;
    }

    if (dec_hal_config_vdh(dev) != DEC_OK) {
        dprint(PRN_ERROR, "%s dec_hal_config_vdh failed!\n", __func__);
        goto exit1;
    }

    if (dec_hal_alloc_vdh_mem(dev) != DEC_OK) {
        dprint(PRN_ERROR, "%s dec_hal_alloc_vdh_mem failed!\n", __func__);
        goto exit2;
    }

    if (dec_hal_open_vdh_clock(dev) != DEC_OK) {
        dprint(PRN_ERROR, "%s dec_hal_open_vdh_clock failed!\n", __func__);
        goto exit3;
    }

    dec_hal_enable_vdh(dev);

    return DEC_OK;

exit3:
    dec_hal_free_vdh_mem(dev);
exit2:
    dec_hal_clear_vdh(dev);
exit1:
    dec_hal_destroy_vdh_inst(dev);

    return DEC_ERR;
}

td_s32 dec_hal_close_vdh(td_void *dec_dev)
{
    dec_dev_info *dev = (dec_dev_info *)dec_dev;

    dec_hal_disable_vdh(dev);

    dec_hal_close_vdh_clock(dev);

    dec_hal_free_vdh_mem(dev);

    dec_hal_clear_vdh(dev);

    dec_hal_destroy_vdh_inst(dev);

    return DEC_OK;
}

td_s32 dec_hal_reset_vdh(const td_void *dec_dev)
{
    td_s32 i;
    UADDR phy_addr;
    td_u8 *vir_addr = TD_NULL;
    td_u32 rst_req;
    td_u32 rst_ok;

    vfmw_unused(dec_dev);
    vfmw_check_ret(os_kmap_reg != TD_NULL, DEC_ERR);

    /* vdh crg */
    phy_addr = VDH_CRG_REG_PHY_ADDR;
    vir_addr = (td_u8 *)os_kmap_reg(phy_addr, VDH_CRG_REGISTER_LEN);
    vfmw_check_ret(vir_addr != TD_NULL, DEC_ERR);

    vfmw_check_goto(pdt_add_sleep_veto(VFMW_ID_VDH) == EXT_SUCCESS, err0);
    vfmw_check_goto(pdt_add_sleep_apb_veto(VFMW_ID_VDH) == EXT_SUCCESS, err1);

    rst_req = *((volatile td_u32*)(vir_addr + VDM_RST_REQ_OFFSET));
    ((vcrg_vdh_srst_req *)(&rst_req))->vdh_mfd_srst_req = 1;
    *((volatile td_u32*)(vir_addr + VDM_RST_REQ_OFFSET)) = rst_req;
    os_mb();

    /* check ok */
    for (i = 0; i < 1000; i++) { /* 1000 cnt */
        rst_ok = *((volatile td_u32*)(vir_addr + VDM_RST_OK_OFFSET));
        if (((vcrg_vdh_srst_req_ok *)(&rst_ok))->vdh_mfd_srst_ok == 1) {
            break;
        }
        os_udelay(10); /* 10 us */
    }
    if (i == 1000) { /* 1000 cnt */
        vdec_record_chr_event(-1, MT_VDEC_KEY_ERROR, MT_VDEC_VFMW_KEY_ERROR, MT_VDEC_VFMW_VDH_RESET_ERR);
        dprint(PRN_ALWS, "%s reset vdh error!\r\n", __func__);
        goto err2;
    }

    ((vcrg_vdh_srst_req *)(&rst_req))->vdh_mfd_srst_req = 0;
    *((volatile td_u32*)(vir_addr + VDM_RST_REQ_OFFSET)) = rst_req;
    os_mb();
    for (i = 0; i < 1000; i++) { /* 1000 cnt */
        rst_ok = *((volatile td_u32*)(vir_addr + VDM_RST_OK_OFFSET));
        if (((vcrg_vdh_srst_req_ok *)(&rst_ok))->vdh_mfd_srst_ok == 0) {
            break;
        }
        os_udelay(10); /* 10 us */
    }
    if (i == 1000) { /* 1000 cnt */
        vdec_record_chr_event(-1, MT_VDEC_KEY_ERROR, MT_VDEC_VFMW_KEY_ERROR, MT_VDEC_VFMW_VDH_RESET_ERR);
        dprint(PRN_ALWS, "%s reset vdh ok cancel error!\r\n", __func__);
        goto err2;
    }

    pdt_remove_sleep_apb_veto(VFMW_ID_VDH);
    pdt_remove_sleep_veto(VFMW_ID_VDH);
    os_kunmap_reg(vir_addr);

    return DEC_OK;

err2:
    pdt_remove_sleep_apb_veto(VFMW_ID_VDH);
err1:
    pdt_remove_sleep_veto(VFMW_ID_VDH);
err0:
    os_kunmap_reg(vir_addr);

    return DEC_ERR;
}

td_s32 dec_hal_resume_vdh(const td_void *dec_dev)
{
    vfmw_unused(dec_dev);
    return DEC_OK;
}

td_s32 dec_hal_suspend_vdh(const td_void *dec_dev)
{
    vfmw_unused(dec_dev);
    return DEC_OK;
}

td_s32 dec_hal_init_irq_vdh(const td_void *dec_dev)
{
    td_s32 ret = DEC_ERR;
    const td_u32 irq_flag = 0x80;
    static const td_char irq_name[4][14] = { /* 4 14 num */
        "vdec_vdh",
        "vdec_vdh_safe",
        "mmu_vdh",
        "mmu_vdh_safe"
    };

    vfmw_unused(dec_dev);

    ret = os_request_irq(VDM_PXP_INT_NUM, dec_dev_isr_pxp0, irq_flag, irq_name[VDH_IRQ_NAME_INDEX],
        &g_dec_irq_handle[0]);
    if (ret != OSAL_OK) {
        dprint(PRN_ERROR, "%s %d os_request_irq err,int num %d, name %s\n", __func__, __LINE__, VDM_PXP_INT_NUM,
            irq_name[VDH_IRQ_NAME_INDEX]);
        return DEC_ERR;
    }

    return DEC_OK;
}

td_s32 dec_hal_exit_irq_vdh(const td_void *dec_dev)
{
    const td_char irq_name[4][14] = {
        "vdec_vdh",
        "vdec_vdh_safe",
        "mmu_vdh",
        "mmu_vdh_safe"
    };

    vfmw_unused(dec_dev);

    os_free_irq(VDM_PXP_INT_NUM, irq_name[VDH_IRQ_NAME_INDEX], &g_dec_irq_handle[0]);

    return DEC_OK;
}

td_s32 dec_hal_config_msg_vdh(const td_void *dec_dev, const dec_hal_ops *ops, const td_void *dec_task)
{
    dec_dev_info *dev = (dec_dev_info *)dec_dev;
    dec_dev_vdh *vdh = (dec_dev_vdh *)dev->handle;
    dec_chan_task *task = (dec_chan_task *)dec_task;
    vdh_msg_info *msg = TD_NULL;
    td_s32 ret = DEC_ERR;

    ret = dev->ops->get_msg(dev, task->msg_id, &msg);
    if (ret != DEC_OK) {
        dprint(PRN_ERROR, "%s get_msg error!\n", __func__);
        return DEC_ERR;
    }

    ret = ops->write_vdh_pic_msg(msg->pic_msg_vir_addr, task->dps->pic_param, &task->mem_param, &vdh->vdh_ext_mem, msg);
    if (ret != DEC_OK) {
        dprint(PRN_ERROR, "%s write_vdh_pic_msg error!\n", __func__);
        return DEC_ERR;
    }

    ret = ops->write_vdh_slice_lst(task->dps->pic_param, &task->mem_param, &task->slice_list, msg->slice_msg_vir_addr,
        msg->slice_msg_phy_addr);
    if (ret != DEC_OK) {
        dprint(PRN_ERROR, "%s write_vdh_slice_lst error!\n", __func__);
        return DEC_ERR;
    }

    return DEC_OK;
}

td_s32 dec_hal_config_reg_vdh(const td_void *dec_dev, const dec_hal_ops *ops, const td_void *dec_task)
{
    dec_dev_info *dev = (dec_dev_info *)dec_dev;
    dec_chan_task *task = (dec_chan_task *)dec_task;
    vdh_reg_info *reg = TD_NULL;
    vdh_msg_info *msg = TD_NULL;
    td_s32 ret = DEC_ERR;

    ret = dev->ops->get_reg(dev, task->reg_id, &reg);
    if (ret != DEC_OK) {
        return DEC_ERR;
    }

    ret = dev->ops->get_msg(dev, task->msg_id, &msg);
    if (ret != DEC_OK) {
        return DEC_ERR;
    }

    WR_VREG(reg->reg_vir_addr, VREG_OFS_VDH_SELRST, 1); // enable self reset

    ret = ops->write_vdh_reg(reg->reg_vir_addr, task->dps->pic_param, &task->mem_param,
        msg->pic_msg_phy_addr, msg->up_msg_phy_addr);
    if (ret != DEC_OK) {
        return DEC_ERR;
    }

    return DEC_OK;
}

td_s32 dec_hal_start_dec(const td_void *dec_dev, td_u16 reg_id)
{
    dec_dev_info *dev = (dec_dev_info *)dec_dev;
    vdh_reg_info *reg = TD_NULL;

    dev->ops->get_reg(dev, reg_id, &reg);

    os_mb();

    WR_VREG(reg->reg_vir_addr, VREG_OFS_VDH_START, 0);
    WR_VREG(reg->reg_vir_addr, VREG_OFS_VDH_START, 1);
    WR_VREG(reg->reg_vir_addr, VREG_OFS_VDH_START, 0);

    return DEC_OK;
}

td_s32 dec_hal_cancel_dec(td_void *dec_dev, td_u16 reg_id)
{
    vfmw_unused(dec_dev);
    vfmw_unused(reg_id);

    return DEC_OK;
}

td_u16 dec_hal_read_olp_indicator(td_void *dec_dev, td_u16 reg_id)
{
    vfmw_unused(dec_dev);
    vfmw_unused(reg_id);

    return 0;
}

td_u16 dec_hal_read_ilp_indicator(td_void *dec_dev, td_u16 reg_id)
{
    vfmw_unused(dec_dev);
    vfmw_unused(reg_id);

    return 0;
}

td_s32 dec_hal_check_indicator(td_u16 idc, td_u16 cfg_id)
{
    vfmw_unused(idc);
    vfmw_unused(cfg_id);

    return DEC_OK;
}

td_s32 dec_hal_read_int_info(td_void *dec_dev, td_s32 reg_id)
{
    dec_dev_info *dev = TD_NULL;
    vdh_reg_info *reg = TD_NULL;
    td_u32 *vir = TD_NULL;
    td_u32 data = 0;
    td_u32 data1 = 0;
    td_bool err_flag = 0;
    td_bool stream_overflow_up = 0;

    vfmw_unused(reg_id);

    dev = (dec_dev_info *)dec_dev;
    vfmw_check_sec_func(memset_s(&dev->back_up, sizeof(dec_back_up), 0, sizeof(dec_back_up)));

    if (dev->type == DEC_DEV_TYPE_MDMA) {
        dprint(PRN_ERROR, "%s, %d, not support mdma now!\n", __func__, __LINE__);
    } else {
        dev->ops->get_reg(dev, (td_u16)reg_id, &reg);
        vir = (td_u32 *)(reg->reg_vir_addr + reg->base_ofs[0]);
        RD_VREG(vir, VREG_OFS_VDH_STATE, data);
        dev->back_up.int_flag = (data >> 17) & 0x1; /* move 17 bits right */
        dev->back_up.int_flag |= ((data >> 19) & 0x1) << 1; /* 19 is bit count */
        err_flag = (data >> 18) & 0x1; /* move 18 bits right */
        dev->back_up.slice_num = data & 0x1ffff;
        dev->back_up.int_reg = 0;
        dev->back_up.int_unid = 0;

        RD_VREG(vir, VREG_OFS_VDH_CYCLEPERPIC, data);
        dev->back_up.pxp_cycle = data;
        dev->back_up.bsp_cycle = 0;

        RD_VREG(vir, VREG_OFS_SED_STATE1_0, data1);
        stream_overflow_up = (data1 >> 22) & 0x1; /* move 22 bits right */
        if (err_flag == 0x1 && (stream_overflow_up == 0x1)) {
            dev->back_up.err_flag = 1;
            dev->back_up.rd_slc_msg = 1;
        }
    }

    return DEC_OK;
}

td_s32 dec_hal_disable_int_state_dec(td_void *dec_dev, td_u16 reg_id)
{
    dec_dev_info *dev = (dec_dev_info *)dec_dev;
    vdh_reg_info *reg = TD_NULL;
    td_u32 d32 = 0xFFFFFFFF;

    dev->ops->get_reg(dev, reg_id, &reg);

    WR_VREG(reg->reg_vir_addr, VREG_OFS_INT_MASK, d32);

    return DEC_OK;
}

td_s32 dec_hal_enable_int_state_dec(td_void *dec_dev, td_u16 reg_id)
{
    dec_dev_info *dev = (dec_dev_info *)dec_dev;
    vdh_reg_info *reg = TD_NULL;
    td_u32 d32 = 0xFFFFFFFE;

    dev->ops->get_reg(dev, reg_id, &reg);

    WR_VREG(reg->reg_vir_addr, VREG_OFS_INT_MASK, d32);

    return DEC_OK;
}

td_s32 dec_hal_query_int_state_dec(td_void *dec_dev, td_u16 reg_id)
{
    dec_dev_info *dev = (dec_dev_info *)dec_dev;
    vdh_reg_info *reg = TD_NULL;
    td_u32 d32 = 0;

    dev->ops->get_reg(dev, reg_id, &reg);

    RD_VREG(reg->reg_vir_addr, VREG_OFS_VDH_STATE, d32);

    d32 = (d32 >> 17) & 1; /* 17 num */

    return (d32 == 1 ? DEC_OK : DEC_ERR);
}

td_s32 dec_hal_clear_olp_int_state(td_void *dec_dev, td_u16 reg_id)
{
    dec_dev_info *dev = (dec_dev_info *)dec_dev;
    vdh_reg_info *reg = TD_NULL;
    td_u32 d32 = 0xFFFFFFFF;

    dev->ops->get_reg(dev, reg_id, &reg);

    WR_VREG(reg->reg_vir_addr, VREG_OFS_INT_STATE, d32);

    return DEC_OK;
}

td_s32 dec_hal_clear_int_state(td_void *dec_dev, td_u16 reg_id)
{
    dec_dev_info *dev = (dec_dev_info *)dec_dev;
    vdh_reg_info *reg = TD_NULL;
    mdma_reg_info *mdma_reg = TD_NULL;
    td_u32 d32 = 0xFFFFFFFF;

    if (dev->type == DEC_DEV_TYPE_MDMA) {
        dev->ops->get_reg(dev, reg_id, &mdma_reg);
        WR_VREG(mdma_reg->reg_vir_addr, VREG_OFS_INT_STATE, d32);
        return DEC_OK;
    }
    dev->ops->get_reg(dev, reg_id, &reg);

    WR_VREG(reg->reg_vir_addr, VREG_OFS_INT_STATE, d32);

    return DEC_OK;
}

STATIC td_void dec_hal_calc_disp_mem_param_set(dec_chan_mem_param *mem)
{
    mem->disp_mem.is_disp_need = 0;
    mem->disp_mem.vdh_2d_en = mem->vdh_2d_en;
    mem->disp_mem.bit_depth = mem->bit_depth;
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

STATIC td_void dec_hal_set_mem_param(dec_chan_mem_param *mem_param, mem_need_info *mem_need,
    mem_req_info *mem_req, dec_chan_cfg *cfg)
{
    mem_param->cmp_head_stride = mem_need->dec_info.head_stride;
    mem_param->cmp_head_size = mem_need->dec_info.head_size;
    mem_param->image_stride = mem_need->dec_info.dec_stride;
    mem_param->y_stride = mem_need->dec_info.dec_stride;
    mem_param->uv_offset = mem_req->slot_width * mem_req->slot_height + mem_param->cmp_head_size;
    if (mem_req->bit_depth > 8) { /* 8 bit */
        mem_param->nbit_image_stride = mem_param->image_stride >> 2; /* 2 num */
        mem_param->nbit_y_stride     = mem_param->y_stride >> 2; /* 2 num */
        mem_param->nbit_y_offset     = (mem_param->uv_offset * 3) >> 1; /* 3 num */
        mem_param->nbit_uv_offset    = (td_u32)((mem_need->dec_info.dec_stride * mem_req->slot_height) >> 2); /* 2 n */
    } else {
        mem_param->nbit_image_stride = 0;
        mem_param->nbit_y_stride     = 0;
        mem_param->nbit_y_offset     = 0;
        mem_param->nbit_uv_offset    = 0;
    }

    mem_param->low_delay_en = 0;
    mem_param->tunl_line_num = 10; /* 10 lines */
    mem_param->line_num_ofs = 0;
#ifdef VFMW_MODULE_LOWDLY_SUPPORT
    mem_param->low_delay_en = cfg->low_delay_en;
    mem_param->tunl_line_num = cfg->tunl_line_num;
    mem_param->line_num_ofs = mem_need->dec_info.line_num_offset;
#else
    vfmw_unused(cfg);
#endif
    mem_param->bit_depth = mem_req->bit_depth;
}

td_s32 dec_hal_calc_mem_param(td_void *dec_dev, const dec_hal_ops *ops, td_void *chan_cfg, td_void *chan_task)
{
    dec_chan_task *task = (dec_chan_task *)chan_task;
    dec_chan_mem_param *mem_param = &task->mem_param;
    dec_chan_cfg *cfg = (dec_chan_cfg *)chan_cfg;
    mem_req_info mem_req = { 0 };
    mem_need_info mem_need = { 0 };

    vfmw_unused(dec_dev);
#ifdef HI_SMMU_SUPPORT
    mem_param->vdh_mmu_en = 1;
    mem_param->mask_mmu_irq = cfg->mask_mmu_irq;
#else
    mem_param->vdh_mmu_en = 0;
    mem_param->mask_mmu_irq = 1;
#endif
#ifdef ENV_SOS_KERNEL
    mem_param->vdh_safe_flag = 1;
#else
    mem_param->vdh_safe_flag = 0;
#endif
    ops->calc_mem_param(task->dps->pic_param, cfg, mem_param);

    mem_req.vid_std = cfg->vid_std;
    mem_req.cmp_mode = cfg->vcmp_mode;
    mem_req.frm_layout = cfg->layout;
    mem_req.dec_width = task->dps->frm_width;
    mem_req.dec_height = task->dps->frm_height;
    mem_req.disp_height = task->dps->disp_width;
    mem_req.disp_width = task->dps->disp_height;
    mem_req.bit_depth = task->dps->bit_depth;
    mem_req.alloc_type = cfg->alloc_type;
    mem_req.frame_mbs_only = task->dps->frame_mbs_only;
    mem_req.is_max_buf_size_en = cfg->is_max_buf_size_en;
    mem_req.max_dec_width = cfg->max_dec_width;
    mem_req.max_dec_height = cfg->max_dec_height;
    pdt_get_mem_req(&mem_req, &mem_need);

    dec_hal_set_mem_param(mem_param, &mem_need, &mem_req, cfg);

    dec_hal_calc_disp_mem_param_set(mem_param);

    return DEC_OK;
}

td_s32 dec_hal_read_frm_msg(td_void *dec_dev, td_void *chan, td_void *task)
{
    vfmw_unused(dec_dev);
    vfmw_unused(chan);
    vfmw_unused(task);
    return DEC_OK;
}

td_s32 dec_hal_read_slc_msg(td_void *dec_dev, td_u16 reg_id, td_void *slice_list)
{
    dec_dev_info *dev = (dec_dev_info *)dec_dev;
    dec_dev_vdh *vdh = (dec_dev_vdh *)dev->handle;
    dec_report_info *dec_report = &vdh->dec_report;
    vdh_reg_info *reg = TD_NULL;
    vdh_msg_info *msg = TD_NULL;
    td_u32 *slot = TD_NULL;
    td_u32 i, step;
    td_u32 d32 = 0;
#ifndef VFMW_PRSAM_FLUSH_ALL_CACHE_SUPPORT
    td_u32 up_msg_size = 0;
#endif
    vfmw_unused(slice_list);

    dev->ops->get_reg(dev, reg_id, &reg);
    dev->ops->get_msg(dev, reg->msg_id, &msg);

    slot = (td_u32 *)msg->up_msg_vir_addr;

    RD_VREG(reg->reg_vir_addr, VREG_OFS_VDH_STATE, d32);
    d32 &= 0X1FFFF;

    if (d32 <= MAX_UP_MSG_SLICE_NUM) {
        dec_report->over_flow = 0;
        dec_report->slice_num = (td_u16)d32;
    } else {
        dec_report->over_flow = 1;
        dec_report->slice_num = MAX_UP_MSG_SLICE_NUM;
        dprint(PRN_ERROR, "UpMsg report slice num %d > %d.\n", d32, MAX_UP_MSG_SLICE_NUM);
    }
#ifndef VFMW_PRSAM_FLUSH_ALL_CACHE_SUPPORT
    up_msg_size = dec_hal_calc_vdh_up_msg_mem_size(dec_report->slice_num * H264_UP_MSG_SLOT_LEN);
    os_flush_cache(msg->up_msg_vir_addr, msg->up_msg_phy_addr, up_msg_size);
#endif
    step = H264_UP_MSG_SLOT_LEN / (td_u32)sizeof(td_u32);
    for (i = 0; i < dec_report->slice_num; i++, slot += step) {
        dec_report->slice_mb_range[i][0] = slot[1] & 0xFFFFF;
        dec_report->slice_mb_range[i][1] = slot[2] & 0xFFFFF; /* 2 idx */
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
        dprint(PRN_ERROR, "%s error!\n", __func__);
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
    td_s32 ret = DEC_ERR;
    td_s32 logic_id = 0;
    fsp_logic *logic_fs = TD_NULL;
    vdh_msg_info *msg = TD_NULL;
    vfmw_image *image = TD_NULL;
    dec_dev_info *dev = TD_NULL;
    dec_chan *chan = (dec_chan *)chan_arg;
    dec_chan_task *task = (dec_chan_task *)task_arg;
    vdh_reg_info *reg = TD_NULL;
    td_u32 *vir = TD_NULL;
    td_u32 data;

    dec_dev_get_dev((td_u16)dev_id, &dev);
    vfmw_alert_ret(dev != TD_NULL, DEC_ERR);

    ret = dev->ops->get_msg(dev, task->msg_id, &msg);
    vfmw_alert_ret(ret == DEC_OK, DEC_ERR);

    logic_id = dps_get_fsp_logic_id(chan->chan_id, task->dps);
    logic_fs = fsp_get_logic(chan->chan_id, logic_id);
    vfmw_alert_ret(logic_fs != TD_NULL, DEC_ERR);
    image = fsp_get_image(chan->chan_id, logic_id);
    vfmw_alert_ret(image != TD_NULL, DEC_ERR);

    dev->ops->get_reg(dev, task->reg_id, &reg);
    vfmw_alert_ret(reg != TD_NULL, DEC_ERR);

    vir = (td_u32 *)(reg->reg_vir_addr + reg->base_ofs[0]);

    if (fsp_is_dec_over(logic_fs) == TD_TRUE) { /* top&&bot valid */
        if (image->is_fld_flag == 1) {
            RD_VREG(vir, VREG_OFS_VDH_LUMA_PIXEL_SUM, data);
            image->pix_info.pix_sum_luma += data;
            RD_VREG(vir, VREG_OFS_VDH_CHROMA_PIXEL_SUM, data);
            image->pix_info.pix_sum_chroma += data;
        } else {
            RD_VREG(vir, VREG_OFS_VDH_LUMA_PIXEL_SUM, data);
            image->pix_info.pix_sum_luma = data;
            RD_VREG(vir, VREG_OFS_VDH_CHROMA_PIXEL_SUM, data);
            image->pix_info.pix_sum_chroma = data;
        }
        dprint(PRN_PIX_SUM, "\r\n ==pix sum== [0x%x, 0x%x]\r\n", image->pix_info.pix_sum_luma,
            image->pix_info.pix_sum_chroma);
    } else { /* top or bot valid */
        RD_VREG(vir, VREG_OFS_VDH_LUMA_PIXEL_SUM, data);
        image->pix_info.pix_sum_luma = data;
        RD_VREG(vir, VREG_OFS_VDH_CHROMA_PIXEL_SUM, data);
        image->pix_info.pix_sum_chroma = data;
    }

    return DEC_OK;
}

td_s32 dec_hal_open_mdma(td_void *dec_dev)
{
    dec_dev_info *dev = (dec_dev_info *)dec_dev;

    if (dec_hal_create_mdma_inst(dev) != DEC_OK) {
        goto exit;
    }

    if (dec_hal_config_mdma(dev) != DEC_OK) {
        goto exit;
    }

    if (dec_hal_alloc_mdma_mem(dev) != DEC_OK) {
        goto exit;
    }

    return DEC_OK;

exit:

    return DEC_ERR;
}

td_s32 dec_hal_close_mdma(td_void *dec_dev)
{
    dec_dev_info *dev = (dec_dev_info *)dec_dev;

    dec_hal_free_mdma_mem(dev);

    dec_hal_clear_mdma(dev);

    dec_hal_destroy_mdma_inst(dev);

    return DEC_OK;
}

td_s32 dechal_reset_mdma(td_void *dec_dev)
{
    vfmw_unused(dec_dev);
    return DEC_OK;
}

td_s32 dec_hal_init_irq_mdma(td_void *dec_dev)
{
    vfmw_unused(dec_dev);
    return DEC_OK;
}

td_s32 dec_hal_exit_irq_mdma(td_void *dec_dev)
{
    vfmw_unused(dec_dev);
    return DEC_OK;
}

td_s32 dec_hal_config_msg_mdma(td_void *dec_dev, dec_hal_ops *ops, td_void *dec_task)
{
    dec_dev_info *dev = (dec_dev_info *)dec_dev;
    dec_chan_task *task = (dec_chan_task *)dec_task;
    mdma_msg_info *msg = TD_NULL;
    td_s32 ret = DEC_ERR;

    ret = dev->ops->get_msg(dev, task->msg_id, &msg);
    if (ret != DEC_OK) {
        dprint(PRN_ERROR, "%s get_msg error!\n", __func__);
        return DEC_ERR;
    }

    vfmw_check_ret(ops->write_mdma_msg != TD_NULL, DEC_ERR);

    ret = ops->write_mdma_msg(msg->pic_msg_vir_addr, task->dps->pic_param, &task->mem_param, msg);
    if (ret != DEC_OK) {
        dprint(PRN_ERROR, "%s write_mdma_msg error!\n", __func__);
        return DEC_ERR;
    }

    return DEC_OK;
}

static td_s32 dec_hal_write_reg_mdma(td_u8 *reg_vir_addr, td_void *dec_task_mem, UADDR dn_msg_phy_addr)
{
    dec_chan_mem_param *task_mem = (dec_chan_mem_param *)dec_task_mem;
    td_u32 *reg = (td_u32 *)reg_vir_addr;
    td_u32 d32 = 0;

    d32 = dn_msg_phy_addr;
    WR_VREG(reg, VREG_OFS_AVM_ADDR, d32);

    d32 = 0x4000000; /* normal repair mode */
    WR_VREG(reg, VREG_OFS_BASIC_CFG0, d32);

    d32 = 0x2000C203;
    d32 |= task_mem->vdh_mmu_en << 12; /* 12 bit */
    WR_VREG(reg, VREG_OFS_BASIC_CFG1, d32);

    d32 = 0x00300C03;
    WR_VREG(reg, VREG_OFS_SED_TO, d32);
    WR_VREG(reg, VREG_OFS_ITRANS_TO, d32);
    WR_VREG(reg, VREG_OFS_PMV_TO, d32);
    WR_VREG(reg, VREG_OFS_PRC_TO, d32);
    WR_VREG(reg, VREG_OFS_RCN_TO, d32);
    WR_VREG(reg, VREG_OFS_DBLK_TO, d32);
    WR_VREG(reg, VREG_OFS_PPFD_TO, d32);

    WR_VREG(reg, VREG_OFS_INT_MASK, 0xFFFFFFFE);

    return DEC_OK;
}

td_s32 dec_hal_config_reg_mdma(td_void *dec_dev, dec_hal_ops *ops, td_void *dec_task)
{
    dec_dev_info *dev = (dec_dev_info *)dec_dev;
    dec_chan_task *task = (dec_chan_task *)dec_task;
    mdma_reg_info *reg = TD_NULL;
    mdma_msg_info *msg = TD_NULL;
    td_s32 ret = DEC_ERR;

    vfmw_unused(ops);
    ret = dev->ops->get_msg(dev, task->msg_id, &msg);
    if (ret != DEC_OK) {
        return DEC_ERR;
    }

    ret = dev->ops->get_reg(dev, task->reg_id, &reg);
    if (ret != DEC_OK) {
        return DEC_ERR;
    }

    ret = dec_hal_write_reg_mdma(reg->reg_vir_addr, &task->mem_param, msg->pic_msg_phy_addr);
    if (ret != DEC_OK) {
        return DEC_ERR;
    }

    return DEC_OK;
}

td_s32 dec_hal_start_rep(td_void *dec_dev, td_u16 reg_id)
{
    dec_dev_info *dev = (dec_dev_info *)dec_dev;
    mdma_reg_info *reg = TD_NULL;

    dev->ops->get_reg(dev, reg_id, &reg);

    os_mb();

    WR_VREG(reg->reg_vir_addr, VREG_OFS_VDH_START, 0);
    WR_VREG(reg->reg_vir_addr, VREG_OFS_VDH_START, 1);
    WR_VREG(reg->reg_vir_addr, VREG_OFS_VDH_START, 0);

    return DEC_OK;
}

td_s32 dec_hal_read_dec_reg(td_void *dec_dev, td_u16 reg_id)
{
    dec_dev_info *dev = (dec_dev_info *)dec_dev;
    dec_dev_vdh *vdh = (dec_dev_vdh *)dev->handle;
    dec_report_info *dec_report = &vdh->dec_report;
    vdh_reg_info *vdh_reg = TD_NULL;

    td_u32 d32 = 0;

    dev->ops->get_reg(dev, reg_id, &vdh_reg);

    if (dev->type == DEC_DEV_TYPE_VDH) {
        RD_VREG(vdh_reg->reg_vir_addr, VREG_OFS_MB0_QP_IN_CURR_PIC, d32);
        dec_report->prev_pic_mb0_qp = d32;

        d32 = 0;
        RD_VREG(vdh_reg->reg_vir_addr, VREG_OFS_SWITCH_ROUNDING, d32);
        dec_report->switch_rounding = d32;
    }

    return DEC_OK;
}

td_u32 dec_hal_get_init_mem_size_vdh(td_void *dec_dev)
{
    td_u32 size = 0;
    td_u32 vdh_msg_mem = 0;

    vfmw_unused(dec_dev);
    size = vfmw_align_up((td_u32)sizeof(dec_dev_vdh), DEC_DEV_VDH_UP_MSG_ALIGN);
    // inner
    vdh_msg_mem = vfmw_align_up((H264_MAX_SLICE_NUM * (td_u32)sizeof(UADDR)), DEC_DEV_VDH_UP_MSG_ALIGN);
    vdh_msg_mem += vfmw_align_up((H264_MAX_SLICE_NUM * (td_u32)sizeof(td_u8 *)), DEC_DEV_VDH_UP_MSG_ALIGN);
    vdh_msg_mem += dec_hal_calc_msg_size();
    size += vdh_msg_mem * VDH_MSG_NUM;
    // ext
    vdh_msg_mem = dec_hal_calc_vdh_ext_mem_size();
    size += vfmw_align_up(vdh_msg_mem, DEC_DEV_VDH_UP_MSG_ALIGN);

    return size;
}

td_s32 dec_hal_get_mem_need_size(const td_void *dec_dev, vfmw_svr_mem_need *mem)
{
    td_u32 size = 0;
    td_u32 vdh_msg_mem = 0;

    vfmw_check_ret(mem != TD_NULL, DEC_ERR);
    vfmw_unused(dec_dev);

    size = vfmw_align_up((td_u32)sizeof(dec_dev_vdh), DEC_DEV_VDH_UP_MSG_ALIGN);

    // inner
    vdh_msg_mem = vfmw_align_up((H264_MAX_SLICE_NUM * (td_u32)sizeof(UADDR)), DEC_DEV_VDH_UP_MSG_ALIGN);
    vdh_msg_mem += vfmw_align_up((H264_MAX_SLICE_NUM * (td_u32)sizeof(td_u8 *)), DEC_DEV_VDH_UP_MSG_ALIGN);
    vdh_msg_mem += dec_hal_calc_msg_size();
    size += vdh_msg_mem * VDH_MSG_NUM;

    // ext
    vdh_msg_mem = dec_hal_calc_vdh_ext_mem_size();
    size += vfmw_align_up(vdh_msg_mem, DEC_DEV_VDH_UP_MSG_ALIGN);
    mem->hal_size = size;

    mem->chan_cache_size = 0;
    mem->chan_no_cache_size = 0;
    mem->hal_umap_size = 0;

    return (td_s32)size;
}

td_s32 mdma_hal_get_init_mem_size(td_void *dec_dev)
{
    td_u32 size = 0;
    td_u32 msg_mem = 0;

    vfmw_unused(dec_dev);

    size = vfmw_align_up((td_u32)sizeof(dec_dev_mdma), DEC_DEV_VDH_UP_MSG_ALIGN);

    msg_mem = dec_hal_calc_mdma_msg_mem_size();

    size += vfmw_align_up(msg_mem, DEC_DEV_VDH_UP_MSG_ALIGN);

    return (td_s32)size;
}

STATIC td_s32 dec_hal_get_dec_num(td_void)
{
    td_s32 ret;
    td_u32 cnt;
    td_s32 id;
    td_s32 dec_num = 0;
    dec_dev_info *dev = TD_NULL;
    dec_dev_vdh *vdh = TD_NULL;
    vdh_reg_info *reg = TD_NULL;

    for (cnt = 0; cnt < DEC_DEV_NUM; cnt++) {
        ret = dec_dev_get_dev((td_u16)cnt, &dev);
        if (ret != DEC_OK ||
            dev->handle == TD_NULL ||
            dev->type != DEC_DEV_TYPE_VDH) {
            continue;
        }

        vdh = (dec_dev_vdh *)dev->handle;
        for (id = 0; id < VDH_REG_NUM; id++) {
            reg = &vdh->vdh_reg_pool.vdh_reg[id];
            if (reg->used == 1) {
                dec_num++;
            }
        }
    }

    return dec_num;
}

static td_u32 dec_hal_get_pxp_num(td_void)
{
    return g_pxp_num;
}

STATIC td_u32 dec_hal_calc_pxp_num(const dec_chan *chan, const dps_info *dps)
{
    vfmw_unused(chan);
    vfmw_unused(dps);
    return VDH_PXP_NUM;
}

STATIC td_s32 dec_hal_wait_vdh(const dec_chan *chan, const dec_chan_task *task)
{
    td_s32 dec_num;
    const td_s32 max_dec_num = 4; /* 4 frm void vdh bug */

    vfmw_check_ret(chan != TD_NULL, DEC_ERR);
    vfmw_check_ret(task != TD_NULL, DEC_ERR);
    vfmw_check_ret(task->dps != TD_NULL, DEC_ERR);

    dec_num = dec_hal_get_dec_num();
    if (dec_num == 0) {
        return DEC_OK;
    } else if (dec_num >= max_dec_num) {
        return DEC_ERR;
    }

    if (dec_hal_get_pxp_num() != dec_hal_calc_pxp_num(chan, task->dps)) {
        return DEC_ERR;
    }

    return DEC_OK;
}

STATIC td_s32 dec_hal_is_vdh_int(td_u16 dev_id, const td_s32 *reg_id)
{
    dec_dev_info *dev = TD_NULL;
    vdh_reg_info *reg = TD_NULL;
    td_u32 *vir = TD_NULL;
    td_u32 int_flag;
    td_u32 data;

    dec_dev_get_dev(dev_id, &dev);
    if (dev == TD_NULL || dev->ops == TD_NULL || dev->ops->get_reg == TD_NULL) {
        dprint(PRN_ERROR, "get int_flag error!\n");
        return DEC_ERR;
    }

    dev->ops->get_reg(dev, (td_u16)(*reg_id), &reg);
    vfmw_alert_ret(reg != TD_NULL, DEC_ERR);

    vir = (td_u32 *)(reg->reg_vir_addr + reg->base_ofs[0]);
    RD_VREG(vir, VREG_OFS_VDH_STATE, data);
    int_flag = ((data >> 17) & 0x1); /* 17:dec_over */
    if (int_flag == 0) {
        return DEC_ERR;
    }

    return DEC_OK;
}

td_s32 dec_hal_get_dev(td_s32 is_sec, td_void *chan_task, td_u16 *dev_id)
{
    vfmw_unused(chan_task);
    vfmw_check_ret(dev_id != TD_NULL, DEC_ERR);

    *dev_id = (is_sec == 0) ? 0 : 1;

    return DEC_OK;
}

td_s32 dec_hal_control(td_u16 dev_id, td_s32 cid, const td_void *arg1, const td_void *arg2)
{
    td_s32 ret;
    dec_chan *chan = TD_NULL;
    dec_chan_task *task = TD_NULL;

    switch (cid) {
        case DHAL_CID_WAIT_DEC:
            chan = (dec_chan *)arg1;
            task = (dec_chan_task *)arg2;
            ret = dec_hal_wait_vdh(chan, task);
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

