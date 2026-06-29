/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef STM_DEV_H
#define STM_DEV_H

#include "vfmw.h"
#include "stm_base.h"
#include "scd_hal.h"
#include "vfmw_sys.h"
#include "vfmw_osal.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define STM_DEV_NUM SCD_NUM

typedef scd_reg stm_reg;
typedef os_irqret (*fn_stm_dev_isr)(td_s32, td_void *);

typedef enum {
    STM_DEV_STATE_NULL = 0,
    STM_DEV_STATE_IDLE,
    STM_DEV_STATE_BUSY,
    STM_DEV_STATE_MAX,
} stm_dev_state;

typedef struct {
    td_s32 id;
    td_u32 unid;
    td_void *handle;
    stm_dev_state state;
    os_lock spin_lock;

    UADDR crg_phy;
    td_u8 *crg_vir;
    UADDR reg_phy;
    volatile td_u8 *reg_vir;
    td_u32 reg_size;

    td_u64 msg_dma_buf;
    UADDR dn_msg_phy;
    td_u8 *dn_msg_vir;
    td_u32 dn_msg_size;
    UADDR up_msg_phy;
    td_u8 *up_msg_vir;
    td_u32 up_msg_size;

    td_s32 chan_id;
    td_s32 start_cnt;
    td_s32 int_count;
    td_s32 start_num;
    td_s32 insert_num;
    td_u32 start_time;
    td_u32 cancel;
    td_u32 int_mask;
    td_u32 int_time;

    stm_reg reg;

    os_sema sema;
} stm_dev;

td_void stm_dev_init(td_void);
td_void stm_dev_exit(td_void);
td_s32 stm_dev_open(td_s32 dev_id);
td_s32 stm_dev_close(td_s32 dev_id);
td_s32 stm_dev_reset(td_s32 dev_id);
td_s32 stm_dev_start(td_s32 chan_id, td_s32 dev_id);
td_u32 stm_dev_hal_size(td_s32 dev_id);
td_u8 stm_dev_is_scd_bypass(td_s32 chan_id);
td_u8 stm_dev_is_seg_bypass(td_s32 chan_id);
td_s32 stm_dev_raw_to_seg(td_s32 chan_id, stm_seg_info *seg);
td_s32 stm_dev_run_proc(td_void);
td_void stm_dev_try_open_device(td_void);
td_void stm_dev_try_close_device(td_void);
td_s32 stm_dev_cancel(td_s32 dev_id, td_s32 chan_id);
os_irqret stm_dev_isr0(td_s32 irq, td_void *dev_id);
td_s32 stm_dev_suspend(td_void);
td_s32 stm_dev_resume(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* STM_DEV_H */
