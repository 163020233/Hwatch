/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#include "stm_dev.h"
#include "stm_ctrl.h"
#include "stm_chan.h"
#include "stm_seg.h"
#include "vfmw_pdt.h"
#include "event.h"
#include "vfmw_pdt.h"
#include "vdec_chr.h"

static inline td_void stm_seg_phy_2_vir(const seg_list *seg_lst, UADDR phy_addr, td_u8 **vir)
{
    (*vir) = phy_addr - seg_lst->seg_buf_phy_addr + seg_lst->seg_buf_vir_addr;
}

td_u8 stm_dev_is_scd_bypass(td_s32 chan_id)
{
    stm_chan *chan;

    chan = STM_CTRL_GET_CHAN(chan_id);

    return (td_u8)scd_hal_is_bypass(chan->cfg.vid_std, &chan->cfg.std_ext);
}

td_u8 stm_dev_is_seg_bypass(td_s32 chan_id)
{
    stm_chan *chan;
    chan = STM_CTRL_GET_CHAN(chan_id);
    if (chan->cfg.vid_std == VFMW_VP9) {
        return 1;
    }

    return 0;
}

td_s32 stm_dev_raw_to_seg(td_s32 chan_id, stm_seg_info *seg)
{
    return stm_seg_raw_to_seg(chan_id, seg);
}

STATIC td_s32 stm_dev_reset_last_seg(td_s32 dev_id)
{
    td_s32 chan_id;
    stm_dev *dev = TD_NULL;
    seg_list *seg_lst = TD_NULL;

    dev = stm_ctrl_get_dev(dev_id);
    chan_id = dev->chan_id;
    seg_lst = STM_CTRL_GET_CHAN(chan_id)->seg_list_info;

    seg_lst->roll_flag = 0;
    seg_lst->roll_addr = 0;
    seg_lst->first_start_flag = 1;
    seg_lst->pre_lsb = 0xffffffff;
    seg_lst->pre_msb = 0xffff;

    seg_lst->last_seg_addr = 0;
    seg_lst->last_seg_size = 0;

    return STM_OK;
}

#ifdef RAW_MEM_LESS
STATIC td_void stm_dev_proc_roll(td_s32 dev_id, UADDR roll_addr, td_s32 cut_result1, td_s32 cut_result2)
{
    td_s32 chan_id;
    stm_dev *dev = TD_NULL;
    seg_list *seg_lst = TD_NULL;
    stm_seg_info seg_packet;
    UADDR seg_buf_phy;
    td_u8 *seg_buf_vir = TD_NULL;
    td_u32 *up_msg = TD_NULL;
    td_u32 first_half_seg_len;
    td_u32 second_half_seg_len;
    td_u32 seg_packet_len;

    dev = stm_ctrl_get_dev(dev_id);
    chan_id = dev->chan_id;
    up_msg = (td_u32 *)dev->up_msg_vir;
    seg_lst = STM_CTRL_GET_CHAN(chan_id)->seg_list_info;
    seg_buf_phy = seg_lst->seg_buf_phy_addr;
    seg_buf_vir = seg_lst->seg_buf_vir_addr;

    first_half_seg_len = (td_u32)(roll_addr - seg_lst->last_seg_addr);
    second_half_seg_len = (td_u32)(scd_hal_get_seg_addr(up_msg) - seg_buf_phy);
    seg_packet_len = first_half_seg_len + second_half_seg_len;

    vfmw_check_sec_func(memcpy_s(&seg_packet, sizeof(seg_packet), &seg_lst->last_seg, sizeof(stm_seg_info)));
    seg_packet.phy_addr = seg_lst->last_seg_addr;
    stm_seg_phy_2_vir(seg_lst, seg_packet.phy_addr, &seg_packet.vir_addr);
    seg_packet.length = seg_packet_len;

    if (seg_packet_len <= STM_SC_LEN) {
        return;
    }

    if (second_half_seg_len == 0) {
        seg_packet.length = first_half_seg_len;
        seg_packet.is_last_seg = 1;
        seg_packet.cut_result = (td_s8)cut_result1;
        stm_seg_insert(chan_id, &seg_packet);
    } else {
        seg_packet.length = first_half_seg_len;
        seg_packet.is_last_seg = 0;
        seg_packet.cut_result = (td_s8)cut_result1;
        stm_seg_insert(chan_id, &seg_packet);

        seg_packet.phy_addr = seg_buf_phy;
        seg_packet.vir_addr = seg_buf_vir;
        seg_packet.length = second_half_seg_len;
        seg_packet.is_last_seg = 1;
        seg_packet.cut_result = (td_s8)cut_result2;
        stm_seg_insert(chan_id, &seg_packet);
    }

    return;
}

STATIC td_void stm_dev_proc_seg_packet_half_seg_en(stm_seg_info *seg_packet, td_s32 chan_id,
    const seg_list *seg_lst, td_u32 first_half_len, td_u32 second_half_len)
{
    td_s32 free_num;

    free_num = stm_seg_get_free_num(chan_id);
    if (free_num > 3 && seg_lst->last_seg_size > STM_SC_LEN) { /* 3 is seg num */
        seg_packet->phy_addr = seg_lst->last_seg_addr;
        stm_seg_phy_2_vir(seg_lst, seg_packet->phy_addr, &seg_packet->vir_addr);
        seg_packet->length = first_half_len;
        seg_packet->is_last_seg = 0;
        seg_packet->cut_result = SEG_CUT_HALF;
        stm_seg_insert(chan_id, seg_packet);

        seg_packet->phy_addr = seg_lst->seg_buf_phy_addr;
        stm_seg_phy_2_vir(seg_lst, seg_packet->phy_addr, &seg_packet->vir_addr);
        seg_packet->length = second_half_len;
        seg_packet->is_last_seg = 1;
        seg_packet->cut_result = SEG_CUT_HALF;
        stm_seg_insert(chan_id, seg_packet);
    }
}

STATIC td_s32 stm_dev_proc_last_seg(td_s32 dev_id, td_s32 msg_id)
{
    stm_dev *dev = stm_ctrl_get_dev(dev_id);
    td_s32 chan_id = dev->chan_id;
    stm_chan *chan = TD_NULL;
    seg_list *seg_lst = TD_NULL;
    td_u32 *up_msg = (td_u32 *)dev->up_msg_vir;
    td_u32 msg_step, eaten_len, first_half_len, second_half_len;
    stm_seg_info seg_packet = { 0 };

    msg_step = dev->reg.up_step;
    up_msg += (td_u32)msg_id * msg_step;
    chan = STM_CTRL_GET_CHAN(chan_id);
    seg_lst = chan->seg_list_info;

    eaten_len = scd_hal_src_eaten(up_msg);
    stm_raw_get_pts(chan_id, (td_s32)eaten_len, &seg_lst->last_pts, &seg_lst->last_seg);
    vfmw_check_sec_func(memcpy_s(&seg_packet, sizeof(seg_packet), &seg_lst->last_seg, sizeof(stm_seg_info)));

    seg_lst->last_seg_addr = scd_hal_get_seg_addr(up_msg);
    seg_lst->seg_next_addr = dev->reg.next_addr;

    if (seg_lst->seg_next_addr < seg_lst->seg_buf_phy_addr) {
        seg_lst->seg_next_addr = seg_lst->seg_buf_phy_addr;
    }

    if (seg_lst->seg_next_addr < seg_lst->last_seg_addr) {
        seg_lst->roll_flag = 1;
        seg_lst->roll_addr = dev->reg.roll_addr;
        if (dev->reg.next_addr < seg_lst->seg_buf_phy_addr) {
            seg_lst->roll_addr -= (seg_lst->seg_buf_phy_addr - dev->reg.next_addr);
        }

        first_half_len = (td_u32)(seg_lst->roll_addr - seg_lst->last_seg_addr);
        second_half_len = (td_u32)(seg_lst->seg_next_addr - seg_lst->seg_buf_phy_addr);
        seg_lst->last_seg_size = first_half_len + second_half_len;

        if (chan->cfg.half_seg_en == 1) {
            stm_dev_proc_seg_packet_half_seg_en(&seg_packet, chan_id, seg_lst, first_half_len, second_half_len);
        }
    } else {
        seg_lst->roll_flag = 0;
        seg_lst->roll_addr = 0;
        seg_lst->last_seg_size = (td_u32)(seg_lst->seg_next_addr - seg_lst->last_seg_addr);

        if (chan->cfg.half_seg_en == 1) {
            if (seg_lst->last_seg_size > STM_SC_LEN) {
                seg_packet.phy_addr = seg_lst->last_seg_addr;
                stm_seg_phy_2_vir(seg_lst, seg_packet.phy_addr, &seg_packet.vir_addr);
                seg_packet.length = seg_lst->last_seg_size;
                seg_packet.is_last_seg = 1;
                seg_packet.cut_result = SEG_CUT_HALF;
                stm_seg_insert(chan_id, &seg_packet);
            }
        }
    }

    seg_lst->first_start_flag = 0;

    return STM_OK;
}

STATIC td_void stm_dev_proc_set_seg_list(seg_list *seg_lst, const stm_dev *dev,
    const raw_list_info *raw_list, UADDR roll_addr)
{
    if ((seg_lst->roll_flag == 0) && (dev->reg.next_addr < seg_lst->last_seg_addr)) {
        seg_lst->roll_flag = 1;
        seg_lst->roll_addr = roll_addr;
    }

    seg_lst->first_start_flag = 0;
    seg_lst->seg_next_addr = dev->reg.next_addr;
    if (seg_lst->seg_next_addr < seg_lst->seg_buf_phy_addr) {
        seg_lst->seg_next_addr = seg_lst->seg_buf_phy_addr;
    }

    seg_lst->last_seg_size += (td_u32)raw_list->push_len;
}

STATIC td_void stm_dev_proc_set_seg_packet(stm_seg_info *seg_packet, const seg_list *seg_lst)
{
    seg_packet->phy_addr = seg_lst->last_seg_addr;
    stm_seg_phy_2_vir(seg_lst, seg_packet->phy_addr, &seg_packet->vir_addr);
    seg_packet->length = (td_u32)(seg_lst->seg_next_addr - seg_lst->last_seg_addr);
    seg_packet->is_last_seg = 1;
    seg_packet->cut_result = SEG_CUT_HALF;
}

STATIC td_s32 stm_dev_proc_pre_seg(td_s32 dev_id, td_s32 seg_num)
{
    stm_dev *dev = stm_ctrl_get_dev(dev_id);
    td_s32 chan_id = dev->chan_id;
    stm_chan *chan = STM_CTRL_GET_CHAN(chan_id);
    raw_list_info *raw_list = chan->raw_list;
    seg_list *seg_lst = chan->seg_list_info;
    stm_seg_info seg_packet;
    td_u32 *up_msg = (td_u32 *)dev->up_msg_vir;
    UADDR roll_addr = dev->reg.roll_addr;

    if (dev->reg.next_addr < seg_lst->seg_buf_phy_addr) {
        roll_addr -= (seg_lst->seg_buf_phy_addr - dev->reg.next_addr);
    }

    if (seg_num == 0) {
        raw_list->delete_len = (td_u32)raw_list->push_len;

        if (seg_lst->last_seg_addr != 0) {
            stm_dev_proc_set_seg_list(seg_lst, dev, raw_list, roll_addr);

            /* roll need insert two half seg */
            if ((chan->cfg.half_seg_en == 1) && (seg_lst->roll_flag == 0)) {
                vfmw_check_sec_func(memcpy_s(&seg_packet, sizeof(seg_packet),
                                             &seg_lst->last_seg, sizeof(stm_seg_info)));
                stm_dev_proc_set_seg_packet(&seg_packet, seg_lst);

                vfmw_check_ret_no_print(seg_packet.length > STM_SC_LEN, STM_OK);
                vfmw_check_ret(stm_seg_insert(chan_id, &seg_packet) == STM_OK, STM_ERR);
            }
        }

        return STM_OK;
    }

    if (seg_lst->last_seg_addr != 0) {
        if (seg_lst->roll_flag == 1) {
            stm_dev_proc_roll(dev_id, seg_lst->roll_addr, SEG_CUT_REPEAT, SEG_CUT_REPEAT);
        } else {
            if (scd_hal_get_seg_addr(up_msg) < seg_lst->last_seg_addr) {
                stm_dev_proc_roll(dev_id, roll_addr, SEG_CUT_REPEAT, SEG_CUT_NORMAL);
            } else {
                vfmw_check_sec_func(memcpy_s(&seg_packet, sizeof(seg_packet),
                                             &seg_lst->last_seg, sizeof(stm_seg_info)));
                seg_packet.phy_addr = seg_lst->last_seg_addr;
                stm_seg_phy_2_vir(seg_lst, seg_packet.phy_addr, &seg_packet.vir_addr);
                seg_packet.length = (td_u32)(scd_hal_get_seg_addr(up_msg) - seg_lst->last_seg_addr);
                seg_packet.is_last_seg = 1;
                seg_packet.cut_result = SEG_CUT_REPEAT;

                vfmw_check_ret_no_print(seg_packet.length > STM_SC_LEN, STM_OK);
                vfmw_check_ret(stm_seg_insert(chan_id, &seg_packet) == STM_OK, STM_ERR);
            }
        }
    }

    return STM_OK;
}
#endif

STATIC td_s32 stm_dev_proc_other_normal(td_s32 chan_id, const stm_dev *dev,
    stm_seg_info *seg_packet, td_u32 seg_index)
{
    td_u32 *up_msg = TD_NULL;
    td_u32 msg_step;
    seg_list *seg_lst = STM_CTRL_GET_CHAN(chan_id)->seg_list_info;
    UADDR cur_seg_addr;
    UADDR next_seg_addr;

    vfmw_check_ret(dev != TD_NULL, STM_ERR);
    vfmw_check_ret(seg_packet != TD_NULL, STM_ERR);

    up_msg = (td_u32 *)dev->up_msg_vir;
    msg_step = dev->reg.up_step;
    cur_seg_addr = scd_hal_get_seg_addr(up_msg + msg_step * seg_index);
    next_seg_addr = scd_hal_get_seg_addr(up_msg + msg_step * (seg_index + 1));

    seg_packet->phy_addr = cur_seg_addr;
    stm_seg_phy_2_vir(seg_lst, seg_packet->phy_addr, &seg_packet->vir_addr);
    seg_packet->length = (td_u32)(next_seg_addr - cur_seg_addr);
    seg_packet->is_last_seg = 1;
    seg_packet->cut_result = SEG_CUT_NORMAL;
    seg_packet->raw_len = scd_hal_src_eaten(up_msg + msg_step * (seg_index + 1)) -
                         scd_hal_src_eaten(up_msg + msg_step * seg_index);

    if (seg_packet->length <= STM_SC_LEN) {
        return STM_OK;
    }

    if (stm_seg_insert(chan_id, seg_packet) != STM_OK) {
        return STM_ERR;
    }

    return STM_OK;
}

STATIC td_s32 stm_dev_proc_other_roll(td_s32 chan_id, const stm_dev *dev,
    stm_seg_info *seg_packet, td_u32 seg_index)
{
    td_u32 msg_step;
    td_u32 *up_msg = TD_NULL;
    UADDR roll_addr, seg_buf_phy, cur_seg_addr, next_seg_addr;
    td_u32 first_half_seg_len, second_half_seg_len, seg_packet_len;
    td_u32 free_seg_slot_num = (td_u32)stm_seg_get_free_num(chan_id);
    seg_list *seg_lst = STM_CTRL_GET_CHAN(chan_id)->seg_list_info;

    vfmw_check_ret(dev != TD_NULL, STM_ERR);
    vfmw_check_ret(seg_packet != TD_NULL, STM_ERR);

    roll_addr = dev->reg.roll_addr;
    seg_buf_phy = seg_lst->seg_buf_phy_addr;
    msg_step = dev->reg.up_step;
    up_msg = (td_u32 *)dev->up_msg_vir;
    cur_seg_addr = scd_hal_get_seg_addr(up_msg + msg_step * seg_index);
    next_seg_addr = scd_hal_get_seg_addr(up_msg + msg_step * (seg_index + 1));
    first_half_seg_len = (td_u32)(roll_addr - cur_seg_addr);
    second_half_seg_len = (td_u32)(next_seg_addr - seg_buf_phy);
    seg_packet_len = first_half_seg_len + second_half_seg_len;

    seg_packet->phy_addr = cur_seg_addr;
    stm_seg_phy_2_vir(seg_lst, seg_packet->phy_addr, &seg_packet->vir_addr);
    seg_packet->length = seg_packet_len;
    seg_packet->raw_len = scd_hal_src_eaten(up_msg + msg_step * (seg_index + 1)) -
                         scd_hal_src_eaten(up_msg + msg_step * seg_index);

    if (seg_packet->length <= STM_SC_LEN) {
        return STM_OK;
    }

    if (free_seg_slot_num <= 2) { /* 2 is seg num */
        return STM_ERR;
    }

    if (second_half_seg_len == 0) {
        seg_packet->length = first_half_seg_len;
        seg_packet->is_last_seg = 1;
        seg_packet->cut_result = SEG_CUT_NORMAL;
        stm_seg_insert(chan_id, seg_packet);
    } else {
        seg_packet->length = first_half_seg_len;
        seg_packet->is_last_seg = 0;
        seg_packet->cut_result = SEG_CUT_NORMAL;
        stm_seg_insert(chan_id, seg_packet);

        seg_packet->phy_addr = seg_buf_phy;
        seg_packet->vir_addr = seg_lst->seg_buf_vir_addr;
        seg_packet->length = second_half_seg_len;
        seg_packet->is_last_seg = 1;
        seg_packet->cut_result = SEG_CUT_NORMAL;
        stm_seg_insert(chan_id, seg_packet);
    }

    return STM_OK;
}

STATIC td_s32 stm_dev_proc_other(td_s32 dev_id, td_s32 chan_id, td_s32 seg_num)
{
    td_s32 ret;
    td_u32 cnt;
    stm_dev *dev = stm_ctrl_get_dev(dev_id);
    seg_list *seg_lst = STM_CTRL_GET_CHAN(chan_id)->seg_list_info;
    raw_list_info *raw_list = STM_CTRL_GET_CHAN(chan_id)->raw_list;
    td_u32 msg_step = dev->reg.up_step;
    td_u32 *up_msg = (td_u32 *)dev->up_msg_vir;
    stm_seg_info seg_packet = { 0 };
    UADDR cur_seg_addr, next_seg_addr;
    td_u32 seg_num_cnt = seg_lst->seg_num_cnt;
    td_u32 delete_len;

    for (cnt = 0; cnt < (td_u32)seg_num; cnt++) {
        cur_seg_addr = scd_hal_get_seg_addr(up_msg + msg_step * cnt);
        next_seg_addr = scd_hal_get_seg_addr(up_msg + msg_step * (cnt + 1));

        if (seg_num_cnt == 0 && cnt > SEG_START_NUM) {
            break;
        }

        stm_raw_get_pts(chan_id, (td_s32)scd_hal_src_eaten(up_msg + msg_step * cnt), &seg_lst->last_pts, &seg_packet);

        if (next_seg_addr >= cur_seg_addr) {
            ret = stm_dev_proc_other_normal(chan_id, dev, &seg_packet, cnt);
        } else {
            ret = stm_dev_proc_other_roll(chan_id, dev, &seg_packet, cnt);
        }

        if (ret != STM_OK) {
            break;
        }
    }

#ifdef RAW_MEM_LESS
    if (cnt == dev->reg.start_num - 1) {
        stm_dev_proc_last_seg(dev_id, (td_s32)cnt);

        delete_len = (td_u32)raw_list->push_len;
        raw_list->delete_len = delete_len;
    } else if (cnt == 0 && seg_lst->last_seg_addr == 0) {
        delete_len = scd_hal_src_eaten(up_msg + cnt * msg_step);
        raw_list->delete_len = delete_len;
    } else {
#endif
        stm_dev_reset_last_seg(dev_id);
        delete_len = scd_hal_src_eaten(up_msg + cnt * msg_step);
        raw_list->delete_len = delete_len;
#ifdef RAW_MEM_LESS
    }
#endif
    dev->insert_num = (td_s32)cnt;

    return STM_OK;
}

static inline td_u32 stm_min(td_u32 a, td_u32 b)
{
    return (a > b) ? b : a;
}

STATIC td_void stm_dev_proc_return(td_s32 dev_id, td_s32 chan_id)
{
    stm_dev *dev = TD_NULL;
    stm_reg *reg = TD_NULL;
    stm_chan *chan = TD_NULL;
    td_s32 start_code_num;

    dev = stm_ctrl_get_dev(dev_id);
    reg = &(dev->reg);
    chan = STM_CTRL_GET_CHAN(chan_id);
    if ((reg->start_num > MAX_SEG_ALLOW) && ((chan->cfg.vid_std == VFMW_H264) || (chan->cfg.vid_std == VFMW_HEVC))) {
        dprint(PRN_ERROR, "%s start_code = %d too many !", __func__, reg->start_num);
        stm_raw_delete_head(chan_id);
        return;
    }

    dev->start_num = (td_s32)reg->start_num;
    start_code_num = (td_s32)stm_min(reg->start_num, reg->up_len / reg->up_step);
#ifdef RAW_MEM_LESS
    if (stm_dev_proc_pre_seg(dev_id, (td_s32)start_code_num) != STM_OK) {
        return;
    }
#endif

    if (start_code_num == 0) {
        return;
    }

    stm_dev_proc_other(dev_id, chan_id, start_code_num - 1);
}

#ifndef VFMW_PRSAM_FLUSH_ALL_CACHE_SUPPORT
STATIC td_void stm_dev_flash_cache(td_s32 dev_id)
{
    stm_dev *dev = TD_NULL;

    dev = stm_ctrl_get_dev(dev_id);

    os_flush_cache(dev->dn_msg_vir, dev->dn_msg_phy, dev->dn_msg_size);
    os_flush_cache(dev->up_msg_vir, dev->up_msg_phy, dev->up_msg_size);
}
#endif
STATIC td_void stm_dev_int_process(td_s32 dev_id)
{
    td_s32 chan_id;
    stm_dev *dev = TD_NULL;
    stm_chan *chan = TD_NULL;
    td_ulong lock_flag;
    td_u32 t1, t2;

    vfmw_check(os_get_time_us != TD_NULL);

#ifdef VFMW_PRSAM_FLUSH_ALL_CACHE_SUPPORT
    os_flush_all_cache();
#endif

    vfmw_check(pdt_add_sleep_apb_veto(VFMW_ID_SCD) == EXT_SUCCESS);

    t1 = os_get_time_us();
    dev = stm_ctrl_get_dev(dev_id);
    vfmw_check_goto(scd_hal_check_int(dev_id) == STM_OK, out);

    scd_hal_read_reg(dev_id);

    scd_hal_clear_int(dev_id);

    dev->int_count++;
    dev->start_num = 0;
    dev->insert_num = 0;

#ifdef IP_UNID_SUPPORT
    vfmw_check_goto(scd_hal_check_unid(dev_id) == STM_OK, out);
#endif

    chan_id = dev->chan_id;
    stm_chan_lock(chan_id, &lock_flag);
    chan = STM_CTRL_GET_CHAN(chan_id);
    if (chan->handle == TD_NULL) {
        stm_chan_unlock(chan_id, &lock_flag);
        goto out;
    }

    if (dev->state == STM_DEV_STATE_BUSY) {
        if (dev->cancel == 0 && chan->state != STM_CHAN_STATE_NULL) {
#ifndef VFMW_PRSAM_FLUSH_ALL_CACHE_SUPPORT
            os_flush_cache(dev->up_msg_vir, dev->up_msg_phy, dev->up_msg_size);
#endif
            stm_dev_proc_return(dev_id, chan_id);
        }
    }

    dev->cancel = 0;
    dev->state = STM_DEV_STATE_IDLE;
    chan->raw_list->is_cuting = 0;
    chan->seg_list_info->insert_seg_num = (td_u32)dev->insert_num;

    stm_chan_unlock(chan_id, &lock_flag);

out:
    pdt_remove_sleep_veto(VFMW_ID_SCD);
    pdt_remove_sleep_apb_veto(VFMW_ID_SCD);

    t2 = os_get_time_us();
    if (t2 > t1 && t2 - t1 > dev->int_time) {
        dev->int_time = t2 - t1;
    }
}

os_irqret stm_dev_isr0(td_s32 irq, td_void *dev_id)
{
    const td_s32 irq_handled = 1;

    vfmw_unused(irq);
    vfmw_unused(dev_id);
    stm_dev_int_process(0);

    return irq_handled;
}

STATIC td_s32 stm_dev_irq_init(td_s32 dev_id)
{
    return scd_hal_irq_init(dev_id);
}

STATIC td_s32 stm_dev_irq_exit(td_s32 dev_id)
{
    return scd_hal_irq_exit(dev_id);
}

td_s32 stm_dev_open(td_s32 dev_id)
{
    td_s32 ret;
    stm_dev *dev = TD_NULL;

    dev = stm_ctrl_get_dev(dev_id);

    os_sema_down(dev->sema);

    if (dev->handle != TD_NULL) {
        os_sema_up(dev->sema);
        return STM_OK;
    }

    vfmw_check_goto(pdt_add_sleep_veto(VFMW_ID_SCD) == EXT_SUCCESS, err1);
    vfmw_check_goto(pdt_add_sleep_apb_veto(VFMW_ID_SCD) == EXT_SUCCESS, err2);

    ret = scd_hal_open(dev_id);
    if (ret != STM_OK) {
        os_sema_up(dev->sema);
        pdt_remove_sleep_apb_veto(VFMW_ID_SCD);
        pdt_remove_sleep_veto(VFMW_ID_SCD);
        return STM_ERR;
    }

    dev->id = dev_id;
    dev->state = STM_DEV_STATE_IDLE;
    dev->handle = dev;
    dev->int_mask = 0;
    dev->start_cnt = 0;
    dev->int_count = 0;
    dev->start_num = 0;
    dev->insert_num = 0;
    dev->start_time = 0;
    dev->int_time = 0;

    scd_hal_reset(dev_id);

    pdt_remove_sleep_apb_veto(VFMW_ID_SCD);
    pdt_remove_sleep_veto(VFMW_ID_SCD);

    ret = stm_dev_irq_init(dev_id);

    os_sema_up(dev->sema);

    return ret;

err2:
    pdt_remove_sleep_veto(VFMW_ID_SCD);
err1:
    os_sema_up(dev->sema);
    return STM_ERR;
}

td_s32 stm_dev_close(td_s32 dev_id)
{
    td_s32 ret;
    stm_dev *dev = TD_NULL;

    dev = stm_ctrl_get_dev(dev_id);

    os_sema_down(dev->sema);
    if (dev->handle == TD_NULL) {
        os_sema_up(dev->sema);
        return STM_OK;
    }

    ret = stm_dev_irq_exit(dev_id);
    if (ret != STM_OK) {
        os_sema_up(dev->sema);
        return STM_ERR;
    }

    /* dec close sys crg first */
    ret = scd_hal_close(dev_id);
    if (ret != STM_OK) {
        os_sema_up(dev->sema);
        return STM_ERR;
    }

    dev->id = 0;
    dev->state = STM_DEV_STATE_NULL;
    dev->handle = TD_NULL;

    os_sema_up(dev->sema);

    return STM_OK;
}

td_s32 stm_dev_reset(td_s32 dev_id)
{
    td_s32 ret;

    ret = scd_hal_reset(dev_id);

    return ret;
}

td_s32 stm_dev_start(td_s32 chan_id, td_s32 dev_id)
{
    td_s32 ret;
    stm_dev *dev = TD_NULL;
    stm_chan *chan = TD_NULL;
    td_s32 seg_free_size;
    td_s32 raw_fresh_size;

    vfmw_check_ret(os_get_time_ms != TD_NULL, STM_ERR);

    dev = stm_ctrl_get_dev(dev_id);
    chan = STM_CTRL_GET_CHAN(chan_id);
    chan->raw_list->fill_new_flag = 0;
    chan->seg_list_info->insert_seg_num = 0;

    vfmw_check_ret(pdt_add_sleep_veto(VFMW_ID_SCD) == EXT_SUCCESS, STM_ERR);
    vfmw_check_goto(pdt_add_sleep_apb_veto(VFMW_ID_SCD) == EXT_SUCCESS, err);

    scd_hal_set_reg(chan_id, dev_id);

    ret = scd_hal_write_msg(chan_id, dev_id);
    if (ret != STM_OK) {
        pdt_remove_sleep_veto(VFMW_ID_SCD);
        pdt_remove_sleep_apb_veto(VFMW_ID_SCD);
        return STM_ERR;
    }

    scd_hal_write_reg(dev_id);

    dev->chan_id = chan_id;
    dev->state = STM_DEV_STATE_BUSY;
    chan->dev_id = dev_id;
    chan->raw_list->is_cuting = 1;
    chan->raw_list->cfg_dev_cnt++;
    dev->start_time = os_get_time_ms();

    chan->raw_list->last_cfg_all_flag = 0;
    raw_fresh_size = stm_raw_get_fresh_size(chan_id);
    seg_free_size = stm_seg_get_free_size(chan_id);
    if (chan->raw_list->push_len < seg_free_size &&
        chan->raw_list->push_len == raw_fresh_size) {
        chan->raw_list->last_cfg_all_flag = 1;
    }

    dev->cancel = 0;
    dev->start_cnt++;

#ifdef VFMW_PRSAM_FLUSH_ALL_CACHE_SUPPORT
    os_flush_all_cache();
#else
    stm_dev_flash_cache(dev_id);
#endif

    scd_hal_start(dev_id);

    pdt_remove_sleep_apb_veto(VFMW_ID_SCD);

    return STM_OK;
err:
    pdt_remove_sleep_veto(VFMW_ID_SCD);
    return STM_ERR;
}

td_s32 stm_dev_suspend(td_void)
{
    td_s32 dev_id;
    stm_dev *dev = TD_NULL;
    const td_u32 time = 0x400;
    td_u32 cnt;

    for (dev_id = 0; dev_id < STM_DEV_NUM; dev_id++) {
        dev = stm_ctrl_get_dev(dev_id);
        if (dev->handle == TD_NULL) {
            continue;
        }

        for (cnt = 0; cnt < time; cnt++) {
            if (dev->start_cnt == dev->int_count) {
                break;
            } else {
                os_msleep(1);
            }
        }

        if (cnt >= time) {
            dprint(PRN_ERROR, "wait stm %d t-o\n", dev_id);
        }
    }

    return STM_OK;
}

td_s32 stm_dev_resume(td_void)
{
    pdt_init_mmu_reg_addr(TD_NULL);

    return STM_OK;
}

td_u32 stm_dev_hal_size(td_s32 dev_id)
{
    return scd_hal_msg_size(dev_id);
}

td_s32 stm_dev_cancel(td_s32 dev_id, td_s32 chan_id)
{
#ifndef IP_CANCEL_SUPPORT
    vfmw_unused(dev_id);
    vfmw_unused(chan_id);

    return STM_ERR;
#else
    td_s32 ret = STM_OK;
    stm_dev *dev = TD_NULL;

    dev = stm_ctrl_get_dev(dev_id);
    if (dev->chan_id == chan_id &&
        dev->state == STM_DEV_STATE_BUSY) {
        dev->cancel = 1;
        ret = scd_hal_cancel(dev_id);
    }

    return ret;
#endif
}

STATIC td_s32 stm_dev_time_out(td_s32 dev_id)
{
    stm_dev *dev = TD_NULL;
    stm_chan *chan = TD_NULL;
    td_u32 cur_time;
    td_u32 delay_time;
    td_u32 scd_time_out;

    vfmw_check_ret(os_get_time_ms != TD_NULL, STM_ERR);

    dev = stm_ctrl_get_dev(dev_id);
    chan = STM_CTRL_GET_CHAN(dev->chan_id);

    cur_time = os_get_time_ms();
    delay_time = cur_time - dev->start_time;
    if (cur_time < dev->start_time) {
        delay_time = 0;
        dev->start_time = cur_time;
    }

#ifdef VCODEC_FPGA_SUPPORT
    scd_time_out = SCD_TIME_OUT * 100; /* 100 times */
#else
    scd_time_out = SCD_TIME_OUT;
#endif

    if (delay_time > scd_time_out) {
        dprint(PRN_ERROR, "scd timeout(%d ms)\n", delay_time);
        vdec_record_chr_event(dev->chan_id, MT_VDEC_KEY_ERROR, MT_VDEC_VFMW_KEY_ERROR, MT_VDEC_VFMW_SCD_TIMEOUT);

        stm_dev_reset(dev_id);
        dev->state = STM_DEV_STATE_IDLE;
        if (chan->handle != TD_NULL) {
            chan->raw_list->is_cuting = 0;
            stm_raw_del_cfg_raw(dev->chan_id);
        }
    }

    return STM_OK;
}

STATIC td_s32 stm_dev_check(td_s32 dev_id, td_s32 chan_id)
{
    stm_chan *chan = TD_NULL;
    td_u32 cur_time_in_ms;
    td_s32 wait_time_in_ms;
    const td_s32 over_time_ms = 20;
    const td_u32 init_time_ms = 20;

    chan = STM_CTRL_GET_CHAN(chan_id);
    if ((chan->handle == TD_NULL) ||
        (chan->state != STM_CHAN_STATE_RUN) ||
        (chan->cfg.scd_bypass == 1) ||
        (chan->raw_list->delete_len != 0)) {
        return STM_ERR;
    }

    vfmw_unused(dev_id);
    vfmw_check_ret(os_get_time_ms != TD_NULL, STM_ERR);

    if (chan->es_valid == 0) {
        stm_chan_print_block(chan->id, "es invalid");
        return STM_ERR;
    }

    cur_time_in_ms = os_get_time_ms();
    wait_time_in_ms = (td_s32)cur_time_in_ms - chan->raw_list->last_start_time;
    if (wait_time_in_ms < 0) {
        wait_time_in_ms = (td_s32)init_time_ms;
        chan->raw_list->last_start_time = (td_s32)cur_time_in_ms;
    }

    if (chan->raw_list->fill_new_flag == 0 &&
        chan->raw_list->last_cfg_all_flag == 1 &&
        wait_time_in_ms < over_time_ms) {
        stm_chan_print_block(chan->id, "no raw");
        return STM_ERR;
    }

    if (stm_seg_check(chan_id) != STM_OK) {
        stm_chan_print_block(chan->id, "seg full");
        return STM_ERR;
    }

    chan->raw_list->last_start_time = (td_s32)cur_time_in_ms;

    return STM_OK;
}

STATIC td_s32 stm_dev_do_next(td_s32 dev_id)
{
    td_s32 loop;
    td_s32 chan_id;
    stm_entry *entry = TD_NULL;

    entry = stm_ctrl_get_entry();
    chan_id = entry->start_chan;

    for (loop = 0; loop < STM_CHAN_NUM; loop++) {
        chan_id = (chan_id + 1) % STM_CHAN_NUM;

        if (stm_dev_check(dev_id, chan_id) != STM_OK) {
            continue;
        }

        if (stm_dev_start(chan_id, dev_id) == STM_OK) {
            entry->start_chan = chan_id;
            return STM_OK;
        }
    }

    return STM_OK;
}

td_void stm_dev_try_open_device(td_void)
{
    td_s32 dev_id;
    td_u32 open_flag;
    stm_chan_type_num num = { 0 };

    for (dev_id = 0; dev_id < STM_DEV_NUM; dev_id++) {
        stm_chan_get_num(&num);
        if (STM_DEV_NUM > 1) {
            open_flag = dev_id == 0 && num.no_sec > 0;
            open_flag |= (td_u32)(dev_id == 1 && num.sec > 0);
        } else {
            open_flag = num.no_sec > 0 || num.sec > 0;
        }

        if (open_flag != 0) {
            if (stm_dev_open(dev_id) != STM_OK) {
                vdec_record_chr_event(-1, MT_VDEC_KEY_ERROR, MT_VDEC_VFMW_KEY_ERROR, MT_VDEC_VFMW_SCD_OPEN_ERR);
                continue;
            }
        }
    }
}

td_void stm_dev_try_close_device(td_void)
{
    td_s32 dev_id;
    td_u32 close_flag;
    stm_chan_type_num num = { 0 };

    for (dev_id = 0; dev_id < STM_DEV_NUM; dev_id++) {
        stm_chan_get_num(&num);
        if (STM_DEV_NUM > 1) {
            close_flag = dev_id == 0 && num.no_sec == 0;
            close_flag |= (td_u32)(dev_id == 1 && num.sec == 0);
        } else {
            close_flag = num.sec == 0 && num.no_sec == 0;
        }

        if (close_flag != 0) {
            if (stm_dev_close(dev_id) != STM_OK) {
                continue;
            }
        }
    }
}

td_s32 stm_dev_run_proc(td_void)
{
    td_s32 dev_id;
    stm_dev *dev = TD_NULL;

    stm_dev_try_open_device();

    for (dev_id = 0; dev_id < STM_DEV_NUM; dev_id++) {
        dev = stm_ctrl_get_dev(dev_id);
        if (dev->handle == TD_NULL) {
            continue;
        }

        if (dev->state == STM_DEV_STATE_BUSY) {
            stm_dev_time_out(dev_id);
#ifdef VFMW_SCD_DFX_SUPPORT
            scd_hal_read_int_state(dev_id);
#endif
        } else if (dev->state == STM_DEV_STATE_IDLE) {
            stm_dev_do_next(dev_id);
        }
    }

    stm_dev_try_close_device();

    return STM_OK;
}

td_void stm_dev_init(td_void)
{
    td_s32 dev_id;
    stm_dev *dev = TD_NULL;

    for (dev_id = 0; dev_id < STM_DEV_NUM; dev_id++) {
        dev = stm_ctrl_get_dev(dev_id);
        if (dev != TD_NULL) {
            os_sema_init(&dev->sema);
        }
    }
}

td_void stm_dev_exit(td_void)
{
    td_s32 dev_id;
    stm_dev *dev = TD_NULL;

    for (dev_id = 0; dev_id < STM_DEV_NUM; dev_id++) {
        dev = stm_ctrl_get_dev(dev_id);
        if (dev != TD_NULL) {
            os_sema_exit(dev->sema);
        }
    }
}
