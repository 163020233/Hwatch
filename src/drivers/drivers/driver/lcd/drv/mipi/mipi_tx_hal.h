/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: mipi tx hal header file.
 *
 * Create: 2020-04-14
 */
#ifndef __MIPI_TX_HAL_H__
#define __MIPI_TX_HAL_H__

#include "soc_mipi_tx.h"
#include "common_def.h"
#include "chip_io.h"
#include "mipi_tx_reg.h"
#ifdef GPU_1A22
#include "sw22_video_ctl_rb_reg_offset.h"
#endif
#include "mipi_tx_def.h"

#define MIPI_TX_INTERRUPTS_ENABLE 0

#ifdef GPU_1A22
#define MIPI_TX_REGS_BASE_ADDR  0x535D0000
#define VIDEO_CTL_MIPI_RST_REG  0x535D332C
#else
#define MIPI_TX_REGS_BASE_ADDR  0x520D0000
#define VIDEO_CTL_MIPI_RST_REG  VIDEO_CTL_RB_MIPITX_CRG_CKEN_SRST_REQ_REG
#endif

#define MIPI_TX_SOFTRST_STA_REG (MIPI_TX_REGS_BASE_ADDR + 0x8) // MIPI TX软复位状态寄存器 查询为0表示处于复位状态
#define MIPI_TX_SOFTRST_VAL     0

#ifdef GPU_3322
#define LANEBYTE_PCTRL          0
#else
#define LANEBYTE_PCTRL          1           // 模拟输出给数字的时钟需要取反沿使用
#endif

#define PHY_FILTER_BYPASS       0xA1        // 关闭滤毛刺功能
#define TPOST_DELAY             1
#define TCLK_PRE_DELAY          1
#define HSTX_CKG_SEL_DIV2       1
#define HSTX_CKG_SEL_DIV4       2
#define HSTX_CKG_SEL_DIV8       3
#define HSTX_CKG_SEL_DIV16      4
#define RG_TOP_DMY_VAL          0x80

#ifdef PRE_FPGA
#define CLK_VIDEO_RTC           31850
#define CLK_VIDEO_APB           32000000
#else
#define CLK_VIDEO_RTC           32768
#define CLK_VIDEO_APB           200000000
#endif

#define TE_INT_START_CNT_VAL_REG        (GLB_CTL_M_RB_BASE + 0x298)
#define TE_INT_STOP_CNT_VAL_REG         (GLB_CTL_M_RB_BASE + 0x29C)

#ifdef GPU_1A22
#define TE_INT_MODE_REG                 (VIDEO_CTL_RB_BASE + 0x200)
#define TE_INT_REG                      (GLB_CTL_M_RB_BASE + 0x290)
#else
#define TE_INT_MODE_REG                 (VIDEO_CTL_RB_BASE + 0x80)
#endif

/* mipi_tx interrupt number */
#define MIPI_TX_IRQ         21
/* mipi_tx phy ref_clk freq */
#define MIPI_TX_REF_CLK     32
#define MIPI_TX_MAX_DATA_RATE 1000
/* mipi_tx dfx define */
#define MIPI_INT_DFX_NUM    2
#define MIPI_TX_INT0_STATUS (MIPI_TX_REGS_BASE_ADDR + 0x00BC)
#define MIPI_TX_INT1_STATUS (MIPI_TX_REGS_BASE_ADDR + 0x00C0)

/* mipi_tx max cmd length */
#define MIPI_TX_SET_DATA_SIZE 128
#define MIPI_TX_GET_DATA_SIZE 64

/* mipi_tx fifo define */
#define MIPI_TX_BYTES_PER_WORD 4
#define MIPI_TX_CMD_BYTE1 0
#define MIPI_TX_CMD_BYTE2 1
#define MIPI_TX_CMD_BYTE3 2
#define MIPI_TX_CMD_BYTE4 3

#define MIPI_TX_US_PRE_SECOND 1000000

/* pll */
#define TLPX                60
#define TCLK_PREPARE        65
#define TCLK_ZERO           248
#define TCLK_TRAIL          65
#define THS_PREPARE         60
#define THS_ZERO            110
#define THS_TRAIL           83

#ifdef GPU_3322 /* 3322 phy */
#define PLL_REG_ENCS        (MIPI_TX_REGS_BASE_ADDR + 0x1d44)
#define PLL_SET_CKG_SEL     (MIPI_TX_REGS_BASE_ADDR + 0x1d20)
#define PLL_SET_FBDIV       (MIPI_TX_REGS_BASE_ADDR + 0x1d28)
#define PLL_SET_FBDIV_PRDIV (MIPI_TX_REGS_BASE_ADDR + 0x1d24)
#define PLL_DIV_UPDATE      (MIPI_TX_REGS_BASE_ADDR + 0x1d2C)
#define RX_TXPLL_ICTRL      (MIPI_TX_REGS_BASE_ADDR + 0x1008)

/* other */
#define PLL_FEEDBACK        (MIPI_TX_REGS_BASE_ADDR + 0x109C)
#define PLL_SR_PASVE        (MIPI_TX_REGS_BASE_ADDR + 0x1D4C)
#define FB_DIV_SSC          (MIPI_TX_REGS_BASE_ADDR + 0x106C)
#define SEL_LOCK            (MIPI_TX_REGS_BASE_ADDR + 0x1004)
#define CPHY_DEDRV_EN       (MIPI_TX_REGS_BASE_ADDR + 0x1d5c)
#define TCALALTSEQ          (MIPI_TX_REGS_BASE_ADDR + 0x1c38)

/* clk */
#define CLK_TLPX            (MIPI_TX_REGS_BASE_ADDR + 0x1EC8)
#define CLK_TCLK_PREPARE    (MIPI_TX_REGS_BASE_ADDR + 0x1ECC)
#define CLK_TCLK_ZERO       (MIPI_TX_REGS_BASE_ADDR + 0x1ED0)
#define CLK_TCLK_TRAIL      (MIPI_TX_REGS_BASE_ADDR + 0x1ED4)
#define CLK_TCLK_PRE_DELAY  (MIPI_TX_REGS_BASE_ADDR + 0x1EC0)
#define CLK_TCLK_POST_DELAY (MIPI_TX_REGS_BASE_ADDR + 0x1EC4)
#define CLK_TCK_DATAHS      (MIPI_TX_REGS_BASE_ADDR + 0x1ED8)
/* data0 */
#define DATA0_TLPX          (MIPI_TX_REGS_BASE_ADDR + 0x1DC8)
#define DATA0_THS_PREPARE   (MIPI_TX_REGS_BASE_ADDR + 0x1DCC)
#define DATA0_THS_ZERO      (MIPI_TX_REGS_BASE_ADDR + 0x1DD0)
#define DATA0_THS_TRAIL     (MIPI_TX_REGS_BASE_ADDR + 0x1DD4)
#define DATA0_TPRE_DELAY    (MIPI_TX_REGS_BASE_ADDR + 0x1DC0)
#define DATA0_TPOST_DELAY   (MIPI_TX_REGS_BASE_ADDR + 0x1DC4)

/* data1 */
#define DATA1_TLPX          (MIPI_TX_REGS_BASE_ADDR + 0x1E48)
#define DATA1_THS_PREPARE   (MIPI_TX_REGS_BASE_ADDR + 0x1E4C)
#define DATA1_THS_ZERO      (MIPI_TX_REGS_BASE_ADDR + 0x1E50)
#define DATA1_THS_TRAIL     (MIPI_TX_REGS_BASE_ADDR + 0x1E54)
#define DATA1_TPRE_DELAY    (MIPI_TX_REGS_BASE_ADDR + 0x1E40)
#define DATA1_TPOST_DELAY   (MIPI_TX_REGS_BASE_ADDR + 0x1E44)
#define PERFORMANCE_REG     (MIPI_TX_REGS_BASE_ADDR + 0X100C)

#define PHY_TEST_MODE       (MIPI_TX_REGS_BASE_ADDR + 0x1CC0) // bit6 test_mode_en for reg_t_da_lptx_en
#define PHY_LPTX_EN         (MIPI_TX_REGS_BASE_ADDR + 0x1CEC)

#define HAL_MIPI_REF_CLK  32
#else

#define PLL_SET_CKG_SEL     (MIPI_TX_REGS_BASE_ADDR + 0x580)
#define PLL_SET_FBDIV       (MIPI_TX_REGS_BASE_ADDR + 0x590)
#define PLL_SET_FBDIV_PRDIV (MIPI_TX_REGS_BASE_ADDR + 0x594)

/* phy analog */
#define RG_LPTX_SELENFOR_SELFORHI (MIPI_TX_REGS_BASE_ADDR + 0x588)

/* clk */
#define CLK_TLPX            (MIPI_TX_REGS_BASE_ADDR + 0x440)
#define CLK_TCLK_PREPARE    (MIPI_TX_REGS_BASE_ADDR + 0x444)
#define CLK_TCLK_ZERO       (MIPI_TX_REGS_BASE_ADDR + 0x448)
#define CLK_TCLK_TRAIL      (MIPI_TX_REGS_BASE_ADDR + 0x44c)
#define CKG_EN_WAIT         (MIPI_TX_REGS_BASE_ADDR + 0x43C)
#define CLK_TCK_DATAHS      (MIPI_TX_REGS_BASE_ADDR + 0x454) // 屏端上升沿还是下降沿采集数据，取值范围：0x55和0xAA
#define CLK_TCLK_PRE_DELAY  (MIPI_TX_REGS_BASE_ADDR + 0x45C)
#define CLK_TCLK_POST_DELAY (MIPI_TX_REGS_BASE_ADDR + 0x460)

/* data0 */
#define DATA0_TLPX          (MIPI_TX_REGS_BASE_ADDR + 0x480)
#define DATA0_THS_PREPARE   (MIPI_TX_REGS_BASE_ADDR + 0x484)
#define DATA0_THS_ZERO      (MIPI_TX_REGS_BASE_ADDR + 0x488)
#define DATA0_THS_TRAIL     (MIPI_TX_REGS_BASE_ADDR + 0x48C)
#define DATA0_FILTER_BYPASS (MIPI_TX_REGS_BASE_ADDR + 0x49C) // bit7 为data lane0 RXCLKESC时钟滤毛刺BYPASS
#define DATA0_TPRE_DELAY    (MIPI_TX_REGS_BASE_ADDR + 0x4A0)
#define DATA0_TPOST_DELAY   (MIPI_TX_REGS_BASE_ADDR + 0x4A4)

/* data1 */
#define DATA1_TLPX          (MIPI_TX_REGS_BASE_ADDR + 0x4C0)
#define DATA1_THS_PREPARE   (MIPI_TX_REGS_BASE_ADDR + 0x4C4)
#define DATA1_THS_ZERO      (MIPI_TX_REGS_BASE_ADDR + 0x4C8)
#define DATA1_THS_TRAIL     (MIPI_TX_REGS_BASE_ADDR + 0x4CC)
#define DATA1_FILTER_BYPASS (MIPI_TX_REGS_BASE_ADDR + 0x4DC) // bit7 为data lane1 RXCLKESC时钟滤毛刺BYPASS
#define DATA1_TPRE_DELAY    (MIPI_TX_REGS_BASE_ADDR + 0x4E0)
#define DATA1_TPOST_DELAY   (MIPI_TX_REGS_BASE_ADDR + 0x4E4)

#define RG_TOP_DMY          (MIPI_TX_REGS_BASE_ADDR + 0x5A0)

#define PHY_LPTX_EN         (MIPI_TX_REGS_BASE_ADDR + 0x5B8)
#define PHY_BAND_EN         (MIPI_TX_REGS_BASE_ADDR + 0x598)
#define PHY_LPTX_DP         (MIPI_TX_REGS_BASE_ADDR + 0x5cc)

#define SET_CKG_EN_WAIT_VALUE     16

#endif /* end of #ifdef GPU_3322 */

#define MIPI_TX_READ_TIMEOUT_CNT 30000
#define MIPI_TX_MAX_RESET_PHY_TIMES 10
#define MIPI_TX_EXIT_SDLP_TIMEOUT_CNT 200
#define MIPI_TX_EXIT_WAIT_ULPS_TIMEOUT_CNT 100

#define PREPARE_COMPENSATE    10
#define ROUNDUP_VALUE     7999
#define INNER_PEROID    8000   /* 8 * 1000 ,1000 is 1us = 1000ns, 8 is division ratio */

#define MIPI_TX_BITS_PER_BYTE 8
#define MIPI_TX_PHY_READY_TIMEOUT_CNT 60

#define MIPI_TX_DEFAULT_ENTRY_ULPS_CYLE_NUM 0x200 // hsclk period

/*
 * global operation timing parameters
 */
typedef struct {
    uint8_t data_tpre_delay;
    uint8_t clk_post_delay;
    uint8_t clk_tlpx;
    uint8_t clk_tclk_prepare;
    uint8_t clk_tclk_zero;
    uint8_t clk_tclk_trail;
    uint8_t data_tlpx;
    uint8_t data_ths_prepare;
    uint8_t data_ths_zero;
    uint8_t data_ths_trail;
} mipi_tx_phy_timing_parameters;

typedef struct {
    uint32_t vall_det;
    uint32_t vact_det;
    uint32_t hall_det;
    uint32_t hact_det;
    uint32_t hbp_det;
    uint32_t hsa_det;
    uint32_t vsa_det;
    uint32_t vall_ppi;
    uint32_t vact_ppi;
    uint32_t hact_ppi;
    uint32_t lane_num;
} mipi_tx_dev_phy_t;

typedef struct {
    uint32_t idle_active;
    uint32_t exit_ulps;
    uint32_t exit_sdlp;
} mipi_tx_lane_state_t;

typedef enum {
    INPUT_COLOUR_MODE = 0,
    INPUT_BLACK_MODE = 1,
    INPUT_ALTERNATING_MODE = 2,

    INPUT_MODE_BUTT
} colorbar_input_mode;

typedef enum {
    ORIEN_HORIZONTAL_MODE = 0,
    ORIEN_VERTICAL_MODE = 1,

    ORIEN_MODE_BUTT
} colorbar_orien_mode;

typedef enum {
    MIPI_TX_LANE_DATA0,
    MIPI_TX_LANE_DATA1,
    MIPI_TX_LANE_CLK,
} mipi_tx_lane_id;

__inline static uint8_t get_low_byte(uint16_t short_data)
{
    return (uint8_t)(short_data & 0xff);
}

__inline static uint8_t get_high_byte(uint16_t short_data)
{
    return (uint8_t)((short_data & 0xff00) >> MIPI_TX_BITS_PER_BYTE);
}

uint32_t mipi_tx_align_up(uint32_t x, uint32_t a);
void mipi_tx_drv_set_irq_num(uint32_t irq_num);
void mipi_tx_drv_set_regs(mipi_tx_regs_type_t *regs);

void mipi_tx_drv_set_phy_cfg(const combo_dev_cfg_t *dev_cfg);
void mipi_tx_drv_get_dev_status(mipi_tx_dev_phy_t *mipi_tx_phy_ctx);
void mipi_tx_drv_set_controller_cfg(const combo_dev_cfg_t *dev_cfg);

uint32_t mipi_tx_drv_set_cmd_info(const cmd_info_t *cmd_info);

uint32_t mipi_tx_drv_get_cmd_info(get_cmd_info_t *get_cmd_info);
void mipi_tx_drv_enable_input(const combo_dev_cfg_t *dev_cfg);
uint32_t mipi_tx_drv_disable_input(void);

uint32_t mipi_tx_drv_init(void);
void mipi_tx_drv_exit(void);
void mipi_tx_hal_colorbar_mode(combo_dev_cfg_t *dev_cfg, colorbar_orien_mode orien, colorbar_input_mode mode);
void mipi_tx_hal_unset_colorbar(const combo_dev_cfg_t *dev_cfg);

void mipi_tx_drv_get_int_status(uint32_t *int_dfx_info, uint32_t info_len);

uint32_t mipi_tx_get_actual_phy_data_rate(void);

void mipi_tx_hal_auto_enter_sdlp(void);

uint32_t mipi_tx_hal_exit_sdlp(void);

uint32_t mipi_tx_hal_get_ulps_entry_count(void);

uint32_t mipi_tx_hal_get_ulps_exit_count(void);

void mipi_tx_hal_exit_ulps(void);

void mipi_tx_hal_clear_ulps_entry_cnt(void);

void mipi_tx_hal_clear_exit_ulps(void);

void mipi_tx_hal_set_auto_exit_ulps_en(td_bool cfg_ulps_auto_exit_en);

void mipi_tx_hal_set_ulps_entry_delay(uint32_t ulps_entry_delay);

void mipi_tx_hal_set_cfg_uflag2te_err_cnt_en(uint32_t cfg_uflag2te_err_cnt_en);

void mipi_tx_hal_set_cfg_uflag2te_err_cnt_clr(uint32_t cfg_uflag2te_err_cnt_clr);

void mipi_tx_hal_set_cfg_uflag2te_thsd_max(uint32_t cfg_uflag2te_thsd_max);

uint32_t mipi_tx_hal_get_cfg_uflag2te_thsd_max(void);

void mipi_tx_hal_set_cfg_uflag2te_thsd_min(uint32_t cfg_uflag2te_thsd_min);

uint32_t mipi_tx_hal_get_cfg_uflag2te_thsd_min(void);

uint32_t mipi_tx_hal_get_uflag2te_err_cnt_value(void);

void mipi_tx_hal_set_ulpsflg2te_time(uint32_t ulpsflg2te_time_us);

uint32_t mipi_tx_hal_get_ulpsflg2te_time(void);

void mipi_tx_hal_gen_ulps_flg_pulse(void);

void mipi_tx_hal_autoulps_en(uint8_t enable);

void mipi_tx_hal_get_lane_state(mipi_tx_lane_id lane_num, mipi_tx_lane_state_t *status);

void mipi_tx_hal_set_request_hsclk_en(uint8_t clk_en);

void mipi_tx_hal_set_clklane_continue_en(uint8_t clk_en);

td_bool mipi_tx_hal_is_pld_err(void);

void mipi_tx_hal_set_auto_ulps_entry_suspend(td_bool ulps_suspend_en);

void mipi_tx_hal_set_dfx_pkt_cnt_en(td_bool en);
uint32_t mipi_tx_hal_dfx_get_frame_cnt(void);
uint32_t mipi_tx_hal_dfx_get_lp_pkt_cnt(void);
uint32_t mipi_tx_hal_dfx_get_hs_pkt_cnt(void);
uint32_t mipi_tx_hal_dfx_get_dpu_pkt_cnt(void);
td_bool mipi_tx_hal_get_mipi2dpu_rdy(void);
void mipi_tx_hal_set_cmd_video_mode(cmd_video_mode cmd_video);
#ifdef GPU_1A22
void mipi_tx_hal_set_te_int_mask(td_bool enable);
#endif

#endif /* end of #ifndef __MIPI_TX_HAL_H__ */
