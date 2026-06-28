/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: mipi tx hal file.
 *
 * Create: 2020-04-14
 */

#include "qspi_display_hal.h"
#include "qspi_display_reg.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef GPU_1A22
#define QSPI_LCD_BASE_ADDR          0x52029000
#else
#define QSPI_LCD_BASE_ADDR          0x24000000
#endif
#define DPU_QSPI_DR_ADDR            (QSPI_LCD_BASE_ADDR + 0x30)

volatile qspi_display_regs_type_t *g_qspi_disp_hal_regs_va = NULL;

void qspi_disp_hal_reg_init(void)
{
    g_qspi_disp_hal_regs_va = (qspi_display_regs_type_t *)(uintptr_t)QSPI_LCD_BASE_ADDR;
}

void qspi_disp_hal_enable(td_bool enable)
{
    volatile u_spi_er spi_er;
    spi_er.u32 = g_qspi_disp_hal_regs_va->spi_er.u32;
    spi_er.bits.start_en = enable;
    g_qspi_disp_hal_regs_va->spi_er.u32 = spi_er.u32;
}

void qspi_disp_hal_set_sst_enable(bool enable)
{
    volatile u_spi_ctra spi_ctra;
    spi_ctra.u32 = g_qspi_disp_hal_regs_va->spi_ctra.u32;
    spi_ctra.bits.ssn_te = enable;
    g_qspi_disp_hal_regs_va->spi_ctra.u32 = spi_ctra.u32;
}

void qspi_disp_hal_set_frame_cfg(frame_protocol prs, frame_format fmt, transmit_format trsm,
    td_u32 frame_bit, td_u32 control_bit)
{
    volatile u_spi_ctra spi_ctra;
    spi_ctra.u32 = g_qspi_disp_hal_regs_va->spi_ctra.u32;
    spi_ctra.bits.prs = prs;
    spi_ctra.bits.enhff = fmt;
    spi_ctra.bits.trsm = trsm;
    spi_ctra.bits.dfs32 = frame_bit - 1;
    spi_ctra.bits.cfs16 = control_bit - 1;
    g_qspi_disp_hal_regs_va->spi_ctra.u32 = spi_ctra.u32;
}

void qspi_disp_hal_set_receive_frm_len(td_u32 len)
{
    volatile u_spi_ctrb spi_ctrb;
    spi_ctrb.u32 = g_qspi_disp_hal_regs_va->spi_ctrb.u32;
    spi_ctrb.bits.nrdf = len - 1;
    g_qspi_disp_hal_regs_va->spi_ctrb.u32 = spi_ctrb.u32;
}

void qspi_disp_hal_set_ins_addr_cfg(cmd_bit_length cmd_len, td_u32 addr_len, ins_addr_formate fmt)
{
    volatile u_spi_enhctl spi_enhctl;
    spi_enhctl.u32 = g_qspi_disp_hal_regs_va->spi_enhctl.u32;
    spi_enhctl.bits.ilen = cmd_len;
    spi_enhctl.bits.addrlen = addr_len / 4; // 4 is num
    spi_enhctl.bits.aaitf = fmt;
    g_qspi_disp_hal_regs_va->spi_enhctl.u32 = spi_enhctl.u32;
}

void qspi_disp_hal_set_ddr_enable(td_bool ddr_en, td_bool ins_ddr_en, td_bool rxds_en)
{
    volatile u_spi_enhctl spi_enhctl;
    spi_enhctl.u32 = g_qspi_disp_hal_regs_va->spi_enhctl.u32;
    spi_enhctl.bits.ddren = ddr_en;
    spi_enhctl.bits.iddren = ins_ddr_en;
    spi_enhctl.bits.rdsen = rxds_en;
    g_qspi_disp_hal_regs_va->spi_enhctl.u32 = spi_enhctl.u32;
}

void qspi_disp_hal_set_microwire_cfg(microwire_ctl transmit, microwire_trans_mode mode,
    td_bool hand_shake_en)
{
    volatile u_spi_mcr spi_mcr;
    spi_mcr.u32 = g_qspi_disp_hal_regs_va->spi_mcr.u32;
    spi_mcr.bits.mtrc = transmit;
    spi_mcr.bits.mss = mode;
    spi_mcr.bits.mh = hand_shake_en;
    g_qspi_disp_hal_regs_va->spi_mcr.u32 = spi_mcr.u32;
}

void qspi_disp_hal_set_freq_div(td_u32 freq_div)
{
    volatile u_spi_brs spi_brs;
    spi_brs.u32 = g_qspi_disp_hal_regs_va->spi_brs.u32;
    spi_brs.bits.frdv = freq_div;
    g_qspi_disp_hal_regs_va->spi_brs.u32 = spi_brs.u32;
}

void qspi_disp_hal_set_dma_enable(td_bool receive_en, td_bool transmit_en)
{
    volatile u_spi_dcr spi_dcr;
    spi_dcr.u32 = g_qspi_disp_hal_regs_va->spi_dcr.u32;
    spi_dcr.bits.rden = receive_en;
    spi_dcr.bits.tden = transmit_en;
    g_qspi_disp_hal_regs_va->spi_dcr.u32 = spi_dcr.u32;
}

void qspi_disp_hal_set_dma_rx_watermark(td_u8 watermark_depth)
{
    volatile u_spi_drdl spi_drdl;
    spi_drdl.u32 = g_qspi_disp_hal_regs_va->spi_drdl.u32;
    spi_drdl.bits.rdl = watermark_depth;
    g_qspi_disp_hal_regs_va->spi_drdl.u32 = spi_drdl.u32;
}

void qspi_disp_hal_set_dma_tx_watermark(td_u8 watermark_depth)
{
    volatile u_spi_dtdl spi_dtdl;
    spi_dtdl.u32 = g_qspi_disp_hal_regs_va->spi_dtdl.u32;
    spi_dtdl.bits.tdl = watermark_depth;
    g_qspi_disp_hal_regs_va->spi_dtdl.u32 = spi_dtdl.u32;
}

void qspi_disp_hal_write_data(uint32_t data)
{
    g_qspi_disp_hal_regs_va->spi_drnm.u32 = data;
}

uint32_t qspi_disp_hal_read_data(void)
{
    return g_qspi_disp_hal_regs_va->spi_drnm.u32;
}


void qspi_disp_hal_set_interrupt_mask(uint32_t mask)
{
    volatile u_spi_inmar spi_inmar;
    spi_inmar.u32 = g_qspi_disp_hal_regs_va->spi_inmar.u32;
    spi_inmar.u32 = mask;
    g_qspi_disp_hal_regs_va->spi_inmar.u32 = spi_inmar.u32;
}

void qspi_disp_hal_set_slave_enable(td_u8 enable)
{
    volatile u_spi_slenr spi_slenr;
    spi_slenr.u32 = g_qspi_disp_hal_regs_va->spi_slenr.u32;
    spi_slenr.bits.ssef = enable;
    g_qspi_disp_hal_regs_va->spi_slenr.u32 = spi_slenr.u32;
}

void qspi_disp_hal_set_send_fifo_waterline(td_u8 len)
{
    volatile u_spi_twlr spi_twlr;
    spi_twlr.u32 = g_qspi_disp_hal_regs_va->spi_twlr.u32;
    spi_twlr.bits.twl = len;
    g_qspi_disp_hal_regs_va->spi_twlr.u32 = spi_twlr.u32;
}

td_u8 qspi_disp_hal_get_send_fifo_datasize(void)
{
    return g_qspi_disp_hal_regs_va->spi_tlr.bits.tfl;
}

void qspi_disp_hal_set_receive_fifo_waterline(td_u8 len)
{
    volatile u_spi_rwlr spi_rwlr;
    spi_rwlr.u32 = g_qspi_disp_hal_regs_va->spi_rwlr.u32;
    spi_rwlr.bits.rwl = len;
    g_qspi_disp_hal_regs_va->spi_rwlr.u32 = spi_rwlr.u32;
}

td_u8 qspi_disp_hal_get_receive_fifo_datasize(void)
{
    return g_qspi_disp_hal_regs_va->spi_rlr.bits.rfl;
}

td_bool qspi_disp_hal_tx_fifo_is_full(void)
{
    return (g_qspi_disp_hal_regs_va->spi_wsr.bits.tfnf == 0);
}

void qspi_disp_hal_set_data_wait_en(bool enable)
{
    volatile u_spi_rsvd spi_rsvd;
    spi_rsvd.u32 = g_qspi_disp_hal_regs_va->spi_rsvd.u32;
    spi_rsvd.bits.cfg_spi_display_data_wait_en = enable;
    g_qspi_disp_hal_regs_va->spi_rsvd.u32 = spi_rsvd.u32;
}

void qspi_disp_hal_set_data_cfg(data_write_order type, td_u8 wait_cnt, bool data_only,
    bool pready_enable, td_u8 water_line)
{
    volatile u_spi_rsvd spi_rsvd;
    spi_rsvd.u32 = g_qspi_disp_hal_regs_va->spi_rsvd.u32;
    spi_rsvd.bits.spi_qspi_inv_byte_en = type;
    spi_rsvd.bits.spi_qspi_data_cnt = wait_cnt;
    spi_rsvd.bits.spi_qspi_data_en = data_only;
    spi_rsvd.bits.spi_pready_en = pready_enable;
    spi_rsvd.bits.tx_fifo_th = water_line;
    g_qspi_disp_hal_regs_va->spi_rsvd.u32 = spi_rsvd.u32;
}

bool qspi_disp_hal_get_inv_en(void)
{
    return (g_qspi_disp_hal_regs_va->spi_rsvd.bits.spi_qspi_inv_byte_en == 0);
}

void qspi_disp_hal_clear_data_cfg(void)
{
    g_qspi_disp_hal_regs_va->spi_rsvd.u32 = 0;
}

#ifdef __cplusplus
#if __cplusplus
}

#endif
#endif
