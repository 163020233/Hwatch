/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef __DEC_DEV_H__
#define __DEC_DEV_H__

#include "dec_base.h"
#include "dec_hal.h"
#include "dec_post.h"
#include "vfmw_osal.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef enum {
    DEC_DEV_STATE_NULL = 0,
    DEC_DEV_STATE_RUN,
    DEC_DEV_STATE_SUSPEND,
    DEC_DEV_STATE_STOP,
    DEC_DEV_STATE_MAX,
} dec_dev_state;

typedef enum {
    DEC_DEV_TYPE_NULL = 0,
    DEC_DEV_TYPE_VDH,
    DEC_DEV_TYPE_MDMA,
    DEC_DEV_TYPE_MAX,
} dec_dev_type;

typedef enum {
    DEC_INT_STATE_NULL = 0,
    DEC_INT_STATE_PART,
    DEC_INT_STATE_OVER,
    DEC_INT_STATE_BOTH,
} dec_int_state;

typedef struct {
    td_s32 (*open) (td_void *dev);
    td_s32 (*close) (td_void *dev);
    td_s32 (*reset) (const td_void *dev);
    td_s32 (*resume) (const td_void *dev);
    td_s32 (*suspend) (const td_void *dev);
    td_s32 (*init_irq) (const td_void *dev);
    td_s32 (*exit_irq) (const td_void *dev);
    td_s32 (*config_msg)(const td_void *dev, const dec_hal_ops *ops, const td_void *task);
    td_s32 (*config_reg)(const td_void *dev, const dec_hal_ops *ops, const td_void *task);
    td_s32 (*start_reg) (const td_void *dev, td_u16 reg_id);
    td_s32 (*get_msg) (const td_void *dev, td_u16 msg_id, td_void *msg);
    td_s32 (*get_reg) (const td_void *dev, td_u16 reg_id, td_void *reg);
    td_s32 (*get_mem_need_size) (const td_void *dev, vfmw_svr_mem_need *mem);
} dec_dev_ops;

typedef struct {
    /*
    default is 0,
    set to 1 after configing,
    set to 0 after pxp int,
    set to 0 after finish cancelling decoding
    */
    td_u8 used;
    td_u16 reg_id;
    td_u16 msg_id;
    td_s32 chan_id;
    td_s32 is_sec;
    td_u32 base_ofs[2]; /* 2 :a number */
    UADDR reg_phy_addr;
    td_u8 *reg_vir_addr;
    td_u8 *mmu_vir_addr;
    UADDR crg_phy_addr;
    td_u8 *crg_vir_addr;
} vdh_reg_info;

typedef struct {
    td_u16 reg_num;                    /* number of Reg of this VDH */
    td_u16 next_cfg_reg;               /* next Reg index to config */
    td_u16 next_bsp_reg;               /* next Reg index to return bsp */
    td_u16 next_pxp_reg;               /* next Reg index to return pxp */
    vdh_reg_info vdh_reg[VDH_REG_NUM]; /* array of Reg information */
} vdh_reg_pool_info;

typedef struct {
    td_u16 msg_num; /* number of Msg */
    td_u16 next_cfg_msg;
    vdh_msg_info vdh_msg[VDH_MSG_NUM]; /* array of Msg information */
} vdh_msg_pool_info;

typedef struct {
    vdh_reg_pool_info vdh_reg_pool; /* Reg pool */
    vdh_msg_pool_info vdh_msg_pool; /* Msg pool */
    vdh_ext_mem_info vdh_ext_mem;   /* extertal memory of VDH, only need one for one VDH */
    td_u32 bsp_int_cnt;
    td_u32 pxp_int_cnt;
    td_u32 reg_cfg_cnt;
    dec_report_info dec_report;
    mem_record slc_bin;
} dec_dev_vdh;

typedef struct {
    UADDR reg_phy_addr;
    td_u8 *reg_vir_addr;
} mdma_reg_info;

typedef struct {
    td_u64 dma_buf;
    UADDR base_phy_addr;
    td_u8 *base_vir_addr;
    td_u32 length;
    UADDR pic_msg_phy_addr;
    td_u8 *pic_msg_vir_addr;
    UADDR mbg_msg_phy_addr;
    td_u8 *mbg_msg_vir_addr;
} mdma_msg_info;

typedef struct {
    td_u8 used;
    td_s32 chan_id;
    mdma_reg_info reg;
    mdma_msg_info msg;
    td_u32 irq_num;
    td_u32 int_cnt;
    td_u32 reg_cfg_cnt;
} dec_dev_mdma;

typedef struct {
    td_u32 int_reg;
    td_u32 int_flag;
    td_u32 int_unid;
    td_u32 err_flag;
    td_u32 rd_slc_msg;
    td_u32 slice_num;
    td_u32 ctb_num;
    td_u32 bsp_cycle;
    td_u32 pxp_cycle;
} dec_back_up;

typedef struct {
    td_u16 dev_id;
    dec_dev_type type;
    dec_dev_ops *ops;
    dec_dev_state state;
    dec_back_up back_up;
    td_void *handle;
    td_u16 cfg_frm_cnt;
    td_u32 dec_frm_cnt;
    td_u32 last_start_time;
    td_u32 step_start_time; /* start time when Muti Reg cfg in same period */
    td_u32 update_step_time;
    td_u32 last_count_time;
    td_u32 work_time;
    td_u32 work_max_time;
    td_u32 peroid_work_time;
    td_u32 kilo_peroid_bsp_cycle;
    td_u32 kilo_peroid_pxp_cycle;
    td_u32 avg_bsp_cycle;
    td_u32 avg_pxp_cycle;
    td_u32 peroid_frm_cnt;
    td_u32 ratio;

    td_u32 isr_last_time;
    td_u32 isr_schd_intval[DEC_DEBUG_CNT];
    td_u32 isr_cost_time[DEC_DEBUG_CNT];
    td_u32 isr_max_cost_time;
    td_s32 slice_bin_size;
    os_sema sema;
    vfmw_vid_std prev_reg_vid_std;  /* only record conflict std to avoid logic error */
} dec_dev_info;

typedef struct {
    dec_dev_info dev_inst[DEC_DEV_NUM]; /* device instance */
} dec_dev_pool;

typedef struct {
    td_u16 slice_num;
    td_u16 *slice_mb_range;
} dec_err_report;

td_s32 dec_dev_get_dev(td_u16 dev_id, dec_dev_info **dev);

td_void dec_dev_init(td_void);
td_void dec_dev_deinit(td_void);

td_s32 dec_dev_open(td_s32 is_sec);
td_s32 dec_dev_close(td_s32 is_sec);
td_s32 dec_dev_resume(td_void);
td_s32 dec_dev_suspend(td_void);
td_s32 dec_dev_config(td_u16 dev_id, td_void *cfg);
os_irqret dec_dev_isr_bsp0(td_s32 irq, td_void *dev_id);
os_irqret dec_dev_isr_pxp0(td_s32 irq, td_void *dev_id);
os_irqret dec_dev_isr_mdma0(td_s32 irq, td_void *dev_id);
os_irqret dec_dev_isr_pxp1(td_s32 irq, td_void *dev_id);
os_irqret dec_dev_isr_mdma1(td_s32 irq, td_void *dev_id);
os_irqret dec_dev_isr_mmu0(td_s32 irq, td_void *dev_id);
os_irqret dec_dev_isr_mmu1(td_s32 irq, td_void *dev_id);

td_s32 dec_dev_get_mem_need_size(vfmw_svr_mem_need *mem);
td_s32 dec_dev_acquire_dev(td_s32 is_sec, td_void *task, td_u16 *dev_id);

td_s32 dec_dev_acquire_msg(td_s32 chan_id, td_u16 dev_id, td_u16 *msg_id);
td_s32 dec_dev_acquire_reg(td_s32 chan_id, td_u16 dev_id, td_u16 *reg_id);
td_s32 dec_dev_acquire_cfg_id(td_u16 dev_id, td_u16 *cfg_id);
td_s32 dec_dev_release_msg(td_u16 dev_id, td_u16 msg_id);
td_s32 dec_dev_release_reg(td_u16 dev_id, td_u16 reg_id);
td_s32 dec_dev_config_msg(td_u16 dev_id, td_u16 msg_id, dec_hal_ops *ops, td_void *task);
td_s32 dec_dev_config_reg(td_u16 dev_id, td_u16 reg_id, dec_hal_ops *ops, td_void *task);
td_s32 dec_dev_time_out(td_u16 dev_id);
td_s32 dec_dev_start_reg(td_u16 dev_id, td_u16 reg_id);
td_s32 dec_dev_cancel_reg(td_u16 dev_id, td_u16 reg_id);

td_s32 dec_dev_calc_mem_param(td_u16 dev_id, dec_hal_ops *ops, td_void *cfg, td_void *task);
td_s32 dec_dev_read_report(td_u16 dev_id, td_u16 reg_id, td_void *slice_list, dec_report_info **dec_report);
td_s32 dec_dev_refresh_bin(td_void);
td_s32 dec_dev_wait_hardware(td_void *chan, td_void *task);
td_void dec_dev_bypass_process(td_s32 dev_id);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
