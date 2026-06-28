/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: vicap register
 * Author: AuthorNameMagicTag
 * Create: 2025-06-30
 */

#ifndef VICAP_REG_H
#define VICAP_REG_H

#define VICAP_PT_NUM  1
#define VICAP_CHN_NUM 2

/* Define the union u_wk_mode */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    power_mode            : 1   ; /* [0]  */
        unsigned int    axi_clk_en            : 1   ; /* [1]  */
        unsigned int    reserved_0            : 30  ; /* [31..2]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_wk_mode;

/* Define the union u_memctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved_0 : 3; /* [2..0]  */
        unsigned int rfs_emaw : 2;   /* [4..3]  */
        unsigned int rfs_ema : 3;    /* [7..5]  */
        unsigned int reserved_1 : 1; /* [8]  */
        unsigned int    rft_emaa              : 3   ; /* [11..9]  */
        unsigned int    rft_emab              : 3   ; /* [14..12]  */
        unsigned int rft_emasa : 1;  /* [15]  */
        unsigned int reserved_2 : 6; /* [21..16]  */
        unsigned int ras_ema : 3;    /* [24..22]  */
        unsigned int ras_emaw : 2;   /* [26..25]  */
        unsigned int ras_emas : 1;   /* [27]  */
        unsigned int    reserved_3            : 4   ; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_memctrl;

/* Define the union u_memctrl1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved_0 : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_memctrl1;

/* Define the union u_axi_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 4   ; /* [3..0]  */
        unsigned int    w_outstanding         : 4   ; /* [7..4]  */
        unsigned int    r_outstanding         : 4   ; /* [11..8]  */
        unsigned int    reserved_1            : 19  ; /* [30..12]  */
        unsigned int    axi0_aline32_en       : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_axi_cfg;

/* Define the union u_mac_cfg */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int mac_cfg                : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_mac_cfg;
/* Define the union u_axi_rd_prot */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    src0_prot             : 1   ; /* [0]  */
        unsigned int    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_axi_rd_prot;

/* Define the union u_axi_wr_prot */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    ch0_prot              : 1   ; /* [0]  */
        unsigned int    ch1_prot              : 1   ; /* [1]  */
        unsigned int    reserved_0            : 6   ; /* [7..2]  */
        unsigned int    isp0_stt_prot         : 1   ; /* [8]  */
        unsigned int    isp1_stt_prot         : 1   ; /* [9]  */
        unsigned int    reserved_1            : 22  ; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_axi_wr_prot;

/* Define the union u_pt_sel */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pt0_sel               : 1   ; /* [0]  */
        unsigned int    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_pt_sel;

/* Define the union u_pt_mux */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pt0_mux               : 4   ; /* [3..0]  */
        unsigned int    pt1_mux               : 4   ; /* [7..4]  */
        unsigned int    reserved_0            : 24  ; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_pt_mux;

/* Define the union u_line_afifo_mode */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    line_afifo0_mode      : 1   ; /* [0]  */
        unsigned int    reserved_0            : 7   ; /* [7..1]  */
        unsigned int    line_afifo0_en        : 1   ; /* [8]  */
        unsigned int    reserved_1            : 23  ; /* [31..9]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_line_afifo_mode;

/* Define the union u_line_afifo_blk0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    line_afifo_blank0     : 16  ; /* [15..0]  */
        unsigned int    reserved_0            : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_line_afifo_blk0;

/* Define the union u_cmux_sel */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    chn0_sel              : 4   ; /* [3..0]  */
        unsigned int    chn1_sel              : 4   ; /* [7..4]  */
        unsigned int    reserved_0            : 24  ; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_cmux_sel;

/* Define the union u_cmux_id_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    chn0_id               : 4   ; /* [3..0]  */
        unsigned int    chn1_id               : 4   ; /* [7..4]  */
        unsigned int    reserved_0            : 24  ; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_cmux_id_cfg;

/* Define the union u_isp_mode */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp0_mode             : 1   ; /* [0]  */
        unsigned int    isp1_mode             : 1   ; /* [1]  */
        unsigned int    reserved_0            : 14  ; /* [15..2]  */
        unsigned int    isp0_in_yuv_mode      : 1   ; /* [16]  */
        unsigned int    isp1_in_yuv_mode      : 1   ; /* [17]  */
        unsigned int    reserved_1            : 6   ; /* [23..18]  */
        unsigned int    isp0_clk_en           : 1   ; /* [24]  */
        unsigned int    isp1_clk_en           : 1   ; /* [25]  */
        unsigned int    reserved_2            : 6   ; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_mode;

/* Define the union u_apb_timeout */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    apb_timeout           : 16  ; /* [15..0]  */
        unsigned int    apb_timeout_en        : 1   ; /* [16]  */
        unsigned int    reserved_0            : 15  ; /* [31..17]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_apb_timeout;

/* Define the union u_vicap_int1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    int1_pt0              : 1   ; /* [0]  */
        unsigned int    int1_pt1              : 1   ; /* [1]  */
        unsigned int    reserved_0            : 6   ; /* [7..2]  */
        unsigned int    int1_ch0              : 1   ; /* [8]  */
        unsigned int    int1_ch1              : 1   ; /* [9]  */
        unsigned int    reserved_1            : 6   ; /* [15..10]  */
        unsigned int    int1_isp0             : 1   ; /* [16]  */
        unsigned int    int1_isp1             : 1   ; /* [17]  */
        unsigned int    reserved_2            : 6   ; /* [23..18]  */
        unsigned int    int1_src0             : 1   ; /* [24]  */
        unsigned int    reserved_3            : 1   ; /* [25]  */
        unsigned int    int1_dbg              : 1   ; /* [26]  */
        unsigned int    reserved_4            : 5   ; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_vicap_int1;

/* Define the union u_vicap_int1_mask */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    mask1_pt0             : 1   ; /* [0]  */
        unsigned int    mask1_pt1             : 1   ; /* [1]  */
        unsigned int    reserved_0            : 6   ; /* [7..2]  */
        unsigned int    mask1_ch0             : 1   ; /* [8]  */
        unsigned int    mask1_ch1             : 1   ; /* [9]  */
        unsigned int    reserved_1            : 6   ; /* [15..10]  */
        unsigned int    mask1_isp0            : 1   ; /* [16]  */
        unsigned int    mask1_isp1            : 1   ; /* [17]  */
        unsigned int    reserved_2            : 6   ; /* [23..18]  */
        unsigned int    mask1_src0            : 1   ; /* [24]  */
        unsigned int    reserved_3            : 1   ; /* [25]  */
        unsigned int    mask1_dbg             : 1   ; /* [26]  */
        unsigned int    reserved_4            : 5   ; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_vicap_int1_mask;

/* Define the union u_vicap_int0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    int0_pt0              : 1   ; /* [0]  */
        unsigned int    int0_pt1              : 1   ; /* [1]  */
        unsigned int    reserved_0            : 6   ; /* [7..2]  */
        unsigned int    int0_ch0              : 1   ; /* [8]  */
        unsigned int    int0_ch1              : 1   ; /* [9]  */
        unsigned int    reserved_1            : 6   ; /* [15..10]  */
        unsigned int    int0_isp0             : 1   ; /* [16]  */
        unsigned int    int0_isp1             : 1   ; /* [17]  */
        unsigned int    reserved_2            : 6   ; /* [23..18]  */
        unsigned int    int0_src0             : 1   ; /* [24]  */
        unsigned int    reserved_3            : 1   ; /* [25]  */
        unsigned int    int0_dbg              : 1   ; /* [26]  */
        unsigned int    reserved_4            : 5   ; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_vicap_int0;

/* Define the union u_vicap_int0_mask */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    mask0_pt0             : 1   ; /* [0]  */
        unsigned int    mask0_pt1             : 1   ; /* [1]  */
        unsigned int    reserved_0            : 6   ; /* [7..2]  */
        unsigned int    mask0_ch0             : 1   ; /* [8]  */
        unsigned int    mask0_ch1             : 1   ; /* [9]  */
        unsigned int    reserved_1            : 6   ; /* [15..10]  */
        unsigned int    mask0_isp0            : 1   ; /* [16]  */
        unsigned int    mask0_isp1            : 1   ; /* [17]  */
        unsigned int    reserved_2            : 6   ; /* [23..18]  */
        unsigned int    mask0_src0            : 1   ; /* [24]  */
        unsigned int    reserved_3            : 1   ; /* [25]  */
        unsigned int    mask0_dbg             : 1   ; /* [26]  */
        unsigned int    reserved_4            : 5   ; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_vicap_int0_mask;

/* Define the union u_slave0_mode_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    slave0_update_mode    : 1   ; /* [0]  */
        unsigned int    reserved_0            : 15  ; /* [15..1]  */
        unsigned int    slave0_hs_inv         : 1   ; /* [16]  */
        unsigned int    slave0_vs_inv         : 1   ; /* [17]  */
        unsigned int    reserved_1            : 12  ; /* [29..18]  */
        unsigned int    slave0_hs_enable      : 1   ; /* [30]  */
        unsigned int    slave0_vs_enable      : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_slave0_mode_cfg;

/* Define the union u_slave0_mode_vs_time_h */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int slave0_vs_time_h       : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_slave0_mode_vs_time_h;
/* Define the union u_slave0_mode_vs_time_l */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int slave0_vs_time_l       : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_slave0_mode_vs_time_l;
/* Define the union u_slave0_mode_hs_time */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int slave0_hs_time         : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_slave0_mode_hs_time;
/* Define the union u_slave0_mode_vs_cyc_h */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int slave0_vs_cyc_h        : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_slave0_mode_vs_cyc_h;
/* Define the union u_slave0_mode_vs_cyc_l */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int slave0_vs_cyc_l        : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_slave0_mode_vs_cyc_l;
/* Define the union u_slave0_mode_hs_cyc */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int slave0_hs_cyc          : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_slave0_mode_hs_cyc;
/* Define the union u_slave0_mode_hs_dly_cyc */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int slave0_hs_dly_cyc      : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_slave0_mode_hs_dly_cyc;
/* Define the union u_slave0_mode_vs_dly_cyc_h */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int slave0_vs_dly_cyc_h    : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_slave0_mode_vs_dly_cyc_h;
/* Define the union u_slave0_mode_vs_dly_cyc_l */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int slave0_vs_dly_cyc_l    : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_slave0_mode_vs_dly_cyc_l;
/* Define the union u_slave1_mode_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    slave1_update_mode    : 1   ; /* [0]  */
        unsigned int    reserved_0            : 15  ; /* [15..1]  */
        unsigned int    slave1_hs_inv         : 1   ; /* [16]  */
        unsigned int    slave1_vs_inv         : 1   ; /* [17]  */
        unsigned int    reserved_1            : 12  ; /* [29..18]  */
        unsigned int    slave1_hs_enable      : 1   ; /* [30]  */
        unsigned int    slave1_vs_enable      : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_slave1_mode_cfg;

/* Define the union u_slave1_mode_vs_time_h */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int slave1_vs_time_h       : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_slave1_mode_vs_time_h;
/* Define the union u_slave1_mode_vs_time_l */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int slave1_vs_time_l       : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_slave1_mode_vs_time_l;
/* Define the union u_slave1_mode_hs_time */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int slave1_hs_time         : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_slave1_mode_hs_time;
/* Define the union u_slave1_mode_vs_cyc_h */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int slave1_vs_cyc_h        : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_slave1_mode_vs_cyc_h;
/* Define the union u_slave1_mode_vs_cyc_l */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int slave1_vs_cyc_l        : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_slave1_mode_vs_cyc_l;
/* Define the union u_slave1_mode_hs_cyc */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int slave1_hs_cyc          : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_slave1_mode_hs_cyc;
/* Define the union u_slave1_mode_hs_dly_cyc */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int slave1_hs_dly_cyc      : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_slave1_mode_hs_dly_cyc;
/* Define the union u_slave1_mode_vs_dly_cyc_h */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int slave1_vs_dly_cyc_h    : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_slave1_mode_vs_dly_cyc_h;
/* Define the union u_slave1_mode_vs_dly_cyc_l */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int slave1_vs_dly_cyc_l    : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_slave1_mode_vs_dly_cyc_l;
/* Define the union u_slave_mode_time_cfg_sel */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    sensor0_time_sel      : 2   ; /* [1..0]  */
        unsigned int    sensor1_time_sel      : 2   ; /* [3..2]  */
        unsigned int    reserved_0            : 28  ; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_slave_mode_time_cfg_sel;

/* Define the union u_slave_mode_syn */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    sensor0_sync          : 1   ; /* [0]  */
        unsigned int    sensor1_sync          : 1   ; /* [1]  */
        unsigned int    reserved_0            : 30  ; /* [31..2]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_slave_mode_syn;

/* Define the union u_online0_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    online0_hsync_mode    : 1   ; /* [0]  */
        unsigned int    online0_vsync_mode    : 1   ; /* [1]  */
        unsigned int    reserved_0            : 29  ; /* [30..2]  */
        unsigned int    online0_en            : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_online0_cfg;

/* Define the union u_online0_des0_info */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int online0_des0_info      : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_online0_des0_info;
/* Define the union u_online0_last_des0_info */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int online0_last_des0_info : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_online0_last_des0_info;
/* Define the union u_online0_size_total */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    online0_total_width   : 16  ; /* [15..0]  */
        unsigned int    reserved_0            : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_online0_size_total;

/* Define the union u_online0_size */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    online0_width         : 16  ; /* [15..0]  */
        unsigned int    online0_height        : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_online0_size;
/* Define the union u_vicap_isp0_in_crop_cfg */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_vicap_isp0_in_crop_cfg;
/* Define the union u_vicap_afifo0_start */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    afifo0_x_start        : 16  ; /* [15..0]  */
        unsigned int    afifo0_y_start        : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_vicap_afifo0_start;

/* Define the union u_vicap_afifo0_size */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    afifo0_width          : 16  ; /* [15..0]  */
        unsigned int    afifo0_height         : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_vicap_afifo0_size;

/* define afifo crop struct */
typedef struct {
    u_vicap_isp0_in_crop_cfg        vicap_isp_in_crop_cfg           ; /* 0x300 */
    unsigned int                    reserved_20                      ; /* 0x304 */
    u_vicap_afifo0_start            vicap_afifo_start               ; /* 0x308 */
    u_vicap_afifo0_size             vicap_afifo_size                ; /* 0x30c */
} u_vicap_afifo_crop;

/* Define the union u_isp0_in_size_total */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp0_in_total_width   : 16  ; /* [15..0]  */
        unsigned int    reserved_0            : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp0_in_size_total;

/* Define the union u_isp0_in_size */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp0_in_width         : 16  ; /* [15..0]  */
        unsigned int    isp0_in_height        : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp0_in_size;

/* Define the union u_vicap_latency_det_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    lantency_det_enable   : 1   ; /* [0]  */
        unsigned int    lantency_det_clr      : 1   ; /* [1]  */
        unsigned int    reserved_0            : 30  ; /* [31..2]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_vicap_latency_det_cfg;

/* Define the union u_vicap_axi0_rd_latency */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    axi0_rd_max_latency   : 16  ; /* [15..0]  */
        unsigned int    axi0_rd_ave_latency   : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_vicap_axi0_rd_latency;

/* Define the union u_vicap_axi0_rd_latency_overflow */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    axi0_rd_overflow_latency1 : 16  ; /* [15..0]  */
        unsigned int    axi0_rd_overflow_latency0 : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_vicap_axi0_rd_latency_overflow;

/* Define the union u_vicap_axi_status0 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int axi_src_status0        : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_vicap_axi_status0;
/* Define the union u_vicap_axi_status1 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int axi_src_status1        : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_vicap_axi_status1;
/* Define the union u_vicap_axi1_rd_latency */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_vicap_axi1_rd_latency;
/* Define the union u_vicap_axi1_rd_latency_overflow */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_vicap_axi1_rd_latency_overflow;
/* Define the union u_vicap_pt0_aff_status */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int pt0_aff_status         : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_vicap_pt0_aff_status;
/* Define the union u_vicap_pt1_aff_status */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int pt1_aff_status         : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_vicap_pt1_aff_status;
/* Define the union u_vicap_isp0_line_aff_status */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_vicap_isp0_line_aff_status;
/* Define the union u_vicap_isp0_aff_status */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_vicap_isp0_aff_status;
/* Define the union u_vicap_axi0_rd_error */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    src0_bus_err          : 1   ; /* [0]  */
        unsigned int    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_vicap_axi0_rd_error;

/* Define the union u_vicap_axi0_wr_error */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    ch0_y_bus_err         : 1   ; /* [0]  */
        unsigned int    ch0_c_bus_err         : 1   ; /* [1]  */
        unsigned int    ch1_y_bus_err         : 1   ; /* [2]  */
        unsigned int    ch1_c_bus_err         : 1   ; /* [3]  */
        unsigned int    ch2_y_bus_err         : 1   ; /* [4]  */
        unsigned int    ch2_c_bus_err         : 1   ; /* [5]  */
        unsigned int    ch3_y_bus_err         : 1   ; /* [6]  */
        unsigned int    ch3_c_bus_err         : 1   ; /* [7]  */
        unsigned int    reserved_0            : 8   ; /* [15..8]  */
        unsigned int    ch2_em_bus_err        : 1   ; /* [16]  */
        unsigned int    reserved_1            : 5   ; /* [21..17]  */
        unsigned int    isp0_stt_bus_err      : 1   ; /* [22]  */
        unsigned int    isp1_stt_bus_err      : 1   ; /* [23]  */
        unsigned int    isp2_stt_bus_err      : 1   ; /* [24]  */
        unsigned int    isp3_stt_bus_err      : 1   ; /* [25]  */
        unsigned int    reserved_2            : 1   ; /* [26]  */
        unsigned int    ch0_tunl_bus_err      : 1   ; /* [27]  */
        unsigned int    ch1_tunl_bus_err      : 1   ; /* [28]  */
        unsigned int    reserved_3            : 3   ; /* [31..29]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_vicap_axi0_wr_error;

/* Define the union u_vicap_dbg_int */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    axi_bus_err           : 1   ; /* [0]  */
        unsigned int    apb_bus_err           : 1   ; /* [1]  */
        unsigned int    reserved_0            : 30  ; /* [31..2]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_vicap_dbg_int;

/* Define the union u_vicap_dbg_int_mask */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    axi_bus_err_mask      : 1   ; /* [0]  */
        unsigned int    apb_bus_err_mask      : 1   ; /* [1]  */
        unsigned int    reserved_0            : 30  ; /* [31..2]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_vicap_dbg_int_mask;

/* Define the union u_pt_intf_mod */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pt_mode               : 1   ; /* [0]  */
        unsigned int    reserved_0            : 30  ; /* [30..1]  */
        unsigned int    pt_enable             : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_pt_intf_mod;

/* Define the union u_pt_offset0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    offset0               : 6   ; /* [5..0]  */
        unsigned int    reserved_0            : 9   ; /* [14..6]  */
        unsigned int    offset0_rev           : 1   ; /* [15]  */
        unsigned int    offset0_mask          : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_pt_offset0;

/* Define the union u_pt_offset1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    offset1               : 6   ; /* [5..0]  */
        unsigned int    reserved_0            : 9   ; /* [14..6]  */
        unsigned int    offset1_rev           : 1   ; /* [15]  */
        unsigned int    offset1_mask          : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_pt_offset1;

/* Define the union u_pt_offset2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    offset2               : 6   ; /* [5..0]  */
        unsigned int    reserved_0            : 9   ; /* [14..6]  */
        unsigned int    offset2_rev           : 1   ; /* [15]  */
        unsigned int    offset2_mask          : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_pt_offset2;

/* Define the union u_pt_bt656_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    bt656_mode            : 4   ; /* [3..0]  */
        unsigned int    reserved_0            : 4   ; /* [7..4]  */
        unsigned int    bt656_hsync_inv       : 1   ; /* [8]  */
        unsigned int    bt656_vsync_inv       : 1   ; /* [9]  */
        unsigned int    bt656_field_inv       : 1   ; /* [10]  */
        unsigned int    reserved_1            : 20  ; /* [30..11]  */
        unsigned int    bt656_enable          : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_pt_bt656_cfg;

/* Define the union u_pt_unify_timing_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    unify_de_sel          : 2   ; /* [1..0]  */
        unsigned int    unify_de_inv          : 1   ; /* [2]  */
        unsigned int    reserved_0            : 5   ; /* [7..3]  */
        unsigned int    unify_hsync_sel       : 2   ; /* [9..8]  */
        unsigned int    unify_hsync_inv       : 1   ; /* [10]  */
        unsigned int    unify_hsync_and       : 2   ; /* [12..11]  */
        unsigned int    unify_hsync_mode      : 2   ; /* [14..13]  */
        unsigned int    reserved_1            : 1   ; /* [15]  */
        unsigned int    unify_vsync_sel       : 2   ; /* [17..16]  */
        unsigned int    unify_vsync_inv       : 1   ; /* [18]  */
        unsigned int    unify_vsync_mode      : 2   ; /* [20..19]  */
        unsigned int    reserved_2            : 3   ; /* [23..21]  */
        unsigned int    unify_field_sel       : 2   ; /* [25..24]  */
        unsigned int    unify_field_inv       : 1   ; /* [26]  */
        unsigned int    reserved_3            : 5   ; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_pt_unify_timing_cfg;

/* Define the union u_pt_gen_timing_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 1   ; /* [0]  */
        unsigned int    gen_hsync_mode        : 1   ; /* [1]  */
        unsigned int    gen_vsync_mode        : 1   ; /* [2]  */
        unsigned int    reserved_1            : 27  ; /* [29..3]  */
        unsigned int    gen_mode              : 1   ; /* [30]  */
        unsigned int    gen_enable            : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_pt_gen_timing_cfg;

/* Define the union u_pt_unify_data_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    unify_data_comp_num   : 2   ; /* [1..0]  */
        unsigned int    unify_data_yc_seq     : 1   ; /* [2]  */
        unsigned int    unify_data_uv_seq     : 1   ; /* [3]  */
        unsigned int    reserved_0            : 27  ; /* [30..4]  */
        unsigned int    unify_data_enable     : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_pt_unify_data_cfg;

/* Define the union u_pt_gen_data_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    gen_data_de_move      : 1   ; /* [0]  */
        unsigned int    gen_data_hsync_move   : 1   ; /* [1]  */
        unsigned int    gen_data_vsync_move   : 1   ; /* [2]  */
        unsigned int    gen_data_hsync_reset  : 1   ; /* [3]  */
        unsigned int    gen_data_vsync_reset  : 1   ; /* [4]  */
        unsigned int    gen_data2_move        : 1   ; /* [5]  */
        unsigned int    gen_data1_move        : 1   ; /* [6]  */
        unsigned int    gen_data0_move        : 1   ; /* [7]  */
        unsigned int    reserved_0            : 23  ; /* [30..8]  */
        unsigned int    gen_data_enable       : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_pt_gen_data_cfg;

/* Define the union u_pt_gen_data_coef */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    step_space            : 8   ; /* [7..0]  */
        unsigned int    inc_space             : 8   ; /* [15..8]  */
        unsigned int    step_frame            : 8   ; /* [23..16]  */
        unsigned int    inc_frame             : 8   ; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_pt_gen_data_coef;

/* Define the union u_pt_gen_data_init */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    data0                 : 8   ; /* [7..0]  */
        unsigned int    data1                 : 8   ; /* [15..8]  */
        unsigned int    data2                 : 8   ; /* [23..16]  */
        unsigned int    reserved_0            : 8   ; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_pt_gen_data_init;

/* Define the union u_pt_yuv444_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 31  ; /* [30..0]  */
        unsigned int    yuv44_enable          : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_pt_yuv444_cfg;

/* Define the union u_pt_fstart_dly */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int pt_fstart_dly          : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_pt_fstart_dly;
/* Define the union u_pt_fstart_h_dly */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    fstart_h_dly_id       : 4   ; /* [3..0]  */
        unsigned int    reserved_0            : 12  ; /* [15..4]  */
        unsigned int    fstart_h_dly          : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_pt_fstart_h_dly;

/* Define the union u_pt_intf_hfb */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    hfb                   : 16  ; /* [15..0]  */
        unsigned int    reserved_0            : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_pt_intf_hfb;

/* Define the union u_pt_intf_hact */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int hact                   : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_pt_intf_hact;
/* Define the union u_pt_intf_hbb */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    hbb                   : 16  ; /* [15..0]  */
        unsigned int    reserved_0            : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_pt_intf_hbb;

/* Define the union u_pt_intf_vfb */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    vfb                   : 16  ; /* [15..0]  */
        unsigned int    reserved_0            : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_pt_intf_vfb;

/* Define the union u_pt_intf_vact */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    vact                  : 16  ; /* [15..0]  */
        unsigned int    reserved_0            : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_pt_intf_vact;

/* Define the union u_pt_intf_vbb */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    vbb                   : 16  ; /* [15..0]  */
        unsigned int    reserved_0            : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_pt_intf_vbb;

/* Define the union u_pt_intf_vbfb */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    vbfb                  : 16  ; /* [15..0]  */
        unsigned int    reserved_0            : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_pt_intf_vbfb;

/* Define the union u_pt_intf_vbact */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    vbact                 : 16  ; /* [15..0]  */
        unsigned int    reserved_0            : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_pt_intf_vbact;

/* Define the union u_pt_intf_vbbb */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    vbbb                  : 16  ; /* [15..0]  */
        unsigned int    reserved_0            : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_pt_intf_vbbb;

/* Define the union u_pt_id_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    id                    : 2   ; /* [1..0]  */
        unsigned int    id_max                : 2   ; /* [3..2]  */
        unsigned int    id_det                : 2   ; /* [5..4]  */
        unsigned int    reserved_0            : 23  ; /* [28..6]  */
        unsigned int    id_reset              : 1   ; /* [29]  */
        unsigned int    id_mode               : 1   ; /* [30]  */
        unsigned int    id_enable             : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_pt_id_cfg;

/* Define the union u_pt_id_sel */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    id_sel                : 4   ; /* [3..0]  */
        unsigned int    reserved_0            : 28  ; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_pt_id_sel;
/* Define the union u_pt_size_total1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pt_vbb_size           : 16  ; /* [15..0]  */
        unsigned int    pt_vfb_size           : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_pt_size_total1;

/* Define the union u_pt_flash_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    flash_en              : 1   ; /* [0]  */
        unsigned int    flash_phase           : 1   ; /* [1]  */
        unsigned int    reserved_0            : 14  ; /* [15..2]  */
        unsigned int    shutter_en            : 1   ; /* [16]  */
        unsigned int    shutter_phase         : 1   ; /* [17]  */
        unsigned int    shutter_times         : 1   ; /* [18]  */
        unsigned int    reserved_1            : 13  ; /* [31..19]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_pt_flash_cfg;

/* Define the union u_pt_flash_cyc0 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int pt_flash_cyc0          : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_pt_flash_cyc0;
/* Define the union u_pt_flash_cyc1 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int pt_flash_cyc1          : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_pt_flash_cyc1;
/* Define the union u_pt_shutter_cyc0 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int pt_shutter_cyc0        : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_pt_shutter_cyc0;
/* Define the union u_pt_shutter_cyc1 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int pt_shutter_cyc1        : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_pt_shutter_cyc1;
/* Define the union u_pt_shutter_cyc2 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int pt_shutter_cyc2        : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_pt_shutter_cyc2;
/* Define the union u_pt_shutter_cyc3 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int pt_shutter_cyc3        : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_pt_shutter_cyc3;
/* Define the union u_pt_status */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pt_status_de          : 1   ; /* [0]  */
        unsigned int    pt_status_hsync       : 1   ; /* [1]  */
        unsigned int    pt_status_vsync       : 1   ; /* [2]  */
        unsigned int    pt_status_field       : 1   ; /* [3]  */
        unsigned int    pt_status_id          : 2   ; /* [5..4]  */
        unsigned int    reserved_0            : 26  ; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_pt_status;

/* Define the union u_pt_bt656_status */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 8   ; /* [7..0]  */
        unsigned int    pt_bt656_seav         : 8   ; /* [15..8]  */
        unsigned int    reserved_1            : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_pt_bt656_status;

/* Define the union u_pt_size_total */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pt_total_width        : 16  ; /* [15..0]  */
        unsigned int    reserved_0            : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_pt_size_total;

/* Define the union u_pt_size */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pt_width              : 16  ; /* [15..0]  */
        unsigned int    pt_height             : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_pt_size;

/* Define the union u_pt_int */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pt_fstart             : 1   ; /* [0]  */
        unsigned int    pt_width_err          : 1   ; /* [1]  */
        unsigned int    pt_height_err         : 1   ; /* [2]  */
        unsigned int    pt_fstart_h_dly       : 1   ; /* [3]  */
        unsigned int    reserved_0            : 28  ; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_pt_int;

/* Define the union u_pt_int_mask */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pt_mask_fstart        : 1   ; /* [0]  */
        unsigned int    pt_mask_width_err     : 1   ; /* [1]  */
        unsigned int    pt_mask_height_err    : 1   ; /* [2]  */
        unsigned int    pt_mask_fstart_h_dly  : 1   ; /* [3]  */
        unsigned int    reserved_0            : 28  ; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_pt_int_mask;

/* Define the union u_src_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 17  ; /* [16..0]  */
        unsigned int    src_finish_mode       : 1   ; /* [17]  */
        unsigned int    reserved_1            : 12  ; /* [29..18]  */
        unsigned int    src_reg_newer_mode    : 1   ; /* [30]  */
        unsigned int    src_enable            : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_src_ctrl;

/* Define the union u_src_reg_newer */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    src_reg_newer         : 1   ; /* [0]  */
        unsigned int    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_src_reg_newer;

/* Define the union u_src_update */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    src_update            : 1   ; /* [0]  */
        unsigned int    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_src_update;

/* Define the union u_src_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    src_bit_width         : 6   ; /* [5..0]  */
        unsigned int    src_p2_en             : 1   ; /* [6]  */
        unsigned int    reserved_0            : 24  ; /* [30..7]  */
        unsigned int    src_rch_enable        : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_src_cfg;

/* Define the union u_src_size */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    src_width             : 16  ; /* [15..0]  */
        unsigned int    src_height            : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_src_size;

/* Define the union u_src_faddr_h */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int src_faddr_h            : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_src_faddr_h;
/* Define the union u_src_faddr_l */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int src_faddr_l            : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_src_faddr_l;
/* Define the union u_src_stride */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    src_stride            : 16  ; /* [15..0]  */
        unsigned int    reserved_0            : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_src_stride;

/* Define the union u_src_buf_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    src_buf_cfg           : 16  ; /* [15..0]  */
        unsigned int    reserved_0            : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_src_buf_cfg;

/* Define the union u_src_start2valid_cnt */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int src_start2valid_cnt    : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_src_start2valid_cnt;
/* Define the union u_src_out_size_total */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    src_out_total_width   : 16  ; /* [15..0]  */
        unsigned int    reserved_0            : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_src_out_size_total;

/* Define the union u_src_out_size */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    src_out_width         : 16  ; /* [15..0]  */
        unsigned int    src_out_height        : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_src_out_size;

/* Define the union u_src_int */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    src_int_fstart        : 1   ; /* [0]  */
        unsigned int    src_int_cc_int        : 1   ; /* [1]  */
        unsigned int    src_int_buf_ovf       : 1   ; /* [2]  */
        unsigned int    src_int_field_throw   : 1   ; /* [3]  */
        unsigned int    src_int_update_cfg    : 1   ; /* [4]  */
        unsigned int    reserved_0            : 27  ; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_src_int;

/* Define the union u_src_mask */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    src_mask_fstart       : 1   ; /* [0]  */
        unsigned int    src_mask_cc_int       : 1   ; /* [1]  */
        unsigned int    src_mask_buf_ovf      : 1   ; /* [2]  */
        unsigned int    src_mask_field_throw  : 1   ; /* [3]  */
        unsigned int    src_mask_update_cfg   : 1   ; /* [4]  */
        unsigned int    reserved_0            : 27  ; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_src_mask;

/* Define the union u_ch_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 17  ; /* [16..0]  */
        unsigned int    ch_finish_mode        : 1   ; /* [17]  */
        unsigned int    reserved_1            : 12  ; /* [29..18]  */
        unsigned int    ch_reg_newer_mode     : 1   ; /* [30]  */
        unsigned int    ch_enable             : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_ch_ctrl;

/* Define the union u_ch_reg_newer */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    ch_reg_newer          : 1   ; /* [0]  */
        unsigned int    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_ch_reg_newer;

/* Define the union u_ch_update */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    ch_update             : 1   ; /* [0]  */
        unsigned int    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_ch_update;

/* Define the union u_ch_adapter_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    ch_hsync_mode         : 1   ; /* [0]  */
        unsigned int    ch_vsync_mode         : 1   ; /* [1]  */
        unsigned int    reserved_0            : 30  ; /* [31..2]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_ch_adapter_cfg;

/* Define the union u_ch_int_det_cfg */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ch_int_det_cfg         : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_ch_int_det_cfg;
/* Define the union u_ch_int_det_info */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ch_int_det_info        : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_ch_int_det_info;
/* Define the union u_ch_int_det_cnt */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ch_int_det_cnt         : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_ch_int_det_cnt;
/* Define the union u_ch_count */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ch_count               : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_ch_count;
/* Define the union u_ch_dly_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 16  ; /* [15..0]  */
        unsigned int    ch_v_dly_cfg          : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_ch_dly_cfg;

/* Define the union u_ch_frm_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 16  ; /* [15..0]  */
        unsigned int    ch_frm_cnt            : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_ch_frm_cnt;

/* Define the union u_ch_timestamp_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    ch_timestamp_pos      : 16  ; /* [15..0]  */
        unsigned int    ch_timestamp_level    : 8   ; /* [23..16]  */
        unsigned int    reserved_0            : 6   ; /* [29..24]  */
        unsigned int    ch_timestamp_mode     : 1   ; /* [30]  */
        unsigned int    ch_timestamp_en       : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_ch_timestamp_cfg;

/* Define the union u_ch_crop_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    ch_crop_n0_en         : 1   ; /* [0]  */
        unsigned int    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_ch_crop_cfg;

/* Define the union u_ch_crop0_start */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    ch_crop_x_start       : 16  ; /* [15..0]  */
        unsigned int    ch_crop_y_start       : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_ch_crop0_start;

/* Define the union u_ch_crop0_size */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    ch_crop_width         : 16  ; /* [15..0]  */
        unsigned int    ch_crop_height        : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_ch_crop0_size;

/* Define the union u_ch_skip_y_cfg */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ch_skipy_cfg           : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_ch_skip_y_cfg;
/* Define the union u_ch_skip_y_win */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    ch_skipy_width        : 3   ; /* [2..0]  */
        unsigned int    reserved_0            : 13  ; /* [15..3]  */
        unsigned int    ch_skipy_height       : 2   ; /* [17..16]  */
        unsigned int    reserved_1            : 14  ; /* [31..18]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_ch_skip_y_win;

/* Define the union u_ch_skip_c_cfg */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ch_skipc_cfg           : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_ch_skip_c_cfg;
/* Define the union u_ch_skip_c_win */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    ch_skipc_width        : 3   ; /* [2..0]  */
        unsigned int    reserved_0            : 13  ; /* [15..3]  */
        unsigned int    ch_skipc_height       : 2   ; /* [17..16]  */
        unsigned int    reserved_1            : 14  ; /* [31..18]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_ch_skip_c_win;

/* Define the union u_ch_split_cfg0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    ch_split_en           : 1   ; /* [0]  */
        unsigned int    ch_split0_invert_en   : 1   ; /* [1]  */
        unsigned int    ch_split1_invert_en   : 1   ; /* [2]  */
        unsigned int    reserved_0            : 29  ; /* [31..3]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_ch_split_cfg0;

/* Define the union u_ch_split_cfg1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ch_split_in_width : 16;  /* [15..0]  */
        unsigned int ch_split_in_height : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_ch_split_cfg1;

/* Define the union u_ch_split_cfg2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ch_split_start : 16; /* [15..0]  */
        unsigned int ch_split_width : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_ch_split_cfg2;
/* Define the union u_ch_uv2c_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 30  ; /* [29..0]  */
        unsigned int    ch_uv2c_mode          : 1   ; /* [30]  */
        unsigned int    ch_uv2c_enable        : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_ch_uv2c_cfg;

/* Define the union u_ch_wch_y_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    wch_y_bit_width       : 6   ; /* [5..0]  */
        unsigned int    reserved_0            : 1   ; /* [6]  */
        unsigned int    wch_y_bit_sft_mode    : 1   ; /* [7]  */
        unsigned int    wch_y_bit_sft         : 4   ; /* [11..8]  */
        unsigned int    wch_y_cmp_en          : 1   ; /* [12]  */
        unsigned int    wch_y_cmp_mode        : 1   ; /* [13]  */
        unsigned int    wch_y_wrap_en         : 1   ; /* [14]  */
        unsigned int    wch_y_wrap_line       : 14  ; /* [28..15]  */
        unsigned int    wch_y_tunl_en         : 1   ; /* [29]  */
        unsigned int    wch_y_32b_align       : 1   ; /* [30]  */
        unsigned int    wch_y_enable          : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_ch_wch_y_cfg;

/* Define the union u_ch_wch_y_size */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    wch_y_width           : 16  ; /* [15..0]  */
        unsigned int    wch_y_height          : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_ch_wch_y_size;

/* Define the union u_ch_wch_y_offset */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 16  ; /* [15..0]  */
        unsigned int    wch_y_ver_offset      : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_ch_wch_y_offset;

/* Define the union u_ch_wch_y_faddr_l */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int wch_y_faddr_l          : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_ch_wch_y_faddr_l;
/* Define the union u_ch_wch_y_stride */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    wch_y_stride          : 16  ; /* [15..0]  */
        unsigned int    reserved_0            : 8   ; /* [23..16]  */
        unsigned int    wch_y_fcnt            : 8   ; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_ch_wch_y_stride;

/* Define the union u_ch_wch_y_buf_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    wch_y_buf_cfg         : 16  ; /* [15..0]  */
        unsigned int    wch_y_ofs             : 8   ; /* [23..16]  */
        unsigned int    reserved_0            : 8   ; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_ch_wch_y_buf_cfg;

/* Define the union u_ch_wch_c_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    wch_c_bit_width       : 6   ; /* [5..0]  */
        unsigned int    reserved_0            : 7   ; /* [12..6]  */
        unsigned int    wch_tunl_finish_line  : 14  ; /* [26..13]  */
        unsigned int    wch_tunl_mode         : 2   ; /* [28..27]  */
        unsigned int    wch_c_tunl_en         : 1   ; /* [29]  */
        unsigned int    reserved_1            : 1   ; /* [30]  */
        unsigned int    wch_c_enable          : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_ch_wch_c_cfg;

/* Define the union u_ch_wch_c_size */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    wch_c_width           : 16  ; /* [15..0]  */
        unsigned int    wch_c_height          : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_ch_wch_c_size;

/* Define the union u_ch_wch_c_faddr_l */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int wch_c_faddr_l          : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_ch_wch_c_faddr_l;
/* Define the union u_ch_wch_tunl_haddr_l */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int wch_tunl_addr_l        : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_ch_wch_tunl_haddr_l;
/* Define the union u_ch_wch_c_stride */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    wch_c_stride          : 16  ; /* [15..0]  */
        unsigned int    reserved_0            : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_ch_wch_c_stride;

/* Define the union u_ch_wch_c_buf_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    wch_c_buf_cfg         : 16  ; /* [15..0]  */
        unsigned int    wch_c_ofs             : 8   ; /* [23..16]  */
        unsigned int    reserved_0            : 8   ; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_ch_wch_c_buf_cfg;

/* Define the union u_ch_y_act_size */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    ch_y_width_act        : 16  ; /* [15..0]  */
        unsigned int    ch_y_height_act       : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_ch_y_act_size;

/* Define the union u_ch_c_act_size */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    ch_c_width_act        : 16  ; /* [15..0]  */
        unsigned int    ch_c_height_act       : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_ch_c_act_size;

/* Define the union u_ch_int */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    ch_int_fstart         : 1   ; /* [0]  */
        unsigned int    ch_int_cc_int         : 1   ; /* [1]  */
        unsigned int    ch_int_buf_ovf        : 1   ; /* [2]  */
        unsigned int    ch_int_field_throw    : 1   ; /* [3]  */
        unsigned int    ch_int_update_cfg     : 1   ; /* [4]  */
        unsigned int    reserved_0            : 10  ; /* [14..5]  */
        unsigned int    ch_int_fstart_dly     : 1   ; /* [15]  */
        unsigned int    ch_stt_ae_finish      : 1   ; /* [16]  */
        unsigned int    reserved_1            : 1   ; /* [17]  */
        unsigned int    ch_stt_em_finish      : 1   ; /* [18]  */
        unsigned int    ch_stt_int_buf_ovf    : 1   ; /* [19]  */
        unsigned int    ch_em_int_buf_ovf     : 1   ; /* [20]  */
        unsigned int    ch_tunl_line_int      : 1   ; /* [21]  */
        unsigned int    reserved_2            : 10  ; /* [31..22]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_ch_int;

/* Define the union u_ch_int_mask */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    mask_fstart           : 1   ; /* [0]  */
        unsigned int    mask_cc_int           : 1   ; /* [1]  */
        unsigned int    mask_buf_ovf          : 1   ; /* [2]  */
        unsigned int    mask_field_throw      : 1   ; /* [3]  */
        unsigned int    mask_update_cfg       : 1   ; /* [4]  */
        unsigned int    reserved_0            : 10  ; /* [14..5]  */
        unsigned int    mask_fstart_dly       : 1   ; /* [15]  */
        unsigned int    mask_stt_ae_finish    : 1   ; /* [16]  */
        unsigned int    reserved_1            : 1   ; /* [17]  */
        unsigned int    mask_stt_em_finish    : 1   ; /* [18]  */
        unsigned int    mask_stt_buf_ovf      : 1   ; /* [19]  */
        unsigned int    mask_em_buf_ovf       : 1   ; /* [20]  */
        unsigned int    mask_tunl_line_int    : 1   ; /* [21]  */
        unsigned int    reserved_2            : 10  ; /* [31..22]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_ch_int_mask;

/* Define the union u_vi_bayer_cmp_glb_info */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    ice_en                : 1   ; /* [0]  */
        unsigned int    is_lossless           : 1   ; /* [1]  */
        unsigned int    cmp_mode              : 1   ; /* [2]  */
        unsigned int    bayer_mode            : 2   ; /* [4..3]  */
        unsigned int    bit_depth             : 2   ; /* [6..5]  */
        unsigned int    qp_force_en           : 1   ; /* [7]  */
        unsigned int    stride_en             : 1   ; /* [8]  */
        unsigned int    align_mode            : 1   ; /* [9]  */
        unsigned int    reserved_0            : 22  ; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_vi_bayer_cmp_glb_info;

/* Define the union u_vi_bayer_cmp_frame_size */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    frame_width           : 14  ; /* [13..0]  */
        unsigned int    reserved_0            : 2   ; /* [15..14]  */
        unsigned int    frame_height          : 14  ; /* [29..16]  */
        unsigned int    reserved_1            : 2   ; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_vi_bayer_cmp_frame_size;

/* Define the union u_vi_bayer_cmp_rc_cfg0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    budget_mb_bits        : 10  ; /* [9..0]  */
        unsigned int    budget_mb_bits_last   : 10  ; /* [19..10]  */
        unsigned int    min_mb_bits           : 10  ; /* [29..20]  */
        unsigned int    reserved_0            : 2   ; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_vi_bayer_cmp_rc_cfg0;

/* Define the union u_vi_bayer_cmp_rc_cfg1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    first_mb_adj_bits     : 8   ; /* [7..0]  */
        unsigned int    first_row_adj_bits    : 6   ; /* [13..8]  */
        unsigned int    reserved_0            : 2   ; /* [15..14]  */
        unsigned int    first_col_adj_bits    : 8   ; /* [23..16]  */
        unsigned int    smooth_status_thr     : 4   ; /* [27..24]  */
        unsigned int    reserved_1            : 4   ; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_vi_bayer_cmp_rc_cfg1;

/* Define the union u_vi_bayer_cmp_rc_cfg2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    smth_thr              : 8   ; /* [7..0]  */
        unsigned int    still_thr             : 8   ; /* [15..8]  */
        unsigned int    big_grad_thr          : 10  ; /* [25..16]  */
        unsigned int    reserved_0            : 6   ; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_vi_bayer_cmp_rc_cfg2;

/* Define the union u_vi_bayer_cmp_rc_cfg3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    smth_pix_num_thr      : 6   ; /* [5..0]  */
        unsigned int    reserved_0            : 2   ; /* [7..6]  */
        unsigned int    still_pix_num_thr     : 6   ; /* [13..8]  */
        unsigned int    reserved_1            : 2   ; /* [15..14]  */
        unsigned int    noise_pix_num_thr     : 6   ; /* [21..16]  */
        unsigned int    reserved_2            : 10  ; /* [31..22]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_vi_bayer_cmp_rc_cfg3;

/* Define the union u_vi_bayer_cmp_rc_cfg4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    qp_inc1_bits_thr      : 8   ; /* [7..0]  */
        unsigned int    qp_inc2_bits_thr      : 8   ; /* [15..8]  */
        unsigned int    reserved_0            : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_vi_bayer_cmp_rc_cfg4;

/* Define the union u_vi_bayer_cmp_rc_cfg5 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int buf_fullness_thr_reg0  : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_vi_bayer_cmp_rc_cfg5;
/* Define the union u_vi_bayer_cmp_rc_cfg6 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int buf_fullness_thr_reg1  : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_vi_bayer_cmp_rc_cfg6;
/* Define the union u_vi_bayer_cmp_rc_cfg7 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int buf_fullness_thr_reg2  : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_vi_bayer_cmp_rc_cfg7;
/* Define the union u_vi_bayer_cmp_rc_cfg8 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int qp_rge_reg0            : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_vi_bayer_cmp_rc_cfg8;
/* Define the union u_vi_bayer_cmp_rc_cfg9 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int qp_rge_reg1            : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_vi_bayer_cmp_rc_cfg9;
/* Define the union u_vi_bayer_cmp_rc_cfg10 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int qp_rge_reg2            : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_vi_bayer_cmp_rc_cfg10;
/* Define the union u_vi_bayer_cmp_rc_cfg11 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int bits_offset_reg0       : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_vi_bayer_cmp_rc_cfg11;
/* Define the union u_vi_bayer_cmp_rc_cfg12 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int bits_offset_reg1       : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_vi_bayer_cmp_rc_cfg12;
/* Define the union u_vi_bayer_cmp_rc_cfg13 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int bits_offset_reg2       : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_vi_bayer_cmp_rc_cfg13;
/* Define the union u_vi_bayer_cmp_rc_cfg14 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int est_err_gain_map       : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_vi_bayer_cmp_rc_cfg14;
/* Define the union u_vi_bayer_cmp_rc_cfg15 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    buffer_init_bits      : 16  ; /* [15..0]  */
        unsigned int    max_mb_qp             : 4   ; /* [19..16]  */
        unsigned int    reserved_0            : 12  ; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_vi_bayer_cmp_rc_cfg15;

/* Define the union u_vi_bayer_cmp_bs_size */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    frame_size            : 23  ; /* [22..0]  */
        unsigned int    reserved_0            : 9   ; /* [31..23]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_vi_bayer_cmp_bs_size;

/* Define the union u_vi_bayer_cmp_glb_st */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    max_left_bits_buffer  : 16  ; /* [15..0]  */
        unsigned int    max_row_len           : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_vi_bayer_cmp_glb_st;

/* Define the union u_vi_bayer_cmp_dbg_reg */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int debug_info             : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_vi_bayer_cmp_dbg_reg;
/* Define the union u_ch_wch_stt_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 17  ; /* [16..0]  */
        unsigned int    ch_wch_stt_finish_mode : 1   ; /* [17]  */
        unsigned int    reserved_1            : 12  ; /* [29..18]  */
        unsigned int    ch_wch_stt_32b_align  : 1   ; /* [30]  */
        unsigned int    reserved_2            : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_ch_wch_stt_cfg;

/* Define the union u_ch_wch_stt_ae_hist_addr_l */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ch_wch_stt_ae_hist_addr_l : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_ch_wch_stt_ae_hist_addr_l;
/* Define the union u_ch_wch_stt_ae_aver_r_gr_addr_l */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ch_wch_stt_ae_aver_r_gr_addr_l : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_ch_wch_stt_ae_aver_r_gr_addr_l;
/* Define the union u_ch_wch_stt_ae_aver_gb_b_addr_l */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ch_wch_stt_ae_aver_gb_b_addr_l : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_ch_wch_stt_ae_aver_gb_b_addr_l;
/* Define the union u_ch_wch_em_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    ch_wch_em_bit_width   : 6   ; /* [5..0]  */
        unsigned int    reserved_0            : 11  ; /* [16..6]  */
        unsigned int    ch_wch_em_finish_mode : 1   ; /* [17]  */
        unsigned int    reserved_1            : 12  ; /* [29..18]  */
        unsigned int    ch_wch_em_32b_align   : 1   ; /* [30]  */
        unsigned int    ch_wch_enable         : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_ch_wch_em_cfg;

/* Define the union u_ch_wch_em_size */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    ch_wch_em_size        : 16  ; /* [15..0]  */
        unsigned int    reserved_0            : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_ch_wch_em_size;

/* Define the union u_ch_wch_em_faddr_l */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ch_wch_em_faddr_l      : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_ch_wch_em_faddr_l;
/* Define the union u_isp_fe_version */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fe_patch          : 8   ; /* [7..0]  */
        unsigned int    isp_fe_build          : 8   ; /* [15..8]  */
        unsigned int    isp_fe_release        : 8   ; /* [23..16]  */
        unsigned int    isp_fe_version        : 8   ; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fe_version;

/* Define the union u_isp_fe_date */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_fe_date            : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_isp_fe_date;
/* Define the union u_isp_fe_fpga_date */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_fe_fpga_date       : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_isp_fe_fpga_date;
/* Define the union u_isp_fe_module_pos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fe_module_posl    : 16  ; /* [15..0]  */
        unsigned int    isp_fe_module_posh    : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fe_module_pos;

/* Define the union u_isp_fe_fstart */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_fe_fstart          : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_isp_fe_fstart;
/* Define the union u_isp_fe_user_define0 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_fe_user_define0    : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_isp_fe_user_define0;
/* Define the union u_isp_fe_user_define1 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_fe_user_define1    : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_isp_fe_user_define1;
/* Define the union u_isp_fe_startup */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_fe_fcnt            : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_isp_fe_startup;
/* Define the union u_isp_fe_format */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_fe_format          : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_isp_fe_format;
/* Define the union u_isp_fe_int */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fe_int_fstart     : 1   ; /* [0]  */
        unsigned int    isp_fe_int_update_cfg : 1   ; /* [1]  */
        unsigned int    isp_fe_int_cfg_loss   : 1   ; /* [2]  */
        unsigned int    isp_fe_int_int_delay  : 1   ; /* [3]  */
        unsigned int    isp_fe_int_ae1_int    : 1   ; /* [4]  */
        unsigned int    reserved_0            : 1   ; /* [5]  */
        unsigned int    isp_fe_int_af1_int    : 1   ; /* [6]  */
        unsigned int    isp_fe_int_dis1_int   : 1   ; /* [7]  */
        unsigned int    isp_fe_int_done_int   : 1   ; /* [8]  */
        unsigned int    reserved_1            : 23  ; /* [31..9]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fe_int;

/* Define the union u_isp_fe_int_mask */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fe_int_mask_fstart : 1   ; /* [0]  */
        unsigned int    isp_fe_int_mask_update_cfg : 1   ; /* [1]  */
        unsigned int    isp_fe_int_mask_cfg_loss : 1   ; /* [2]  */
        unsigned int    isp_fe_int_mask_int_delay : 1   ; /* [3]  */
        unsigned int    isp_fe_int_mask_ae1_int : 1   ; /* [4]  */
        unsigned int    reserved_0            : 1   ; /* [5]  */
        unsigned int    isp_fe_int_mask_af1_int : 1   ; /* [6]  */
        unsigned int    isp_fe_int_mask_dis1_int : 1   ; /* [7]  */
        unsigned int    isp_fe_int_mask_int   : 1   ; /* [8]  */
        unsigned int    reserved_1            : 23  ; /* [31..9]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fe_int_mask;

/* Define the union u_isp_fe_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 2   ; /* [1..0]  */
        unsigned int    isp_colorbar_en       : 1   ; /* [2]  */
        unsigned int    isp_crop_en           : 1   ; /* [3]  */
        unsigned int    isp_blc1_en           : 1   ; /* [4]  */
        unsigned int    isp_rc_en             : 1   ; /* [5]  */
        unsigned int    isp_hrs_en            : 1   ; /* [6]  */
        unsigned int    isp_rlsc1_en          : 1   ; /* [7]  */
        unsigned int    isp_dg2_en            : 1   ; /* [8]  */
        unsigned int    isp_dis1_en           : 1   ; /* [9]  */
        unsigned int    isp_af1_en            : 1   ; /* [10]  */
        unsigned int    isp_wb1_en            : 1   ; /* [11]  */
        unsigned int    isp_drcs_en           : 1   ; /* [12]  */
        unsigned int    isp_ae1_en            : 1   ; /* [13]  */
        unsigned int    isp_loglut1_en        : 1   ; /* [14]  */
        unsigned int    isp_dg_en             : 1   ; /* [15]  */
        unsigned int    isp_gamma_en          : 1   ; /* [16]  */
        unsigned int    reserved_1            : 15  ; /* [31..17]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fe_ctrl;

/* Define the union u_isp_fe_adapter_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    fe_hsync_mode         : 1   ; /* [0]  */
        unsigned int    fe_vsync_mode         : 1   ; /* [1]  */
        unsigned int    reserved_0            : 30  ; /* [31..2]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fe_adapter_cfg;

/* Define the union u_isp_fe_out_size */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    fe_out_width          : 16  ; /* [15..0]  */
        unsigned int    fe_out_height         : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fe_out_size;

/* Define the union u_isp_fe_fstart_delay */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_fe_delay           : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_isp_fe_fstart_delay;
/* Define the union u_isp_fe_ctrl_f */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fe_rggb_cfg       : 2   ; /* [1..0]  */
        unsigned int    reserved_0            : 30  ; /* [31..2]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fe_ctrl_f;

/* Define the union u_isp_fe_ctrl_i */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fe_update_mode    : 1   ; /* [0]  */
        unsigned int    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fe_ctrl_i;

/* Define the union u_isp_fe_timing_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 1   ; /* [0]  */
        unsigned int    isp_fe_fix_timing     : 13  ; /* [13..1]  */
        unsigned int    reserved_1            : 18  ; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fe_timing_cfg;

/* Define the union u_isp_fe_reg_update */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fe_update         : 1   ; /* [0]  */
        unsigned int    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fe_reg_update;

/* Define the union u_isp_fe_size */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fe_width          : 16  ; /* [15..0]  */
        unsigned int    isp_fe_height         : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fe_size;

/* Define the union u_isp_fe_in_sum_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fe_csc_sum_en     : 1   ; /* [0]  */
        unsigned int    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fe_in_sum_cfg;

/* Define the union u_isp_fe_out_sum_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fe_yuv444_sum_en  : 1   ; /* [0]  */
        unsigned int    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fe_out_sum_cfg;

/* Define the union u_isp_fe_blk_size */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fe_blk_width      : 16  ; /* [15..0]  */
        unsigned int    isp_fe_blk_height     : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fe_blk_size;

/* Define the union u_isp_fe_blk_hblank */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fe_blk_b_hblank   : 16  ; /* [15..0]  */
        unsigned int    isp_fe_blk_f_hblank   : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fe_blk_hblank;

/* Define the union u_isp_fe_blk_vblank */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fe_blk_b_vblank   : 16  ; /* [15..0]  */
        unsigned int    isp_fe_blk_f_vblank   : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fe_blk_vblank;

/* Define the union u_isp_fe_in_sum0 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_fe_y_sum0          : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_isp_fe_in_sum0;
/* Define the union u_isp_fe_in_sum1 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_fe_y_sum1          : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_isp_fe_in_sum1;
/* Define the union u_isp_fe_out_sum0 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_fe_yuv444_y_sum0   : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_isp_fe_out_sum0;
/* Define the union u_isp_fe_out_sum1 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_fe_yuv444_y_sum1   : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_isp_fe_out_sum1;
/* Define the union u_isp_blc1_version */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_isp_blc1_version;
/* Define the union u_isp_blc1_offset1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_blc1_ofsgr        : 15  ; /* [14..0]  */
        unsigned int    reserved_0            : 1   ; /* [15]  */
        unsigned int    isp_blc1_ofsr         : 15  ; /* [30..16]  */
        unsigned int    reserved_1            : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_blc1_offset1;

/* Define the union u_isp_blc1_offset2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_blc1_ofsgb        : 15  ; /* [14..0]  */
        unsigned int    reserved_0            : 1   ; /* [15]  */
        unsigned int    isp_blc1_ofsb         : 15  ; /* [30..16]  */
        unsigned int    reserved_1            : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_blc1_offset2;

/* Define the union u_isp_dg2_blc_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dg2_en_in         : 1   ; /* [0]  */
        unsigned int    isp_dg2_en_out        : 1   ; /* [1]  */
        unsigned int    reserved_0            : 30  ; /* [31..2]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dg2_blc_cfg;

/* Define the union u_isp_dg2_version */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_isp_dg2_version;
/* Define the union u_isp_dg2_gain1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dg2_grgain        : 16  ; /* [15..0]  */
        unsigned int    isp_dg2_rgain         : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dg2_gain1;

/* Define the union u_isp_dg2_gain2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dg2_gbgain        : 16  ; /* [15..0]  */
        unsigned int    isp_dg2_bgain         : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dg2_gain2;

/* Define the union u_isp_dg2_blc_offset1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dg2_ofsgr         : 15  ; /* [14..0]  */
        unsigned int    reserved_0            : 1   ; /* [15]  */
        unsigned int    isp_dg2_ofsr          : 15  ; /* [30..16]  */
        unsigned int    reserved_1            : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dg2_blc_offset1;

/* Define the union u_isp_dg2_blc_offset2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dg2_ofsgb         : 15  ; /* [14..0]  */
        unsigned int    reserved_0            : 1   ; /* [15]  */
        unsigned int    isp_dg2_ofsb          : 15  ; /* [30..16]  */
        unsigned int    reserved_1            : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dg2_blc_offset2;

/* Define the union u_isp_dg2_clip_value */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dg2_clip_value    : 20  ; /* [19..0]  */
        unsigned int    reserved_0            : 12  ; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dg2_clip_value;

/* Define the union u_isp_wb1_blc_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wb1_en_in         : 1   ; /* [0]  */
        unsigned int    isp_wb1_en_out        : 1   ; /* [1]  */
        unsigned int    reserved_0            : 30  ; /* [31..2]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wb1_blc_cfg;

/* Define the union u_isp_wb1_version */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_isp_wb1_version;
/* Define the union u_isp_wb1_gain1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wb1_grgain        : 16  ; /* [15..0]  */
        unsigned int    isp_wb1_rgain         : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wb1_gain1;

/* Define the union u_isp_wb1_gain2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wb1_gbgain        : 16  ; /* [15..0]  */
        unsigned int    isp_wb1_bgain         : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wb1_gain2;

/* Define the union u_isp_wb1_blc_offset1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wb1_ofsgr         : 15  ; /* [14..0]  */
        unsigned int    reserved_0            : 1   ; /* [15]  */
        unsigned int    isp_wb1_ofsr          : 15  ; /* [30..16]  */
        unsigned int    reserved_1            : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wb1_blc_offset1;

/* Define the union u_isp_wb1_blc_offset2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wb1_ofsgb         : 15  ; /* [14..0]  */
        unsigned int    reserved_0            : 1   ; /* [15]  */
        unsigned int    isp_wb1_ofsb          : 15  ; /* [30..16]  */
        unsigned int    reserved_1            : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wb1_blc_offset2;

/* Define the union u_isp_wb1_clip_value */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wb1_clip_value    : 20  ; /* [19..0]  */
        unsigned int    reserved_0            : 12  ; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wb1_clip_value;

/* Define the union u_isp_ae_version */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ae_version         : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_isp_ae_version;
/* Define the union u_isp_ae_zone */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_hnum           : 5   ; /* [4..0]  */
        unsigned int    reserved_0            : 3   ; /* [7..5]  */
        unsigned int    isp_ae_vnum           : 5   ; /* [12..8]  */
        unsigned int    reserved_1            : 19  ; /* [31..13]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_zone;

/* Define the union u_isp_ae_skip_crg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_skip_x         : 3   ; /* [2..0]  */
        unsigned int    isp_ae_offset_x       : 1   ; /* [3]  */
        unsigned int    isp_ae_skip_y         : 3   ; /* [6..4]  */
        unsigned int    isp_ae_offset_y       : 1   ; /* [7]  */
        unsigned int    reserved_0            : 24  ; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_skip_crg;

/* Define the union u_isp_ae_total_stat */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_total_pixels   : 27  ; /* [26..0]  */
        unsigned int    reserved_0            : 5   ; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_total_stat;

/* Define the union u_isp_ae_count_stat */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_count_pixels   : 31  ; /* [30..0]  */
        unsigned int    reserved_0            : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_count_stat;

/* Define the union u_isp_ae_total_r_aver */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_isp_ae_total_r_aver;
/* Define the union u_isp_ae_total_gr_aver */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_isp_ae_total_gr_aver;
/* Define the union u_isp_ae_total_gb_aver */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_isp_ae_total_gb_aver;
/* Define the union u_isp_ae_total_b_aver */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_isp_ae_total_b_aver;
/* Define the union u_isp_ae_hist_high */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ae_hist_high       : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_isp_ae_hist_high;
/* Define the union u_isp_ae_bitmove */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_bitmove        : 5   ; /* [4..0]  */
        unsigned int    isp_ae_hist_gamma_mode : 2   ; /* [6..5]  */
        unsigned int    isp_ae_aver_gamma_mode : 2   ; /* [8..7]  */
        unsigned int    isp_ae_blc_en         : 1   ; /* [9]  */
        unsigned int    reserved_0            : 22  ; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_bitmove;

/* Define the union u_isp_ae_offset_r_gr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_offset_r       : 15  ; /* [14..0]  */
        unsigned int    reserved_0            : 1   ; /* [15]  */
        unsigned int    isp_ae_offset_gr      : 15  ; /* [30..16]  */
        unsigned int    reserved_1            : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_offset_r_gr;

/* Define the union u_isp_ae_offset_gb_b */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_offset_gb      : 15  ; /* [14..0]  */
        unsigned int    reserved_0            : 1   ; /* [15]  */
        unsigned int    isp_ae_offset_b       : 15  ; /* [30..16]  */
        unsigned int    reserved_1            : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_offset_gb_b;

/* Define the union u_isp_ae_lut_update */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_lut_update     : 1   ; /* [0]  */
        unsigned int    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_lut_update;

/* Define the union u_isp_ae_stt_bst */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_stt_size       : 16  ; /* [15..0]  */
        unsigned int    isp_ae_stt_bst        : 4   ; /* [19..16]  */
        unsigned int    isp_ae_stt_en         : 1   ; /* [20]  */
        unsigned int    reserved_0            : 11  ; /* [31..21]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_stt_bst;

/* Define the union u_isp_ae_crop_pos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_crop_pos_x     : 16  ; /* [15..0]  */
        unsigned int    isp_ae_crop_pos_y     : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_crop_pos;

/* Define the union u_isp_ae_crop_outsize */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_crop_out_width : 16  ; /* [15..0]  */
        unsigned int    isp_ae_crop_out_height : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_crop_outsize;

/* Define the union u_isp_ae_hist_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_isp_ae_hist_waddr;
/* Define the union u_isp_ae_hist_wdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_isp_ae_hist_wdata;
/* Define the union u_isp_ae_hist_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ae_hist_raddr      : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_isp_ae_hist_raddr;
/* Define the union u_isp_ae_hist_rdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ae_hist_rdata      : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_isp_ae_hist_rdata;
/* Define the union u_isp_ae_aver_r_gr_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_isp_ae_aver_r_gr_waddr;
/* Define the union u_isp_ae_aver_r_gr_wdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_isp_ae_aver_r_gr_wdata;
/* Define the union u_isp_ae_aver_r_gr_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_isp_ae_aver_r_gr_raddr;
/* Define the union u_isp_ae_aver_r_gr_rdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_isp_ae_aver_r_gr_rdata;
/* Define the union u_isp_ae_aver_gb_b_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_isp_ae_aver_gb_b_waddr;
/* Define the union u_isp_ae_aver_gb_b_wdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_isp_ae_aver_gb_b_wdata;
/* Define the union u_isp_ae_aver_gb_b_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_isp_ae_aver_gb_b_raddr;
/* Define the union u_isp_ae_aver_gb_b_rdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_isp_ae_aver_gb_b_rdata;
/* Define the union u_isp_ae_weight_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ae_wei_waddr       : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_isp_ae_weight_waddr;
/* Define the union u_isp_ae_weight_wdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ae_wei_wdata       : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_isp_ae_weight_wdata;
/* Define the union u_isp_ae_weight_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_isp_ae_weight_raddr;
/* Define the union u_isp_ae_weight_rdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_isp_ae_weight_rdata;
/* Define the union u_isp_dg_blc_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dg_en_in          : 1   ; /* [0]  */
        unsigned int    isp_dg_en_out         : 1   ; /* [1]  */
        unsigned int    reserved_0            : 30  ; /* [31..2]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dg_blc_cfg;

/* Define the union u_isp_dg_version */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_isp_dg_version;
/* Define the union u_isp_dg_gain1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dg_grgain         : 16  ; /* [15..0]  */
        unsigned int    isp_dg_rgain          : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dg_gain1;

/* Define the union u_isp_dg_gain2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dg_gbgain         : 16  ; /* [15..0]  */
        unsigned int    isp_dg_bgain          : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dg_gain2;

/* Define the union u_isp_dg_blc_offset1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dg_ofsgr          : 15  ; /* [14..0]  */
        unsigned int    reserved_0            : 1   ; /* [15]  */
        unsigned int    isp_dg_ofsr           : 15  ; /* [30..16]  */
        unsigned int    reserved_1            : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dg_blc_offset1;

/* Define the union u_isp_dg_blc_offset2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dg_ofsgb          : 15  ; /* [14..0]  */
        unsigned int    reserved_0            : 1   ; /* [15]  */
        unsigned int    isp_dg_ofsb           : 15  ; /* [30..16]  */
        unsigned int    reserved_1            : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dg_blc_offset2;

/* Define the union u_isp_dg_clip_value */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dg_clip_value     : 20  ; /* [19..0]  */
        unsigned int    reserved_0            : 12  ; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dg_clip_value;

/* Define the union u_isp_gamma_version */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_gamma_version      : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_isp_gamma_version;
/* Define the union u_isp_gamma_bitw */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_gamma_bitw_out    : 5   ; /* [4..0]  */
        unsigned int    reserved_0            : 11  ; /* [15..5]  */
        unsigned int    isp_gamma_bitw_in     : 5   ; /* [20..16]  */
        unsigned int    reserved_1            : 11  ; /* [31..21]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_gamma_bitw;

/* Define the union u_isp_gamma_lut_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_gamma_lut_waddr    : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_isp_gamma_lut_waddr;
/* Define the union u_isp_gamma_lut_wdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_gamma_lut_wdata    : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_isp_gamma_lut_wdata;
/* Define the union u_isp_gamma_lut_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_gamma_lut_raddr    : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_isp_gamma_lut_raddr;
/* Define the union u_isp_gamma_lut_rdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_gamma_lut_rdata    : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_isp_gamma_lut_rdata;
/* Define the union u_isp_rc_blcen */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_rc_blc_in_en      : 1   ; /* [0]  */
        unsigned int    reserved_0            : 7   ; /* [7..1]  */
        unsigned int    isp_rc_blc_out_en     : 1   ; /* [8]  */
        unsigned int    reserved_1            : 23  ; /* [31..9]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_rc_blcen;

/* Define the union u_isp_rc_blc0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_rc_blc_r          : 15  ; /* [14..0]  */
        unsigned int    reserved_0            : 1   ; /* [15]  */
        unsigned int    isp_rc_blc_gr         : 15  ; /* [30..16]  */
        unsigned int    reserved_1            : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_rc_blc0;

/* Define the union u_isp_rc_blc1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_rc_blc_b          : 15  ; /* [14..0]  */
        unsigned int    reserved_0            : 1   ; /* [15]  */
        unsigned int    isp_rc_blc_gb         : 15  ; /* [30..16]  */
        unsigned int    reserved_1            : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_rc_blc1;

/* Define the union u_isp_rc_sqradius */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_rc_sqradius        : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_isp_rc_sqradius;
/* Define the union u_isp_rc_center_coor */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_rc_cenhor_coor    : 14  ; /* [13..0]  */
        unsigned int    reserved_0            : 2   ; /* [15..14]  */
        unsigned int    isp_rc_cenver_coor    : 14  ; /* [29..16]  */
        unsigned int    reserved_1            : 2   ; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_rc_center_coor;

/* Define the union u_isp_crop_version */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_crop_version       : 32  ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
        unsigned int    u32;
} u_isp_crop_version;
/* Define the union u_isp_crop_sizeout */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_crop_width_out    : 16  ; /* [15..0]  */
        unsigned int    isp_crop_height_out   : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_crop_sizeout;

/* Define the union u_isp_crop_pos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_crop_pos_x        : 16  ; /* [15..0]  */
        unsigned int    isp_crop_pos_y        : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_crop_pos;

/* Define the global struct */
typedef struct {
    u_wk_mode                       wk_mode                          ; /* 0x0 */
    u_memctrl                       memctrl                          ; /* 0x4 */
    u_memctrl1                      memctrl1                         ; /* 0x8 */
    unsigned int                    reserved_0                       ; /* 0xc */
    u_axi_cfg                       axi_cfg                          ; /* 0x10 */
    u_mac_cfg                       mac_cfg                          ; /* 0x14 */
    u_axi_rd_prot                   axi_rd_prot                      ; /* 0x18 */
    u_axi_wr_prot                   axi_wr_prot                      ; /* 0x1c */
    u_pt_sel                        pt_sel                           ; /* 0x20 */
    u_pt_mux                        pt_mux                           ; /* 0x24 */
    unsigned int                    reserved_1[2]                    ; /* 0x28~0x2c */
    u_line_afifo_mode               line_afifo_mode                  ; /* 0x30 */
    unsigned int                    reserved_2[3]                    ; /* 0x34~0x3c */
    u_line_afifo_blk0               line_afifo_blk0                  ; /* 0x40 */
    unsigned int                    reserved_3[3]                    ; /* 0x44~0x4c */
    u_cmux_sel                      cmux_sel                         ; /* 0x50 */
    u_cmux_id_cfg                   cmux_id_cfg                      ; /* 0x54 */
    u_isp_mode                      isp_mode                         ; /* 0x58 */
    unsigned int                    reserved_4[29]                   ; /* 0x5c~0xcc */
    u_apb_timeout                   apb_timeout                      ; /* 0xd0 */
    unsigned int                    reserved_5[3]                    ; /* 0xd4~0xdc */
    u_vicap_int1                    vicap_int1                       ; /* 0xe0 */
    u_vicap_int1                    vicap_int1_masked                ; /* 0xe4 */
    u_vicap_int1_mask               vicap_int1_mask                  ; /* 0xe8 */
    unsigned int                    reserved_7                       ; /* 0xec */
    u_vicap_int0                    vicap_int0                       ; /* 0xf0 */
    u_vicap_int0                    vicap_int0_masked                ; /* 0xf4 */
    u_vicap_int0_mask               vicap_int0_mask                  ; /* 0xf8 */
    unsigned int                    reserved_9                       ; /* 0xfc */
    u_slave0_mode_cfg               slave0_mode_cfg                  ; /* 0x100 */
    u_slave0_mode_vs_time_h         slave0_mode_vs_time_h            ; /* 0x104 */
    u_slave0_mode_vs_time_l         slave0_mode_vs_time_l            ; /* 0x108 */
    u_slave0_mode_hs_time           slave0_mode_hs_time              ; /* 0x10c */
    unsigned int                    reserved_10[2]                   ; /* 0x110~0x114 */
    u_slave0_mode_vs_cyc_h          slave0_mode_vs_cyc_h             ; /* 0x118 */
    u_slave0_mode_vs_cyc_l          slave0_mode_vs_cyc_l             ; /* 0x11c */
    u_slave0_mode_hs_cyc            slave0_mode_hs_cyc               ; /* 0x120 */
    unsigned int                    reserved_11[2]                   ; /* 0x124~0x128 */
    u_slave0_mode_hs_dly_cyc        slave0_mode_hs_dly_cyc           ; /* 0x12c */
    u_slave0_mode_vs_dly_cyc_h      slave0_mode_vs_dly_cyc_h         ; /* 0x130 */
    u_slave0_mode_vs_dly_cyc_l      slave0_mode_vs_dly_cyc_l         ; /* 0x134 */
    unsigned int                    reserved_12[2]                   ; /* 0x138~0x13c */
    u_slave1_mode_cfg               slave1_mode_cfg                  ; /* 0x140 */
    u_slave1_mode_vs_time_h         slave1_mode_vs_time_h            ; /* 0x144 */
    u_slave1_mode_vs_time_l         slave1_mode_vs_time_l            ; /* 0x148 */
    u_slave1_mode_hs_time           slave1_mode_hs_time              ; /* 0x14c */
    unsigned int                    reserved_13[2]                   ; /* 0x150~0x154 */
    u_slave1_mode_vs_cyc_h          slave1_mode_vs_cyc_h             ; /* 0x158 */
    u_slave1_mode_vs_cyc_l          slave1_mode_vs_cyc_l             ; /* 0x15c */
    u_slave1_mode_hs_cyc            slave1_mode_hs_cyc               ; /* 0x160 */
    unsigned int                    reserved_14[2]                   ; /* 0x164~0x168 */
    u_slave1_mode_hs_dly_cyc        slave1_mode_hs_dly_cyc           ; /* 0x16c */
    u_slave1_mode_vs_dly_cyc_h      slave1_mode_vs_dly_cyc_h         ; /* 0x170 */
    u_slave1_mode_vs_dly_cyc_l      slave1_mode_vs_dly_cyc_l         ; /* 0x174 */
    unsigned int                    reserved_15[32]                  ; /* 0x178~0x1f4 */
    u_slave_mode_time_cfg_sel       slave_mode_time_cfg_sel          ; /* 0x1f8 */
    u_slave_mode_syn                slave_mode_syn                   ; /* 0x1fc */
    u_online0_cfg                   online0_cfg                      ; /* 0x200 */
    unsigned int                    reserved_16[31]                  ; /* 0x204~0x27c */
    u_online0_des0_info             online0_des0_info                ; /* 0x280 */
    unsigned int                    reserved_17[3]                   ; /* 0x284~0x28c */
    u_online0_last_des0_info        online0_last_des0_info           ; /* 0x290 */
    unsigned int                    reserved_18[5]                   ; /* 0x294~0x2a4 */
    u_online0_size_total            online0_size_total               ; /* 0x2a8 */
    u_online0_size                  online0_size                     ; /* 0x2ac */
    unsigned int                    reserved_19[20]                  ; /* 0x2b0~0x2fc */
    volatile u_vicap_afifo_crop     vicap_afifo_crop[VICAP_PT_NUM]	 ; /* 0x300~0x31c */
    unsigned int                    reserved_21[44]                  ; /* 0x320~0x3bc */
    u_isp0_in_size_total            isp0_in_size_total               ; /* 0x3c0 */
    u_isp0_in_size                  isp0_in_size                     ; /* 0x3c4 */
    unsigned int                         rsr20[14];                           /* 3c8 ~ 3fc */
    u_vicap_latency_det_cfg              vicap_latency_det_cfg;               /* 400 */
    u_vicap_axi0_rd_latency              vicap_axi0_rd_latency;               /* 404 */
    u_vicap_axi0_rd_latency_overflow     vicap_axi0_rd_latency_overflow;      /* 408 */
    u_vicap_axi_status0                  vicap_axi_status0;                   /* 40c */
    u_vicap_axi_status1                  vicap_axi_status1;                   /* 410 */
    u_vicap_axi1_rd_latency              vicap_axi1_rd_latency;               /* 414 */
    u_vicap_axi1_rd_latency_overflow     vicap_axi1_rd_latency_overflow;      /* 418 */
    unsigned int                         rsr21;                               /* 41c */
    u_vicap_pt0_aff_status               vicap_pt0_aff_status;                /* 420 */
    u_vicap_pt1_aff_status               vicap_pt1_aff_status;                /* 424 */
    unsigned int                         rsr22[6];                            /* 428 ~ 43c */
    u_vicap_isp0_line_aff_status         vicap_isp0_line_aff_status;          /* 440 */
    unsigned int                         rsr23[7];                            /* 444 ~ 45c */
    u_vicap_isp0_aff_status              vicap_isp0_aff_status;               /* 460 */
    unsigned int                         rsr24[31];                           /* 464 ~ 4dc */
    u_vicap_axi0_rd_error                vicap_axi0_rd_error;                 /* 4e0 */
    u_vicap_axi0_wr_error                vicap_axi0_wr_error;                 /* 4e4 */
    unsigned int                         rsr25[2];                            /* 4e8 ~ 4ec */
    u_vicap_dbg_int                      vicap_dbg_int;                       /* 4f0 */
    unsigned int                         rsr26;                               /* 4f4 */
    u_vicap_dbg_int_mask                 vicap_dbg_int_mask;                  /* 4f8 */
    unsigned int                         rsr27;                               /* 4fc */
    u_pt_intf_mod                        pt_intf_mod;                         /* 500 */
    unsigned int                         rsr28[3];                            /* 504 ~ 50c */
    u_pt_offset0                         pt_offset0;                          /* 510 */
    u_pt_offset1                         pt_offset1;                          /* 514 */
    u_pt_offset2                         pt_offset2;                          /* 518 */
    unsigned int                         rsr29;                               /* 51c */
    u_pt_bt656_cfg                       pt_bt656_cfg;                        /* 520 */
    unsigned int                         rsr30[5];                            /* 524 ~ 534 */
    u_pt_unify_timing_cfg                pt_unify_timing_cfg;                 /* 538 */
    u_pt_gen_timing_cfg                  pt_gen_timing_cfg;                   /* 53c */
    u_pt_unify_data_cfg                  pt_unify_data_cfg;                   /* 540 */
    u_pt_gen_data_cfg                    pt_gen_data_cfg;                     /* 544 */
    u_pt_gen_data_coef                   pt_gen_data_coef;                    /* 548 */
    u_pt_gen_data_init                   pt_gen_data_init;                    /* 54c */
    u_pt_yuv444_cfg                      pt_yuv444_cfg;                       /* 550 */
    unsigned int                         rsr31[3];                            /* 554 ~ 55c */
    u_pt_fstart_dly                      pt_fstart_dly;                       /* 560 */
    u_pt_fstart_h_dly                    pt_fstart_h_dly;                     /* 564 */
    unsigned int                         rsr32[6];                            /* 568 ~ 57c */
    u_pt_intf_hfb                        pt_intf_hfb;                         /* 580 */
    u_pt_intf_hact                       pt_intf_hact;                        /* 584 */
    u_pt_intf_hbb                        pt_intf_hbb;                         /* 588 */
    u_pt_intf_vfb                        pt_intf_vfb;                         /* 58c */
    u_pt_intf_vact                       pt_intf_vact;                        /* 590 */
    u_pt_intf_vbb                        pt_intf_vbb;                         /* 594 */
    u_pt_intf_vbfb                       pt_intf_vbfb;                        /* 598 */
    u_pt_intf_vbact                      pt_intf_vbact;                       /* 59c */
    u_pt_intf_vbbb                       pt_intf_vbbb;                        /* 5a0 */
    u_pt_id_cfg                          pt_id_cfg;                           /* 5a4 */
    unsigned int                         rsr33[2];                            /* 5a8 ~ 5ac */
    u_pt_id_sel                          pt_id_sel;                           /* 5b0 */
    u_pt_size_total1                     pt_size_total1;                      /* 5b4 */
    unsigned int                         rsr34[2];                            /* 5b8 ~ 5bc */
    u_pt_flash_cfg                       pt_flash_cfg;                        /* 5c0 */
    u_pt_flash_cyc0                      pt_flash_cyc0;                       /* 5c4 */
    u_pt_flash_cyc1                      pt_flash_cyc1;                       /* 5c8 */
    unsigned int                         rsr35;                               /* 5cc */
    u_pt_shutter_cyc0                    pt_shutter_cyc0;                     /* 5d0 */
    u_pt_shutter_cyc1                    pt_shutter_cyc1;                     /* 5d4 */
    u_pt_shutter_cyc2                    pt_shutter_cyc2;                     /* 5d8 */
    u_pt_shutter_cyc3                    pt_shutter_cyc3;                     /* 5dc */
    u_pt_status                          pt_status;                           /* 5e0 */
    u_pt_bt656_status                    pt_bt656_status;                     /* 5e4 */
    u_pt_size_total                      pt_size_total;                       /* 5e8 */
    u_pt_size                            pt_size;                             /* 5ec */
    u_pt_int                             pt_int;                              /* 5f0 */
    unsigned int                         rsr36;                               /* 5f4 */
    u_pt_int_mask                        pt_int_mask;                         /* 5f8 */
    unsigned int                         rsr37[65];                           /* 5fc ~ 6fc */
    u_src_ctrl                           src_ctrl;                            /* 700 */
    u_src_reg_newer                      src_reg_newer;                       /* 704 */
    u_src_update                         src_update;                          /* 708 */
    unsigned int                         rsr38[25];                           /* 70c ~ 76c */
    u_src_cfg                            src_cfg;                             /* 770 */
    unsigned int                         rsr39[4];                            /* 774 ~ 780 */
    u_src_size                           src_size;                            /* 784 */
    unsigned int                         rsr40[2];                            /* 788 ~ 78c */
    u_src_faddr_h                        src_faddr_h;                         /* 790 */
    u_src_faddr_l                        src_faddr_l;                         /* 794 */
    unsigned int                         rsr41[2];                            /* 798 ~ 79c */
    u_src_stride                         src_stride;                          /* 7a0 */
    u_src_buf_cfg                        src_buf_cfg;                         /* 7a4 */
    unsigned int                         rsr42[15];                           /* 7a8 ~ 7e0 */
    u_src_start2valid_cnt                src_start2valid_cnt;                 /* 7e4 */
    u_src_out_size_total                 src_out_size_total;                  /* 7e8 */
    u_src_out_size                       src_out_size;                        /* 7ec */
    u_src_int                            src_int;                             /* 7f0 */
    unsigned int                         rsr43;                               /* 7f4 */
    u_src_mask                           src_mask;                            /* 7f8 */
    unsigned int                         rsr44;                               /* 7fc */
    unsigned int                         rsr65_1[192];                          /* 800 ~ 0xAFC */
    u_ch_ctrl                            ch_ctrl;                             /* 800 */
    u_ch_reg_newer                       ch_reg_newer;                        /* 804 */
    u_ch_update                          ch_update;                           /* 808 */
    unsigned int                         rsr45;                               /* 80c */
    u_ch_adapter_cfg                     ch_adapter_cfg;                      /* 810 */
    unsigned int                         rsr46[3];                            /* 814 ~ 81c */
    u_ch_int_det_cfg                     ch_int_det_cfg;                      /* 820 */
    u_ch_int_det_info                    ch_int_det_info;                     /* 824 */
    u_ch_int_det_cnt                     ch_int_det_cnt;                      /* 828 */
    unsigned int                         rsr47;                               /* 82c */
    u_ch_count                           ch_count;                            /* 830 */
    u_ch_dly_cfg                         ch_dly_cfg;                          /* 834 */
    u_ch_frm_cnt                         ch_frm_cnt;                          /* 838 */
    u_ch_timestamp_cfg                   ch_timestamp_cfg;                    /* 83c */
    u_ch_crop_cfg                        ch_crop_cfg;                         /* 840 */
    u_ch_crop0_start                     ch_crop0_start;                      /* 844 */
    u_ch_crop0_size                      ch_crop0_size;                       /* 848 */
    unsigned int                         rsr48;                               /* 84c */
    u_ch_skip_y_cfg                      ch_skip_y_cfg;                       /* 850 */
    u_ch_skip_y_win                      ch_skip_y_win;                       /* 854 */
    u_ch_skip_c_cfg                      ch_skip_c_cfg;                       /* 858 */
    u_ch_skip_c_win                      ch_skip_c_win;                       /* 85c */
    u_ch_split_cfg0                      ch_split_cfg0;                       /* 860 */
    u_ch_split_cfg1                      ch_split_cfg1;                       /* 864 */
    u_ch_split_cfg2                      ch_split_cfg2;                       /* 868 */
    unsigned int                         rsr49;                               /* 86c */
    u_ch_uv2c_cfg                        ch_uv2c_cfg;                         /* 870 */
    unsigned int                         rsr50[3];                            /* 874 ~ 87c */
    u_ch_wch_y_cfg                       ch_wch_y_cfg;                        /* 880 */
    u_ch_wch_y_size                      ch_wch_y_size;                       /* 884 */
    u_ch_wch_y_offset                    ch_wch_y_offset;                     /* 888 */
    unsigned int                         rsr51[2];                            /* 88c ~ 890 */
    u_ch_wch_y_faddr_l                   ch_wch_y_faddr_l;                    /* 894 */
    unsigned int                         rsr52[2];                            /* 898 ~ 89c */
    u_ch_wch_y_stride                    ch_wch_y_stride;                     /* 8a0 */
    u_ch_wch_y_buf_cfg                   ch_wch_y_buf_cfg;                    /* 8a4 */
    unsigned int                         rsr53[2];                            /* 8a8 ~ 8ac */
    u_ch_wch_c_cfg                       ch_wch_c_cfg;                        /* 8b0 */
    u_ch_wch_c_size                      ch_wch_c_size;                       /* 8b4 */
    unsigned int                         rsr54[3];                            /* 8b8 ~ 8c0 */
    u_ch_wch_c_faddr_l                   ch_wch_c_faddr_l;                    /* 8c4 */
    unsigned int                         rsr55;                               /* 8c8 */
    u_ch_wch_tunl_haddr_l                ch_wch_tunl_haddr_l;                 /* 8cc */
    u_ch_wch_c_stride                    ch_wch_c_stride;                     /* 8d0 */
    u_ch_wch_c_buf_cfg                   ch_wch_c_buf_cfg;                    /* 8d4 */
    unsigned int                         rsr56[4];                            /* 8d8 ~ 8e4 */
    u_ch_y_act_size                      ch_y_act_size;                       /* 8e8 */
    u_ch_c_act_size                      ch_c_act_size;                       /* 8ec */
    u_ch_int                             ch_int;                              /* 8f0 */
    unsigned int                         rsr57;                               /* 8f4 */
    u_ch_int_mask                        ch_int_mask;                         /* 8f8 */
    unsigned int                         rsr58;                               /* 8fc */
    u_vi_bayer_cmp_glb_info              vi_bayer_cmp_glb_info;               /* 900 */
    u_vi_bayer_cmp_frame_size            vi_bayer_cmp_frame_size;             /* 904 */
    u_vi_bayer_cmp_rc_cfg0               vi_bayer_cmp_rc_cfg0;                /* 908 */
    u_vi_bayer_cmp_rc_cfg1               vi_bayer_cmp_rc_cfg1;                /* 90c */
    u_vi_bayer_cmp_rc_cfg2               vi_bayer_cmp_rc_cfg2;                /* 910 */
    u_vi_bayer_cmp_rc_cfg3               vi_bayer_cmp_rc_cfg3;                /* 914 */
    u_vi_bayer_cmp_rc_cfg4               vi_bayer_cmp_rc_cfg4;                /* 918 */
    u_vi_bayer_cmp_rc_cfg5               vi_bayer_cmp_rc_cfg5;                /* 91c */
    u_vi_bayer_cmp_rc_cfg6               vi_bayer_cmp_rc_cfg6;                /* 920 */
    u_vi_bayer_cmp_rc_cfg7               vi_bayer_cmp_rc_cfg7;                /* 924 */
    u_vi_bayer_cmp_rc_cfg8               vi_bayer_cmp_rc_cfg8;                /* 928 */
    u_vi_bayer_cmp_rc_cfg9               vi_bayer_cmp_rc_cfg9;                /* 92c */
    u_vi_bayer_cmp_rc_cfg10              vi_bayer_cmp_rc_cfg10;               /* 930 */
    u_vi_bayer_cmp_rc_cfg11              vi_bayer_cmp_rc_cfg11;               /* 934 */
    u_vi_bayer_cmp_rc_cfg12              vi_bayer_cmp_rc_cfg12;               /* 938 */
    u_vi_bayer_cmp_rc_cfg13              vi_bayer_cmp_rc_cfg13;               /* 93c */
    u_vi_bayer_cmp_rc_cfg14              vi_bayer_cmp_rc_cfg14;               /* 940 */
    u_vi_bayer_cmp_rc_cfg15              vi_bayer_cmp_rc_cfg15;               /* 944 */
    u_vi_bayer_cmp_bs_size               vi_bayer_cmp_bs_size;                /* 948 */
    u_vi_bayer_cmp_glb_st                vi_bayer_cmp_glb_st;                 /* 94c */
    u_vi_bayer_cmp_dbg_reg               vi_bayer_cmp_dbg_reg;                /* 950 */
    unsigned int                         rsr59[43];                           /* 954 ~ 9fc */
    u_ch_wch_stt_cfg                     ch_wch_stt_cfg;                      /* a00 */
    unsigned int                         rsr60;                               /* a04 */
    u_ch_wch_stt_ae_hist_addr_l          ch_wch_stt_ae_hist_addr_l;           /* a08 */
    unsigned int                         rsr61;                               /* a0c */
    u_ch_wch_stt_ae_aver_r_gr_addr_l     ch_wch_stt_ae_aver_r_gr_addr_l;      /* a10 */
    unsigned int                         rsr62;                               /* a14 */
    u_ch_wch_stt_ae_aver_gb_b_addr_l     ch_wch_stt_ae_aver_gb_b_addr_l;      /* a18 */
    unsigned int                         rsr63[25];                           /* a1c ~ a7c */
    u_ch_wch_em_cfg                      ch_wch_em_cfg;                       /* a80 */
    u_ch_wch_em_size                     ch_wch_em_size;                      /* a84 */
    unsigned int                         rsr64;                               /* a88 */
    u_ch_wch_em_faddr_l                  ch_wch_em_faddr_l;                   /* a8c */
    unsigned int                         rsr65[28];                           /* a90 ~ ffc */
    u_isp_fe_version                     isp_fe_version;                      /* 1000 */
    u_isp_fe_date                        isp_fe_date;                         /* 1004 */
    u_isp_fe_fpga_date                   isp_fe_fpga_date;                    /* 1008 */
    unsigned int                         rsr66;                               /* 100c */
    u_isp_fe_module_pos                  isp_fe_module_pos;                   /* 1010 */
    u_isp_fe_fstart                      isp_fe_fstart;                       /* 1014 */
    unsigned int                         rsr67[2];                            /* 1018 ~ 101c */
    u_isp_fe_user_define0                isp_fe_user_define0;                 /* 1020 */
    u_isp_fe_user_define1                isp_fe_user_define1;                 /* 1024 */
    unsigned int                         rsr68[2];                            /* 1028 ~ 102c */
    u_isp_fe_startup                     isp_fe_startup;                      /* 1030 */
    u_isp_fe_format                      isp_fe_format;                       /* 1034 */
    unsigned int                         rsr69[2];                            /* 1038 ~ 103c */
    u_isp_fe_int                         isp_fe_int;                          /* 1040 */
    unsigned int                         rsr70;                               /* 1044 */
    u_isp_fe_int_mask                    isp_fe_int_mask;                     /* 1048 */
    unsigned int                         rsr71;                               /* 104c */
    u_isp_fe_ctrl                        isp_fe_ctrl;                         /* 1050 */
    unsigned int                         rsr72[4];                            /* 1054 ~ 1060 */
    u_isp_fe_adapter_cfg                 isp_fe_adapter_cfg;                  /* 1064 */
    u_isp_fe_out_size                    isp_fe_out_size;                     /* 1068 */
    unsigned int                         rsr73[2];                            /* 106c ~ 1070 */
    u_isp_fe_fstart_delay                isp_fe_fstart_delay;                 /* 1074 */
    unsigned int                         rsr74[2];                            /* 1078 ~ 107c */
    u_isp_fe_ctrl_f                      isp_fe_ctrl_f;                       /* 1080 */
    u_isp_fe_ctrl_i                      isp_fe_ctrl_i;                       /* 1084 */
    u_isp_fe_timing_cfg                  isp_fe_timing_cfg;                   /* 1088 */
    u_isp_fe_reg_update                  isp_fe_reg_update;                   /* 108c */
    u_isp_fe_size                        isp_fe_size;                         /* 1090 */
    unsigned int                         rsr75[3];                            /* 1094 ~ 109c */
    u_isp_fe_in_sum_cfg                  isp_fe_in_sum_cfg;                   /* 10a0 */
    u_isp_fe_out_sum_cfg                 isp_fe_out_sum_cfg;                  /* 10a4 */
    unsigned int                         rsr76[3];                            /* 10a8 ~ 10b0 */
    u_isp_fe_blk_size                    isp_fe_blk_size;                     /* 10b4 */
    u_isp_fe_blk_hblank                  isp_fe_blk_hblank;                   /* 10b8 */
    u_isp_fe_blk_vblank                  isp_fe_blk_vblank;                   /* 10bc */
    u_isp_fe_in_sum0                     isp_fe_in_sum0;                      /* 10c0 */
    u_isp_fe_in_sum1                     isp_fe_in_sum1;                      /* 10c4 */
    unsigned int                         rsr77[2];                            /* 10c8 ~ 10cc */
    u_isp_fe_out_sum0                    isp_fe_out_sum0;                     /* 10d0 */
    u_isp_fe_out_sum1                    isp_fe_out_sum1;                     /* 10d4 */
    unsigned int                         rsr78[5];                            /* 10d8 ~ 10e8 */
    u_isp_blc1_version                   isp_blc1_version;                    /* 10ec */
    u_isp_blc1_offset1                   isp_blc1_offset1;                    /* 10f0 */
    u_isp_blc1_offset2                   isp_blc1_offset2;                    /* 10f4 */
    unsigned int                         rsr79[3];                            /* 10f8 ~ 1100 */
    u_isp_dg2_blc_cfg                    isp_dg2_blc_cfg;                     /* 1104 */
    unsigned int                         rsr80;                               /* 1108 */
    u_isp_dg2_version                    isp_dg2_version;                     /* 110c */
    u_isp_dg2_gain1                      isp_dg2_gain1;                       /* 1110 */
    u_isp_dg2_gain2                      isp_dg2_gain2;                       /* 1114 */
    u_isp_dg2_blc_offset1                isp_dg2_blc_offset1;                 /* 1118 */
    u_isp_dg2_blc_offset2                isp_dg2_blc_offset2;                 /* 111c */
    u_isp_dg2_clip_value                 isp_dg2_clip_value;                  /* 1120 */
    unsigned int                         rsr81[4];                            /* 1124 ~ 1130 */
    u_isp_wb1_blc_cfg                    isp_wb1_blc_cfg;                     /* 1134 */
    unsigned int                         rsr82;                               /* 1138 */
    u_isp_wb1_version                    isp_wb1_version;                     /* 113c */
    u_isp_wb1_gain1                      isp_wb1_gain1;                       /* 1140 */
    u_isp_wb1_gain2                      isp_wb1_gain2;                       /* 1144 */
    u_isp_wb1_blc_offset1                isp_wb1_blc_offset1;                 /* 1148 */
    u_isp_wb1_blc_offset2                isp_wb1_blc_offset2;                 /* 114c */
    u_isp_wb1_clip_value                 isp_wb1_clip_value;                  /* 1150 */
    unsigned int                         rsr83[46];                           /* 1154 ~ 1208 */
    u_isp_ae_version                     isp_ae_version;                      /* 120c */
    u_isp_ae_zone                        isp_ae_zone;                         /* 1210 */
    u_isp_ae_skip_crg                    isp_ae_skip_crg;                     /* 1214 */
    u_isp_ae_total_stat                  isp_ae_total_stat;                   /* 1218 */
    u_isp_ae_count_stat                  isp_ae_count_stat;                   /* 121c */
    u_isp_ae_total_r_aver                isp_ae_total_r_aver;                 /* 1220 */
    u_isp_ae_total_gr_aver               isp_ae_total_gr_aver;                /* 1224 */
    u_isp_ae_total_gb_aver               isp_ae_total_gb_aver;                /* 1228 */
    u_isp_ae_total_b_aver                isp_ae_total_b_aver;                 /* 122c */
    u_isp_ae_hist_high                   isp_ae_hist_high;                    /* 1230 */
    unsigned int                         rsr84[3];                            /* 1234 ~ 123c */
    u_isp_ae_bitmove                     isp_ae_bitmove;                      /* 1240 */
    u_isp_ae_offset_r_gr                 isp_ae_offset_r_gr;                  /* 1244 */
    unsigned int                         rsr85;                               /* 1248 */
    u_isp_ae_offset_gb_b                 isp_ae_offset_gb_b;                  /* 124c */
    unsigned int                         rsr86[5];                            /* 1250 ~ 1260 */
    u_isp_ae_lut_update                  isp_ae_lut_update;                   /* 1264 */
    u_isp_ae_stt_bst                     isp_ae_stt_bst;                      /* 1268 */
    unsigned int                         rsr87[2];                            /* 126c ~ 1270 */
    u_isp_ae_crop_pos                    isp_ae_crop_pos;                     /* 1274 */
    u_isp_ae_crop_outsize                isp_ae_crop_outsize;                 /* 1278 */
    unsigned int                         rsr88;                               /* 127c */
    u_isp_ae_hist_waddr                  isp_ae_hist_waddr;                   /* 1280 */
    u_isp_ae_hist_wdata                  isp_ae_hist_wdata;                   /* 1284 */
    u_isp_ae_hist_raddr                  isp_ae_hist_raddr;                   /* 1288 */
    u_isp_ae_hist_rdata                  isp_ae_hist_rdata;                   /* 128c */
    u_isp_ae_aver_r_gr_waddr             isp_ae_aver_r_gr_waddr;              /* 1290 */
    u_isp_ae_aver_r_gr_wdata             isp_ae_aver_r_gr_wdata;              /* 1294 */
    u_isp_ae_aver_r_gr_raddr             isp_ae_aver_r_gr_raddr;              /* 1298 */
    u_isp_ae_aver_r_gr_rdata             isp_ae_aver_r_gr_rdata;              /* 129c */
    u_isp_ae_aver_gb_b_waddr             isp_ae_aver_gb_b_waddr;              /* 12a0 */
    u_isp_ae_aver_gb_b_wdata             isp_ae_aver_gb_b_wdata;              /* 12a4 */
    u_isp_ae_aver_gb_b_raddr             isp_ae_aver_gb_b_raddr;              /* 12a8 */
    u_isp_ae_aver_gb_b_rdata             isp_ae_aver_gb_b_rdata;              /* 12ac */
    u_isp_ae_weight_waddr                isp_ae_weight_waddr;                 /* 12b0 */
    u_isp_ae_weight_wdata                isp_ae_weight_wdata;                 /* 12b4 */
    u_isp_ae_weight_raddr                isp_ae_weight_raddr;                 /* 12b8 */
    u_isp_ae_weight_rdata                isp_ae_weight_rdata;                 /* 12bc */
    unsigned int                         rsr89[17];                           /* 12c0 ~ 1300 */
    u_isp_dg_blc_cfg                     isp_dg_blc_cfg;                      /* 1304 */
    unsigned int                         rsr90;                               /* 1308 */
    u_isp_dg_version                     isp_dg_version;                      /* 130c */
    u_isp_dg_gain1                       isp_dg_gain1;                        /* 1310 */
    u_isp_dg_gain2                       isp_dg_gain2;                        /* 1314 */
    u_isp_dg_blc_offset1                 isp_dg_blc_offset1;                  /* 1318 */
    u_isp_dg_blc_offset2                 isp_dg_blc_offset2;                  /* 131c */
    u_isp_dg_clip_value                  isp_dg_clip_value;                   /* 1320 */
    unsigned int                         rsr91[58];                           /* 1324 ~ 1408 */
    u_isp_gamma_version                  isp_gamma_version;                   /* 140c */
    u_isp_gamma_bitw                     isp_gamma_bitw;                      /* 1410 */
    unsigned int                         rsr92[27];                           /* 1414 ~ 147c */
    u_isp_gamma_lut_waddr                isp_gamma_lut_waddr;                 /* 1480 */
    u_isp_gamma_lut_wdata                isp_gamma_lut_wdata;                 /* 1484 */
    u_isp_gamma_lut_raddr                isp_gamma_lut_raddr;                 /* 1488 */
    u_isp_gamma_lut_rdata                isp_gamma_lut_rdata;                 /* 148c */
    unsigned int                         rsr93[32];                           /* 1490 ~ 150c */
    u_isp_rc_blcen                       isp_rc_blcen;                        /* 1510 */
    u_isp_rc_blc0                        isp_rc_blc0;                         /* 1514 */
    u_isp_rc_blc1                        isp_rc_blc1;                         /* 1518 */
    unsigned int                         rsr94;                               /* 151c */
    u_isp_rc_sqradius                    isp_rc_sqradius;                     /* 1520 */
    u_isp_rc_center_coor                 isp_rc_center_coor;                  /* 1524 */
    unsigned int                         rsr95[25];                           /* 1528 ~ 1588 */
    u_isp_crop_version                   isp_crop_version;                    /* 158c */
    u_isp_crop_sizeout                   isp_crop_sizeout;                    /* 1590 */
    u_isp_crop_pos                       isp_crop_pos;                        /* 1594 */
} vicap_regs_type;

#endif /* VICAP_REG_H */