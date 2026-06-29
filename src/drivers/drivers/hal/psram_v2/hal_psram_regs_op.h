/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: psram driver regs op header file \n
 *
 * History: \n
 * 2024-05-14, Create file. \n
 */
#ifndef HAL_PSRAM_REGS_OP_H
#define HAL_PSRAM_REGS_OP_H

#include "errcode.h"
#include "stdbool.h"
#include "psram_porting.h"
#include "hal_psram_regs_def.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup _
 * @ingroup
 * @{
 */

#define PSRAM_DLYM_WAIT_TIME    4
#define PSRAM_RDQSG_START_POS   7

#define PSRAM_PHY_CTRL_PCODE_VAL  11
#define PSRAM_PHY_CTRL_NCODE_VAL  11

#define PSRAM_DMAC_MEM_ROW       4
#define PSRAM_DMAC_MEM_COL       1
#define PSRAM_DMAC_WS_BUF_PRI1   0x7
#define PSRAM_DMAC_TIMING0_TDQSCK   1
#define PSRAM_DMAC_TIMING0_TCPHW   6
#define PSRAM_DMAC_TIMING0_TCPHR   6
#define PSRAM_DMAC_TIMING0_TWL   7
#define PSRAM_DMAC_TIMING0_TRL   7

#define PSRAM_WB_DMAC_TIMING0_VAL 0x45050E0E

#if defined(PRE_ASIC)
#define PSRAM_INIT_DLYM_EN       1
#define PSRAM_INIT_ZCAL_EN       1
#define PSRAM_RXFIFO_RDEN_POS    0
#define PSRAM_DMAC_TIMING1_THS      0x77
#define PSRAM_DMAC_TIMING1_TCSS     0x1
#define PSRAM_DMAC_TIMING2_TXPHS    0xD
#define PSRAM_DMAC_TIMING2_TXHS     0x77
#define PSRAM_DMAC_TIMING3_TRST     0x100
#define PSRAM_DMAC_TIMING3_TDPD     0x1B7
#define PSRAM_DMAC_TIMING4_TXPDPD   0xD
#define PSRAM_DMAC_TIMING4_TXDPD    0x77
#define PSRAM_DMAC_CS_EXT_WDEN      1
#define PSRAM_DMAC_CS_EXT_WR_CS     1
#else
#define PSRAM_INIT_DLYM_EN       0
#define PSRAM_INIT_ZCAL_EN       0
#define PSRAM_RXFIFO_RDEN_POS    0x11
#define PSRAM_DMAC_TIMING1_THS      0x7530
#define PSRAM_DMAC_TIMING1_TCSS      0x0
#define PSRAM_DMAC_TIMING2_TXPHS    0xC
#define PSRAM_DMAC_TIMING2_TXHS     0x7530
#define PSRAM_DMAC_TIMING3_TRST     0x190
#define PSRAM_DMAC_TIMING3_TDPD     0x186A0
#define PSRAM_DMAC_TIMING4_TXPDPD   0xC
#define PSRAM_DMAC_TIMING4_TXDPD    0x7530
#define PSRAM_DMAC_CS_EXT_WDEN      0
#define PSRAM_DMAC_CS_EXT_WR_CS     0
#endif

#define PSRAM_DMAC_SFC_CMD_RESET 0x6

#define PSRAM_AXI_REGION_SIZE  0x5  /* 32M */
#define PSRAM_AXI_REGION_BASE_ADDR0  0x20  /* psram0 */
#define PSRAM_AXI_REGION_BASE_ADDR1  0x40  /* psram1 */
#define PSRAM_AXI_REGION_ADDR_ALIGNED  0x7  /* 1K */
#define PSRAM_AXI_RD_WRAP_BOUNDARY  0x4  /* 128B */


#define PSRAM_WAIT_TIMEOUT         3000

typedef enum {
    RESERVED = 0,
    MID_LOOPBACK_IN,
    SSTL_IN,
    SSTL_IN_WITH_LOW_VOLT,
} psram_phy_io_inmode_t;

typedef enum {
    OHMS_0 = 0,   /* High Impedance */
    OHMS_1,       /* VDDIO 1.8V->240Ω  VDDIO 1.2V->360Ω */
    OHMS_2,       /* VDDIO 1.8V->120Ω  VDDIO 1.2V->180Ω */
    OHMS_3,       /* VDDIO 1.8V->80Ω   VDDIO 1.2V->120Ω */
    OHMS_4,       /* VDDIO 1.8V->60Ω   VDDIO 1.2V->90Ω */
    OHMS_5,       /* VDDIO 1.8V->48Ω   VDDIO 1.2V->72Ω */
    OHMS_6,       /* VDDIO 1.8V->40Ω   VDDIO 1.2V->60Ω */
} psram_output_driver_impedance_sel_t;

typedef enum {
    QOS_PRI0 = 0,
    QOS_PRI1,
    QOS_PRI2,
    QOS_PRI3,
    QOS_PRI4,
    QOS_PRI5,
    QOS_PRI6,
    QOS_PRI7,
} psram_qop_wr_rd_pri_t;

typedef enum {
    MR0 = 0,
    MR4 = 4,
    MR6 = 6,
    MR8 = 8
} psram_mr_t;

/* psram phy config */
void hal_psram_regs_phy_init_zcal_en(psram_id_t id);
void hal_psram_regs_phy_dlym_en(void);
void hal_psram_regs_phy_init_ck_pulldown_set(uint32_t value);
void hal_psram_regs_phy_init_ctrl(void);
void hal_psram_regs_phy_io_ctrl1(void);
void hal_psram_regs_phy_io_ctrl3(void);
void hal_psram_regs_phy_ctrl2_init(void);
void hal_psram_regs_phy_ctrl2(psram_id_t id);
void hal_psram_regs_phy_ctrl2_step_1(void);
void hal_psram_regs_phy_work_mode0(uint32_t psram_freq);
void hal_psram_regs_phy_work_mode1(void);
void hal_psram_regs_phy_ctrl0(uint32_t psram_freq);
void hal_psram_regs_phy_ctrl1_init_rdqs(void);
void hal_psram_regs_phy_ctrl1(uint32_t psram_freq);
void hal_psram_regs_phy_ctrl2_step_2(void);
void hal_psram_regs_phy_io_ctrl0(void);
uint32_t hal_psram_regs_phy_get_id0_zcal(void);

/* psram dmac config */
void hal_psram_regs_dmac_cfg_hs_enter_req(void);
void hal_psram_regs_dmac_cfg_hs_exit_req(void);
void hal_psram_regs_dmac_cfg_mem_mode(void);
void hal_psram_regs_dmac_cfg_work_mode(void);
void hal_psram_regs_dmac_ws_buf(void);
void hal_psram_regs_dmac_cfg_timing(void);
void hal_psram_regs_dmac_cfg_lat(void);
void hal_psram_regs_dmac_cfg_ext(void);
void hal_psram_regs_dmac_cfg_lp(void);
bool hal_psram_regs_dmac_cur_hs_status(void);
uint32_t hal_dmac_get_data(void);
/* before psram clock switch, need exec cmd bp 反压 */
void hal_psram_regs_dmac_cfg_hs_exec_cmd_bp(void);
/* after psram clock switch, need close cmd bp 反压 */
void hal_psram_regs_dmac_cfg_hs_close_cmd_bp(void);

/* psram axi config */
void hal_psram_regs_axi_region_map(psram_id_t id);
void hal_psram_regs_axi_region_attr(void);
void hal_psram_regs_axi_action(void);
void hal_psram_regs_axi_qos_wrpri(void);
void hal_psram_regs_axi_qos_rdpri(void);

void hal_psram_dmc_cfg_mr(uint32_t val);
void hal_psram_dmac_mr_reset(void);
errcode_t hal_dmac_read_mr_reg(psram_mr_t mr, uint32_t *val);
errcode_t hal_dmac_mr_cfg(psram_mr_t mr);
void hal_psram_get_addr(psram_id_t id);

/* winbond flag get */
bool psram_port_winbond_flag_get(void);

/* psram rx fifo reset */
void hal_psram_regs_phy_rxfifo_rst(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif