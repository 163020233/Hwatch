/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: psram driver regs define header file \n
 *
 * History: \n
 * 2024-05-14, Create file. \n
 */
#ifndef HAL_PSRAM_REGS_DEF_H
#define HAL_PSRAM_REGS_DEF_H

#include <stdint.h>
#include "product.h"

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


/**
 * @brief  Registers associated with psram phy.
 */
typedef struct psram_phy_regs_t {
    volatile uint32_t phy_rst;
    volatile uint32_t phy_init;
    volatile uint32_t phy_init_ctrl;
    volatile uint32_t phy_init_info;
    volatile uint32_t phy_work_mode0;
    volatile uint32_t phy_work_mode1;
    volatile uint32_t reserved;
    volatile uint32_t reserve2;
    volatile uint32_t phy_ctrl0;
    volatile uint32_t phy_ctrl1;
    volatile uint32_t phy_ctrl2;
    volatile uint32_t reserve3;
    volatile uint32_t phy_io_ctrl0;
    volatile uint32_t phy_io_ctrl1;
    volatile uint32_t phy_io_ctrl2;
    volatile uint32_t phy_io_ctrl3;
} psram_phy_regs_t;

/**
 * @brief  This union represents the bit fields in the psram phy reset Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union psram_phy_rst_t {
    uint32_t d32;                                /*!< Raw register data. */
    struct {
        uint32_t phy_soft_rst            : 1;
        uint32_t reserved                : 3;
        uint32_t psram_rst               : 1;
        uint32_t psram_rst_oe            : 1;
        uint32_t reserved2               : 26;
    } b;                                         /*!< Register bits. */
} psram_phy_rst_t;

/**
 * @brief  This union represents the bit fields in the psram phy init Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union psram_phy_init_t {
    uint32_t d32;                                /*!< Raw register data. */
    struct {
        uint32_t dlym_en                  : 1;
        uint32_t zcal_en                  : 1;
        uint32_t dlym_en_1                : 1;
        uint32_t reserved                 : 1;
        uint32_t rxfifo_dis               : 1;
        uint32_t cs_pullup                : 1;
        uint32_t ck_pulldown              : 1;
        uint32_t reserved2                : 25;
    } b;                                         /*!< Register bits. */
} psram_phy_init_t;

/**
 * @brief  This union represents the bit fields in the psram phy init ctrl Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union psram_phy_init_ctrl_t {
    uint32_t d32;                                /*!< Raw register data. */
    struct {
        uint32_t dlym_wait_time           : 4;
        uint32_t zcal_pnsequence          : 1;
        uint32_t zcal_errstopup           : 1;
        uint32_t reserved                 : 2;
        uint32_t zcal_wait_time           : 4;
        uint32_t reserved2                : 20;
    } b;                                         /*!< Register bits. */
} psram_phy_init_ctrl_t;

/**
 * @brief  This union represents the bit fields in the psram phy init info Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union psram_phy_init_info_t {
    uint32_t d32;                                /*!< Raw register data. */
    struct {
        uint32_t dlym_overflow            : 1;
        uint32_t dlym_overflow_1          : 1;
        uint32_t reserved                 : 2;
        uint32_t dlym_init_result         : 1;
        uint32_t dlym_init_result_1       : 1;
        uint32_t reserved1                : 2;
        uint32_t zcal_err                 : 1;
        uint32_t zcal_pdrv_err            : 1;
        uint32_t zcal_ndrv_err            : 1;
        uint32_t reserved2                : 21;
    } b;                                         /*!< Register bits. */
} psram_phy_init_info_t;

/**
 * @brief  This union represents the bit fields in the psram phy work mode 0 Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union psram_phy_work_mode0_t {
    uint32_t d32;                                /*!< Raw register data. */
    struct {
        uint32_t rdqsg_start_pos           : 5;
        uint32_t reserved                  : 3;
        uint32_t rxfifo_pop_mode           : 1;
        uint32_t reserved2                 : 3;
        uint32_t rxfifo_pop_sel            : 2;
        uint32_t reserved3                 : 2;
        uint32_t rxfifo_rden_pos           : 5;
        uint32_t reserved4                 : 7;
        uint32_t debug_en                  : 1;
        uint32_t reserved5                 : 3;
    } b;                                         /*!< Register bits. */
} psram_phy_work_mode0_t;

/**
 * @brief  This union represents the bit fields in the psram phy work mode 1 Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union psram_phy_work_mode1_t {
    uint32_t d32;                                /*!< Raw register data. */
    struct {
        uint32_t dqs_fdbk_samp_pos          : 3;
        uint32_t reserved                   : 1;
        uint32_t dqs_fdbk_en                : 1;
        uint32_t reserved2                  : 3;
        uint32_t dm_io_use                  : 1;
        uint32_t reserved3                  : 3;
        uint32_t dm_swap_en                 : 1;
        uint32_t dqs_swap_en                : 1;
        uint32_t dq_swap_en                 : 1;
        uint32_t reserved4                  : 17;
    } b;                                         /*!< Register bits. */
} psram_phy_work_mode1_t;

/**
 * @brief  This union represents the bit fields in the psram phy ctrl 0 Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union psram_phy_ctrl0_t {
    uint32_t d32;                                /*!< Raw register data. */
    struct {
        uint32_t dq_oe_ext                 : 1;
        uint32_t dqs_oe_ext                : 1;
        uint32_t cmd_oe_ext                : 1;
        uint32_t dqs_ie_ext                : 1;
        uint32_t cs_ext                    : 2;
        uint32_t reserved                  : 2;
        uint32_t rdqsg_ext                 : 1;
        uint32_t oe_opt                    : 1;
        uint32_t reserved2                 : 22;
    } b;                                         /*!< Register bits. */
} psram_phy_ctrl0_t;

/**
 * @brief  This union represents the bit fields in the psram phy ctrl 1 Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union psram_phy_ctrl1_t {
    uint32_t d32;                                /*!< Raw register data. */
    struct {
        uint32_t rdqs_bcdl_en              : 1;
        uint32_t cs_bcdl_en                : 1;
        uint32_t ck_bcdl_en                : 1;
        uint32_t rdqs_bcdl_en_1            : 1;
        uint32_t rdqs_bcdl_code            : 6;
        uint32_t reserved2                 : 2;
        uint32_t cs_bcdl_code              : 6;
        uint32_t reserved3                 : 2;
        uint32_t ck_bcdl_code              : 6;
        uint32_t rdqs_bcdl_code_1          : 6;
    } b;                                         /*!< Register bits. */
} psram_phy_ctrl1_t;

/**
 * @brief  This union represents the bit fields in  psram phy ctrl 2 Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union psram_phy_ctrl2_t {
    uint32_t d32;                                /*!< Raw register data. */
    struct {
        uint32_t rdqs_bcdl_code_offset       : 5;
        uint32_t reserved                    : 3;
        uint32_t rdqs_bcdl_code_inv          : 1;
        uint32_t cs_shift_en                 : 1;
        uint32_t ck_shift_en                 : 1;
        uint32_t reserved2                   : 1;
        uint32_t zcal_pdrv_latch             : 6;
        uint32_t reserved3                   : 2;
        uint32_t zcal_ndrv_latch             : 6;
        uint32_t reserved4                   : 6;
    } b;                                         /*!< Register bits. */
} psram_phy_ctrl2_t;

/**
 * @brief  This union represents the bit fields  psram phy io ctrl 0 Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union psram_phy_io_ctrl0_t {
    uint32_t d32;                                /*!< Raw register data. */
    struct {
        uint32_t io_dq_ronsel                 : 3;
        uint32_t reserved                     : 1;
        uint32_t io_dqs_ronsel                : 3;
        uint32_t reserved2                    : 1;
        uint32_t io_ck_ronsel                 : 3;
        uint32_t reserved3                    : 1;
        uint32_t io_cs_ronsel                 : 3;
        uint32_t reserved4                    : 1;
        uint32_t io_dq_odtsel                 : 3;
        uint32_t reserved5                    : 1;
        uint32_t io_dqs_odtsel                : 3;
        uint32_t reserved6                    : 1;
        uint32_t io_ck_diff                   : 1;
        uint32_t reserved7                    : 3;
        uint32_t io_ck_oe                     : 1;
        uint32_t io_cs_oe                     : 1;
        uint32_t reserved8                    : 2;
    } b;                                         /*!< Register bits. */
} psram_phy_io_ctrl0_t;

/**
 * @brief  This union represents the bit fields  psram phy io ctrl 1 Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union psram_phy_io_ctrl1_t {
    uint32_t d32;                                /*!< Raw register data. */
    struct {
        uint32_t io_dq0_en                    : 1;
        uint32_t io_dq1_en                    : 1;
        uint32_t io_dq2_en                    : 1;
        uint32_t io_dq3_en                    : 1;
        uint32_t io_dq4_en                    : 1;
        uint32_t io_dq5_en                    : 1;
        uint32_t io_dq6_en                    : 1;
        uint32_t io_dq7_en                    : 1;
        uint32_t io_dq8_en                    : 1;
        uint32_t io_dq9_en                    : 1;
        uint32_t io_dq10_en                   : 1;
        uint32_t io_dq11_en                   : 1;
        uint32_t io_dq12_en                   : 1;
        uint32_t io_dq13_en                   : 1;
        uint32_t io_dq14_en                   : 1;
        uint32_t io_dq15_en                   : 1;
        uint32_t io_dqs0_en                   : 1;
        uint32_t io_dqs1_en                   : 1;
        uint32_t reserved                     : 14;
    } b;                                         /*!< Register bits. */
} psram_phy_io_ctrl1_t;

/**
 * @brief  This union represents the bit fields  psram phy io ctrl 3 Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union psram_phy_io_ctrl3_t {
    uint32_t d32;                                /*!< Raw register data. */
    struct {
        uint32_t io_poc_vddio_sel              : 1;
        uint32_t io_poc_vref_en                : 1;
        uint32_t io_poc_vref_drv_sel           : 1;
        uint32_t reserved                      : 1;
        uint32_t io_poc_rsv                    : 2;
        uint32_t io_poc_refsel                 : 2;
        uint32_t io_zcal_res_inner             : 2;
        uint32_t io_ck_inmode                  : 2;
        uint32_t io_cs_inmode                  : 2;
        uint32_t io_dq_inmode                  : 2;
        uint32_t io_dqs_inmode                 : 2;
        uint32_t reserved2                     : 14;
    } b;                                         /*!< Register bits. */
} psram_phy_io_ctrl3_t;

/**
 * @brief  Registers associated with psram dmac.
 */
typedef struct psram_dmac_regs_t {
    volatile uint32_t dmc_ctrl_hs;
    volatile uint32_t dmc_ctrl_dpd;
    volatile uint32_t dmc_ctrl_sfc;
    volatile uint32_t dmc_cfg_lp;
    volatile uint32_t dmc_cfg_mem_mode;
    volatile uint32_t dmc_cfg_work_mode;
    volatile uint32_t dmc_cfg_mr;
    volatile uint32_t dmc_cfg_auto_hs;
    volatile uint32_t dmc_cfg_timing0;
    volatile uint32_t dmc_cfg_timing1;
    volatile uint32_t dmc_cfg_timing2;
    volatile uint32_t dmc_cfg_timing3;
    volatile uint32_t dmc_cfg_timing4;
    volatile uint32_t reserved;
    volatile uint32_t reserved2;
    volatile uint32_t reserved3;
    volatile uint32_t dmc_cfg_lat;
    volatile uint32_t dmc_cfg_cs_ext;
} psram_dmac_regs_t;

/**
 * @brief  Registers associated with psram dmac cfg sfc. offset:0xB0~0xEC
 */
typedef struct psram_dmac_cfg_sfc_regs_t {
    volatile uint32_t dmc_cfg_sfc_cmd;
    volatile uint32_t dmc_cfg_sfc_addr;
    volatile uint32_t reserved;
    volatile uint32_t reserved2;
    volatile uint32_t dmc_cfg_sfc_wdata0;
    volatile uint32_t dmc_cfg_sfc_wdata1;
    volatile uint32_t dmc_cfg_sfc_wdata2;
    volatile uint32_t dmc_cfg_sfc_wdata3;
    volatile uint32_t dmc_cfg_sfc_wmask;
    volatile uint32_t reserved3;
    volatile uint32_t reserved4;
    volatile uint32_t reserved5;
    volatile uint32_t dmc_cfg_sfc_rdata0;
    volatile uint32_t dmc_cfg_sfc_rdata1;
    volatile uint32_t dmc_cfg_sfc_rdata2;
    volatile uint32_t dmc_cfg_sfc_rdata3;
} psram_dmac_cfg_sfc_regs_t;

/**
 * @brief  Registers associated with psram dmac debug. offset:0x200~0x224
 */
typedef struct psram_dmac_debug_regs_t {
    volatile uint32_t dmc_cur_status;
    volatile uint32_t dmc_cur_que_cmd_vld;
    volatile uint32_t dmc_cur_que_cmd_type;
    volatile uint32_t dmc_cur_rpws_status;
    volatile uint32_t dmc_debug;
    volatile uint32_t reserved;
    volatile uint32_t reserved2;
    volatile uint32_t reserved3;
    volatile uint32_t dmc_int;
    volatile uint32_t dmc_ws_buf;
} psram_dmac_debug_regs_t;

/**
 * @brief  This union represents the bit fields  psram dmac ctrl hs Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union psram_dmc_ctrl_hs_t {
    uint32_t d32;                                /*!< Raw register data. */
    struct {
        uint32_t hs_enter_req                  : 1;
        uint32_t hs_exit_req                   : 1;
        uint32_t reserved                      : 2;
        uint32_t dmc_cmd_bp                    : 1;
        uint32_t reserved2                     : 3;
        uint32_t exec_cmd_bp                   : 1;
        uint32_t reserved3                     : 23;
    } b;                                         /*!< Register bits. */
} psram_dmc_ctrl_hs_t;

/**
 * @brief  This union represents the bit fields  psram dmac cfg lp Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union psram_dmc_cfg_lp_t {
    uint32_t d32;                                /*!< Raw register data. */
    struct {
        uint32_t dram_clk_dis_en               : 1;
        uint32_t reserved                      : 3;
        uint32_t dram_clk_dis_prd              : 8;
        uint32_t exec_clkon                    : 1;
        uint32_t que_clkon                     : 1;
        uint32_t ws_clkon                      : 1;
        uint32_t rp_clkon                      : 1;
        uint32_t ddrca_clkon                   : 1;
        uint32_t reserved2                     : 3;
        uint32_t power_opt_en                  : 1;
        uint32_t reserved3                     : 11;
    } b;                                         /*!< Register bits. */
} psram_dmc_cfg_lp_t;

/**
 * @brief  This union represents the bit fields  psram dmac mem mode Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union psram_dmc_mem_mode_t {
    uint32_t d32;                                /*!< Raw register data. */
    struct {
        uint32_t mem_type                      : 4;
        uint32_t mem_width                     : 2;
        uint32_t reserved                      : 2;
        uint32_t mem_col                       : 2;
        uint32_t mem_col_ext                   : 1;
        uint32_t reserved2                     : 1;
        uint32_t mem_row                       : 3;
        uint32_t reserved3                     : 5;
        uint32_t wrap_en                       : 1;
        uint32_t reserved4                     : 3;
        uint32_t latency_type                  : 1;
        uint32_t reserved5                     : 3;
        uint32_t pra_en                        : 1;
        uint32_t reserved6                     : 3;
    } b;                                         /*!< Register bits. */
} psram_dmc_mem_mode_t;

/**
 * @brief  This union represents the bit fields  psram dmac work mode Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union psram_dmc_work_mode_t {
    uint32_t d32;                                /*!< Raw register data. */
    struct {
        uint32_t clk_ratio                     : 1;
        uint32_t reserved                      : 3;
        uint32_t burst_len                     : 2;
        uint32_t reserved2                     : 2;
        uint32_t addr_push_en                  : 1;
        uint32_t mid_push_en                   : 1;
        uint32_t reserved3                     : 2;
        uint32_t ws_data_ready_num             : 8;
        uint32_t ws_release_opt                : 1;
        uint32_t reserved4                     : 3;
        uint32_t ap_rlat                       : 4;
        uint32_t ap_vl_opt                     : 1;
        uint32_t cs_msk                        : 1;
        uint32_t rd_merge_cnt_auto_clr         : 1;
        uint32_t reserved5                     : 1;
    } b;                                         /*!< Register bits. */
} psram_dmc_work_mode_t;

/**
 * @brief  This union represents the bit fields  psram dmac cfg timing 0 Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union psram_dmc_cfg_timing0_t {
    uint32_t d32;                                /*!< Raw register data. */
    struct {
        uint32_t trl                           : 5;
        uint32_t reserved                      : 3;
        uint32_t twl                           : 5;
        uint32_t reserved2                     : 3;
        uint32_t tcphr                         : 6;
        uint32_t reserved3                     : 2;
        uint32_t tcphw                         : 6;
        uint32_t tdqsck                        : 2;
    } b;                                         /*!< Register bits. */
} psram_dmc_cfg_timing0_t;

/**
 * @brief  This union represents the bit fields  psram dmac cfg timing 1 Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union psram_dmc_cfg_timing1_t {
    uint32_t d32;                                /*!< Raw register data. */
    struct {
        uint32_t ths                           : 18;
        uint32_t reserved                      : 2;
        uint32_t tcss                          : 4;
        uint32_t trfc                          : 8;
    } b;                                         /*!< Register bits. */
} psram_dmc_cfg_timing1_t;

/**
 * @brief  This union represents the bit fields  psram dmac cfg timing 2 Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union psram_dmc_cfg_timing2_t {
    uint32_t d32;                                /*!< Raw register data. */
    struct {
        uint32_t txhs                          : 18;
        uint32_t reserved                      : 2;
        uint32_t txphs                         : 6;
        uint32_t reserved2                     : 6;
    } b;                                        /*!< Register bits. */
} psram_dmc_cfg_timing2_t;

/**
 * @brief  This union represents the bit fields  psram dmac cfg timing 3 Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union psram_dmc_cfg_timing3_t {
    uint32_t d32;                                /*!< Raw register data. */
    struct {
        uint32_t tdpd                          : 18;
        uint32_t reserved                      : 2;
        uint32_t trst                          : 12;
    } b;                                        /*!< Register bits. */
} psram_dmc_cfg_timing3_t;

/**
 * @brief  This union represents the bit fields  psram dmac cfg timing 4 Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union psram_dmc_cfg_timing4_t {
    uint32_t d32;                                /*!< Raw register data. */
    struct {
        uint32_t txdpd                         : 18;
        uint32_t reserved                      : 2;
        uint32_t txpdpd                        : 6;
        uint32_t reserved2                     : 6;
    } b;                                        /*!< Register bits. */
} psram_dmc_cfg_timing4_t;

/**
 * @brief  This union represents the bit fields  psram dmac cfg lat Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union psram_dmc_cfg_lat_t {
    uint32_t d32;                                /*!< Raw register data. */
    struct {
        uint32_t wden_lat                      : 4;
        uint32_t wdata_lat                     : 4;
        uint32_t rden_lat                      : 4;
        uint32_t cs_lat                        : 4;
        uint32_t cmd_lat                       : 4;
        uint32_t reserved                      : 12;
    } b;                                        /*!< Register bits. */
} psram_dmc_cfg_lat_t;

/**
 * @brief  This union represents the bit fields  psram dmac cfg cs ext Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union psram_dmc_cfg_cs_ext_t {
    uint32_t d32;                                /*!< Raw register data. */
    struct {
        uint32_t wr_cs_ext                     : 2;
        uint32_t reserved                      : 2;
        uint32_t rd_cs_ext                     : 2;
        uint32_t reserved2                     : 2;
        uint32_t mrw_cs_ext                    : 2;
        uint32_t reserved3                     : 2;
        uint32_t wden_ext                      : 1;
        uint32_t reserved4                     : 19;
    } b;                                        /*!< Register bits. */
} psram_dmc_cfg_cs_ext_t;

/**
 * @brief  This union represents the bit fields  psram dmac ws buf Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union psram_dmc_ws_buf_t {
    uint32_t d32;                                /*!< Raw register data. */
    struct {
        uint32_t ws_buf_pri1                   : 3;
        uint32_t ws_buf_lvl                    : 1;
        uint32_t ws_buf_ptun_en                : 1;
        uint32_t reserved                      : 27;
    } b;                                        /*!< Register bits. */
} psram_dmc_ws_buf_t;

/**
 * @brief  This union represents the bit fields  psram dmac cfg sfc cmd Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union psram_dmc_cfg_sfc_cmd_t {
    uint32_t d32;                                /*!< Raw register data. */
    struct {
        uint32_t sfc_cmd_type                  : 4;
        uint32_t reserved                      : 28;
    } b;                                        /*!< Register bits. */
} psram_dmc_cfg_sfc_cmd_t;

/**
 * @brief  Registers associated with psram axi. offset:0x0~0x24
 */
typedef struct psram_axi_regs_t {
    volatile uint32_t axi_rtl_inf;
    volatile uint32_t axi_cfg_lock;
    volatile uint32_t axi_ckg;
    volatile uint32_t axi_ckg_rd;
    volatile uint32_t axi_ckg_wd;
    volatile uint32_t reserved;
    volatile uint32_t reserved2;
    volatile uint32_t reserved3;
    volatile uint32_t axi_action;
    volatile uint32_t axi_rw_arb_dis;
} psram_axi_regs_t;

/**
 * @brief  Registers associated with psram axi region. offset:0x100~0x108
 */
typedef struct psram_axi_region_regs_t {
    volatile uint32_t axi_region_map;
    volatile uint32_t axi_region_attrib;
    volatile uint32_t axi_region_srcmbl;
} psram_axi_region_regs_t;

/**
 * @brief  Registers associated with psram axi qos. offset:0x200~0x20C
 */
typedef struct psram_axi_qos_regs_t {
    volatile uint32_t axi_qos_map;
    volatile uint32_t axi_qos_wrpri;
    volatile uint32_t axi_qos_rdpri;
    volatile uint32_t axi_qos_adpt;
} psram_axi_qos_regs_t;

/**
 * @brief  This union represents the bit fields  psram axi region map Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union psram_axi_region_map_t {
    uint32_t d32;                                /*!< Raw register data. */
    struct {
        uint32_t rgn_base_addr                 : 12;
        uint32_t rgn_size                      : 3;
        uint32_t reserved                      : 1;
        uint32_t rgn_en                        : 1;
        uint32_t reserved2                     : 15;
    } b;                                        /*!< Register bits. */
} psram_axi_region_map_t;

/**
 * @brief  This union represents the bit fields  psram axi region attr Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union psram_axi_region_attr_t {
    uint32_t d32;                                /*!< Raw register data. */
    struct {
        uint32_t ch_start                      : 2;
        uint32_t ch_mode                       : 2;
        uint32_t ch_intlv                      : 2;
        uint32_t reserved                      : 2;
        uint32_t ch_offset                     : 8;
        uint32_t addr_aligned                  : 3;
        uint32_t reserved2                     : 1;
        uint32_t ecc_byp                       : 1;
        uint32_t reserved3                     : 3;
        uint32_t rnk_mod                       : 2;
        uint32_t reserved4                     : 2;
        uint32_t ch_offset_ext                 : 4;
    } b;                                        /*!< Register bits. */
} psram_axi_region_attr_t;

/**
 * @brief  This union represents the bit fields  psram axi action Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union psram_axi_action_t {
    uint32_t d32;                                /*!< Raw register data. */
    struct {
        uint32_t exclusive_en                   : 1;
        uint32_t rd_wrap_split_en               : 1;
        uint32_t ecc_rmw_en                     : 1;
        uint32_t reserved                       : 1;
        uint32_t rd_wrap_boundary               : 3;
        uint32_t reserved2                      : 1;
        uint32_t wr_rcv_mode                    : 12;
        uint32_t axi_resp_mask                  : 1;
        uint32_t reserved3                      : 11;
    } b;                                        /*!< Register bits. */
} psram_axi_action_t;


/**
 * @brief  This union represents the bit fields  psram axi qos wrpri Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union psram_axi_qos_wrpri_t {
    uint32_t d32;                                /*!< Raw register data. */
    struct {
        uint32_t wr_pri0                         : 3;
        uint32_t reserved                        : 1;
        uint32_t wr_pri1                         : 3;
        uint32_t reserved2                       : 1;
        uint32_t wr_pri2                         : 3;
        uint32_t reserved3                       : 1;
        uint32_t wr_pri3                         : 3;
        uint32_t reserved4                       : 1;
        uint32_t wr_pri4                         : 3;
        uint32_t reserved5                       : 1;
        uint32_t wr_pri5                         : 3;
        uint32_t reserved6                       : 1;
        uint32_t wr_pri6                         : 3;
        uint32_t reserved7                       : 1;
        uint32_t wr_pri7                         : 3;
        uint32_t reserved8                       : 1;
    } b;                                        /*!< Register bits. */
} psram_axi_qos_wrpri_t;

/**
 * @brief  This union represents the bit fields  psram axi qos rdpri Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union psram_axi_qos_rdpri_t {
    uint32_t d32;                                /*!< Raw register data. */
    struct {
        uint32_t rd_pri0                         : 3;
        uint32_t reserved                        : 1;
        uint32_t rd_pri1                         : 3;
        uint32_t reserved2                       : 1;
        uint32_t rd_pri2                         : 3;
        uint32_t reserved3                       : 1;
        uint32_t rd_pri3                         : 3;
        uint32_t reserved4                       : 1;
        uint32_t rd_pri4                         : 3;
        uint32_t reserved5                       : 1;
        uint32_t rd_pri5                         : 3;
        uint32_t reserved6                       : 1;
        uint32_t rd_pri6                         : 3;
        uint32_t reserved7                       : 1;
        uint32_t rd_pri7                         : 3;
        uint32_t reserved8                       : 1;
    } b;                                        /*!< Register bits. */
} psram_axi_qos_rdpri_t;

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif