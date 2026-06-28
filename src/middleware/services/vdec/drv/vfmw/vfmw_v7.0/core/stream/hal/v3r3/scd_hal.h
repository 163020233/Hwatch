/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef SCD_HAL_H
#define SCD_HAL_H

#include "vfmw.h"
#include "stm_raw.h"
#include "stm_seg.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define SCD_REG_LEN    256
#define SCD_ADDR_SHIFT 4
#define SCD_DMSG_NUM   1024
#define ONE_DMSG_SIZE  (4 * (td_u32)sizeof(td_u32))
#define SCD_DMSG_SIZE  (SCD_DMSG_NUM * ONE_DMSG_SIZE)
#define SCD_UMSG_NUM   2048
#define SCD_UMSG_EXT   3
#define SCD_UMSG_STEP  3
#define ONE_UMSG_SIZE  (SCD_UMSG_STEP * (td_u32)sizeof(td_u32))
#define SCD_UMSG_SIZE  ((SCD_UMSG_NUM + SCD_UMSG_EXT) * ONE_UMSG_SIZE)

#define reg_vir_(id)                    (stm_ctrl_get_dev(id)->reg_vir)
#define REG_VIR reg_vir_
#define wr_scdreg_(id, reg, data)       *(volatile td_u32 *)(REG_VIR(id) + (reg)) = (data)
#define WR_SCDREG wr_scdreg_
#define rd_scdreg_(id, reg)             *(volatile td_u32 *)(REG_VIR(id) + (reg))
#define RD_SCDREG rd_scdreg_
#define wr_scdreg_vir_(pvir, reg, data) *(volatile td_u32 *)((pvir) + (reg)) = (data)
#define WR_SCDREG_VIR wr_scdreg_vir_
#define rd_scdreg_vir_(pvir, reg)       *(volatile td_u32 *)((pvir) + (reg))
#define RD_SCDREG_VIR rd_scdreg_vir_

#define REG_OFFSET 0x000

#define REG_SCD_START         (REG_OFFSET + 0x000)
#define REG_SCD_INT_MASK      (REG_OFFSET + 0x004)
#define REG_SCD_INT_CLR       (REG_OFFSET + 0x008)
#define REG_SCD_OVER          (REG_OFFSET + 0x00c)
#define REG_SCD_DESTROY       (REG_OFFSET + 0x010)
#define REG_SCD_PROTOCOL      (REG_OFFSET + 0x014)
#define REG_PRE_BYTE_LSB      (REG_OFFSET + 0x018)
#define REG_PRE_BYTE_MSB      (REG_OFFSET + 0x01c)
#define REG_LIST_ADDRESS      (REG_OFFSET + 0x020)
#define REG_UP_ADDRESS        (REG_OFFSET + 0x024)
#define REG_SCD_UP_LEN        (REG_OFFSET + 0x028)
#define REG_BUFFER_FIRST      (REG_OFFSET + 0x02c)
#define REG_BUFFER_LAST       (REG_OFFSET + 0x030)
#define REG_BUFFER_INIT       (REG_OFFSET + 0x034)
#define REG_BUFFER_INIT_MSB   (REG_OFFSET + 0x038)
#define REG_BYTE_VALID        (REG_OFFSET + 0x03c)
#define REG_SCD_NUM           (REG_OFFSET + 0x040)
#define REG_SRC_EATEN         (REG_OFFSET + 0x048)
#define REG_SEG_NEXT_ADDR     (REG_OFFSET + 0x04c)
#define REG_SEG_NEXT_ADDR_MSB (REG_OFFSET + 0x050)
#define REG_RUN_CYCLE         (REG_OFFSET + 0x054)
#define REG_ROLL_ADDR         (REG_OFFSET + 0x058)
#define REG_ROLL_ADDR_MSB     (REG_OFFSET + 0x05c)

typedef enum {
    SM_NOT_MPEG4 = 0,
    SM_MPEG4_NOT_H263,
    SM_MPEG4_H263,
    SM_MPEG4_MAX
} mpeg4_sub_std;

typedef struct {
    td_u8 int_mask;
    td_u8 scd_start;
    td_u8 scd_init_clr;
    td_u8 scd_mmu_en;
    td_u8 vdh_mmu_en;
    td_u8 avs_flag;
    td_u8 slice_check;
    td_u8 safe_flag;
    td_u8 scd_over;
    UADDR buffer_first;
    UADDR buffer_last;
    UADDR buffer_init;
    UADDR roll_addr;
    UADDR next_addr;
    UADDR dn_msg_phy;
    UADDR up_msg_phy;
    td_s32 *dn_msg_vir;
    td_s32 *up_msg_vir;
    td_u32 up_len;
    td_u32 up_step;
    td_u32 std_type;
    td_u32 pre_lsb;
    td_u32 pre_msb;
    td_u32 byte_valid;
    td_u32 short_num;
    td_u32 start_num;
    td_u32 src_eaten;
    td_u32 cfg_unid;
    td_u32 int_unid;
} scd_reg;

td_s32 scd_hal_open(td_s32 scd_id);
td_s32 scd_hal_close(td_s32 scd_id);
td_s32 scd_hal_irq_init(td_s32 dev_id);
td_s32 scd_hal_irq_exit(td_s32 dev_id);
td_s32 scd_hal_reset(td_s32 scd_id);
td_void scd_hal_start(td_s32 scd_id);
td_void scd_hal_clear_int(td_s32 scd_id);
td_void scd_hal_mask_int(td_s32 scd_id);
td_void scd_hal_enable_int(td_s32 scd_id);
td_s32 scd_hal_set_reg(td_s32 chan_id, td_s32 scd_id);
td_s32 scd_hal_write_reg(td_s32 scd_id);
td_s32 scd_hal_write_msg(td_s32 chan_id, td_s32 scd_id);
td_u32 scd_hal_msg_size(td_s32 scd_id);
td_s32 scd_hal_check_int(td_s32 scd_id);
td_void scd_hal_read_reg(td_s32 scd_id);
td_u32 scd_hal_src_eaten(const td_u32 *msg);
UADDR scd_hal_get_seg_addr(const td_u32 *msg);
td_s32 scd_hal_is_bypass(td_s32 vid_std, const vfmw_std_ext *std_ext);
td_s32 scd_hal_check_unid(td_s32 scd_id);
td_s32 scd_hal_cancel(td_s32 scd_id);
#ifdef VFMW_SCD_DFX_SUPPORT
td_void scd_hal_read_int_state(td_s32 scd_id);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* SCD_HAL_H */
