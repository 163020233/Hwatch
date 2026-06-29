// ******************************************************************************
// Copyright     :  Copyright (C) 2025, Hisilicon Technologies Co. Ltd.
// File name     :  c_union_define.h
// Project line  :  Ã½Ìå²úÆ·Ïß
// Department    :
// Author        :  x57490
// Version       :  1.0
// Date          :  2012/10/17
// Description   :  ÏîÄ¿ÃèÊöÐÅÏ¢
// Others        :  Generated automatically by nManager V4.1
// History       :  x57490 2025/07/11 15:29:43 Create file
// ******************************************************************************

#ifndef __REG_CRG_EXT_H__
#define __REG_CRG_EXT_H__

/* Define the union u_peri_crg_pll0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int apll_frac : 24;    /* [23..0]  */
        unsigned int apll_postdiv1 : 3; /* [26..24]  */
        unsigned int reserved_0 : 1;    /* [27]  */
        unsigned int apll_postdiv2 : 3; /* [30..28]  */
        unsigned int reserved_1 : 1;    /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_peri_crg_pll0;

/* Define the union u_peri_crg_pll1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int apll_fbdiv : 12;       /* [11..0]  */
        unsigned int apll_refdiv : 6;       /* [17..12]  */
        unsigned int reserved_0 : 2;        /* [19..18]  */
        unsigned int apll_pd : 1;           /* [20]  */
        unsigned int apll_foutvcopd : 1;    /* [21]  */
        unsigned int apll_fout4phasepd : 1; /* [22]  */
        unsigned int apll_postdivpd : 1;    /* [23]  */
        unsigned int apll_dacpd : 1;        /* [24]  */
        unsigned int apll_dsmpd : 1;        /* [25]  */
        unsigned int apll_bypass : 1;       /* [26]  */
        unsigned int apll_foutvco2xpd : 1;  /* [27]  */
        unsigned int reserved_1 : 4;        /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_peri_crg_pll1;

/* Define the union u_peri_crg4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int apll_ssmod_cfg : 17; /* [16..0]  */
        unsigned int reserved_0 : 15;     /* [31..17]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_peri_crg4;

/* Define the union u_peri_crg14 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int apll_lock : 1;       /* [0]  */
        unsigned int reserved_0 : 3;      /* [3..1]  */
        unsigned int apll_lock_final : 1; /* [4]  */
        unsigned int reserved_1 : 27;     /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_peri_crg14;

/* Define the union u_peri_crg15 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int apll_lock_cnt : 4; /* [3..0]  */
        unsigned int reserved_0 : 28;   /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_peri_crg15;

/* Define the union u_peri_crg16 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int apll_lock_srst_req : 1; /* [0]  */
        unsigned int reserved_0 : 23;        /* [23..1]  */
        unsigned int apll_lock_int_clr : 1;  /* [24]  */
        unsigned int reserved_1 : 7;         /* [31..25]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg16;

/* Define the union u_peri_crg25 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int apll_lock_cfg : 1; /* [0]  */
        unsigned int reserved_0 : 31;   /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg25;

/* Define the union u_peri_crg26 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int apll_cfg_nonsec_wr_disable : 4; /* [3..0]  */
        unsigned int reserved_0 : 28;                /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg26;

/* Define the union u_peri_crg27 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int apll_vco_cksel : 1; /* [0]  */
        unsigned int reserved_0 : 31;    /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg27;

/* Define the union u_peri_crg_pll32 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vpll_frac : 24;    /* [23..0]  */
        unsigned int vpll_postdiv1 : 3; /* [26..24]  */
        unsigned int reserved_0 : 1;    /* [27]  */
        unsigned int vpll_postdiv2 : 3; /* [30..28]  */
        unsigned int reserved_1 : 1;    /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg_pll32;

/* Define the union u_peri_crg_pll33 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vpll_fbdiv : 12;       /* [11..0]  */
        unsigned int vpll_refdiv : 6;       /* [17..12]  */
        unsigned int reserved_0 : 2;        /* [19..18]  */
        unsigned int vpll_pd : 1;           /* [20]  */
        unsigned int vpll_foutvcopd : 1;    /* [21]  */
        unsigned int vpll_fout4phasepd : 1; /* [22]  */
        unsigned int vpll_postdivpd : 1;    /* [23]  */
        unsigned int vpll_dacpd : 1;        /* [24]  */
        unsigned int vpll_dsmpd : 1;        /* [25]  */
        unsigned int vpll_bypass : 1;       /* [26]  */
        unsigned int vpll_foutvco2xpd : 1;  /* [27]  */
        unsigned int reserved_1 : 4;        /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg_pll33;

/* Define the union u_peri_crg36 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vpll_ssmod_cfg : 17; /* [16..0]  */
        unsigned int reserved_0 : 15;     /* [31..17]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg36;

/* Define the union u_peri_crg46 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vpll_lock : 1;       /* [0]  */
        unsigned int reserved_0 : 3;      /* [3..1]  */
        unsigned int vpll_lock_final : 1; /* [4]  */
        unsigned int reserved_1 : 27;     /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg46;

/* Define the union u_peri_crg47 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vpll_lock_cnt : 4; /* [3..0]  */
        unsigned int reserved_0 : 28;   /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg47;

/* Define the union u_peri_crg48 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vpll_lock_srst_req : 1; /* [0]  */
        unsigned int reserved_0 : 23;        /* [23..1]  */
        unsigned int vpll_lock_int_clr : 1;  /* [24]  */
        unsigned int reserved_1 : 7;         /* [31..25]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg48;

/* Define the union u_peri_crg57 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vpll_lock_cfg : 1; /* [0]  */
        unsigned int reserved_0 : 31;   /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg57;

/* Define the union u_peri_crg58 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vpll_cfg_nonsec_wr_disable : 4; /* [3..0]  */
        unsigned int reserved_0 : 28;                /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg58;

/* Define the union u_peri_crg2048 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved_0 : 4;    /* [3..0]  */
        unsigned int cfgsys_cksel : 3;  /* [6..4]  */
        unsigned int reserved_1 : 1;    /* [7]  */
        unsigned int dataaxi_cksel : 3; /* [10..8]  */
        unsigned int reserved_2 : 1;    /* [11]  */
        unsigned int tcxo_div_cken : 1; /* [12]  */
        unsigned int reserved_3 : 19;   /* [31..13]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg2048;

/* Define the union u_peri_crg2056 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved_0 : 4;       /* [3..0]  */
        unsigned int cfgsys_sc_seled : 3;  /* [6..4]  */
        unsigned int reserved_1 : 1;       /* [7]  */
        unsigned int dataaxi_sc_seled : 3; /* [10..8]  */
        unsigned int reserved_2 : 1;       /* [11]  */
        unsigned int reserved_3 : 20;      /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg2056;

/* Define the union u_peri_crg2064 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved_0 : 3;        /* [2..0]  */
        unsigned int reserved_1 : 1;        /* [3]  */
        unsigned int arm_dbg0_srst_req : 1; /* [4]  */
        unsigned int reserved_2 : 4;        /* [8..5]  */
        unsigned int reserved_3 : 1;        /* [9]  */
        unsigned int arm_dbg1_srst_req : 1; /* [10]  */
        unsigned int reserved_4 : 1;        /* [11]  */
        unsigned int l2_srst_req : 1;       /* [12]  */
        unsigned int reserved_5 : 1;        /* [13]  */
        unsigned int socdbg_srst_req : 1;   /* [14]  */
        unsigned int reserved_6 : 1;        /* [15]  */
        unsigned int cs_srst_req : 1;       /* [16]  */
        unsigned int reserved_7 : 1;        /* [17]  */
        unsigned int a7_cksel : 3;          /* [20..18]  */
        unsigned int reserved_8 : 11;       /* [31..21]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg2064;

/* Define the union u_peri_crg2065 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int a7_sc_seled : 3; /* [2..0]  */
        unsigned int reserved_0 : 8;  /* [10..3]  */
        unsigned int reserved_1 : 21; /* [31..11]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg2065;

/* Define the union u_peri_crg2066 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mem_adjust_bypass : 1; /* [0]  */
        unsigned int reserved_0 : 31;       /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg2066;

/* Define the union u_peri_crg2067 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved_0 : 4;         /* [3..0]  */
        unsigned int arm_core0_srst_req : 1; /* [4]  */
        unsigned int reserved_1 : 3;         /* [7..5]  */
        unsigned int arm_core1_srst_req : 1; /* [8]  */
        unsigned int reserved_2 : 23;        /* [31..9]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg2067;

/* Define the union u_peri_crg2068 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cpu_cfg_nonsec_wr_disable : 4; /* [3..0]  */
        unsigned int reserved_0 : 28;               /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg2068;

/* Define the union u_peri_crg2192 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved_0 : 24;          /* [23..0]  */
        unsigned int soc_cs_ckgate_bypass : 1; /* [24]  */
        unsigned int reserved_1 : 7;           /* [31..25]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg2192;

/* Define the union u_peri_crg2208 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ddr_apb_srst_req : 1;     /* [0]  */
        unsigned int reserved_0 : 3;           /* [3..1]  */
        unsigned int ddr_ref_div_srst_req : 1; /* [4]  */
        unsigned int reserved_1 : 3;           /* [7..5]  */
        unsigned int ddr_bypass_cken : 1;      /* [8]  */
        unsigned int ddr_dfi_cken : 1;         /* [9]  */
        unsigned int ddr_apb_cken : 1;         /* [10]  */
        unsigned int reserved_2 : 1;           /* [11]  */
        unsigned int ddr_ref_clk_div : 5;      /* [16..12]  */
        unsigned int reserved_3 : 3;           /* [19..17]  */
        unsigned int dmc_dfi_cksel : 1;        /* [20]  */
        unsigned int ddr_bypass_cksel : 3;     /* [23..21]  */
        unsigned int ddr_ref_cksel : 2;        /* [25..24]  */
        unsigned int reserved_4 : 2;           /* [27..26]  */
        unsigned int reserved_5 : 4;           /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg2208;

/* Define the union u_peri_crg2209 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved_0 : 8;          /* [7..0]  */
        unsigned int ddr_bypass_sc_seled : 3; /* [10..8]  */
        unsigned int reserved_1 : 21;         /* [31..11]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg2209;

/* Define the union u_peri_crg2216 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ddrt_srst_req : 1; /* [0]  */
        unsigned int reserved_0 : 3;    /* [3..1]  */
        unsigned int ddrt_cken : 1;     /* [4]  */
        unsigned int reserved_1 : 27;   /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg2216;

/* Define the union u_peri_crg2720 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int edma_srst_req : 1; /* [0]  */
        unsigned int reserved_0 : 3;    /* [3..1]  */
        unsigned int edma_apb_cken : 1; /* [4]  */
        unsigned int edma_axi_cken : 1; /* [5]  */
        unsigned int reserved_1 : 26;   /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg2720;

/* Define the union u_peri_crg2784 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gzip_srst_req : 1; /* [0]  */
        unsigned int reserved_0 : 3;    /* [3..1]  */
        unsigned int gzip_cken : 1;     /* [4]  */
        unsigned int reserved_1 : 7;    /* [11..5]  */
        unsigned int gzip_cksel : 2;    /* [13..12]  */
        unsigned int reserved_2 : 18;   /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg2784;

/* Define the union u_peri_crg2785 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gzip_rst_ok : 1; /* [0]  */
        unsigned int reserved_0 : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg2785;

/* Define the union u_peri_crg3440 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sdio0_mmc_cken : 1;        /* [0]  */
        unsigned int sdio0_mmc_hclk_cken : 1;   /* [1]  */
        unsigned int reserved_0 : 14;           /* [15..2]  */
        unsigned int sdio0_mmc_srst_req : 1;    /* [16]  */
        unsigned int sdio0_mmc_rx_srst_req : 1; /* [17]  */
        unsigned int sdio0_mmc_tx_srst_req : 1; /* [18]  */
        unsigned int reserved_1 : 5;            /* [23..19]  */
        unsigned int sdio0_cksel : 3;           /* [26..24]  */
        unsigned int reserved_2 : 5;            /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg3440;

/* Define the union u_peri_crg3441 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sdio0_p4_dll_stop : 1;      /* [0]  */
        unsigned int sdio0_mmc_dll_srst_req : 1; /* [1]  */
        unsigned int reserved_0 : 30;            /* [31..2]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg3441;

/* Define the union u_peri_crg3442 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sdio0_drv_dll_ssel : 8;      /* [7..0]  */
        unsigned int sdio0_drv_dll_mode : 1;      /* [8]  */
        unsigned int sdio0_drv_dll_bypass : 1;    /* [9]  */
        unsigned int sdio0_drv_dll_slave_en : 1;  /* [10]  */
        unsigned int sdio0_drv_dll_tune : 4;      /* [14..11]  */
        unsigned int sdio0_drv_clk_phase_sel : 5; /* [19..15]  */
        unsigned int reserved_0 : 12;             /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg3442;

/* Define the union u_peri_crg3443 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sdio0_sam_dll_ssel : 8;     /* [7..0]  */
        unsigned int sdio0_sam_dll_mode : 1;     /* [8]  */
        unsigned int reserved_0 : 1;             /* [9]  */
        unsigned int sdio0_sam_dll_slave_en : 1; /* [10]  */
        unsigned int sdio0_sam_dll_tune : 4;     /* [14..11]  */
        unsigned int reserved_1 : 17;            /* [31..15]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg3443;

/* Define the union u_peri_crg3444 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sdio0_ds_dll_ssel : 8;     /* [7..0]  */
        unsigned int sdio0_ds_dll_mode : 1;     /* [8]  */
        unsigned int sdio0_ds_dll_bypass : 1;   /* [9]  */
        unsigned int sdio0_ds_dll_slave_en : 1; /* [10]  */
        unsigned int sdio0_ds_dll_tune : 4;     /* [14..11]  */
        unsigned int reserved_0 : 17;           /* [31..15]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg3444;

/* Define the union u_peri_crg3445 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sdio0_sample_b_cclk_sel : 5; /* [4..0]  */
        unsigned int reserved_0 : 27;             /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg3445;

/* Define the union u_peri_crg3446 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sdio0_p4_dll_mdly_tap : 8;         /* [7..0]  */
        unsigned int sdio0_p4_dll_overflow : 1;         /* [8]  */
        unsigned int sdio0_p4_dll_locked : 1;           /* [9]  */
        unsigned int sdio0_ds_dll_ready : 1;            /* [10]  */
        unsigned int sdio0_drv_dll_ready : 1;           /* [11]  */
        unsigned int sdio0_sam_dll_ready : 1;           /* [12]  */
        unsigned int reserve_for_eco_sdio0_crg_out : 4; /* [16..13]  */
        unsigned int reserved_0 : 15;                   /* [31..17]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg3446;

/* Define the union u_peri_crg3504 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sdio1_mmc_cken : 1;        /* [0]  */
        unsigned int sdio1_mmc_hclk_cken : 1;   /* [1]  */
        unsigned int reserved_0 : 14;           /* [15..2]  */
        unsigned int sdio1_mmc_srst_req : 1;    /* [16]  */
        unsigned int sdio1_mmc_rx_srst_req : 1; /* [17]  */
        unsigned int sdio1_mmc_tx_srst_req : 1; /* [18]  */
        unsigned int reserved_1 : 5;            /* [23..19]  */
        unsigned int sdio1_cksel : 3;           /* [26..24]  */
        unsigned int reserved_2 : 5;            /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg3504;

/* Define the union u_peri_crg3505 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sdio1_p4_dll_stop : 1;      /* [0]  */
        unsigned int sdio1_mmc_dll_srst_req : 1; /* [1]  */
        unsigned int reserved_0 : 30;            /* [31..2]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg3505;

/* Define the union u_peri_crg3506 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sdio1_drv_dll_ssel : 8;      /* [7..0]  */
        unsigned int sdio1_drv_dll_mode : 1;      /* [8]  */
        unsigned int sdio1_drv_dll_bypass : 1;    /* [9]  */
        unsigned int sdio1_drv_dll_slave_en : 1;  /* [10]  */
        unsigned int sdio1_drv_dll_tune : 4;      /* [14..11]  */
        unsigned int sdio1_drv_clk_phase_sel : 5; /* [19..15]  */
        unsigned int reserved_0 : 12;             /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg3506;

/* Define the union u_peri_crg3507 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sdio1_sam_dll_ssel : 8;     /* [7..0]  */
        unsigned int sdio1_sam_dll_mode : 1;     /* [8]  */
        unsigned int reserved_0 : 1;             /* [9]  */
        unsigned int sdio1_sam_dll_slave_en : 1; /* [10]  */
        unsigned int sdio1_sam_dll_tune : 4;     /* [14..11]  */
        unsigned int reserved_1 : 17;            /* [31..15]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg3507;

/* Define the union u_peri_crg3509 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sdio1_sample_b_cclk_sel : 5; /* [4..0]  */
        unsigned int reserved_0 : 27;             /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg3509;

/* Define the union u_peri_crg3510 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sdio1_p4_dll_mdly_tap : 8;         /* [7..0]  */
        unsigned int sdio1_p4_dll_overflow : 1;         /* [8]  */
        unsigned int sdio1_p4_dll_locked : 1;           /* [9]  */
        unsigned int reserved_0 : 1;                    /* [10]  */
        unsigned int sdio1_drv_dll_ready : 1;           /* [11]  */
        unsigned int sdio1_sam_dll_ready : 1;           /* [12]  */
        unsigned int reserve_for_eco_sdio1_crg_out : 4; /* [16..13]  */
        unsigned int reserved_1 : 15;                   /* [31..17]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg3510;

/* Define the union u_peri_crg3571 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hrst_eth_s : 1;     /* [0]  */
        unsigned int eth_cken : 1;       /* [1]  */
        unsigned int fephy_cken : 1;     /* [2]  */
        unsigned int fephy_srst_req : 1; /* [3]  */
        unsigned int reserved_0 : 3;     /* [6..4]  */
        unsigned int ethcore_cksel : 1;  /* [7]  */
        unsigned int reserved_1 : 24;    /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg3571;

/* Define the union u_peri_crg3632 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int usb2_phy_rst_req : 1;       /* [0]  */
        unsigned int usb2_phy_utmi_rst_req : 1;  /* [1]  */
        unsigned int usb2_phy_apb_rst_req : 1;   /* [2]  */
        unsigned int reserved_0 : 1;             /* [3]  */
        unsigned int usb2_phy_ref_xtal_cken : 1; /* [4]  */
        unsigned int usb2_phy_ref_pll_cken : 1;  /* [5]  */
        unsigned int reserved_1 : 26;            /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg3632;

/* Define the union u_peri_crg3634 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int usb2_srst_req : 1;      /* [0]  */
        unsigned int reserved_0 : 3;         /* [3..1]  */
        unsigned int usb2_bus_cken : 1;      /* [4]  */
        unsigned int usb2_ref_cken : 1;      /* [5]  */
        unsigned int reserved_1 : 2;         /* [7..6]  */
        unsigned int usb2_utmi_cken : 1;     /* [8]  */
        unsigned int reserved_2 : 7;         /* [15..9]  */
        unsigned int usb2_freeclk_cksel : 1; /* [16]  */
        unsigned int reserved_3 : 15;        /* [31..17]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg3634;

/* Define the union u_peri_crg4048 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int fmc_srst_req : 1; /* [0]  */
        unsigned int reserved_0 : 3;   /* [3..1]  */
        unsigned int fmc_cken : 1;     /* [4]  */
        unsigned int reserved_1 : 7;   /* [11..5]  */
        unsigned int fmc_cksel : 3;    /* [14..12]  */
        unsigned int reserved_2 : 17;  /* [31..15]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg4048;

/* Define the union u_peri_crg4049 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved_0 : 8;   /* [7..0]  */
        unsigned int fmc_sc_seled : 3; /* [10..8]  */
        unsigned int reserved_1 : 21;  /* [31..11]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg4049;

/* Define the union u_peri_crg4192 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int uart0_srst_req : 1; /* [0]  */
        unsigned int reserved_0 : 3;     /* [3..1]  */
        unsigned int uart0_cken : 1;     /* [4]  */
        unsigned int reserved_1 : 7;     /* [11..5]  */
        unsigned int uart0_cksel : 2;    /* [13..12]  */
        unsigned int reserved_2 : 18;    /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg4192;

/* Define the union u_peri_crg4194 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int uart1_srst_req : 1; /* [0]  */
        unsigned int reserved_0 : 3;     /* [3..1]  */
        unsigned int uart1_cken : 1;     /* [4]  */
        unsigned int reserved_1 : 7;     /* [11..5]  */
        unsigned int uart1_cksel : 2;    /* [13..12]  */
        unsigned int reserved_2 : 18;    /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg4194;

/* Define the union u_peri_crg4196 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int uart2_srst_req : 1; /* [0]  */
        unsigned int reserved_0 : 3;     /* [3..1]  */
        unsigned int uart2_cken : 1;     /* [4]  */
        unsigned int reserved_1 : 7;     /* [11..5]  */
        unsigned int uart2_cksel : 2;    /* [13..12]  */
        unsigned int reserved_2 : 18;    /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg4196;

/* Define the union u_peri_crg4256 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int i2c0_srst_req : 1; /* [0]  */
        unsigned int reserved_0 : 3;    /* [3..1]  */
        unsigned int i2c0_cken : 1;     /* [4]  */
        unsigned int reserved_1 : 7;    /* [11..5]  */
        unsigned int i2c0_cksel : 1;    /* [12]  */
        unsigned int reserved_2 : 19;   /* [31..13]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg4256;

/* Define the union u_peri_crg4258 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int i2c1_srst_req : 1; /* [0]  */
        unsigned int reserved_0 : 3;    /* [3..1]  */
        unsigned int i2c1_cken : 1;     /* [4]  */
        unsigned int reserved_1 : 7;    /* [11..5]  */
        unsigned int i2c1_cksel : 1;    /* [12]  */
        unsigned int reserved_2 : 19;   /* [31..13]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg4258;

/* Define the union u_peri_crg4260 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int i2c2_srst_req : 1; /* [0]  */
        unsigned int reserved_0 : 3;    /* [3..1]  */
        unsigned int i2c2_cken : 1;     /* [4]  */
        unsigned int reserved_1 : 7;    /* [11..5]  */
        unsigned int i2c2_cksel : 1;    /* [12]  */
        unsigned int reserved_2 : 19;   /* [31..13]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg4260;

/* Define the union u_peri_crg4384 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ssp0_srst_req : 1; /* [0]  */
        unsigned int reserved_0 : 3;    /* [3..1]  */
        unsigned int ssp0_cken : 1;     /* [4]  */
        unsigned int reserved_1 : 27;   /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg4384;

/* Define the union u_peri_crg4386 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ssp1_srst_req : 1; /* [0]  */
        unsigned int reserved_0 : 3;    /* [3..1]  */
        unsigned int ssp1_cken : 1;     /* [4]  */
        unsigned int reserved_1 : 27;   /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg4386;

/* Define the union u_peri_crg4448 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int svb_pwm_srst_req : 1; /* [0]  */
        unsigned int reserved_0 : 3;       /* [3..1]  */
        unsigned int svb_pwm_cken : 1;     /* [4]  */
        unsigned int reserved_1 : 7;       /* [11..5]  */
        unsigned int svb_pwm_cksel : 2;    /* [13..12]  */
        unsigned int reserved_2 : 18;      /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg4448;

/* Define the union u_peri_crg4454 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pwm_srst_req : 1; /* [0]  */
        unsigned int reserved_0 : 3;   /* [3..1]  */
        unsigned int pwm_cken : 1;     /* [4]  */
        unsigned int reserved_1 : 7;   /* [11..5]  */
        unsigned int pwm_cksel : 2;    /* [13..12]  */
        unsigned int reserved_2 : 18;  /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg4454;

/* Define the union u_peri_crg4528 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int lsadc_srst_req : 1; /* [0]  */
        unsigned int reserved_0 : 3;     /* [3..1]  */
        unsigned int lsadc_cken : 1;     /* [4]  */
        unsigned int reserved_1 : 27;    /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg4528;

/* Define the union u_peri_crg4560 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int tsensor_ctrl_pclk_cken : 1; /* [0]  */
        unsigned int reserved_0 : 3;             /* [3..1]  */
        unsigned int tsensor_ctrl_cken : 1;      /* [4]  */
        unsigned int reserved_1 : 27;            /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg4560;

/* Define the union u_peri_crg4564 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int timer01_pclk_cken : 1; /* [0]  */
        unsigned int reserved_0 : 3;        /* [3..1]  */
        unsigned int timer01_cken : 1;      /* [4]  */
        unsigned int reserved_1 : 27;       /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg4564;

/* Define the union u_peri_crg4565 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int timer23_pclk_cken : 1; /* [0]  */
        unsigned int reserved_0 : 3;        /* [3..1]  */
        unsigned int timer23_cken : 1;      /* [4]  */
        unsigned int reserved_1 : 27;       /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg4565;

/* Define the union u_peri_crg4570 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gpio0_pclk_cken : 1;  /* [0]  */
        unsigned int gpio1_pclk_cken : 1;  /* [1]  */
        unsigned int gpio2_pclk_cken : 1;  /* [2]  */
        unsigned int gpio3_pclk_cken : 1;  /* [3]  */
        unsigned int gpio4_pclk_cken : 1;  /* [4]  */
        unsigned int gpio5_pclk_cken : 1;  /* [5]  */
        unsigned int gpio6_pclk_cken : 1;  /* [6]  */
        unsigned int gpio7_pclk_cken : 1;  /* [7]  */
        unsigned int gpio8_pclk_cken : 1;  /* [8]  */
        unsigned int gpio9_pclk_cken : 1;  /* [9]  */
        unsigned int gpio10_pclk_cken : 1; /* [10]  */
        unsigned int reserved_0 : 13;      /* [23..11]  */
        unsigned int reserved_1 : 8;       /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg4570;

/* Define the union u_peri_crg4705 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hpm_8t_srst_req : 1; /* [0]  */
        unsigned int reserved_0 : 3;      /* [3..1]  */
        unsigned int hpm_8t_cken : 1;     /* [4]  */
        unsigned int reserved_1 : 27;     /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg4705;

/* Define the union u_peri_crg4706 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hpm_ctrl_pclk_cken : 1; /* [0]  */
        unsigned int reserved_0 : 3;         /* [3..1]  */
        unsigned int hpm_ctrl_cken : 1;      /* [4]  */
        unsigned int reserved_1 : 27;        /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg4706;

/* Define the union u_peri_crg4897 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cpm_srst_req : 1; /* [0]  */
        unsigned int reserved_0 : 3;   /* [3..1]  */
        unsigned int cpm_cken : 1;     /* [4]  */
        unsigned int reserved_1 : 27;  /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg4897;

/* Define the union u_peri_crg6558 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved_0 : 4;      /* [3..0]  */
        unsigned int npu_l_cken : 1;      /* [4]  */
        unsigned int reserved_1 : 3;      /* [7..5]  */
        unsigned int npu_l_aclk_cken : 1; /* [8]  */
        unsigned int npu_l_pclk_cken : 1; /* [9]  */
        unsigned int reserved_2 : 22;     /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg6558;

/* Define the union u_peri_crg6559 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved_0 : 4;      /* [3..0]  */
        unsigned int npu_r_cken : 1;      /* [4]  */
        unsigned int reserved_1 : 3;      /* [7..5]  */
        unsigned int npu_r_aclk_cken : 1; /* [8]  */
        unsigned int npu_r_pclk_cken : 1; /* [9]  */
        unsigned int reserved_2 : 22;     /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg6559;

/* Define the union u_peri_crg6560 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int npu_srst_req : 1;      /* [0]  */
        unsigned int npu_pipe_srst_req : 1; /* [1]  */
        unsigned int reserved_0 : 10;       /* [11..2]  */
        unsigned int npu_cksel : 3;         /* [14..12]  */
        unsigned int reserved_1 : 17;       /* [31..15]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg6560;

/* Define the union u_peri_crg6561 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int npu_rst_ok : 1;  /* [0]  */
        unsigned int reserved_0 : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg6561;

/* Define the union u_peri_crg7248 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vedu_srst_req : 1; /* [0]  */
        unsigned int vedu_cken : 1;     /* [1]  */
        unsigned int reserved_0 : 6;    /* [7..2]  */
        unsigned int vedu_cksel : 2;    /* [9..8]  */
        unsigned int reserved_1 : 22;   /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg7248;

/* Define the union u_peri_crg7376 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int jpge_srst_req : 1; /* [0]  */
        unsigned int jpge_cken : 1;     /* [1]  */
        unsigned int reserved_0 : 6;    /* [7..2]  */
        unsigned int jpge_cksel : 2;    /* [9..8]  */
        unsigned int reserved_1 : 22;   /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg7376;

/* Define the union u_peri_crg8464 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sensor0_srst_req : 1;      /* [0]  */
        unsigned int sensor0_ctrl_srst_req : 1; /* [1]  */
        unsigned int reserved_0 : 2;            /* [3..2]  */
        unsigned int sensor0_cken : 1;          /* [4]  */
        unsigned int reserved_1 : 7;            /* [11..5]  */
        unsigned int sensor0_cksel : 4;         /* [15..12]  */
        unsigned int clk_sensor0_pctrl : 1;     /* [16]  */
        unsigned int reserved_2 : 15;           /* [31..17]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg8464;

/* Define the union u_peri_crg8472 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sensor1_srst_req : 1;      /* [0]  */
        unsigned int sensor1_ctrl_srst_req : 1; /* [1]  */
        unsigned int reserved_0 : 2;            /* [3..2]  */
        unsigned int sensor1_cken : 1;          /* [4]  */
        unsigned int reserved_1 : 7;            /* [11..5]  */
        unsigned int sensor1_cksel : 4;         /* [15..12]  */
        unsigned int clk_sensor1_pctrl : 1;     /* [16]  */
        unsigned int reserved_2 : 15;           /* [31..17]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg8472;

/* Define the union u_peri_crg8528 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mipirx_bus_srst_req : 1; /* [0]  */
        unsigned int reserved_0 : 2;          /* [2..1]  */
        unsigned int cbar_cken : 1;           /* [3]  */
        unsigned int cil_cken : 1;            /* [4]  */
        unsigned int mipirx_bus_cken : 1;     /* [5]  */
        unsigned int reserved_1 : 14;         /* [19..6]  */
        unsigned int mipirx_hs0_pctrl : 1;    /* [20]  */
        unsigned int mipirx_hs1_pctrl : 1;    /* [21]  */
        unsigned int reserved_2 : 10;         /* [31..22]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg8528;

/* Define the union u_peri_crg8536 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mipi_pix0_core_srst_req : 1; /* [0]  */
        unsigned int reserved_0 : 3;              /* [3..1]  */
        unsigned int mipi_pix0_cken : 1;          /* [4]  */
        unsigned int reserved_1 : 27;             /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg8536;

/* Define the union u_peri_crg8544 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mipi_pix1_core_srst_req : 1; /* [0]  */
        unsigned int reserved_0 : 3;              /* [3..1]  */
        unsigned int mipi_pix1_cken : 1;          /* [4]  */
        unsigned int reserved_1 : 27;             /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg8544;

/* Define the union u_peri_crg9296 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vicap_srst_req : 1; /* [0]  */
        unsigned int reserved_0 : 3;     /* [3..1]  */
        unsigned int vi_sc_cken : 1;     /* [4]  */
        unsigned int vi_bus_cken : 1;    /* [5]  */
        unsigned int reserved_1 : 6;     /* [11..6]  */
        unsigned int vi_sc_cksel : 3;    /* [14..12]  */
        unsigned int reserved_2 : 17;    /* [31..15]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg9296;

/* Define the union u_peri_crg9297 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vicap_softrst_state : 1; /* [0]  */
        unsigned int reserved_0 : 31;         /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg9297;

/* Define the union u_peri_crg9300 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int isp0_cfg_srst_req : 1;  /* [0]  */
        unsigned int isp0_core_srst_req : 1; /* [1]  */
        unsigned int reserved_0 : 2;         /* [3..2]  */
        unsigned int isp0_cken : 1;          /* [4]  */
        unsigned int reserved_1 : 7;         /* [11..5]  */
        unsigned int isp0_cksel : 3;         /* [14..12]  */
        unsigned int reserved_2 : 17;        /* [31..15]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg9300;

/* Define the union u_peri_crg9301 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int isp1_cfg_srst_req : 1;  /* [0]  */
        unsigned int isp1_core_srst_req : 1; /* [1]  */
        unsigned int reserved_0 : 2;         /* [3..2]  */
        unsigned int reserved_1 : 28;        /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg9301;

/* Define the union u_peri_crg9304 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved_0 : 20;   /* [19..0]  */
        unsigned int vi_cmos_pctrl : 1; /* [20]  */
        unsigned int reserved_1 : 11;   /* [31..21]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg9304;

/* Define the union u_peri_crg9305 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vi_p0_srst_req : 1; /* [0]  */
        unsigned int reserved_0 : 3;     /* [3..1]  */
        unsigned int vi0_cken : 1;       /* [4]  */
        unsigned int reserved_1 : 7;     /* [11..5]  */
        unsigned int vi_p0_cksel : 3;    /* [14..12]  */
        unsigned int reserved_2 : 17;    /* [31..15]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg9305;

/* Define the union u_peri_crg9313 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vi_p1_srst_req : 1; /* [0]  */
        unsigned int reserved_0 : 3;     /* [3..1]  */
        unsigned int vi1_cken : 1;       /* [4]  */
        unsigned int reserved_1 : 7;     /* [11..5]  */
        unsigned int vi_p1_cksel : 3;    /* [14..12]  */
        unsigned int reserved_2 : 17;    /* [31..15]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg9313;

/* Define the union u_peri_crg9331 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int eco12_reserv : 16; /* [15..0]  */
        unsigned int reserved_0 : 16;   /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg9331;

/* Define the union u_peri_crg9680 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int viproc_srst_req : 1;     /* [0]  */
        unsigned int reserved_0 : 3;          /* [3..1]  */
        unsigned int viproc_cken : 1;         /* [4]  */
        unsigned int reserved_1 : 7;          /* [11..5]  */
        unsigned int viproc_cksel : 3;        /* [14..12]  */
        unsigned int reserved_2 : 1;          /* [15]  */
        unsigned int viproc_online_cksel : 1; /* [16]  */
        unsigned int reserved_3 : 15;         /* [31..17]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg9680;

/* Define the union u_peri_crg9681 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int viproc_softrst_state : 1; /* [0]  */
        unsigned int reserved_0 : 31;          /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg9681;

/* Define the union u_peri_crg9936 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vpss_srst_req : 1;     /* [0]  */
        unsigned int reserved_0 : 3;        /* [3..1]  */
        unsigned int vpss_cken : 1;         /* [4]  */
        unsigned int reserved_1 : 7;        /* [11..5]  */
        unsigned int vpss_cksel : 3;        /* [14..12]  */
        unsigned int reserved_2 : 1;        /* [15]  */
        unsigned int vpss_online_cksel : 1; /* [16]  */
        unsigned int reserved_3 : 15;       /* [31..17]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg9936;

/* Define the union u_peri_crg9937 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vpss_softrst_state : 1; /* [0]  */
        unsigned int reserved_0 : 31;        /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg9937;

/* Define the union u_peri_crg10160 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vgs_srst_req : 1; /* [0]  */
        unsigned int reserved_0 : 11;  /* [11..1]  */
        unsigned int vgs_cken : 1;     /* [12]  */
        unsigned int reserved_1 : 19;  /* [31..13]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg10160;

/* Define the union u_peri_crg10161 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vgs_softrst_state : 1; /* [0]  */
        unsigned int reserved_0 : 31;       /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg10161;

/* Define the union u_peri_crg10784 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int aiao_srst_req : 1;  /* [0]  */
        unsigned int reserved_0 : 3;     /* [3..1]  */
        unsigned int aiao_pll_cken : 1;  /* [4]  */
        unsigned int aiao_cken : 1;      /* [5]  */
        unsigned int reserved_1 : 6;     /* [11..6]  */
        unsigned int aiao_pll_cksel : 1; /* [12]  */
        unsigned int aiao_cksel : 2;     /* [14..13]  */
        unsigned int reserved_2 : 17;    /* [31..15]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg10784;

/* Define the union u_peri_crg10912 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int codec_dig_rst_req : 1; /* [0]  */
        unsigned int codec_ana_rst_req : 1; /* [1]  */
        unsigned int reserved_0 : 2;        /* [3..2]  */
        unsigned int codec_apb_cken : 1;    /* [4]  */
        unsigned int reserved_1 : 27;       /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg10912;

/* Define the union u_peri_crg12288 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int test_clk_en : 1;       /* [0]  */
        unsigned int reserved_0 : 3;        /* [3..1]  */
        unsigned int test_clk_sel : 3;      /* [6..4]  */
        unsigned int reserved_1 : 1;        /* [7]  */
        unsigned int vpll_test_out_sel : 1; /* [8]  */
        unsigned int apll_test_out_sel : 1; /* [9]  */
        unsigned int reserved_2 : 22;       /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg12288;

/* Define the union u_peri_crg12290 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int eco0_reserv : 16; /* [15..0]  */
        unsigned int reserved_0 : 16;  /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg12290;

/* Define the union u_peri_crg12291 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int eco1_reserv : 16; /* [15..0]  */
        unsigned int reserved_0 : 16;  /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg12291;

/* Define the union u_peri_crg12292 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int eco1_cfg_nonsec_wr_disable : 4; /* [3..0]  */
        unsigned int reserved_0 : 28;                /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg12292;

/* Define the union u_peri_crg12293 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int eco2_reserv : 16; /* [15..0]  */
        unsigned int reserved_0 : 16;  /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg12293;

/* Define the union u_peri_crg12294 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int eco3_reserv : 16; /* [15..0]  */
        unsigned int reserved_0 : 16;  /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg12294;

/* Define the union u_peri_crg12295 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int eco4_reserv : 16; /* [15..0]  */
        unsigned int reserved_0 : 16;  /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg12295;

/* Define the union u_peri_crg12296 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int eco5_reserv : 16; /* [15..0]  */
        unsigned int reserved_0 : 16;  /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg12296;

/* Define the union u_peri_crg12297 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int eco6_reserv : 16; /* [15..0]  */
        unsigned int reserved_0 : 16;  /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg12297;

/* Define the union u_peri_crg12298 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int eco7_reserv : 16; /* [15..0]  */
        unsigned int reserved_0 : 16;  /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;

} u_peri_crg12298;

//==============================================================================
/* Define the global struct */
typedef struct {
    volatile unsigned int reserved_28[32];   /* 0x4500~0x457c */
    volatile u_peri_crg4448 peri_crg4448;    /* 0x4580 */
    volatile unsigned int reserved_29[5];    /* 0x4584~0x4594 */
    volatile u_peri_crg4454 peri_crg4454;    /* 0x4598 */
    volatile unsigned int reserved_30[73];   /* 0x459c~0x46bc */
    volatile u_peri_crg4528 peri_crg4528;    /* 0x46c0 */
    volatile unsigned int reserved_31[31];   /* 0x46c4~0x473c */
    volatile u_peri_crg4560 peri_crg4560;    /* 0x4740 */
    volatile unsigned int reserved_32[3];    /* 0x4744~0x474c */
    volatile u_peri_crg4564 peri_crg4564;    /* 0x4750 */
    volatile u_peri_crg4565 peri_crg4565;    /* 0x4754 */
    volatile unsigned int reserved_33[4];    /* 0x4758~0x4764 */
    volatile u_peri_crg4570 peri_crg4570;    /* 0x4768 */
    volatile unsigned int reserved_34[134];  /* 0x476c~0x4980 */
    volatile u_peri_crg4705 peri_crg4705;    /* 0x4984 */
    volatile u_peri_crg4706 peri_crg4706;    /* 0x4988 */
    volatile unsigned int reserved_35[190];  /* 0x498c~0x4c80 */
    volatile u_peri_crg4897 peri_crg4897;    /* 0x4c84 */
    volatile unsigned int reserved_36[1660]; /* 0x4c88~0x6674 */
    volatile u_peri_crg6558 peri_crg6558;    /* 0x6678 */
    volatile u_peri_crg6559 peri_crg6559;    /* 0x667c */
    volatile u_peri_crg6560 peri_crg6560;    /* 0x6680 */
    volatile u_peri_crg6561 peri_crg6561;    /* 0x6684 */
    volatile unsigned int reserved_37[686];  /* 0x6688~0x713c */
    volatile u_peri_crg7248 peri_crg7248;    /* 0x7140 */
    volatile unsigned int reserved_38[127];  /* 0x7144~0x733c */
    volatile u_peri_crg7376 peri_crg7376;    /* 0x7340 */
    volatile unsigned int reserved_39[1087]; /* 0x7344~0x843c */
    volatile u_peri_crg8464 peri_crg8464;    /* 0x8440 */
    volatile unsigned int reserved_40[7];    /* 0x8444~0x845c */
    volatile u_peri_crg8472 peri_crg8472;    /* 0x8460 */
    volatile unsigned int reserved_41[55];   /* 0x8464~0x853c */
    volatile u_peri_crg8528 peri_crg8528;    /* 0x8540 */
    volatile unsigned int reserved_42[7];    /* 0x8544~0x855c */
    volatile u_peri_crg8536 peri_crg8536;    /* 0x8560 */
    volatile unsigned int reserved_43[7];    /* 0x8564~0x857c */
    volatile u_peri_crg8544 peri_crg8544;    /* 0x8580 */
    volatile unsigned int reserved_44[751];  /* 0x8584~0x913c */
    volatile u_peri_crg9296 peri_crg9296;    /* 0x9140 */
    volatile u_peri_crg9297 peri_crg9297;    /* 0x9144 */
    volatile unsigned int reserved_45[2];    /* 0x9148~0x914c */
    volatile u_peri_crg9300 peri_crg9300;    /* 0x9150 */
    volatile u_peri_crg9301 peri_crg9301;    /* 0x9154 */
    volatile unsigned int reserved_46[2];    /* 0x9158~0x915c */
    volatile u_peri_crg9304 peri_crg9304;    /* 0x9160 */
    volatile u_peri_crg9305 peri_crg9305;    /* 0x9164 */
    volatile unsigned int reserved_47[7];    /* 0x9168~0x9180 */
    volatile u_peri_crg9313 peri_crg9313;    /* 0x9184 */
    volatile unsigned int reserved_48[17];   /* 0x9188~0x91c8 */
    volatile u_peri_crg9331 peri_crg9331;    /* 0x91cc */
    volatile unsigned int reserved_49[348];  /* 0x91d0~0x973c */
    volatile u_peri_crg9680 peri_crg9680;    /* 0x9740 */
    volatile u_peri_crg9681 peri_crg9681;    /* 0x9744 */
    volatile unsigned int reserved_50[254];  /* 0x9748~0x9b3c */
    volatile u_peri_crg9936 peri_crg9936;    /* 0x9b40 */
    volatile u_peri_crg9937 peri_crg9937;    /* 0x9b44 */
    volatile unsigned int reserved_51[222];  /* 0x9b48~0x9ebc */
    volatile u_peri_crg10160 peri_crg10160;  /* 0x9ec0 */
    volatile u_peri_crg10161 peri_crg10161;  /* 0x9ec4 */
    volatile unsigned int reserved_52[622];  /* 0x9ec8~0xa87c */
    volatile u_peri_crg10784 peri_crg10784;  /* 0xa880 */
    volatile unsigned int reserved_53[127];  /* 0xa884~0xaa7c */
    volatile u_peri_crg10912 peri_crg10912;  /* 0xaa80 */
    volatile unsigned int reserved_54[1375]; /* 0xaa84~0xbffc */
    volatile u_peri_crg12288 peri_crg12288;  /* 0xc000 */
    volatile unsigned int reserved_55;       /* 0xc004 */
    volatile u_peri_crg12290 peri_crg12290;  /* 0xc008 */
    volatile u_peri_crg12291 peri_crg12291;  /* 0xc00c */
    volatile u_peri_crg12292 peri_crg12292;  /* 0xc010 */
    volatile u_peri_crg12293 peri_crg12293;  /* 0xc014 */
    volatile u_peri_crg12294 peri_crg12294;  /* 0xc018 */
    volatile u_peri_crg12295 peri_crg12295;  /* 0xc01c */
    volatile u_peri_crg12296 peri_crg12296;  /* 0xc020 */
    volatile u_peri_crg12297 peri_crg12297;  /* 0xc024 */
    volatile u_peri_crg12298 peri_crg12298;  /* 0xc028 */

} S_CRG_PERIAPBIF_PRST_REGS_TYPE;

/* Declare the struct pointor of the module CAS */
// extern volatile S_CRG_PERIAPBIF_PRST_REGS_TYPE *gopCASAllReg;

#endif /* __C_UNION_DEFINE_H__ */
