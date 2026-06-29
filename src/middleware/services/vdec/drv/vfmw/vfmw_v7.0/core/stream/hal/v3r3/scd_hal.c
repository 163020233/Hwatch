/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#include "scd_hal.h"
#include "stm_dev.h"
#include "stm_base.h"
#include "stm_ctrl.h"
#include "vfmw_osal.h"
#include "vfmw_sys.h"
#include "vfmw_pdt.h"
#include "vfmw_osal.h"
#include "vfmw_define.h"
#include "vdec_chr.h"

STATIC td_s32 g_scd_irq_handle[SCD_NUM];

td_u32 scd_hal_msg_size(td_s32 scd_id)
{
    vfmw_unused(scd_id);
    return SCD_DMSG_SIZE + SCD_UMSG_SIZE;
}

static td_void scd_hal_enable(td_s32 scd_id)
{
    vfmw_unused(scd_id);
    return;
}

static td_void scd_hal_disable(td_s32 scd_id)
{
    vfmw_unused(scd_id);
    return;
}

td_s32 scd_hal_open(td_s32 scd_id)
{
    UADDR phy;
    td_u8 *vir = TD_NULL;
    td_u32 size;
    stm_dev *dev = TD_NULL;
    mem_record mem;

    size = SCD_REG_LEN;
    phy = SCD_REG_PHY_ADDR;

    if (os_kmap_reg == TD_NULL || os_malloc == TD_NULL) {
        dprint(PRN_ERROR, "os_kmap_reg or os_malloc is null !\n");
        return STM_ERR;
    }

    vir = os_kmap_reg(phy, size);
    if (vir == TD_NULL) {
        dprint(PRN_ERROR, "map scd%d reg failed !\n", scd_id);
        return STM_ERR;
    }

    dev = stm_ctrl_get_dev(scd_id);
    dev->reg_phy = phy;
    dev->reg_vir = vir;
    dev->reg_size = size;

    size = scd_hal_msg_size(scd_id);

    /* "vfmw_scd_msg" */
    vfmw_check_sec_func(memset_s(&mem, sizeof(mem), 0, sizeof(mem)));
    mem.length = vfmw_align_up(size, 16); /* 16 is align len */
    mem.is_cached = 0;
    mem.type = DEV_HAL_MEM_TYPE;
    mem.unid = (td_u32)scd_id;
    mem.mem_name = "vfmw_scd_msg";
    if (os_malloc(&mem) != OSAL_OK) {
        os_kunmap_reg(vir);
        dprint(PRN_ERROR, "%s alloc scd msg failed!\n", __func__);
        return STM_ERR;
    }

    dev->dn_msg_phy = mem.phy_addr;
    dev->dn_msg_vir = mem.vir_addr;
    dev->dn_msg_size = SCD_DMSG_SIZE;

    dev->up_msg_phy = mem.phy_addr + dev->dn_msg_size;
    dev->up_msg_vir = mem.vir_addr + dev->dn_msg_size;
    dev->up_msg_size = SCD_UMSG_SIZE;

    stm_ctrl_get_entry()->hal_size += size;

    scd_hal_enable(scd_id);
    scd_hal_clear_int(scd_id);
    scd_hal_enable_int(scd_id);

    return STM_OK;
}

td_s32 scd_hal_close(td_s32 scd_id)
{
    stm_dev *dev = TD_NULL;
    mem_record mem = { 0 };

    scd_hal_disable(scd_id);

    dev = stm_ctrl_get_dev(scd_id);

    /* unmap reg */
    os_kunmap_reg((td_u8 *)dev->reg_vir);

    dev->reg_phy = 0;
    dev->reg_vir = TD_NULL;
    dev->reg_size = 0;

    /* unmap msg */
    mem.dma_buf = dev->msg_dma_buf;
    mem.phy_addr = dev->dn_msg_phy;
    mem.vir_addr = dev->dn_msg_vir;
    mem.length = dev->dn_msg_size + dev->up_msg_size;
    mem.is_cached = 0;
    os_free(&mem);

    dev->dn_msg_phy = 0;
    dev->dn_msg_vir = TD_NULL;
    dev->dn_msg_size = 0;

    dev->up_msg_phy = 0;
    dev->up_msg_vir = TD_NULL;
    dev->up_msg_size = 0;

    stm_ctrl_get_entry()->hal_size -= mem.length;

    return STM_OK;
}

td_s32 scd_hal_irq_init(td_s32 dev_id)
{
    td_s32 ret;
    const td_char *name = TD_NULL;
    const td_u32 irq_flag = 0x80;
    STATIC const td_char irq_name[1][14] = { /* 1 is str num, 14 is char num */
        "int_scd_norm",
    };
    td_u32 irq_num;
    fn_stm_dev_isr isr_func;

    irq_num = SCD_INT_NUM;
    isr_func = stm_dev_isr0;
    name = irq_name[0];

    ret = os_request_irq(irq_num, isr_func, irq_flag, name, &g_scd_irq_handle[dev_id]);
    if (ret != OSAL_OK) {
        dprint(PRN_ERROR, "%s %d os_request_irq err,int num %u name %s\n", __func__, __LINE__, irq_num, irq_name);
        return STM_ERR;
    }

    return STM_OK;
}

td_s32 scd_hal_irq_exit(td_s32 dev_id)
{
    const td_char *name = TD_NULL;
    const td_char irq_name[1][14] = { /* 1 is str num, 14 is char num */
        "int_scd_norm",
    };
    td_u32 irq_num;

    irq_num = SCD_INT_NUM;
    name = irq_name[0];

    os_free_irq(irq_num, name, &g_scd_irq_handle[dev_id]);

    return STM_OK;
}

td_s32 scd_hal_reset(td_s32 scd_id)
{
    td_s32 i;
    UADDR phy_addr;
    td_u8 *vir_addr = TD_NULL;
    td_u32 rst_req;
    td_u32 rst_ok;

    vfmw_unused(scd_id);
    vfmw_check_ret(os_kmap_reg != TD_NULL, STM_ERR);

    /* vdh crg */
    phy_addr = VDH_CRG_REG_PHY_ADDR;
    vir_addr = (td_u8 *)os_kmap_reg(phy_addr, VDH_CRG_REGISTER_LEN);
    vfmw_check_ret(vir_addr != TD_NULL, STM_ERR);

    rst_req = *((volatile td_u32*)(vir_addr + VDM_RST_REQ_OFFSET));
    ((vcrg_vdh_srst_req *)(&rst_req))->vdh_scd_srst_req = 1;
    *((volatile td_u32*)(vir_addr + VDM_RST_REQ_OFFSET)) = rst_req;
    os_mb();

    /* check ok */
    for (i = 0; i < 1000; i++) { /* 1000 cnt */
        rst_ok = *((volatile td_u32*)(vir_addr + VDM_RST_OK_OFFSET));
        if (((vcrg_vdh_srst_req_ok *)(&rst_ok))->vdh_scd_srst_ok == 1) {
            break;
        }
        os_udelay(10); /* 10 us */
    }
    if (i == 1000) { /* 1000 cnt */
        dprint(PRN_ALWS, "%s reset scd error!\r\n", __func__);
        vdec_record_chr_event(-1, MT_VDEC_KEY_ERROR, MT_VDEC_VFMW_KEY_ERROR, MT_VDEC_VFMW_SCD_RESET_ERR);
        goto err;
    }

    ((vcrg_vdh_srst_req *)(&rst_req))->vdh_scd_srst_req = 0;
    *((volatile td_u32*)(vir_addr + VDM_RST_REQ_OFFSET)) = rst_req;
    os_mb();
    for (i = 0; i < 1000; i++) { /* 1000 cnt */
        rst_ok = *((volatile td_u32*)(vir_addr + VDM_RST_OK_OFFSET));
        if (((vcrg_vdh_srst_req_ok *)(&rst_ok))->vdh_scd_srst_ok == 0) {
            break;
        }
        os_udelay(10); /* 10 us */
    }
    if (i == 1000) { /* 1000 cnt */
        dprint(PRN_ALWS, "%s reset scd ok cancel error!\r\n", __func__);
        vdec_record_chr_event(-1, MT_VDEC_KEY_ERROR, MT_VDEC_VFMW_KEY_ERROR, MT_VDEC_VFMW_SCD_RESET_ERR);
        goto err;
    }

    os_kunmap_reg(vir_addr);

    return STM_OK;

err:
    os_kunmap_reg(vir_addr);

    return STM_ERR;
}

td_void scd_hal_start(td_s32 scd_id)
{
    vfmw_unused(scd_id);
    os_mb();
    WR_SCDREG(scd_id, REG_SCD_START, 0);
    WR_SCDREG(scd_id, REG_SCD_START, 1);
}

td_s32 scd_hal_is_bypass(td_s32 vid_std, const vfmw_std_ext *std_ext)
{
    vfmw_check_ret(std_ext != TD_NULL, STM_ERR);

    if ((vid_std == VFMW_H264) ||
        (vid_std == VFMW_HEVC) ||
        (vid_std == VFMW_MPEG2) ||
        (vid_std == VFMW_MPEG4) ||
        (vid_std == VFMW_H263) ||
        (vid_std == VFMW_SORENSON) ||
        (vid_std == VFMW_AVS) ||
        (vid_std == VFMW_AVS2) ||
        (vid_std == VFMW_AVS3) ||
        (vid_std == VFMW_MVC) ||
        ((vid_std == VFMW_VC1) &&
         (std_ext->vc1_ext.is_adv_profile != 0))) {
        return 0;
    }

    return 1;
}

static td_u32 scd_hal_std_type(td_u32 vid_type)
{
    td_u32 scd_type;

    if ((vid_type == VFMW_H264) ||
        (vid_type == VFMW_HEVC) ||
        (vid_type == VFMW_VC1) ||
        (vid_type == VFMW_MVC)) {
        scd_type = 0;
    } else if (vid_type == VFMW_MPEG4 || vid_type == VFMW_H263) {
        scd_type = 2; /* 2, 0010, MPEG4 */
    } else if (vid_type == VFMW_SORENSON) {
        scd_type = 4; /* 4, 0100, H263 */
    } else if (vid_type == VFMW_MPEG2) {
        scd_type = 3; /* 3, 0100, MPEG2 */
    } else if (vid_type == VFMW_AVS || vid_type == VFMW_AVS2 || vid_type == VFMW_AVS3) {
        scd_type = 6; /* 6, 0110, AVS */
    } else {
        scd_type = (td_u32)vid_type;
    }

    return scd_type;
}

static td_u8 scd_hal_slice_check(td_s32 vid_type)
{
    td_u8 slice_check;

    if ((vid_type == VFMW_MPEG2) ||
        (vid_type == VFMW_MPEG4) ||
        (vid_type == VFMW_H263) ||
        (vid_type == VFMW_SORENSON) ||
        (vid_type == VFMW_AVS) ||
        (vid_type == VFMW_AVS2) ||
        (vid_type == VFMW_AVS3)) {
        slice_check = 1;
    } else {
        slice_check = 0;
    }

    return slice_check;
}

td_void scd_hal_mask_int(td_s32 scd_id)
{
    WR_SCDREG(scd_id, REG_SCD_INT_MASK, 1);
}

td_void scd_hal_enable_int(td_s32 scd_id)
{
    WR_SCDREG(scd_id, REG_SCD_INT_MASK, 0);
}

td_void scd_hal_clear_int(td_s32 scd_id)
{
    WR_SCDREG(scd_id, REG_SCD_INT_CLR, 1);
}

td_s32 scd_hal_set_reg(td_s32 chan_id, td_s32 scd_id)
{
    td_s32 seg_num;
    td_s32 stream_num;
    scd_reg *reg = TD_NULL;
    stm_dev *dev = TD_NULL;
    stm_chan *chan = TD_NULL;
    seg_list *seg_lst = TD_NULL;
    const vfmw_product_info *pdt_info = pdt_get_product_info();

    dev = stm_ctrl_get_dev(scd_id);
    reg = &(dev->reg);
    chan = STM_CTRL_GET_CHAN(chan_id);
    seg_lst = chan->seg_list_info;

    seg_num = stm_seg_get_free_num(chan_id);
    stream_num = (seg_num > SCD_UMSG_NUM) ? SCD_UMSG_NUM : seg_num;

    reg->dn_msg_phy = dev->dn_msg_phy;
    reg->up_msg_phy = dev->up_msg_phy;
    reg->up_step = SCD_UMSG_STEP;
    vfmw_check_ret((td_u32)stream_num * reg->up_step < (td_u32)VFMW_MAX_SINT32, STM_ERR);
    reg->up_len = (td_u32)stream_num * reg->up_step;
    reg->buffer_first = seg_lst->seg_buf_phy_addr;
    reg->buffer_last = seg_lst->seg_buf_phy_addr + (UADDR)seg_lst->seg_buf_size;
    reg->std_type = scd_hal_std_type(chan->cfg.vid_std);
    reg->slice_check = scd_hal_slice_check(chan->cfg.vid_std);
    reg->avs_flag = (chan->cfg.vid_std == VFMW_AVS) ? 1 : 0;

    reg->int_unid = 0;
    reg->cfg_unid = (((td_u32)chan_id + 1) << 4) | ((td_u32)dev->start_cnt & 7); /* 4 is bit ofs, 7 is vfmw_max value */

    if (seg_lst->first_start_flag == 1) {
        reg->buffer_init = vfmw_align_up(seg_lst->seg_buf_write_addr, 4); /* 4 is align len */
    } else {
        reg->buffer_init = seg_lst->seg_next_addr;
    }

    reg->pre_lsb = seg_lst->pre_lsb;
    reg->pre_msb = seg_lst->pre_msb;

    if (pdt_info->smmu_support == TD_TRUE) {
        reg->scd_mmu_en = 1;
        reg->vdh_mmu_en = 1;
    } else {
        reg->scd_mmu_en = 0;
        reg->vdh_mmu_en = 0;
    }

    reg->safe_flag = (td_u8)chan->is_sec;
    reg->int_mask = (td_u8)dev->int_mask;
    reg->scd_start = 1;

    return STM_OK;
}

td_u32 scd_hal_src_eaten(const td_u32 *msg)
{
    vfmw_check_ret(msg != TD_NULL, 0);

    return (msg[1] & 0x3fffffff);
}

UADDR scd_hal_get_seg_addr(const td_u32 *msg)
{
    UADDR phy_addr;
    vfmw_check_ret(msg != TD_NULL, 0);

    phy_addr = (UADDR)((((td_u64)(msg[2]) & 0xf0000000) << 4) | ((td_u64)(msg[0]))); /* 2 is word ofs, 4 is bit ofs */

    return phy_addr;
}

static td_u32 scd_hal_get_valid_num(td_u32 valid_byte)
{
    td_u32 bit_shift;
    td_u32 count = 0;
    const td_u32 byte_len = 6;

    for (bit_shift = 0; bit_shift < byte_len; bit_shift++) {
        if (((valid_byte >> bit_shift) & 0x1) == 1) {
            count++;
        } else {
            break;
        }
    }

    return count;
}

static td_u32 scd_hal_save_pre_bytes(td_s32 scd_id)
{
    td_u32 cnt;
    td_s32 chan_id;
    td_u32 valid_num;
    td_ulong lock_flag;
    scd_reg *reg = TD_NULL;
    td_u8 src_byte[6]; /* 6 is scd prev byte num */
    td_u8 dst_byte[6] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }; /* 6 is scd prev byte num */
    seg_list *seg_lst = TD_NULL;
    stm_dev *dev = TD_NULL;

    dev = stm_ctrl_get_dev(scd_id);
    reg = &(dev->reg);
    valid_num = scd_hal_get_valid_num(reg->byte_valid);

    /* for cpu load do not use memcpy */
    *(td_u32 *)src_byte = reg->pre_lsb;
    *(td_u16 *)(src_byte + 4) = reg->pre_msb & 0xffff; /* 4 is byte offset */

    for (cnt = 0; cnt < valid_num; cnt++) {
        dst_byte[cnt] = src_byte[cnt];
    }

    chan_id = dev->chan_id;
    stm_chan_lock(chan_id, &lock_flag);
    if (STM_CTRL_GET_CHAN(chan_id)->handle == TD_NULL) {
        stm_chan_unlock(chan_id, &lock_flag);
        return STM_ERR;
    }
    seg_lst = STM_CTRL_GET_CHAN(chan_id)->seg_list_info;
    seg_lst->pre_lsb = *(td_u32 *)dst_byte;
    seg_lst->pre_msb = *(td_u16 *)(dst_byte + 4); /* 4 is byte offset */
    stm_chan_unlock(chan_id, &lock_flag);

    return STM_OK;
}

td_void scd_hal_read_reg(td_s32 scd_id)
{
    volatile td_u8 *reg_vir = TD_NULL;
    stm_dev *dev = TD_NULL;
    scd_reg *reg = TD_NULL;

    dev = stm_ctrl_get_dev(scd_id);
    reg = &(dev->reg);
    reg_vir = dev->reg_vir;

    reg->scd_over = RD_SCDREG_VIR(reg_vir, REG_SCD_OVER) & 0x1;
    reg->pre_lsb = RD_SCDREG_VIR(reg_vir, REG_PRE_BYTE_LSB);
    reg->pre_msb = RD_SCDREG_VIR(reg_vir, REG_PRE_BYTE_MSB);
    reg->byte_valid = RD_SCDREG_VIR(reg_vir, REG_BYTE_VALID);
    reg->short_num = (RD_SCDREG_VIR(reg_vir, REG_SCD_NUM) >> 22) & 0x3FF; /* 22 is bit ofs */
    reg->start_num = (RD_SCDREG_VIR(reg_vir, REG_SCD_NUM)) & 0x3FFFFF;
    reg->roll_addr = RD_SCDREG_VIR(reg_vir, REG_ROLL_ADDR);
    reg->src_eaten = RD_SCDREG_VIR(reg_vir, REG_SRC_EATEN);
    reg->up_len = RD_SCDREG_VIR(reg_vir, REG_SCD_UP_LEN);
    reg->next_addr = RD_SCDREG_VIR(reg_vir, REG_SEG_NEXT_ADDR);
    reg->int_unid = RD_SCDREG_VIR(reg_vir, REG_SCD_PROTOCOL) >> 16; /* 16 is bit ofs */

    if (dev->cancel == 0) { /* for test cancel */
        scd_hal_save_pre_bytes(scd_id);
    }
}

static td_void scd_hal_print_reg(td_s32 scd_id)
{
    volatile td_u8 *reg_vir = TD_NULL;
    stm_dev *dev = TD_NULL;
    scd_reg *reg = TD_NULL;
    td_u32 data;

    dev = stm_ctrl_get_dev(scd_id);
    reg = &(dev->reg);
    reg_vir = dev->reg_vir;

    reg->dn_msg_phy = RD_SCDREG_VIR(reg_vir, REG_LIST_ADDRESS);
    reg->dn_msg_phy <<= SCD_ADDR_SHIFT;
    reg->up_msg_phy = RD_SCDREG_VIR(reg_vir, REG_UP_ADDRESS);
    reg->up_msg_phy <<= SCD_ADDR_SHIFT;
    reg->up_len = RD_SCDREG_VIR(reg_vir, REG_SCD_UP_LEN);
    reg->buffer_first = RD_SCDREG_VIR(reg_vir, REG_BUFFER_FIRST);
    reg->buffer_first <<= SCD_ADDR_SHIFT;
    reg->buffer_last = RD_SCDREG_VIR(reg_vir, REG_BUFFER_LAST);
    reg->buffer_last <<= SCD_ADDR_SHIFT;
    reg->buffer_init = RD_SCDREG_VIR(reg_vir, REG_BUFFER_INIT);
    reg->pre_lsb = RD_SCDREG_VIR(reg_vir, REG_PRE_BYTE_LSB);
    reg->pre_msb = RD_SCDREG_VIR(reg_vir, REG_PRE_BYTE_MSB);
    reg->int_mask = (td_u8)(RD_SCDREG_VIR(reg_vir, REG_SCD_INT_MASK));
    reg->scd_start = (td_u8)(RD_SCDREG_VIR(reg_vir, REG_SCD_START));
    data = RD_SCDREG_VIR(reg_vir, REG_SCD_PROTOCOL);

    (td_void)data; /* avoid to build err when printing is not supported. */
    dprint(PRN_ALWS, "print scd[%d] reg \n", scd_id);
    dprint(PRN_ALWS, "dn_msg_phy = %x \n", reg->dn_msg_phy);
    dprint(PRN_ALWS, "up_msg_phy = %x \n", reg->up_msg_phy);
    dprint(PRN_ALWS, "up_len = %x \n", reg->up_len);
    dprint(PRN_ALWS, "buffer_first = %x \n", reg->buffer_first);
    dprint(PRN_ALWS, "buffer_last = %x \n", reg->buffer_last);
    dprint(PRN_ALWS, "buffer_init = %x \n", reg->buffer_init);
    dprint(PRN_ALWS, "pre_lsb = %x \n", reg->pre_lsb);
    dprint(PRN_ALWS, "pre_msb = %x \n", reg->pre_msb);
    dprint(PRN_ALWS, "int_mask = %x \n", reg->int_mask);
    dprint(PRN_ALWS, "scd_start = %x \n", reg->scd_start);
    dprint(PRN_ALWS, "scd_protocol = %x \n", data);
}

td_s32 scd_hal_write_reg(td_s32 scd_id)
{
    td_u32 data;
    volatile td_u8 *reg_vir = TD_NULL;
    stm_dev *dev = TD_NULL;
    scd_reg *reg = TD_NULL;

    dev = stm_ctrl_get_dev(scd_id);
    reg = &(dev->reg);
    reg_vir = dev->reg_vir;

    WR_SCDREG_VIR (reg_vir, REG_LIST_ADDRESS, (td_u32)(reg->dn_msg_phy >> SCD_ADDR_SHIFT));
    WR_SCDREG_VIR (reg_vir, REG_UP_ADDRESS, (td_u32)(reg->up_msg_phy >> SCD_ADDR_SHIFT));
    WR_SCDREG_VIR(reg_vir, REG_SCD_UP_LEN, reg->up_len);
    WR_SCDREG_VIR (reg_vir, REG_BUFFER_FIRST, (td_u32)(reg->buffer_first >> SCD_ADDR_SHIFT));
    WR_SCDREG_VIR (reg_vir, REG_BUFFER_LAST, (td_u32)(reg->buffer_last >> SCD_ADDR_SHIFT));
    WR_SCDREG_VIR(reg_vir, REG_BUFFER_INIT, (td_u32)reg->buffer_init);
    WR_SCDREG_VIR(reg_vir, REG_BUFFER_INIT_MSB, 0); /* 32 get high bits */

    data = (reg->cfg_unid << 16) | /* 16 is bit ofs */
           (reg->vdh_mmu_en << 13) | /* 13 is bit ofs */
           (reg->scd_mmu_en << 9) | /* 9 is bit ofs */
           (reg->safe_flag << 7) | /* 7 is bit ofs */
           (reg->slice_check << 4) | /* 4 is bit ofs */
           (reg->std_type & 0xf);
    WR_SCDREG_VIR(reg_vir, REG_SCD_PROTOCOL, data);
    WR_SCDREG_VIR(reg_vir, REG_PRE_BYTE_LSB, reg->pre_lsb);
    WR_SCDREG_VIR(reg_vir, REG_PRE_BYTE_MSB, reg->pre_msb);
    WR_SCDREG_VIR(reg_vir, REG_SCD_INT_MASK, reg->int_mask);

    if (dcheck(PRN_SCD_REGMSG)) {
        scd_hal_print_reg(scd_id);
    }

    return STM_OK;
}

#ifdef VFMW_SCD_DFX_SUPPORT
td_void scd_hal_read_int_state(td_s32 scd_id)
{
    volatile td_u8 *reg_vir;
    stm_dev *dev;
    scd_reg *reg;

    dev = stm_ctrl_get_dev(scd_id);
    reg = &(dev->reg);
    reg_vir = (td_u8 *)dev->reg_vir;

    reg->scd_over = RD_SCDREG_VIR(reg_vir, REG_SCD_OVER) & 0x1;
    reg->start_num = (RD_SCDREG_VIR(reg_vir, REG_SCD_NUM)) & 0x3FFFFF;
    reg->up_len = RD_SCDREG_VIR(reg_vir, REG_SCD_UP_LEN);

    if (reg->scd_over == 1) {
        stm_dev_isr0(0, TD_NULL);
    }
}

td_void scd_hal_print_dn_msg(td_s32 scd_id)
{
    td_u32 raw_cnt;
    td_s32 msg_cnt;
    td_u32 raw_num;
    UADDR dn_msg_phy;
    td_u32 *dn_msg_vir = TD_NULL;
    stm_dev *dev = TD_NULL;
    stm_chan *chan = TD_NULL;

    dev = stm_ctrl_get_dev(scd_id);
    dn_msg_phy = dev->dn_msg_phy;
    dn_msg_vir = (td_u32 *)dev->dn_msg_vir;

    chan = STM_CTRL_GET_CHAN(dev->chan_id);
    raw_num = (td_u32)chan->raw_list->push_num;

    dprint(PRN_ALWS, "***Print SCD[%d] Down Msg Now\n", scd_id);
    dprint(PRN_ALWS, "dn_msg_phy = %x   \n", dn_msg_phy);

    for (raw_cnt = 0, msg_cnt = 0; raw_cnt < raw_num; raw_cnt++) {
        dprint(PRN_ALWS, "DnMsg[%d] = %x   ", msg_cnt, dn_msg_vir[msg_cnt]);
        msg_cnt++;
        dprint(PRN_ALWS, "DnMsg[%d] = %x   ", msg_cnt, dn_msg_vir[msg_cnt]);
        msg_cnt++;
        dprint(PRN_ALWS, "DnMsg[%d] = %x   ", msg_cnt, dn_msg_vir[msg_cnt]);
        msg_cnt++;
        dprint(PRN_ALWS, "DnMsg[%d] = %x   ", msg_cnt, dn_msg_vir[msg_cnt]);
        msg_cnt++;
        dprint(PRN_ALWS, "\n");
    }
}
#endif

STATIC td_void scd_hal_write_one_msg(stm_dev *dev, const stm_raw_info *raw, td_u32 raw_len, td_u32 *index)
{
    td_u32 msg_cnt = (*index);
    td_u32 *dn_msg = (td_u32 *)dev->dn_msg_vir;
    UADDR dn_msg_phy = dev->dn_msg_phy;

    dn_msg[msg_cnt++] = (td_u32)raw->phy_addr;
    dn_msg[msg_cnt++] = (td_u32)(raw->phy_addr + raw_len - 1);
    dn_msg[msg_cnt++] = 0;
    dn_msg[msg_cnt] = (td_u32)((dn_msg_phy + ((msg_cnt + 1) << 2)) >> 4); /* 2 and 4 are bit ofs */

    msg_cnt++;

    (*index) = msg_cnt;

    return;
}

STATIC td_void scd_hal_clear_upmsg(td_s32 scd_id)
{
    stm_dev *dev = stm_ctrl_get_dev(scd_id);
    td_u32 *up_msg = (td_u32 *)dev->up_msg_vir;

    vfmw_alert(up_msg != TD_NULL);
    up_msg[(SCD_UMSG_SIZE >> 2) - 1] = 0; /* 2 1: D2 */
    up_msg[(SCD_UMSG_SIZE >> 2) - 2] = 0; /* 2 2: D1 */
    up_msg[(SCD_UMSG_SIZE >> 2) - 3] = 0; /* 2 3: D0 */
}

STATIC td_void scd_hal_check_upmsg(td_s32 scd_id)
{
    stm_dev *dev = stm_ctrl_get_dev(scd_id);
    td_u32 *up_msg = (td_u32 *)dev->up_msg_vir;

    /* check if scd upmsg write over reserve mem */
    vfmw_alert(up_msg != TD_NULL);
    vfmw_alert(up_msg[(SCD_UMSG_SIZE >> 2) - 1] == 0); /* 2 1: D2 */
    vfmw_alert(up_msg[(SCD_UMSG_SIZE >> 2) - 2] == 0); /* 2 2: D1 */
    vfmw_alert(up_msg[(SCD_UMSG_SIZE >> 2) - 3] == 0); /* 2 3: D0 */
}

td_s32 scd_hal_write_msg(td_s32 chan_id, td_s32 scd_id)
{
    stm_dev *dev = stm_ctrl_get_dev(scd_id);
    stm_chan *chan = STM_CTRL_GET_CHAN(chan_id);
    stm_raw_info raw_packet = { 0 };
    stm_raw_info *raw = &raw_packet;
    td_u32 *dn_msg = (td_u32 *)dev->dn_msg_vir;
    td_s32 seg_free_size, push_raw_num, push_raw_len, raw_show_total_len, first_offset;
    td_u32 raw_len, msg_cnt, dn_msg_num;
    const td_u32 seg_len_align = 128; /* 128 is align */

    push_raw_len = 0;

    stm_raw_show_head(chan_id, &raw_packet);

    first_offset = stm_raw_get_first_offset(chan_id);
    raw_packet.phy_addr += (UADDR)first_offset;
    raw_packet.vir_addr += (td_u32)first_offset;
    raw_packet.length -= (td_u32)first_offset;
    raw_show_total_len = (td_s32)raw_packet.length;

    seg_free_size = stm_seg_get_free_size(chan_id) - (td_s32)(chan->seg_list_info->last_seg_size +
        seg_len_align);
    dn_msg_num = (chan->cfg.dn_msg_num != 0) ? chan->cfg.dn_msg_num : SCD_DMSG_NUM;

    for (push_raw_num = 0, msg_cnt = 0; (raw_show_total_len < seg_free_size) && ((td_u32)push_raw_num < dn_msg_num);) {
        if (raw->length > 0) {
            scd_hal_write_one_msg(dev, raw, raw->length, &msg_cnt);
        }

        push_raw_num++;
        push_raw_len = raw_show_total_len;

        raw = stm_raw_show_next(chan->raw_list);
        if (raw == TD_NULL) {
            break;
        }

        raw_show_total_len += (td_s32)raw->length;
    }

    if ((push_raw_len < seg_free_size - 16) && (seg_free_size < raw_show_total_len) && /* 16 is reserve bytes */
        ((td_u32)push_raw_num < dn_msg_num) && (raw != TD_NULL)) {
        /* last raw len must more than 4 (set 16) */
        if (raw->length >= (td_u32)(seg_free_size - push_raw_len)) {
            raw_len = (td_u32)(seg_free_size - push_raw_len);
            scd_hal_write_one_msg(dev, raw, raw_len, &msg_cnt);

            push_raw_num++;
            push_raw_len += (td_s32)raw_len;
        }
    }

    chan->raw_list->push_num = push_raw_num;
    chan->raw_list->push_len = push_raw_len;
    if (push_raw_len == 0) {
        return STM_ERR; /* do not add err print */
    }

    dn_msg[msg_cnt - 1] = (msg_cnt > 0) ? 0 : dn_msg[msg_cnt - 1];
    scd_hal_clear_upmsg(scd_id);

    return STM_OK;
}

td_s32 scd_hal_check_int(td_s32 scd_id)
{
    if ((RD_SCDREG(scd_id, REG_SCD_OVER) & 0x1) == 0) {
        return STM_ERR;
    }

    scd_hal_check_upmsg(scd_id);

    return STM_OK;
}

td_s32 scd_hal_cancel(td_s32 scd_id)
{
    td_u32 i;
    td_u32 data;
    const td_u32 loop = 1000;
#ifdef VCODEC_FPGA_SUPPORT
    const td_u32 delay = 3000;
#else
    const td_u32 delay = 30;
#endif

    WR_SCDREG(scd_id, REG_SCD_DESTROY, 1);

    os_mb();

    for (i = 0; i < loop; i++) {
        data = RD_SCDREG(scd_id, REG_SCD_OVER);
        if (((data >> 2) & 1) != 0) { /* 2 is bit ofs */
            return STM_OK;
        } else {
            os_udelay(delay);
        }
    }

    dprint(PRN_ERROR, "SCD Cancel fail!\n");

    return STM_ERR;
}

td_s32 scd_hal_check_unid(td_s32 scd_id)
{
    stm_dev *dev = TD_NULL;

    dev = stm_ctrl_get_dev(scd_id);
    if (dev->reg.cfg_unid != dev->reg.int_unid) {
        dprint(PRN_ERROR, "Scd Unid %x != %x\n", dev->reg.cfg_unid, dev->reg.int_unid);
        return STM_ERR;
    }

    return STM_OK;
}

