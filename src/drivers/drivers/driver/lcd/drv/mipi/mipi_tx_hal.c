/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: mipi tx hal file.
 *
 * Create: 2020-04-14
 */

#include "mipi_tx_hal.h"
#include "common_def.h"
#include "securec.h"
#include "tcxo.h"
#include "soc_osal.h"
#include "soc_errno.h"
#include "chip_io.h"
#include "mipi_tx.h"
#include "mipi_tx_reg.h"
#include "chip_core_irq.h"
#include "mipi_tx_dfx.h"
#ifdef PRE_FPGA
#include "mipi_tx_fpga.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define HAL_MIPI_OFFSET_16 16

volatile mipi_tx_regs_type_t *g_mipi_tx_regs_va = NULL;
static uint32_t g_mipi_tx_irq_num = NUM_INTERRUPT_MIPI;
static uint32_t g_actual_phy_data_rate;

#if MIPI_TX_INTERRUPTS_ENABLE
static void mipi_tx_enable_int(uint32_t enable);
#endif

void mipi_tx_drv_set_irq_num(uint32_t irq_num)
{
    g_mipi_tx_irq_num = irq_num;
}

void mipi_tx_drv_set_regs(mipi_tx_regs_type_t *regs)
{
    g_mipi_tx_regs_va = regs;
}

PLT_SEC_ITCM_T static INLINE void mipi_tx_set_actual_phy_data_rate(uint32_t actual_date_rate)
{
    g_actual_phy_data_rate = actual_date_rate;
}

PLT_SEC_ITCM_T uint32_t mipi_tx_get_actual_phy_data_rate(void)
{
    return g_actual_phy_data_rate;
}

void mipi_tx_drv_get_dev_status(mipi_tx_dev_phy_t *mipi_tx_phy_ctx)
{
    volatile reg_hori0_det hori0_det;
    volatile reg_hori1_det hori1_det;
    volatile reg_vert_det vert_det;
    volatile reg_vsa_det vsa_det;
    volatile reg_ppi_v_send1 vert_ppi;
    volatile reg_ppi_h_send1 hori_ppi;
    volatile reg_lane_num lane_num;

    hori0_det.u32 = g_mipi_tx_regs_va->hori0_det.u32;
    hori1_det.u32 = g_mipi_tx_regs_va->hori1_det.u32;
    vert_det.u32 = g_mipi_tx_regs_va->vert_det.u32;
    vsa_det.u32 = g_mipi_tx_regs_va->vsa_det.u32;
    vert_ppi.u32 = g_mipi_tx_regs_va->ppi_v_send1.u32;
    hori_ppi.u32 = g_mipi_tx_regs_va->ppi_h_send1.u32;
    lane_num.u32 = g_mipi_tx_regs_va->lane_num.u32;

    mipi_tx_phy_ctx->hact_det = hori0_det.bits.hact_det;
    mipi_tx_phy_ctx->hall_det = hori0_det.bits.hline_det;
    mipi_tx_phy_ctx->hbp_det  = hori1_det.bits.hbp_det;
    mipi_tx_phy_ctx->hsa_det  = hori1_det.bits.hsa_det;

    mipi_tx_phy_ctx->vact_det = vert_det.bits.vact_det;
    mipi_tx_phy_ctx->vall_det = vert_det.bits.vall_det;
    mipi_tx_phy_ctx->vsa_det  = vsa_det.bits.vsa_det;

    mipi_tx_phy_ctx->vall_ppi = vert_ppi.bits.vtotal_length;
    mipi_tx_phy_ctx->vact_ppi = vert_ppi.bits.vactive_length;
    mipi_tx_phy_ctx->hact_ppi = hori_ppi.bits.hactive_length;

    mipi_tx_phy_ctx->lane_num = lane_num.bits.lane_num + 1;
}

void mipi_tx_drv_get_int_status(uint32_t *int_dfx_info, uint32_t info_len)
{
    if (info_len == MIPI_INT_DFX_NUM) {
        int_dfx_info[0] = reg32(MIPI_TX_INT0_STATUS);
        int_dfx_info[1] = reg32(MIPI_TX_INT1_STATUS);
    }
}

#ifndef PRE_FPGA
#ifdef GPU_3322
PLT_SEC_ITCM_T static INLINE void mipi_tx_drv_set_phy_pll_setx(uint32_t phy_data_rate)
{
    uint8_t phy_div_int;
    uint8_t pll_set_prdiv_fbdiv;
    uint8_t rx_txpll_ictrl;
    if (phy_data_rate >= 800) {
        phy_div_int = (uint8_t)(phy_data_rate /
            HAL_MIPI_REF_CLK * 2); // 2 is power(2, pll_set_prdiv_fbdiv)
        pll_set_prdiv_fbdiv = 1; // 1 is defaule value
        rx_txpll_ictrl = 12; // 12 is defaule value
    } else {
        phy_div_int = (uint8_t)(phy_data_rate /
            HAL_MIPI_REF_CLK * 4); // 4 is power(2, pll_set_prdiv_fbdiv)
        pll_set_prdiv_fbdiv = 2; // 2 is defaule value
        rx_txpll_ictrl = 15; // 15 is defaule value
    }
    writeb(CLK_TCK_DATAHS, 0x55);
    writeb(PLL_REG_ENCS, 0x2); // 2 is set bit1 to 1
    writeb(PLL_SET_CKG_SEL, 0x0);
    writeb(PLL_SET_FBDIV, phy_div_int);
    writeb(PLL_SET_FBDIV_PRDIV, pll_set_prdiv_fbdiv << 4);
    writeb(PLL_DIV_UPDATE, 0x1);  // 1 is update
    writeb(RX_TXPLL_ICTRL, rx_txpll_ictrl);

#ifdef MIPI_TX_DEBUG
    mipi_tx_print("\r\n==========phy pll info=======\r\n");
    mipi_tx_print("pll_set_ckg_sel: 0x0\r\n");
    mipi_tx_print("pll_set_prdiv: 0x%x\r\n", phy_div_int);
    mipi_tx_print("pll_set_fbdiv_prdiv: 0x%x\r\n", pll_set_prdiv_fbdiv);
    mipi_tx_print("pll_set_txpll_ictrl: 0x%x\r\n", rx_txpll_ictrl);
    mipi_tx_print("=========================\r\n");
#endif
}

PLT_SEC_ITCM_T td_void uapi_mipi_tx_set_bypass(td_bool bypass_en)
{
    if (bypass_en) {
        /* mipi bypass (APHY do not drive) */
        writeb(PHY_LPTX_EN, 0x16);
        writeb(PHY_TEST_MODE, 0x40);
    } else {
        writeb(PHY_LPTX_EN, 0x1E);
        writeb(PHY_TEST_MODE, 0x0);
    }
}
#else

PLT_SEC_ITCM_T static uint8_t mipi_tx_drv_get_phy_pll_set_ckg_sel(uint32_t phy_data_rate, uint32_t *clk_pre)
{
    uint8_t ckg_sel;

    if ((phy_data_rate > 500) && (phy_data_rate <= 1000)) { /* bandwidth range (500, 1000] */
        ckg_sel = HSTX_CKG_SEL_DIV2;
        *clk_pre = phy_data_rate * 0x2;
    } else if (phy_data_rate > 250) { /* bandwidth range (250, 500] */
        ckg_sel = HSTX_CKG_SEL_DIV4;
        *clk_pre = phy_data_rate * 0x4;
    } else if (phy_data_rate > 125) { /* bandwidth range (125, 250] */
        ckg_sel = HSTX_CKG_SEL_DIV8;
        *clk_pre = phy_data_rate * 0x8;
    } else { /* bandwidth range [80, 125] */
        ckg_sel = HSTX_CKG_SEL_DIV16;
        *clk_pre = phy_data_rate * 16; // 16 div
    }

    return ckg_sel;
}

PLT_SEC_ITCM_T static uint32_t mipi_tx_drv_get_phy_pll_set_fbdiv(uint32_t clk_pre, uint32_t prdiv)
{
    return ((clk_pre * prdiv) / MIPI_TX_REF_CLK);
}

PLT_SEC_ITCM_T static INLINE void mipi_tx_drv_set_phy_pll_setx(uint32_t phy_data_rate)
{
    uint8_t pll_set_ckg_sel;
    uint8_t pll_set_prdiv_fbdiv;
    uint8_t pll_set_fbdiv;
    uint8_t set_ckg_sel;
    uint8_t set_prdiv = 0x1; /* always 2 div */
    uint32_t set_fbdiv;
    uint32_t clk_pre;

    /* cal set_fbdiv, set_fbdiv, set_prdiv */
    set_ckg_sel = mipi_tx_drv_get_phy_pll_set_ckg_sel(phy_data_rate, &clk_pre);
    set_fbdiv = mipi_tx_drv_get_phy_pll_set_fbdiv(clk_pre, 2); /* 2 div */

    pll_set_ckg_sel = (set_ckg_sel << 1) & 0xf;
    pll_set_fbdiv = (uint8_t)(set_fbdiv & 0xff);
    pll_set_prdiv_fbdiv =
        (((uint8_t)((set_fbdiv >> 8) & 0x7)) << 4) | (set_prdiv); /* val[8..4]: fbdiv[10..8] val[4..0]: prdiv */

    writeb(PLL_SET_CKG_SEL, pll_set_ckg_sel);
    writeb(PLL_SET_FBDIV, pll_set_fbdiv);
    writeb(PLL_SET_FBDIV_PRDIV, pll_set_prdiv_fbdiv);

#ifdef MIPI_TX_DEBUG
    mipi_tx_print("\r\n==========phy pll info=======\r\n");
    mipi_tx_print("pll_set_ckg_sel: 0x%x\r\n", pll_set_ckg_sel);
    mipi_tx_print("pll_set_prdiv: 0x%x\r\n", set_prdiv);
    mipi_tx_print("pll_set_fbdiv: 0x%x\r\n", set_fbdiv);
    mipi_tx_print("=========================\r\n");
#endif
}

PLT_SEC_ITCM_T td_void uapi_mipi_tx_set_bypass(td_bool bypass_en)
{
    if (bypass_en) {
        /* mipi bypass (APHY Strong Drive LP00) */
        writeb(PHY_LPTX_EN, 0xe0);
        writeb(PHY_BAND_EN, 0x0e);
        writeb(PHY_LPTX_DP, 0xc0);
    } else {
        writeb(PHY_LPTX_EN, 0x0);
        writeb(PHY_BAND_EN, 0x0);
        writeb(PHY_LPTX_DP, 0xfc);
    }
}
#endif

PLT_SEC_ITCM_T static inline uint8_t mipi_tx_drv_get_phy_timing_parameters_by_type(uint32_t timing_type)
{
    uint32_t actual_phy_data_rate = mipi_tx_get_actual_phy_data_rate();
    return (uint8_t)((actual_phy_data_rate * timing_type + ROUNDUP_VALUE) / INNER_PEROID);
}

/*
 * get global operation timing parameters.
 */
PLT_SEC_ITCM_T static void mipi_tx_drv_get_phy_timing_parameters(mipi_tx_phy_timing_parameters *tp)
{
    /* CLK_TLPX */
    tp->clk_tlpx = mipi_tx_drv_get_phy_timing_parameters_by_type(TLPX);

    /* CLK_TCLK_PREPARE */
    tp->clk_tclk_prepare = mipi_tx_drv_get_phy_timing_parameters_by_type(TCLK_PREPARE);

    /* CLK_TCLK_ZERO */
    tp->clk_tclk_zero = mipi_tx_drv_get_phy_timing_parameters_by_type(TCLK_ZERO);

    /* CLK_TCLK_TRAIL */
    tp->clk_tclk_trail = mipi_tx_drv_get_phy_timing_parameters_by_type(TCLK_TRAIL);

    /* DATA_TLPX */
    tp->data_tlpx = mipi_tx_drv_get_phy_timing_parameters_by_type(TLPX);

    /* DATA_THS_PREPARE */
    tp->data_ths_prepare = mipi_tx_drv_get_phy_timing_parameters_by_type(THS_PREPARE);

    /* DATA_THS_ZERO */
    tp->data_ths_zero = mipi_tx_drv_get_phy_timing_parameters_by_type(THS_ZERO);

    /* DATA_THS_TRAIL */
    tp->data_ths_trail = mipi_tx_drv_get_phy_timing_parameters_by_type(THS_TRAIL);

    /* DATA0~1 TPRE-DELAY */
    tp->data_tpre_delay = tp->clk_tlpx + tp->clk_tclk_prepare + tp->clk_tclk_zero + 9; /* 9: 3 + 5 + clk_pre_delay */

    /* CLK post delay */
    tp->clk_post_delay = tp->data_ths_trail + 13; /* 13 : 10+data_post_delay */
}

/*
 * set global operation timing parameters.
 */
PLT_SEC_ITCM_T static INLINE void mipi_tx_drv_set_phy_timing_parameters(const mipi_tx_phy_timing_parameters *tp)
{
#ifdef GPU_3322
    writel(PLL_FEEDBACK, 0x28033); // 0x28033 is default value
    writel(PLL_SR_PASVE, 0x38); // 0x38 is default value
    writel(FB_DIV_SSC, 0x10068); // 0x10068 is default value
    reg32_setbit(SEL_LOCK, 28);  // 28 is 28bit:SEL_LOCK
    writel(CPHY_DEDRV_EN, 0x1f); // 0x1f is default value
    writel(TCALALTSEQ, 0x0); // 0x0 is default value
#endif
    /*
     * CLK_TLPX
     * CLK_TCLK_PREPARE
     * CLK_TCLK_ZERO
     * CLK_TCLK_TRAIL
     * CLK_TCLK_PRE_DELAY
     * CLK_TCLK_POST_DELAY
     */
    writeb(CLK_TLPX, tp->clk_tlpx);
    writeb(CLK_TCLK_PREPARE, tp->clk_tclk_prepare);
    writeb(CLK_TCLK_ZERO, tp->clk_tclk_zero);
    writeb(CLK_TCLK_TRAIL, tp->clk_tclk_trail);
    writeb(CLK_TCLK_POST_DELAY, tp->clk_post_delay);
    writeb(CLK_TCLK_PRE_DELAY, TCLK_PRE_DELAY);

#if defined(GPU_1A21) || defined(GPU_1A22)
    /* CKG_EN_WAIT */
    writeb(CKG_EN_WAIT, SET_CKG_EN_WAIT_VALUE);
    writeb(DATA0_FILTER_BYPASS, PHY_FILTER_BYPASS);
    writeb(DATA1_FILTER_BYPASS, PHY_FILTER_BYPASS);
    writeb(RG_TOP_DMY, RG_TOP_DMY_VAL);
#endif
    /*
     * DATA_TLPX
     * DATA_THS_PREPARE
     * DATA_THS_ZERO
     * DATA_THS_TRAIL
     * DATA0~1 TPRE-DELAY
     */
    writeb(DATA0_TLPX, tp->data_tlpx);
    writeb(DATA0_THS_PREPARE, tp->data_ths_prepare);
    writeb(DATA0_THS_ZERO, tp->data_ths_zero);
    writeb(DATA0_THS_TRAIL, tp->data_ths_trail);
    writeb(DATA0_TPRE_DELAY, tp->data_tpre_delay);

    writeb(DATA0_TPOST_DELAY, TPOST_DELAY);
    writeb(DATA1_TLPX, tp->data_tlpx);
    writeb(DATA1_THS_PREPARE, tp->data_ths_prepare);
    writeb(DATA1_THS_ZERO, tp->data_ths_zero);
    writeb(DATA1_THS_TRAIL, tp->data_ths_trail);
    writeb(DATA1_TPRE_DELAY, tp->data_tpre_delay);
    writeb(DATA1_TPOST_DELAY, TPOST_DELAY);

#ifdef GPU_3322
    writeb(PERFORMANCE_REG, 0x8); // 0x8 is default value
#endif

#ifdef MIPI_TX_DEBUG
    mipi_tx_print("\r\n==========phy timing parameters=======\r\n");
    mipi_tx_print("data_tpre_delay(0x4A0/4E0): 0x%x\r\n", tp->data_tpre_delay);
    mipi_tx_print("clk_tlpx(0x440): 0x%x\r\n", tp->clk_tlpx);
    mipi_tx_print("clk_tclk_prepare(0x444): 0x%x\r\n", tp->clk_tclk_prepare);
    mipi_tx_print("clk_tclk_zero(0x448): 0x%x\r\n", tp->clk_tclk_zero);
    mipi_tx_print("clk_tclk_trail(0x44C): 0x%x\r\n", tp->clk_tclk_trail);
    mipi_tx_print("data_tlpx(0x480/4C0): 0x%x\r\n", tp->data_tlpx);
    mipi_tx_print("data_ths_prepare(0x484/4C4): 0x%x\r\n", tp->data_ths_prepare);
    mipi_tx_print("data_ths_zero(0x488/4C8): 0x%x\r\n", tp->data_ths_zero);
    mipi_tx_print("data_ths_trail(0x48C/4CC): 0x%x\r\n", tp->data_ths_trail);
    mipi_tx_print("=========================\r\n");
#endif
}

/*
 * set data lp2hs,hs2lp time
 * set clk lp2hs,hs2lp time
 * unit: hsclk
 */
PLT_SEC_ITCM_T static void mipi_tx_drv_set_phy_hs_lp_switch_time(const mipi_tx_phy_timing_parameters *tp)
{
    /* clk lp2hs,hs2lp time */
    g_mipi_tx_regs_va->clklane_time.u32 = ((tp->clk_tclk_trail + tp->clk_post_delay) << HAL_MIPI_OFFSET_16) +
        tp->clk_tlpx + tp->clk_tclk_prepare + tp->clk_tclk_zero + TCLK_PRE_DELAY;
    /* data lp2hs,hs2lp time */
    g_mipi_tx_regs_va->datalane_time.u32 = ((tp->data_ths_trail + TPOST_DELAY) << HAL_MIPI_OFFSET_16) +
        tp->data_tlpx + tp->data_ths_prepare + tp->data_ths_zero + tp->data_tpre_delay;
#ifdef MIPI_TX_DEBUG
    mipi_tx_print("datalane_time(0x9C): 0x%x\r\n", g_mipi_tx_regs_va->datalane_time.u32);
    mipi_tx_print("clklane_time(0x98): 0x%x\r\n", g_mipi_tx_regs_va->clklane_time.u32);
#endif
}
#endif /* end ifndef PRE_FPGA */

PLT_SEC_ITCM_T static INLINE void mipi_tx_drv_set_input_enable(uint8_t input_en)
{
    reg_operation_mode operation_mode = {0};

    operation_mode.u32 = g_mipi_tx_regs_va->operation_mode.u32;
    if (input_en == 0) {
        operation_mode.u32 = 0;
        g_mipi_tx_regs_va->operation_mode.u32 = operation_mode.u32;
        return;
    }

    operation_mode.bits.mem_ck_en = 1;
    operation_mode.bits.input_en = 1;
    operation_mode.bits.hss_abnormal_rst = 1;
    operation_mode.bits.read_empty_vsync_en = 1;
    g_mipi_tx_regs_va->operation_mode.u32 = operation_mode.u32;
}

PLT_SEC_ITCM_T static INLINE void mipi_tx_drv_wait_phy_ready(const short lane_id[], uint32_t lane_id_len)
{
    uint32_t lane_num;
    uint32_t wait_count = 0;
    uint32_t data_ready = 0;
    uint32_t clk_ready = 0;

    lane_num = mipi_tx_get_lane_num(lane_id, lane_id_len);

    while (wait_count <= MIPI_TX_PHY_READY_TIMEOUT_CNT) {
        clk_ready = (g_mipi_tx_regs_va->phy_status.bits.phy_pll_lock &
            g_mipi_tx_regs_va->phy_status.bits.stopstateclklane);

        if (lane_num == 1) {
            if (lane_id[0] != MIPI_TX_DISABLE_LANE_ID) {
                data_ready = g_mipi_tx_regs_va->phy_status.bits.stopstate0lane;
            } else if (lane_id[1] != MIPI_TX_DISABLE_LANE_ID) {
                data_ready = g_mipi_tx_regs_va->phy_status.bits.stopstate1lane;
            }
        } else if (lane_num == 2) { /* 2: data_lane number */
            data_ready = (g_mipi_tx_regs_va->phy_status.bits.stopstate0lane &
                g_mipi_tx_regs_va->phy_status.bits.stopstate1lane);
        }

        if ((clk_ready & data_ready) != 0) {
            break;
        }

        uapi_tcxo_delay_us(5); /* delay 5 us */
        wait_count++;
    };

    if (wait_count > MIPI_TX_PHY_READY_TIMEOUT_CNT) {
        mipi_tx_err("timeout when wait phy ready \r\n");
    }
}

PLT_SEC_ITCM_T INLINE void mipi_tx_hal_set_request_hsclk_en(uint8_t clk_en)
{
    reg_clklane_ctrl clk_lane_ctrl;

    clk_lane_ctrl.u32 = g_mipi_tx_regs_va->clklane_ctrl.u32;
    clk_lane_ctrl.bits.txrequesthsclk = clk_en;
    g_mipi_tx_regs_va->clklane_ctrl.u32 = clk_lane_ctrl.u32;
}

PLT_SEC_ITCM_T INLINE void mipi_tx_hal_set_clklane_continue_en(uint8_t clk_en)
{
    reg_clklane_ctrl clk_lane_ctrl;

    clk_lane_ctrl.u32 = g_mipi_tx_regs_va->clklane_ctrl.u32;
    clk_lane_ctrl.bits.clklane_continue = clk_en;
    g_mipi_tx_regs_va->clklane_ctrl.u32 = clk_lane_ctrl.u32;
}

td_bool mipi_tx_hal_is_pld_err(void)
{
    reg_int1_status int1_status;
    int1_status.u32 = g_mipi_tx_regs_va->int1_status.u32;
    td_bool is_err = (((int1_status.u32 >> 0x5) & 0x3) != 0); // bit5 pld_wr_err bit6 pld_rd_err
    return is_err;
}

PLT_SEC_ITCM_T STATIC void mipi_tx_hal_set_sdlp_enter_ovr_cfg(void)
{
    volatile reg_mipitx_sdlp_cfg0 mipitx_sdlp_cfg0;

    mipitx_sdlp_cfg0.u32 = g_mipi_tx_regs_va->mipitx_sdlp_cfg0.u32;
    mipitx_sdlp_cfg0.bits.sdlp_crstn_ovr = 1;
    mipitx_sdlp_cfg0.bits.sdlp_crstn_ovr_en = 1;
    mipitx_sdlp_cfg0.bits.sdlp_ref_chen_ovr = 1;
    mipitx_sdlp_cfg0.bits.sdlp_ref_chen_ovr_en = 1;
    g_mipi_tx_regs_va->mipitx_sdlp_cfg0.u32 = mipitx_sdlp_cfg0.u32;
}

PLT_SEC_ITCM_T void mipi_tx_drv_set_phy_cfg(const combo_dev_cfg_t *dev_cfg)
{
#ifdef PRE_FPGA
    mipi_tx_fpga_phy_cfg();
#else
    uint32_t actual_data_rate = mipi_tx_get_actual_phy_data_rate();
    mipi_tx_phy_timing_parameters tp = {0};
    /* set phy pll parameters setx to generate data rate */
    mipi_tx_drv_set_phy_pll_setx(actual_data_rate);
    /* get global operation timing parameters */
    mipi_tx_drv_get_phy_timing_parameters(&tp);
    /* set global operation timing parameters */
    mipi_tx_drv_set_phy_timing_parameters(&tp);
    /* dsi controller set hs switch to lp and lp switch to hs time */
    mipi_tx_drv_set_phy_hs_lp_switch_time(&tp);
#endif
    /* pwr_up unreset */
    g_mipi_tx_regs_va->ctrl_reset.u32 = 0x0;
    g_mipi_tx_regs_va->phy_ctrl.u32 = 0x0;

    /*
     * phy:
     * forcepll: pll enable
     * enableclk: enable clk lane
     * rstz: unreset
     * shutdownz: powerup
     */
    g_mipi_tx_regs_va->phy_ctrl.u32 = 0xF;
    g_mipi_tx_regs_va->ctrl_reset.u32 = 0x1;

    /* wait phy ready */
    mipi_tx_drv_wait_phy_ready(dev_cfg->lane_id, LANE_MAX_NUM);

    /* clk continue enable */
    mipi_tx_hal_set_request_hsclk_en(1);
    if (dev_cfg->clk_continue == CLK_CONTINUE) {
        mipi_tx_hal_set_clklane_continue_en(0);
    } else {
        mipi_tx_hal_set_clklane_continue_en(1);
    }
}

PLT_SEC_ITCM_T static INLINE void set_output_format(const combo_dev_cfg_t *dev_cfg)
{
    uint32_t color_coding = 0;

    if (dev_cfg->out_format == OUT_FORMAT_RGB_16BIT) {
        color_coding = MIPI_TX_DSI_RGB_16BIT;
    } else if (dev_cfg->out_format == OUT_FORMAT_RGB_18BIT) {
        color_coding = MIPI_TX_DSI_RGB_18BIT;
    } else if (dev_cfg->out_format == OUT_FORMAT_RGB_18BIT_LOOSELY) {
        color_coding = MIPI_TX_DSI_RGB_18BIT_LOOSELY;
    } else if (dev_cfg->out_format == OUT_FORMAT_RGB_24BIT) {
        color_coding = MIPI_TX_DSI_RGB_24BIT;
    } else if (dev_cfg->out_format == OUT_FORMAT_YUV420_12BIT) {
        color_coding = MIPI_TX_DSI_YCBCR420_12BIT;
    } else if (dev_cfg->out_format == OUT_FORMAT_YUV422_16BIT) {
        color_coding = MIPI_TX_DSI_YCBCR422_16BIT;
    } else if (dev_cfg->out_format == OUT_FORMAT_RGB_30BIT) {
        color_coding = MIPI_TX_DSI_RGB_30BIT;
    }

    g_mipi_tx_regs_va->data_type.u32 = (uint32_t)color_coding;
#ifdef MIPI_TX_DEBUG
    mipi_tx_print("set_output_format: 0x%x\r\n", color_coding);
#endif
}

void mipi_tx_hal_set_cmd_video_mode(cmd_video_mode cmd_video)
{
    volatile reg_mode_cfg mode_cfg;
    mode_cfg.u32 = g_mipi_tx_regs_va->mode_cfg.u32;
    mode_cfg.bits.cmd_video_mode = cmd_video;
    g_mipi_tx_regs_va->mode_cfg.u32 = mode_cfg.u32;
}

static void mipi_tx_hal_set_lp_cmd_en(uint32_t lp_cmd_en)
{
    volatile reg_mode_cfg mode_cfg;

    mode_cfg.u32 = g_mipi_tx_regs_va->mode_cfg.u32;
    mode_cfg.bits.lp_cmd_en = lp_cmd_en;
    g_mipi_tx_regs_va->mode_cfg.u32 = mode_cfg.u32;
}

#ifdef GPU_SUPPORT_VIDEO_MODE
static void mipi_tx_hal_set_video_mode_type(uint32_t video_mode_type)
{
    volatile reg_mode_cfg mode_cfg;

    mode_cfg.u32 = g_mipi_tx_regs_va->mode_cfg.u32;
    mode_cfg.bits.video_mode_type = video_mode_type;
    g_mipi_tx_regs_va->mode_cfg.u32 = mode_cfg.u32;
}

static INLINE void set_video_mode_cfg(const combo_dev_cfg_t *dev_cfg)
{
    uint32_t video_mode_type;
    volatile reg_read_memory_delay_ctrl read_memory_delay_ctrl;

    if (dev_cfg->video_mode == NON_BURST_MODE_SYNC_PULSES) {
        video_mode_type = 0; /* 0: Non-burst with sync pulse */
    } else if (dev_cfg->video_mode == NON_BURST_MODE_SYNC_EVENTS) {
        video_mode_type = 1; /* 1: Non-burst with sync */
    } else {
        video_mode_type = 2; /* 2: burst mode */
    }

    if (dev_cfg->out_mode == OUT_MODE_DSI_CMD) {
        video_mode_type = 2; /* 2: burst mode */
    }

    /*
     * mode config
     * other: burst mode
     * 00: Non-burst with sync pulse
     * 01: Non-burst with sync
     */
    mipi_tx_hal_set_video_mode_type(video_mode_type);

    g_mipi_tx_regs_va->video_lp_en.u32 = 0x3f;

    /* delay value setting. */
    read_memory_delay_ctrl.u32 = g_mipi_tx_regs_va->read_memory_delay_ctrl.u32;
    if ((dev_cfg->video_mode == NON_BURST_MODE_SYNC_PULSES) ||
        (dev_cfg->video_mode == NON_BURST_MODE_SYNC_EVENTS)) {
        read_memory_delay_ctrl.bits.delay_abnormal = 0x1;
    } else if (dev_cfg->video_mode == BURST_MODE) {
        read_memory_delay_ctrl.bits.delay_abnormal = 0x0;
    }
    g_mipi_tx_regs_va->read_memory_delay_ctrl.u32 = read_memory_delay_ctrl.u32;
}
#endif

PLT_SEC_ITCM_T static INLINE void set_timing_config(const combo_dev_cfg_t *dev_cfg)
{
    uint32_t hsa_time;
    uint32_t hbp_time;
    uint32_t hline_time;
    uint32_t actual_phy_data_rate = mipi_tx_get_actual_phy_data_rate();
    uint32_t htime_tmp = 1000 / 8; /* 1000, 8: 1000Hz / 8bit */

    if (dev_cfg->pixel_clk == 0) {
        mipi_tx_err("dev_cfg->pixel_clk is 0, illegal.\r\n");
        return;
    }

    hsa_time = (actual_phy_data_rate * dev_cfg->sync_info.hsa_pixels * htime_tmp + (dev_cfg->pixel_clk - 1)) /
        dev_cfg->pixel_clk;
    hbp_time = (actual_phy_data_rate * dev_cfg->sync_info.hbp_pixels  * htime_tmp  + (dev_cfg->pixel_clk - 1)) /
        dev_cfg->pixel_clk;
    hline_time = (actual_phy_data_rate * (dev_cfg->sync_info.hsa_pixels +
        dev_cfg->sync_info.hact_pixels +
        dev_cfg->sync_info.hbp_pixels +
        dev_cfg->sync_info.hfp_pixels) * htime_tmp + (dev_cfg->pixel_clk - 1)) / dev_cfg->pixel_clk;

    g_mipi_tx_regs_va->command_pkt_size.u32 = dev_cfg->sync_info.hact_pixels;
    g_mipi_tx_regs_va->vid_hsa_time.u32 = hsa_time;
    g_mipi_tx_regs_va->vid_hbp_time.u32 = hbp_time;
    g_mipi_tx_regs_va->vid_hline_time.u32 = hline_time;

    g_mipi_tx_regs_va->vid_vsa_lines.u32 = dev_cfg->sync_info.vsa_lines;
    g_mipi_tx_regs_va->vid_vbp_lines.u32 = dev_cfg->sync_info.vbp_lines;
    g_mipi_tx_regs_va->vid_vfp_lines.u32 = dev_cfg->sync_info.vfp_lines;
    g_mipi_tx_regs_va->vid_vactive_lines.u32 = dev_cfg->sync_info.vact_lines;

    /* read cmd hsclk */
    g_mipi_tx_regs_va->read_cmd_time.u32 = hline_time - hsa_time - 4 - 4; /* 4 4:eotp time; redundancy time */
#ifdef MIPI_TX_DEBUG
    mipi_tx_print("hsa_pixels(0x48): 0x%x\r\n", dev_cfg->sync_info.hsa_pixels);
    mipi_tx_print("hbp_pixels(0x4c): 0x%x\r\n", dev_cfg->sync_info.hbp_pixels);
    mipi_tx_print("hact_pixels(0x50): 0x%x\r\n", dev_cfg->sync_info.hact_pixels);
    mipi_tx_print("hfp_pixels(0x50): 0x%x\r\n", dev_cfg->sync_info.hfp_pixels);
    mipi_tx_print("vact_lines(0x54): 0x%x\r\n", dev_cfg->sync_info.vact_lines);
    mipi_tx_print("vbp_lines(0x58): 0x%x\r\n", dev_cfg->sync_info.vbp_lines);
    mipi_tx_print("vfp_lines(0x5c): 0x%x\r\n", dev_cfg->sync_info.vfp_lines);
    mipi_tx_print("vsa_lines(0x60): 0x%x\r\n", dev_cfg->sync_info.vsa_lines);
#endif
}

PLT_SEC_ITCM_T static INLINE void mipi_tx_set_lane_id(uint32_t lane_idx, short lane_id)
{
    reg_lane_id lane_id_reg;
    lane_id_reg.u32 = g_mipi_tx_regs_va->lane_id.u32;

    switch (lane_id) {
        case 0:
            lane_id_reg.bits.lane0_id = (uint32_t)lane_idx;
            break;

        case 1:
            lane_id_reg.bits.lane1_id = (uint32_t)lane_idx;
            break;

        default:
            break;
    }
    g_mipi_tx_regs_va->lane_id.u32 = lane_id_reg.u32;
}

PLT_SEC_ITCM_T static INLINE void mipi_tx_set_disable_lane_id(uint32_t lane_idx, short invalid_lane_num)
{
    reg_lane_id lane_id_reg;
    lane_id_reg.u32 = g_mipi_tx_regs_va->lane_id.u32;

    switch (invalid_lane_num) {
        case 1:
            lane_id_reg.bits.lane1_id = (uint32_t)lane_idx;
            break;

        default:
            break;
    }

    g_mipi_tx_regs_va->lane_id.u32 = lane_id_reg.u32;
}

PLT_SEC_ITCM_T static INLINE void set_lane_config(const short lane_id[], uint32_t lane_id_len)
{
    uint32_t max_lane_num = LANE_MAX_NUM;
    uint32_t i;
    uint32_t invalid_lane_num = 0;
    uint32_t unused_id = 3; /* 3, for unused lane */
#ifdef MIPI_TX_DEBUG
    reg_lane_id lane_id_reg;
#endif
    uint32_t lane_num = mipi_tx_get_lane_num(lane_id, lane_id_len);
    if (lane_num == 0) {
        mipi_tx_err("lane num is 0, all lane id is -1\n");
        return;
    }
    g_mipi_tx_regs_va->lane_num.u32  = lane_num - 1;
    for (i = 0; i < max_lane_num; i++) {
        if (lane_id[i] == MIPI_TX_DISABLE_LANE_ID) {
            invalid_lane_num++;
            mipi_tx_set_disable_lane_id(unused_id, (short)invalid_lane_num);
            continue;
        }
        mipi_tx_set_lane_id(i, lane_id[i]);
    }

#ifdef MIPI_TX_DEBUG
    lane_id_reg.u32 = g_mipi_tx_regs_va->lane_id.u32;
    mipi_tx_print("LANE_ID: 0x%x\r\n", lane_id_reg.u32);
#endif
}

PLT_SEC_ITCM_T static void mipi_tx_drv_phy_reset(void)
{
    reg_phy_ctrl phy_ctrl;

    phy_ctrl.u32 = g_mipi_tx_regs_va->phy_ctrl.u32;
    phy_ctrl.bits.rstz = 0x0;
    g_mipi_tx_regs_va->phy_ctrl.u32 = phy_ctrl.u32;
}

PLT_SEC_ITCM_T static void mipi_tx_drv_phy_unreset(void)
{
    reg_phy_ctrl phy_ctrl;

    phy_ctrl.u32 = g_mipi_tx_regs_va->phy_ctrl.u32;
    phy_ctrl.bits.rstz = 0x1;
    g_mipi_tx_regs_va->phy_ctrl.u32 = phy_ctrl.u32;
}

PLT_SEC_ITCM_T static INLINE void mipi_tx_drv_set_clkmgr_cfg(void)
{
    uint32_t actual_phy_data_rate;

    actual_phy_data_rate = mipi_tx_get_actual_phy_data_rate();
    /* set lp_clk frequence to (0, 20]MHz */
    if (actual_phy_data_rate / 160 < 2) { /* 160: half of 320, 2: half */
        g_mipi_tx_regs_va->crg_cfg.u32 = 0x102; /* 258 */
    } else {
        g_mipi_tx_regs_va->crg_cfg.u32 = 0x100 +
            (actual_phy_data_rate + 160) / 160; /* align to 160 */
    }
#ifdef MIPI_TX_DEBUG
    mipi_tx_print("crg_cfg: 0x%x\r\n", g_mipi_tx_regs_va->crg_cfg.u32);
#endif
}

PLT_SEC_ITCM_T static INLINE void mipi_tx_drv_set_controller_cmd_tran_mode(uint8_t low_power_en)
{
    reg_command_tran_mode command_tran_mode;

    command_tran_mode.u32 = g_mipi_tx_regs_va->command_tran_mode.u32;
    command_tran_mode.bits.max_rd_pkt_size = low_power_en;
    command_tran_mode.bits.gen_sw_0p = low_power_en;
    command_tran_mode.bits.gen_sw_1p = low_power_en;
    command_tran_mode.bits.gen_sw_2p = low_power_en;
    command_tran_mode.bits.gen_sr_0p = low_power_en;
    command_tran_mode.bits.gen_sr_1p = low_power_en;
    command_tran_mode.bits.gen_sr_2p = low_power_en;
    command_tran_mode.bits.gen_lw = low_power_en;
    command_tran_mode.bits.dcs_sw_0p = low_power_en;
    command_tran_mode.bits.dcs_sw_1p = low_power_en;
    command_tran_mode.bits.dcs_sr_0p = low_power_en;
    command_tran_mode.bits.dcs_lw = MIPI_TX_WORK_MODE_HS;
    g_mipi_tx_regs_va->command_tran_mode.u32 = command_tran_mode.u32;
}

static void mipi_tx_drv_set_cmd_tran_mode(uint8_t low_power_en, uint16_t data_type)
{
    volatile reg_command_tran_mode command_tran_mode;

    command_tran_mode.u32 = g_mipi_tx_regs_va->command_tran_mode.u32;
    switch (data_type) {
        case MAX_READ_PKT_LEN:
            command_tran_mode.bits.max_rd_pkt_size = low_power_en;
            break;
        case GEN_SHORT_WRITE_0P:
            command_tran_mode.bits.gen_sw_0p = low_power_en;
            break;
        case GEN_SHORT_WRITE_1P:
            command_tran_mode.bits.gen_sw_1p = low_power_en;
            break;
        case GEN_SHORT_WRITE_2P:
            command_tran_mode.bits.gen_sw_2p = low_power_en;
            break;
        case GEN_SHORT_READ_0P:
            command_tran_mode.bits.gen_sr_0p = low_power_en;
            break;
        case GEN_SHORT_READ_1P:
            command_tran_mode.bits.gen_sr_1p = low_power_en;
            break;
        case GEN_SHORT_READ_2P:
            command_tran_mode.bits.gen_sr_2p = low_power_en;
            break;
        case GEN_LONG_WRITE:
            command_tran_mode.bits.gen_lw = low_power_en;
            break;
        case DCS_SHORT_WRITE_0P:
            command_tran_mode.bits.dcs_sw_0p = low_power_en;
            break;
        case DCS_SHORT_WRITE_1P:
            command_tran_mode.bits.dcs_sw_1p = low_power_en;
            break;
        case DCS_SHORT_READ_0P:
            command_tran_mode.bits.dcs_sr_0p = low_power_en;
            break;
        case DCS_LONG_WRITE:
            command_tran_mode.bits.dcs_lw = low_power_en;
            break;
        default:
            mipi_tx_err("invalid data type!\r\n");
            break;
    }

    g_mipi_tx_regs_va->command_tran_mode.u32 = command_tran_mode.u32;
}

PLT_SEC_ITCM_T static INLINE void mipi_tx_drv_set_controller_cmd_datatype(const combo_dev_cfg_t *dev_cfg)
{
    UNUSED(dev_cfg);
    g_mipi_tx_regs_va->datatype0.u32 = 0x111213D;
    g_mipi_tx_regs_va->datatype1.u32 = 0x31081909;
    g_mipi_tx_regs_va->csi_ctrl.u32 = 0x10100;
}

PLT_SEC_ITCM_T uint32_t mipi_tx_align_up(uint32_t x, uint32_t a)
{
    return ((((x) + ((a) - 1)) / (a)) * (a));
}

PLT_SEC_ITCM_T static INLINE void mipi_tx_cal_actual_phy_data_rate(uint32_t input_data_rate)
{
    uint32_t data_rate;
    unused(data_rate);
    uint32_t actual_data_rate;
    data_rate = input_data_rate; /* data_rate * 1.2 for datatype, ECC, Checksum, lp~hs turn */

#ifdef PRE_FPGA
    actual_data_rate = input_data_rate;
#else
    actual_data_rate = mipi_tx_align_up(data_rate, MIPI_TX_REF_CLK);  /* 实际数据率要求参考时钟（MHz）向上对齐 */
#endif /* end of #ifdef PRE_FPGA */
    actual_data_rate = MIN(MIPI_TX_MAX_DATA_RATE, actual_data_rate); /* 最大支持1000M */
    mipi_tx_set_actual_phy_data_rate(actual_data_rate);
}

PLT_SEC_ITCM_T static INLINE void mipi_tx_hal_set_controller_peck_en(const pck_en_t *pck_en_cfg)
{
    reg_pck_cfg pck_cfg;
    /* crc,ecc,eotp tran */
    g_mipi_tx_regs_va->pck_cfg.u32 = 0x1c; /* dsi */

    pck_cfg.u32 = g_mipi_tx_regs_va->pck_cfg.u32;
    pck_cfg.bits.eotp_tx_en = pck_en_cfg->eotp_tx_en;
    pck_cfg.bits.eotp_rx_en = pck_en_cfg->eotp_rx_en;
    g_mipi_tx_regs_va->pck_cfg.u32 = pck_cfg.u32;
}

PLT_SEC_ITCM_T static INLINE void mipi_tx_hal_set_lanebyte_pctrl(uint32_t mipitx_lanebyte_pctrl)
{
    reg_mipitx_crg_ctrl_cfg mipitx_crg_ctrl_cfg;

    mipitx_crg_ctrl_cfg.u32 = g_mipi_tx_regs_va->mipitx_crg_ctrl_cfg.u32;
    mipitx_crg_ctrl_cfg.bits.mipitx_lanebyte_pctrl = mipitx_lanebyte_pctrl;
    g_mipi_tx_regs_va->mipitx_crg_ctrl_cfg.u32 = mipitx_crg_ctrl_cfg.u32;
}

PLT_SEC_ITCM_T void mipi_tx_drv_set_controller_cfg(const combo_dev_cfg_t *dev_cfg)
{
    /* OPERATION_MODE, disable input */
    mipi_tx_drv_set_input_enable(0);

    /* output format */
    set_output_format(dev_cfg);

    /* txescclk,timeout */
    mipi_tx_cal_actual_phy_data_rate(dev_cfg->phy_data_rate);
    mipi_tx_drv_set_clkmgr_cfg();

    /* cmd transmission mode */
    mipi_tx_drv_set_controller_cmd_tran_mode(MIPI_TX_WORK_MODE_LP);

    /* crc,ecc,eotp tran */
    mipi_tx_hal_set_controller_peck_en(&(dev_cfg->pck_en));

#ifdef GPU_SUPPORT_VIDEO_MODE
    /* video mode cfg */
    set_video_mode_cfg(dev_cfg);
    if (dev_cfg->out_mode == OUT_MODE_DSI_VIDEO) {
        g_mipi_tx_regs_va->video_pkt_size.u32 = dev_cfg->sync_info.hact_pixels;
    } else {
        g_mipi_tx_regs_va->command_pkt_size.u32 = dev_cfg->sync_info.hact_pixels;
    }

    /* num_chunks/null_size */
    g_mipi_tx_regs_va->vid_num_chunks.u32 = 0x0;
    g_mipi_tx_regs_va->vid_null_size.u32 = 0x0;
#endif

    /* timing config */
    set_timing_config(dev_cfg);

    /* set data type */
    mipi_tx_drv_set_controller_cmd_datatype(dev_cfg);

    /* lanes num and id */
    set_lane_config(dev_cfg->lane_id, LANE_MAX_NUM);

    mipi_tx_hal_set_lanebyte_pctrl(LANEBYTE_PCTRL);

    mipi_tx_hal_set_sdlp_enter_ovr_cfg();

    mipi_tx_hal_set_dfx_pkt_cnt_en(TD_TRUE);
}

static uint32_t mipi_tx_wait_cmd_fifo_empty(void)
{
    reg_command_status cmd_pkt_status;
    uint32_t wait_count;

    wait_count = 0;
    do {
        cmd_pkt_status.u32 = g_mipi_tx_regs_va->command_status.u32;

        wait_count++;

        uapi_tcxo_delay_us(1);

        if (wait_count > MIPI_TX_READ_TIMEOUT_CNT) {
            mipi_tx_err("timeout when send cmd buffer \r\n");
            return EXT_ERR_MIPI_FIFO_EMPTY;
        }
    } while (cmd_pkt_status.bits.command_empty == 0);

    return EXT_ERR_SUCCESS;
}

static uint32_t mipi_tx_wait_write_fifo_empty(void)
{
    reg_command_status cmd_pkt_status;
    uint32_t wait_count;

    wait_count = 0;
    do {
        cmd_pkt_status.u32 = g_mipi_tx_regs_va->command_status.u32;

        wait_count++;

        uapi_tcxo_delay_us(1);

        if (wait_count > MIPI_TX_READ_TIMEOUT_CNT) {
            mipi_tx_err("timeout when send data buffer \r\n");
            return EXT_ERR_MIPI_FIFO_EMPTY;
        }
    } while (cmd_pkt_status.bits.pld_write_empty == 0);

    return EXT_ERR_SUCCESS;
}

static uint32_t mipi_tx_wait_write_fifo_not_full(void)
{
    reg_command_status cmd_pkt_status;
    uint32_t wait_count;

    wait_count = 0;
    do {
        cmd_pkt_status.u32 = g_mipi_tx_regs_va->command_status.u32;
        if (wait_count > 0) {
            uapi_tcxo_delay_us(1);
            mipi_tx_err("write fifo full happened wait count = %u\r\n", wait_count);
        }
        if (wait_count > MIPI_TX_READ_TIMEOUT_CNT) {
            mipi_tx_err("timeout when wait write fifo not full buffer \r\n");
            return EXT_ERR_MIPI_FIFO_FULL;
        }
        wait_count++;
    } while (cmd_pkt_status.bits.pld_write_full == 1);

    return EXT_ERR_SUCCESS;
}

/*
 * set payloads data by writing register
 * each 4 bytes in cmd corresponds to one register
 */
static void mipi_tx_drv_set_payload_data(const uint8_t *cmd, uint16_t cmd_size)
{
    reg_gen_pld_data gen_pld_data;
    uint32_t i, j;

    gen_pld_data.u32 = g_mipi_tx_regs_va->gen_pld_data.u32;

    for (i = 0; i < (cmd_size / MIPI_TX_BYTES_PER_WORD); i++) {
        gen_pld_data.bits.command_pld_b1 = cmd[i * MIPI_TX_BYTES_PER_WORD + MIPI_TX_CMD_BYTE1];
        gen_pld_data.bits.command_pld_b2 = cmd[i * MIPI_TX_BYTES_PER_WORD + MIPI_TX_CMD_BYTE2];
        gen_pld_data.bits.command_pld_b3 = cmd[i * MIPI_TX_BYTES_PER_WORD + MIPI_TX_CMD_BYTE3];
        gen_pld_data.bits.command_pld_b4 = cmd[i * MIPI_TX_BYTES_PER_WORD + MIPI_TX_CMD_BYTE4];

        mipi_tx_wait_write_fifo_not_full();
        g_mipi_tx_regs_va->gen_pld_data.u32 = gen_pld_data.u32;
    }

    gen_pld_data.u32 = 0;
    j = cmd_size % MIPI_TX_BYTES_PER_WORD;
    if (j != 0) {
        if (j > 0) {
            gen_pld_data.bits.command_pld_b1 = cmd[i * MIPI_TX_BYTES_PER_WORD + MIPI_TX_CMD_BYTE1];
        }
        if (j > 1) {
            gen_pld_data.bits.command_pld_b2 = cmd[i * MIPI_TX_BYTES_PER_WORD + MIPI_TX_CMD_BYTE2];
        }
        if (j > 2) { /* 2 bytes */
            gen_pld_data.bits.command_pld_b3 = cmd[i * MIPI_TX_BYTES_PER_WORD + MIPI_TX_CMD_BYTE3];
        }

        mipi_tx_wait_write_fifo_not_full();
        g_mipi_tx_regs_va->gen_pld_data.u32 = gen_pld_data.u32;
    }

#ifdef MIPI_TX_DEBUG
        mipi_tx_print("\r\n=====set cmd=======\r\n");
        mipi_tx_print("gen_pld_data(0x70): 0x%x\r\n", gen_pld_data.u32);
#endif
}

static void mipi_tx_drv_set_cmd_work_param(mipi_tx_work_mode_t work_mode, uint16_t data_type)
{
    reg_mode_cfg mode_cfg;
    mode_cfg.u32 = g_mipi_tx_regs_va->mode_cfg.u32;

    if (mode_cfg.bits.cmd_video_mode == 1) { /* command mode */
        /* set command work mode */
        mipi_tx_drv_set_cmd_tran_mode(work_mode, data_type);
    } else {
        /* set video work mode */
        mipi_tx_hal_set_lp_cmd_en(work_mode);
    }
}

static void mipi_tx_drv_set_command_header(uint16_t data_type, uint16_t data_param)
{
    volatile reg_gen_hdr gen_hdr;

    gen_hdr.u32 = g_mipi_tx_regs_va->gen_hdr.u32;
    gen_hdr.bits.command_virtualchannel = 0; /* always 0 */
    gen_hdr.bits.command_datatype = data_type;
    gen_hdr.bits.command_wordcount_low = get_low_byte(data_param);
    gen_hdr.bits.command_wordcount_hi = get_high_byte(data_param);
    g_mipi_tx_regs_va->gen_hdr.u32 = gen_hdr.u32;
#ifdef MIPI_TX_DEBUG
    mipi_tx_print("\r\n=====set cmd=======\r\n");
    mipi_tx_print("gen_hdr(0x6C): 0x%x\r\n", gen_hdr.u32);
#endif
}

uint32_t mipi_tx_drv_set_cmd_info(const cmd_info_t *cmd_info)
{
    /* set work mode and clk lane en */
    mipi_tx_drv_set_cmd_work_param(cmd_info->work_mode, cmd_info->data_type);

    if (cmd_info->cmd != NULL) {
        uint8_t cmd[MIPI_TX_SET_DATA_SIZE] = {0};
        if (memcpy_s(cmd, MIPI_TX_SET_DATA_SIZE, cmd_info->cmd, cmd_info->cmd_size) != EOK) {
            return EXT_ERR_MIPI_MEMCPY_FAIL;
        }

        mipi_tx_drv_set_payload_data(cmd, cmd_info->cmd_size);
    }

    mipi_tx_drv_set_command_header(cmd_info->data_type, cmd_info->cmd_size);

    /* wait 10us for transfer end  */
    uapi_tcxo_delay_us(10);

    mipi_tx_wait_cmd_fifo_empty();
    mipi_tx_wait_write_fifo_empty();
    /* resume work mode and clk lane en */
    if ((cmd_info->data_type) == DCS_LONG_WRITE && (cmd_info->work_mode == MIPI_TX_WORK_MODE_LP)) {
        mipi_tx_drv_set_cmd_work_param(MIPI_TX_WORK_MODE_HS, DCS_LONG_WRITE);
    }

#ifdef MIPI_TX_DEBUG
    mipi_tx_print("cmd_info->data_type: 0x%x\r\n", cmd_info->data_type);
    mipi_tx_print("cmd_info->cmd_size: 0x%x\r\n", cmd_info->cmd_size);
#endif

    return EXT_ERR_SUCCESS;
}

static uint32_t mipi_tx_wait_read_fifo_not_empty(void)
{
    reg_int0_status int_st0;
    reg_int1_status int_st1;
    uint32_t wait_count;
    reg_command_status cmd_pkt_status;

    wait_count = 0;
    do {
        int_st1.u32 =  g_mipi_tx_regs_va->int1_status.u32;
        int_st0.u32 =  g_mipi_tx_regs_va->int0_status.u32;

        /*
         * 0x0000101e:
         * to_lp_rx, rxecc_multi_err, rxecc_single_err, rxcrc_err, rxpkt_size_err
         */
        if ((int_st1.u32 & 0x0000101e) != 0) {
            mipi_tx_err("err happened when read data, int_st1 = 0x%x,int_st0 = %x\r\n", int_st1.u32, int_st0.u32);
            return EXT_ERR_MIPI_READ_FAILED;
        }

        if (wait_count > MIPI_TX_READ_TIMEOUT_CNT) {
            mipi_tx_err("timeout when read data\r\n");
            return EXT_ERR_MIPI_FIFO_EMPTY;
        }

        wait_count++;

        uapi_tcxo_delay_us(1);

        cmd_pkt_status.u32 = g_mipi_tx_regs_va->command_status.u32;
    } while (cmd_pkt_status.bits.pld_read_empty == 0x1);

    return EXT_ERR_SUCCESS;
}

static uint32_t mipi_tx_wait_read_fifo_empty(void)
{
    reg_gen_pld_data pld_data;
    reg_int1_status int_st1;
    uint32_t wait_count;

    wait_count = 0;
    do {
        int_st1.u32 = g_mipi_tx_regs_va->int1_status.u32;
        if ((int_st1.bits.rxpld_rd_err) == 0x0) {
            pld_data.u32 = g_mipi_tx_regs_va->gen_pld_data.u32;
        }
        wait_count++;
        uapi_tcxo_delay_us(1);
        if (wait_count > MIPI_TX_READ_TIMEOUT_CNT) {
            mipi_tx_err("timeout when clear cmd data buffer, the last read data is 0x%x\r\n", pld_data.u32);
            return EXT_ERR_MIPI_FIFO_EMPTY;
        }
    } while ((int_st1.bits.rxpld_rd_err) == 0x0);

    return EXT_ERR_SUCCESS;
}

static uint32_t mipi_tx_send_short_packet(uint16_t data_type, uint16_t data_param)
{
    mipi_tx_drv_set_command_header(data_type, data_param);

    if (mipi_tx_wait_cmd_fifo_empty() != EXT_ERR_SUCCESS) {
        return EXT_ERR_MIPI_FIFO_EMPTY;
    }

    return EXT_ERR_SUCCESS;
}

static uint32_t mipi_tx_get_read_fifo_data(uint32_t get_data_size, uint8_t *data_buf)
{
    reg_gen_pld_data pld_data;
    uint32_t i, j;

    for (i = 0; i < get_data_size / MIPI_TX_BYTES_PER_WORD; i++) {
        if (mipi_tx_wait_read_fifo_not_empty() != EXT_ERR_SUCCESS) {
            return EXT_ERR_MIPI_FIFO_EMPTY;
        }
        pld_data.u32 = g_mipi_tx_regs_va->gen_pld_data.u32;
        data_buf[i * MIPI_TX_BYTES_PER_WORD + MIPI_TX_CMD_BYTE1] = pld_data.bits.command_pld_b1;
        data_buf[i * MIPI_TX_BYTES_PER_WORD + MIPI_TX_CMD_BYTE2] = pld_data.bits.command_pld_b2;
        data_buf[i * MIPI_TX_BYTES_PER_WORD + MIPI_TX_CMD_BYTE3] = pld_data.bits.command_pld_b3;
        data_buf[i * MIPI_TX_BYTES_PER_WORD + MIPI_TX_CMD_BYTE4] = pld_data.bits.command_pld_b4;
    }

    j = get_data_size % MIPI_TX_BYTES_PER_WORD;

    if (j != 0) {
        if (mipi_tx_wait_read_fifo_not_empty() != EXT_ERR_SUCCESS) {
            return EXT_ERR_MIPI_FIFO_EMPTY;
        }
        pld_data.u32 = g_mipi_tx_regs_va->gen_pld_data.u32;
        if (j > 0) {
            data_buf[i * MIPI_TX_BYTES_PER_WORD + MIPI_TX_CMD_BYTE1] = pld_data.bits.command_pld_b1;
        }
        if (j > 1) {
            data_buf[i * MIPI_TX_BYTES_PER_WORD + MIPI_TX_CMD_BYTE2] = pld_data.bits.command_pld_b2;
        }
        if (j > 2) { /* 2 bytes */
            data_buf[i * MIPI_TX_BYTES_PER_WORD + MIPI_TX_CMD_BYTE3] = pld_data.bits.command_pld_b3;
        }
    }

    return EXT_ERR_SUCCESS;
}

PLT_SEC_ITCM_T void mipi_tx_reset(void)
{
    g_mipi_tx_regs_va->ctrl_reset.u32 = 0x0;
    mipi_tx_drv_phy_reset();

    g_mipi_tx_regs_va->ctrl_reset.u32 = 0x1;
    mipi_tx_drv_phy_unreset();
}

uint32_t mipi_tx_drv_get_cmd_info(get_cmd_info_t *get_cmd_info)
{
    if (mipi_tx_wait_read_fifo_empty() != EXT_ERR_SUCCESS) {
        return EXT_ERR_MIPI_FIFO_EMPTY;
    }

    /* set work mode and clk lane en */
    mipi_tx_drv_set_cmd_work_param(get_cmd_info->work_mode, get_cmd_info->data_type);

    if (mipi_tx_send_short_packet(get_cmd_info->data_type, get_cmd_info->data_param) != EXT_ERR_SUCCESS) {
        mipi_tx_err("mipi_tx_drv_get_cmd_info: short packet send error\r\n");
        return EXT_ERR_MIPI_SEND_SHORT_PACKET;
    }

    uint8_t data_buf[MIPI_TX_GET_DATA_SIZE] = {0};
    if (mipi_tx_get_read_fifo_data(get_cmd_info->get_data_size, data_buf) != EXT_ERR_SUCCESS) {
        /* fail will block mipi data lane ,so need reset */
        mipi_tx_err("mipi_tx_drv_get_cmd_info: fifo read error\r\n");
        mipi_tx_reset();
        return EXT_ERR_MIPI_READ_FIFO_DATA;
    }

    (void)memcpy_s(get_cmd_info->get_data, get_cmd_info->get_data_size, data_buf, get_cmd_info->get_data_size);

    return EXT_ERR_SUCCESS;
}

PLT_SEC_ITCM_T void mipi_tx_drv_enable_input(const combo_dev_cfg_t *dev_cfg)
{
    if (dev_cfg->out_mode == OUT_MODE_DSI_VIDEO) {
        mipi_tx_hal_set_cmd_video_mode(VIDEO_MODE);
    }

    /* enable input */
    mipi_tx_drv_set_input_enable(1);

    /* reset mipi_tx */
    mipi_tx_reset();
    mipi_tx_hal_set_request_hsclk_en(1);
    if (dev_cfg->clk_continue == CLK_CONTINUE) {
        mipi_tx_hal_set_clklane_continue_en(0);
    } else {
        mipi_tx_hal_set_clklane_continue_en(1);
    }

#if MIPI_TX_INTERRUPTS_ENABLE
    /* enable int */
    mipi_tx_enable_int(0x1);
#endif
}

uint32_t mipi_tx_drv_disable_input(void)
{
    volatile reg_operation_mode operation_mode;

    if (g_mipi_tx_regs_va == TD_NULL) {
        mipi_tx_err("mipi_tx regs is null!");
        return EXT_ERR_FAILURE;
    }

    /* disable int */
#if MIPI_TX_INTERRUPTS_ENABLE
    mipi_tx_enable_int(0x0);
#endif

    /* set to disable input */
    operation_mode.u32 = g_mipi_tx_regs_va->operation_mode.u32;
    operation_mode.bits.input_en = 0x0;
    g_mipi_tx_regs_va->operation_mode.u32 = operation_mode.u32;

    /* set to command mode */
    mipi_tx_hal_set_cmd_video_mode(COMMAND_MODE);

    /* default: clklane disable */
    mipi_tx_hal_set_request_hsclk_en(0);
    mipi_tx_hal_set_clklane_continue_en(0);

    g_mipi_tx_regs_va->ctrl_reset.u32 = 0x0;
    mipi_tx_drv_phy_reset();

    return EXT_ERR_SUCCESS;
}

void mipi_tx_hal_colorbar_mode(combo_dev_cfg_t *dev_cfg, colorbar_orien_mode orien, colorbar_input_mode mode)
{
    if (dev_cfg->out_mode == OUT_MODE_DSI_VIDEO) {
        mipi_tx_hal_set_cmd_video_mode(VIDEO_MODE);
    }

    /* enable colorbar */
    g_mipi_tx_regs_va->hfp_hact_in.u32 = ((uint32_t)(dev_cfg->sync_info.hfp_pixels) << HAL_MIPI_OFFSET_16) +
                                         dev_cfg->sync_info.hact_pixels;
    g_mipi_tx_regs_va->hbp_hsa_in.u32 = ((uint32_t)(dev_cfg->sync_info.hbp_pixels) << HAL_MIPI_OFFSET_16) +
                                        dev_cfg->sync_info.hsa_pixels;
    volatile reg_operation_mode operation_mode = {0};
    operation_mode.bits.read_empty_vsync_en = 1;
    operation_mode.bits.colorbar_en = 1;
    operation_mode.bits.colorbar_orien = orien;
    operation_mode.bits.colorbar_mode = mode;
    operation_mode.bits.cmd_colorbar_en = 1;
    g_mipi_tx_regs_va->operation_mode.u32 = operation_mode.u32;

    uapi_tcxo_delay_us(1);

#if MIPI_TX_INTERRUPTS_ENABLE
    /* enable int */
    mipi_tx_enable_int(0x1);
#endif
}

void mipi_tx_hal_unset_colorbar(const combo_dev_cfg_t *dev_cfg)
{
    /* disable int */
#if MIPI_TX_INTERRUPTS_ENABLE
    mipi_tx_enable_int(0x0);
#endif

    if (dev_cfg->out_mode == OUT_MODE_DSI_VIDEO) {
        mipi_tx_hal_set_cmd_video_mode(VIDEO_MODE);
    }

    /* disable colorbar input */
    g_mipi_tx_regs_va->hfp_hact_in.u32 = 0;
    g_mipi_tx_regs_va->hbp_hsa_in.u32 = 0;

    volatile reg_operation_mode operation_mode = {0};
    operation_mode.bits.mem_ck_en = 1;
    operation_mode.bits.input_en = 1;
    operation_mode.bits.hss_abnormal_rst = 1;
    operation_mode.bits.read_empty_vsync_en = 1;
    g_mipi_tx_regs_va->operation_mode.u32 = operation_mode.u32;
}

void mipi_tx_drv_set_partial_refresh(combo_dev_cfg_t *dev_cfg)
{
    set_timing_config(dev_cfg);
}

void mipi_tx_hal_auto_enter_sdlp(void)
{
    volatile reg_mipitx_sdlp_cfg0 mipitx_sdlp_cfg0 = {0};

    mipitx_sdlp_cfg0.u32 = g_mipi_tx_regs_va->mipitx_sdlp_cfg0.u32;

#ifdef MIPI_ULPS_SUPPORT
    volatile reg_phy_status mipitx_phy_status = {0};
    uint32_t wait_count = 0;

    mipitx_phy_status.u32 = g_mipi_tx_regs_va->phy_status.u32;

    while ((mipitx_phy_status.bits.ulpsactivenotclk == 0x1) && (wait_count < MIPI_TX_EXIT_WAIT_ULPS_TIMEOUT_CNT)) {
        wait_count++;
        uapi_tcxo_delay_us(1);
    }

    if (wait_count == MIPI_TX_EXIT_WAIT_ULPS_TIMEOUT_CNT) {
        mipi_tx_err("timeout when wait mipi_tx into ulps, 0xb0 status is 0x%x\r\n", mipitx_phy_status.u32);
    }
#endif

    mipitx_sdlp_cfg0.bits.sdlp_enter_ovr_en = 0;
    g_mipi_tx_regs_va->mipitx_sdlp_cfg0.u32 = mipitx_sdlp_cfg0.u32;
}

uint32_t mipi_tx_hal_exit_sdlp(void)
{
    volatile reg_mipitx_sdlp_cfg0 mipitx_sdlp_cfg0 = {0};
    volatile reg_ulps_sdlp_dbg ulps_sdlp_dbg = {0};
    uint32_t wait_count;

    mipitx_sdlp_cfg0.u32 = g_mipi_tx_regs_va->mipitx_sdlp_cfg0.u32;
    ulps_sdlp_dbg.u32 = g_mipi_tx_regs_va->ulps_sdlp_dbg.u32;

    mipitx_sdlp_cfg0.bits.sdlp_enter_ovr_en = 1;
    g_mipi_tx_regs_va->mipitx_sdlp_cfg0.u32 = mipitx_sdlp_cfg0.u32;

    if (ulps_sdlp_dbg.bits.mipi_ready == 1) {
        return EXT_ERR_SUCCESS;
    }

    wait_count = 0;
    do {
        ulps_sdlp_dbg.u32 = g_mipi_tx_regs_va->ulps_sdlp_dbg.u32;

        wait_count++;
        uapi_tcxo_delay_us(1);
        if (wait_count > MIPI_TX_EXIT_SDLP_TIMEOUT_CNT) {
            mipitx_sdlp_cfg0.bits.sdlp_enter_ovr_en = 0;
            g_mipi_tx_regs_va->mipitx_sdlp_cfg0.u32 = mipitx_sdlp_cfg0.u32;
            mipi_tx_err("timeout when wait mipi_tx wake from sdlp, the status is 0x%x\r\n", ulps_sdlp_dbg.u32);
            return EXT_ERR_MIPI_EXIT_SDLP_TIMEOUT;
        }
    } while ((ulps_sdlp_dbg.bits.mipi_ready) == 0x0);

    return EXT_ERR_SUCCESS;
}

void mipi_tx_hal_get_lane_state(mipi_tx_lane_id lane_num, mipi_tx_lane_state_t *status)
{
    volatile reg_ulps_sdlp_dbg ulps_sdlp_dbg;
    volatile reg_phy_status phy_status;
    ulps_sdlp_dbg.u32 = g_mipi_tx_regs_va->ulps_sdlp_dbg.u32;
    phy_status.u32 = g_mipi_tx_regs_va->phy_status.u32;

    if (lane_num == MIPI_TX_LANE_DATA0) {
        status->idle_active = phy_status.bits.stopstate0lane;
        status->exit_ulps = phy_status.bits.ulpsactivenot0lane;
    } else if (lane_num == MIPI_TX_LANE_DATA1) {
        status->idle_active = phy_status.bits.stopstate1lane;
        status->exit_ulps = phy_status.bits.ulpsactivenot1lane;
    } else {
        status->idle_active = phy_status.bits.stopstateclklane;
        status->exit_ulps = phy_status.bits.ulpsactivenotclk;
    }
    status->exit_sdlp = ulps_sdlp_dbg.bits.mipi_ready;
}

uint32_t mipi_tx_hal_get_ulps_entry_count(void)
{
    volatile reg_ulps_sdlp_dbg ulps_sdlp_dbg = {0};
    if (g_mipi_tx_regs_va != NULL) {
        ulps_sdlp_dbg.u32 = g_mipi_tx_regs_va->ulps_sdlp_dbg.u32;
        return ulps_sdlp_dbg.bits.ulps_entry_cnt;
    }

    return 0;
}

uint32_t mipi_tx_hal_get_ulps_exit_count(void)
{
    volatile reg_ulps_sdlp_dbg ulps_sdlp_dbg = {0};
    if (g_mipi_tx_regs_va != NULL) {
        ulps_sdlp_dbg.u32 = g_mipi_tx_regs_va->ulps_sdlp_dbg.u32;
        return ulps_sdlp_dbg.bits.ulps_exit_cnt;
    }

    return 0;
}

void mipi_tx_hal_set_dfx_pkt_cnt_en(td_bool en)
{
    volatile reg_dfx_cnt_en dfx_cnt_en = {0};
    if (g_mipi_tx_regs_va == NULL) {
        return;
    }
    dfx_cnt_en.u32 = g_mipi_tx_regs_va->dfx_cnt_en.u32;
    dfx_cnt_en.bits.cmd_mode_frame_cnt_en = en;
    dfx_cnt_en.bits.only_reg_cmd_en = en;
    dfx_cnt_en.bits.hs_pkt_cnt_en = en;
    dfx_cnt_en.bits.lp_pkt_cnt_en = en;
    dfx_cnt_en.bits.dpu_pkt_cnt_en = en;
    g_mipi_tx_regs_va->dfx_cnt_en.u32 = dfx_cnt_en.u32;
}

uint32_t mipi_tx_hal_dfx_get_frame_cnt(void)
{
    volatile reg_cmd_frame_cnt cmd_frame_cnt = {0};
    if (g_mipi_tx_regs_va != NULL) {
        cmd_frame_cnt.u32 = g_mipi_tx_regs_va->cmd_frame_cnt.u32;
        return cmd_frame_cnt.bits.cmd_mode_frame_cnt;
    }

    return 0;
}

uint32_t mipi_tx_hal_dfx_get_lp_pkt_cnt(void)
{
    volatile reg_pkt_cnt pkt_cnt = {0};
    if (g_mipi_tx_regs_va != NULL) {
        pkt_cnt.u32 = g_mipi_tx_regs_va->pkt_cnt.u32;
        return pkt_cnt.bits.lp_pkt_cnt;
    }
    return 0;
}

uint32_t mipi_tx_hal_dfx_get_hs_pkt_cnt(void)
{
    volatile reg_pkt_cnt pkt_cnt = {0};
    if (g_mipi_tx_regs_va != NULL) {
        pkt_cnt.u32 = g_mipi_tx_regs_va->pkt_cnt.u32;
        return pkt_cnt.bits.hs_pkt_cnt;
    }

    return 0;
}

uint32_t mipi_tx_hal_dfx_get_dpu_pkt_cnt(void)
{
    volatile reg_dpu_pkt_cnt dpu_pkt_cnt = {0};
    if (g_mipi_tx_regs_va != NULL) {
        dpu_pkt_cnt.u32 = g_mipi_tx_regs_va->dpu_pkt_cnt.u32;
        return dpu_pkt_cnt.bits.dpu_pkt_cnt;
    }

    return 0;
}

td_bool mipi_tx_hal_get_mipi2dpu_rdy(void)
{
    volatile reg_ulps_sdlp_dbg ulps_sdlp_dbg = {0};
    if (g_mipi_tx_regs_va != NULL) {
        ulps_sdlp_dbg.u32 = g_mipi_tx_regs_va->ulps_sdlp_dbg.u32;
        return ulps_sdlp_dbg.bits.mipi2dpu_rdy;
    }

    return 0;
}

PLT_SEC_ITCM_T void mipi_tx_hal_exit_ulps(void)
{
    volatile reg_auto_ulps_exit_pulse auto_ulps_exit_pulse;     /* 0xec */
    if (g_mipi_tx_regs_va != NULL) {
        auto_ulps_exit_pulse.u32 = g_mipi_tx_regs_va->auto_ulps_exit_pulse.u32;
        auto_ulps_exit_pulse.bits.ulps_exit_pulse = 0x1;
        g_mipi_tx_regs_va->auto_ulps_exit_pulse.u32 = auto_ulps_exit_pulse.u32;
    }
}

PLT_SEC_ITCM_T void mipi_tx_hal_clear_ulps_entry_cnt(void)
{
    volatile reg_auto_ulps_exit_pulse auto_ulps_exit_pulse;     /* 0xec */
    if (g_mipi_tx_regs_va != NULL) {
        auto_ulps_exit_pulse.u32 = g_mipi_tx_regs_va->auto_ulps_exit_pulse.u32;
        auto_ulps_exit_pulse.bits.ulps_entry_cnt_clr = 0x1;
        g_mipi_tx_regs_va->auto_ulps_exit_pulse.u32 = auto_ulps_exit_pulse.u32;
    }
}

PLT_SEC_ITCM_T void mipi_tx_hal_clear_exit_ulps(void)
{
    volatile reg_auto_ulps_exit_pulse auto_ulps_exit_pulse;     /* 0xec */
    if (g_mipi_tx_regs_va != NULL) {
        auto_ulps_exit_pulse.u32 = g_mipi_tx_regs_va->auto_ulps_exit_pulse.u32;
        auto_ulps_exit_pulse.bits.ulps_exit_pulse_clr = 0x1;
        g_mipi_tx_regs_va->auto_ulps_exit_pulse.u32 = auto_ulps_exit_pulse.u32;
    }
}

void mipi_tx_hal_set_auto_exit_ulps_en(td_bool cfg_ulps_auto_exit_en)
{
    volatile reg_auto_ulps_mode auto_ulps_mode;     /* 0xe0 */
    if (g_mipi_tx_regs_va != NULL) {
        auto_ulps_mode.u32 = g_mipi_tx_regs_va->auto_ulps_mode.u32;
        auto_ulps_mode.bits.cfg_ulps_auto_exit_en = cfg_ulps_auto_exit_en;
        g_mipi_tx_regs_va->auto_ulps_mode.u32 = auto_ulps_mode.u32;
    }
}

void mipi_tx_hal_set_auto_ulps_entry_suspend(td_bool ulps_suspend_en)
{
    volatile reg_auto_ulps_mode auto_ulps_mode;     /* 0xe0 */
    if (g_mipi_tx_regs_va != NULL) {
        auto_ulps_mode.u32 = g_mipi_tx_regs_va->auto_ulps_mode.u32;
        auto_ulps_mode.bits.cfg_auto_ulps_entry_suspend = ulps_suspend_en;
        g_mipi_tx_regs_va->auto_ulps_mode.u32 = auto_ulps_mode.u32;
    }
}

void mipi_tx_hal_set_ulps_entry_delay(uint32_t ulps_entry_delay)
{
    volatile reg_auto_ulps_entry_delay auto_ulps_entry_delay;     /* 0xe4 */
    if (g_mipi_tx_regs_va != NULL) {
        auto_ulps_entry_delay.u32 = g_mipi_tx_regs_va->auto_ulps_entry_delay.u32;
        auto_ulps_entry_delay.bits.ulps_entry_delay = ulps_entry_delay;
        g_mipi_tx_regs_va->auto_ulps_entry_delay.u32 = auto_ulps_entry_delay.u32;
    }
}

void mipi_tx_hal_set_cfg_uflag2te_err_cnt_en(uint32_t cfg_uflag2te_err_cnt_en)
{
    volatile reg_auto_ulps_mode auto_ulps_mode;     /* 0x200 */
    if (g_mipi_tx_regs_va != NULL) {
        auto_ulps_mode.u32 = g_mipi_tx_regs_va->auto_ulps_mode.u32;
        auto_ulps_mode.bits.cfg_uflag2te_err_cnt_en = cfg_uflag2te_err_cnt_en;
        g_mipi_tx_regs_va->auto_ulps_mode.u32 = auto_ulps_mode.u32;
    }
}

void mipi_tx_hal_set_cfg_uflag2te_err_cnt_clr(uint32_t cfg_uflag2te_err_cnt_clr)
{
    volatile reg_auto_ulps_mode auto_ulps_mode;     /* 0x200 */
    if (g_mipi_tx_regs_va != NULL) {
        auto_ulps_mode.u32 = g_mipi_tx_regs_va->auto_ulps_mode.u32;
        auto_ulps_mode.bits.cfg_uflag2te_err_cnt_clr = cfg_uflag2te_err_cnt_clr;
        g_mipi_tx_regs_va->auto_ulps_mode.u32 = auto_ulps_mode.u32;
    }
}

void mipi_tx_hal_set_cfg_uflag2te_thsd_max(uint32_t cfg_uflag2te_thsd_max)
{
    volatile reg_uflag2te_thsd_max uflag2te_thsd_max;     /* 0x200 */
    if (g_mipi_tx_regs_va != NULL) {
        uflag2te_thsd_max.u32 = g_mipi_tx_regs_va->uflag2te_thsd_max.u32;
        uflag2te_thsd_max.bits.cfg_uflag2te_thsd_max = cfg_uflag2te_thsd_max;
        g_mipi_tx_regs_va->uflag2te_thsd_max.u32 = uflag2te_thsd_max.u32;
    }
}

uint32_t mipi_tx_hal_get_cfg_uflag2te_thsd_max(void)
{
    volatile reg_uflag2te_thsd_max uflag2te_thsd_max = {0};     /* 0x200 */
    if (g_mipi_tx_regs_va != NULL) {
        uflag2te_thsd_max.u32 = g_mipi_tx_regs_va->uflag2te_thsd_max.u32;
        return uflag2te_thsd_max.bits.cfg_uflag2te_thsd_max;
    }

    return 0;
}

void mipi_tx_hal_set_cfg_uflag2te_thsd_min(uint32_t cfg_uflag2te_thsd_min)
{
    volatile reg_uflag2e_thsd_min uflag2e_thsd_min;     /* 0x204 */
    if (g_mipi_tx_regs_va != NULL) {
        uflag2e_thsd_min.u32 = g_mipi_tx_regs_va->uflag2e_thsd_min.u32;
        uflag2e_thsd_min.bits.cfg_uflag2te_thsd_min = cfg_uflag2te_thsd_min;
        g_mipi_tx_regs_va->uflag2e_thsd_min.u32 = uflag2e_thsd_min.u32;
    }
}

uint32_t mipi_tx_hal_get_cfg_uflag2te_thsd_min(void)
{
    volatile reg_uflag2e_thsd_min uflag2e_thsd_min = {0};     /* 0x204 */
    if (g_mipi_tx_regs_va != NULL) {
        uflag2e_thsd_min.u32 = g_mipi_tx_regs_va->uflag2e_thsd_min.u32;
        return uflag2e_thsd_min.bits.cfg_uflag2te_thsd_min;
    }

    return 0;
}

uint32_t mipi_tx_hal_get_uflag2te_err_cnt_value(void)
{
    volatile reg_uflag2te_err_cnt uflag2te_err_cnt = {0};     /* 0x208 */
    if (g_mipi_tx_regs_va != NULL) {
        uflag2te_err_cnt.u32 = g_mipi_tx_regs_va->uflag2te_err_cnt.u32;
        return uflag2te_err_cnt.bits.uflag2te_err_cnt_value;
    }

    return 0;
}

PLT_SEC_ITCM_T STATIC uint32_t mipi_tx_hal_get_te_int_stop_cnt(uint32_t ulpsflg2te_time_us)
{
    return (MIPI_TX_US_PRE_SECOND / LCD_DEFAULT_FRAME_RATE - ulpsflg2te_time_us) *
        CLK_VIDEO_RTC / MIPI_TX_US_PRE_SECOND;
}

PLT_SEC_ITCM_T void mipi_tx_hal_set_ulpsflg2te_time(uint32_t ulpsflg2te_time_us)
{
    uapi_reg_write(TE_INT_STOP_CNT_VAL_REG, mipi_tx_hal_get_te_int_stop_cnt(ulpsflg2te_time_us));
}

#ifdef GPU_1A22
PLT_SEC_ITCM_T void mipi_tx_hal_set_te_int_mask(td_bool enable)
{
    uapi_reg_setbits((uintptr_t)TE_INT_REG, 4, 1, enable); /* bit4 mask */
}
#endif

uint32_t mipi_tx_hal_get_ulpsflg2te_time(void)
{
    uint32_t start_cnt;
    uapi_reg_read(TE_INT_START_CNT_VAL_REG, start_cnt);
    uint32_t stop_cnt;
    uapi_reg_read(TE_INT_STOP_CNT_VAL_REG, stop_cnt);
    if (start_cnt > stop_cnt) {
        printf("err: te_int start_cnt is bigger than stop_cnt!");
        return 0;
    }

    return (MIPI_TX_US_PRE_SECOND / LCD_DEFAULT_FRAME_RATE) -
        ((uint64_t)(stop_cnt - start_cnt) * MIPI_TX_US_PRE_SECOND / CLK_VIDEO_RTC);
}

void mipi_tx_hal_gen_ulps_flg_pulse(void)
{
#ifdef GPU_1A22
    uapi_reg_setbit((uintptr_t)TE_INT_MODE_REG, 2); // software generate ulps_flg pulse
#else
    uapi_reg_setbit((uintptr_t)TE_INT_MODE_REG, 0);  // switch to soft mode. (0：logic mode 1: soft mode)
    uapi_reg_setbit((uintptr_t)TE_INT_MODE_REG, 1);  // software generate ulps_flg pulse
    uapi_reg_clrbit((uintptr_t)TE_INT_MODE_REG, 0);  // switch to logic mode
#endif
}

PLT_SEC_ITCM_T void mipi_tx_hal_autoulps_en(uint8_t enable)
{
    volatile reg_auto_ulps_mode auto_ulps_mode;                 /* 0xe0 */
    volatile reg_auto_ulps_wakeup_time auto_ulps_wakeup_time;   /* 0xe8 */
    volatile reg_auto_ulps_min_time auto_ulps_min_time;         /* 0xf8 */
    if (g_mipi_tx_regs_va == NULL) {
        return;
    }

    auto_ulps_mode.u32 = g_mipi_tx_regs_va->auto_ulps_mode.u32;
    auto_ulps_wakeup_time.u32 = g_mipi_tx_regs_va->auto_ulps_wakeup_time.u32;
    auto_ulps_min_time.u32 = g_mipi_tx_regs_va->auto_ulps_min_time.u32;
    if (enable != 0) {
        auto_ulps_mode.bits.auto_ulps = 0x1; /* enable auto ulps */
        auto_ulps_mode.bits.cfg_ulps_auto_exit_en = 1;
        auto_ulps_mode.bits.cfg_uflag2te_err_cnt_en = 0;
        auto_ulps_mode.bits.ulps_exit_mux = 0x3; /* 0x3 : ulps counter can be controlled by software and logic */
        auto_ulps_mode.bits.pll_off_ulps = 0x1; /* enable ulps close phy pll */

        uint32_t actual_data_rate = mipi_tx_get_actual_phy_data_rate();
        auto_ulps_wakeup_time.bits.twakeup_cnt = 0x1; /* 0x1: cycle count 2 */
        auto_ulps_wakeup_time.bits.twakeup_clk_div =    /* 0x5e00: clk division of hsclk */
            (actual_data_rate * 1000000 / 0x8) / 0x2 / 1000; /* 1000000： MHz  1000:1ms 8：clk is datarate/8 */

        auto_ulps_min_time.bits.auto_ulps_min_time = 0x200; /* 0x200 :minimum period to next ulps_exiting */
        g_mipi_tx_regs_va->auto_ulps_entry_delay.bits.ulps_entry_delay = MIPI_TX_DEFAULT_ENTRY_ULPS_CYLE_NUM;
        g_mipi_tx_regs_va->auto_ulps_wakeup_time.u32 = auto_ulps_wakeup_time.u32; /* exit ulps period */
        g_mipi_tx_regs_va->auto_ulps_min_time.u32 = auto_ulps_min_time.u32;
        g_mipi_tx_regs_va->auto_ulps_mode.u32 = auto_ulps_mode.u32;
        mipi_tx_hal_set_ulpsflg2te_time(MCU_INT_TO_TE_TIME);
#ifdef GPU_1A22
        mipi_tx_hal_set_te_int_mask(1); /* ulps int isr mask */
#endif
    } else {
        auto_ulps_mode.bits.cfg_ulps_auto_exit_en = 0;
        g_mipi_tx_regs_va->auto_ulps_mode.u32 = auto_ulps_mode.u32;

        mipi_tx_hal_exit_ulps();

        uapi_tcxo_delay_us(1500); /* 1500 : delay 1500us */

        auto_ulps_mode.u32 = 0x20; /* 0x20 : disable auto ulps */
        g_mipi_tx_regs_va->auto_ulps_mode.u32 = auto_ulps_mode.u32;
        mipi_tx_hal_clear_exit_ulps();
        mipi_tx_hal_clear_ulps_entry_cnt();
    }
}

PLT_SEC_ITCM_T static void mipi_tx_drv_reg_init(void)
{
    g_mipi_tx_regs_va = (mipi_tx_regs_type_t *)(uintptr_t)MIPI_TX_REGS_BASE_ADDR;
}

PLT_SEC_ITCM_T static void mipi_tx_drv_reg_exit(void)
{
    g_mipi_tx_regs_va = NULL;
}

#if MIPI_TX_INTERRUPTS_ENABLE
static void mipi_tx_enable_int(uint32_t enable)
{
    volatile reg_int1_mask int1_mask;
    volatile reg_int0_mask int0_mask;

    int0_mask.u32 = g_mipi_tx_regs_va->int0_mask.u32;
    int1_mask.u32 = g_mipi_tx_regs_va->int1_mask.u32;
    if (enable) {
        int0_mask.u32 = 0x001fffff;
        int1_mask.u32 = 0x0f067fff; /* mask b15, b16, b20 */
    } else {
        int0_mask.u32 = 0;
        int1_mask.u32 = 0;
    }
    g_mipi_tx_regs_va->int0_mask.u32 = int0_mask.u32;
    g_mipi_tx_regs_va->int1_mask.u32 = int1_mask.u32;
}

static void mipi_tx_clear_int(void)
{
    volatile reg_int1_status int1_stat;
    volatile reg_int0_status int0_stat;

    /* warning: read register to clear interrupt status, do not delete it. */
    int1_stat.u32 = g_mipi_tx_regs_va->int1_status.u32;
    int0_stat.u32 = g_mipi_tx_regs_va->int0_status.u32;

    /* ignore compile warning: variable 'int_stat' set but not used */
    (void)int1_stat.u32;
    (void)int0_stat.u32;
}

static void mipi_tx_interrupt_route(uint32_t arg)
{
    mipi_tx_clear_int();
    UNUSED(arg);
}

static uint32_t mipi_tx_register_irq(void)
{
    uint32_t ret;

    ret = osal_irq_request(g_mipi_tx_irq_num, (osal_irq_handler)mipi_tx_interrupt_route, NULL, NULL, NULL);
    if (ret != EXT_ERR_SUCCESS) {
        mipi_tx_err("mipi irq request fail:%x\r\n", ret);
        return EXT_ERR_MIPI_IRQ_REQUEST_FAILED;
    }
    ret = osal_irq_enable(g_mipi_tx_irq_num);
    if (ret != EXT_ERR_SUCCESS) {
        mipi_tx_err("mipi irq enable fail:%x\r\n", ret);
        ret = EXT_ERR_MIPI_IRQ_ENABLE_FAILED;
        goto fail;
    }

    return EXT_ERR_SUCCESS;

fail:
    osal_irq_free(g_mipi_tx_irq_num, NULL);
    return ret;
}

static void mipi_tx_unregister_irq(void)
{
    osal_irq_disable(g_mipi_tx_irq_num);
    osal_irq_free(g_mipi_tx_irq_num, NULL);
}
#endif

PLT_SEC_ITCM_T td_void mipi_tx_wait_soft_reset_succ(void)
{
    td_u32 retry = 0;
    while (uapi_reg_read_val32(MIPI_TX_SOFTRST_STA_REG) != MIPI_TX_SOFTRST_VAL) {
        if (retry++ > 0xFF) {
            mipi_tx_err("mipi tx soft reset fail!");
            break;
        }
    }
}

PLT_SEC_ITCM_T static void mipi_tx_crg_init(void)
{
#ifdef GPU_1A22
    writel(VIDEO_CTL_MIPI_RST_REG, 0x2); // bit1： apb_cken   bit0：apb_srst_req_n 0-复位 1-不复位
    mipi_tx_wait_soft_reset_succ();
    writel(VIDEO_CTL_MIPI_RST_REG, 0x3);
#else
    writew(VIDEO_CTL_MIPI_RST_REG, VIDEO_CTL_RB_CFG_VIDEO_MIPITX_APB_SRST_REQ_MASK |
        VIDEO_CTL_RB_CFG_VIDEO_MIPITX_APB_CKEN_MASK);
    mipi_tx_wait_soft_reset_succ();
    writew(VIDEO_CTL_MIPI_RST_REG, VIDEO_CTL_RB_CFG_VIDEO_MIPITX_APB_CKEN_MASK); // mipi clk & rst
#endif
}

PLT_SEC_ITCM_T uint32_t mipi_tx_drv_init(void)
{
    mipi_tx_drv_reg_init();
    mipi_tx_crg_init();
#if MIPI_TX_INTERRUPTS_ENABLE
    uint32_t ret = mipi_tx_register_irq();
    if (ret != EXT_ERR_SUCCESS) {
        mipi_tx_err("mipi_tx_register_irq fail!\r\n");
        mipi_tx_drv_reg_exit();
        return ret;
    }
#endif
    return EXT_ERR_SUCCESS;
}

PLT_SEC_ITCM_T void mipi_tx_drv_exit(void)
{
#if MIPI_TX_INTERRUPTS_ENABLE
    mipi_tx_unregister_irq();
#endif
    g_mipi_tx_regs_va->ctrl_reset.u32 = 0x0;
    mipi_tx_drv_phy_reset();

    mipi_tx_drv_reg_exit();
}

#ifdef __cplusplus
#if __cplusplus
}

#endif
#endif
