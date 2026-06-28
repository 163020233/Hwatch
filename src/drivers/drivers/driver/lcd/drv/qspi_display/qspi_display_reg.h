/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: mipi tx register header file.
 *
 * Create: 2026-04-14
 */

#ifndef __QSPI_DISPLAY_REG_H__
#define __QSPI_DISPLAY_REG_H__

/*
 * DEFINE REGISTER UNION
 */
/* Define the union spi_er */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int start_en                : 1;       /* [0] */
        unsigned int spi_er_rsvd             : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_er;

/* Define the union spi_ctra */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int soe                     : 1;       /* [0] */
        unsigned int srlt                    : 1;       /* [1] */
        unsigned int ssn_te                  : 1;       /* [2] */
        unsigned int scph                    : 1;       /* [3] */
        unsigned int scpol                   : 1;       /* [4] */
        unsigned int cfs16                   : 4;       /* [8:5] */
        unsigned int dfs16                   : 4;       /* [12:9] */
        unsigned int dfs32                   : 5;       /* [17:13] */
        unsigned int trsm                    : 2;       /* [19:18] */
        unsigned int enhff                   : 2;       /* [21:20] */
        unsigned int prs                     : 2;       /* [23:22] */
        unsigned int spi_ctra_rsvd           : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_ctra;

/* Define the union spi_ctrb */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int nrdf                    : 16;      /* [15:0] */
        unsigned int spi_ctrb_rsvd           : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_ctrb;

/* Define the union spi_enhctl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int waitnum                 : 5;       /* [4:0] */
        unsigned int aaitf                   : 2;       /* [6:5] */
        unsigned int rdsen                   : 1;       /* [7] */
        unsigned int addrlen                 : 4;       /* [11:8] */
        unsigned int ilen                    : 2;       /* [13:12] */
        unsigned int iddren                  : 1;       /* [14] */
        unsigned int ddren                   : 1;       /* [15] */
        unsigned int reserved                : 1;       /* [16] */
        unsigned int spi_enhctl_rsvd         : 15;      /* [31:17] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_enhctl;

/* Define the union spi_mcr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mh                      : 1;       /* [0] */
        unsigned int mss                     : 1;       /* [1] */
        unsigned int mtrc                    : 1;       /* [2] */
        unsigned int spi_mcr_rsvd            : 29;      /* [31:3] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_mcr;

/* Define the union spi_brs */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int frdv                    : 16;      /* [15:0] */
        unsigned int spi_brs_rsvd            : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_brs;

/* Define the union spi_dcr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int tden                    : 1;       /* [0] */
        unsigned int rden                    : 1;       /* [1] */
        unsigned int spi_dcr_rsvd            : 30;      /* [31:2] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_dcr;

/* Define the union spi_drdl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int rdl                     : 5;       /* [4:0] */
        unsigned int spi_drdl_rsvd           : 27;      /* [31:5] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_drdl;

/* Define the union spi_dtdl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int tdl                     : 5;       /* [4:0] */
        unsigned int spi_dtdl_rsvd           : 27;      /* [31:5] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_dtdl;

/* Define the union spi_rsdr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int rxdsd                   : 8;       /* [7:0] */
        unsigned int spi_rsdr_rsvd           : 24;      /* [31:8] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_rsdr;

/* Define the union spi_tder */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int txdde                   : 8;       /* [7:0] */
        unsigned int spi_tder_rsvd           : 24;      /* [31:8] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_tder;

/* Define the union spi_drnm */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int datar                   : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_drnm;

/* Define the union spi_rainsr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int rffris                  : 1;       /* [0] */
        unsigned int rfofris                 : 1;       /* [1] */
        unsigned int rfufris                 : 1;       /* [2] */
        unsigned int spi_rainsr_rsvd2        : 4;       /* [6:3] */
        unsigned int tferis                  : 1;       /* [7] */
        unsigned int tfofris                 : 1;       /* [8] */
        unsigned int spi_rainsr_rsvd1        : 6;       /* [14:9] */
        unsigned int mmcris                  : 1;       /* [15] */
        unsigned int spi_rainsr_rsvd0        : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_rainsr;

/* Define the union spi_insr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int rffis                   : 1;       /* [0] */
        unsigned int rfofis                  : 1;       /* [1] */
        unsigned int rfufis                  : 1;       /* [2] */
        unsigned int spi_insr_rsvd2          : 4;       /* [6:3] */
        unsigned int tfeis                   : 1;       /* [7] */
        unsigned int tfofis                  : 1;       /* [8] */
        unsigned int spi_insr_rsvd1          : 6;       /* [14:9] */
        unsigned int mmcis                   : 1;       /* [15] */
        unsigned int spi_insr_rsvd0          : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_insr;

/* Define the union spi_inmar */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int rffim                   : 1;       /* [0] */
        unsigned int rfofim                  : 1;       /* [1] */
        unsigned int rfufim                  : 1;       /* [2] */
        unsigned int spi_inmar_rsvd2         : 4;       /* [6:3] */
        unsigned int tfeim                   : 1;       /* [7] */
        unsigned int tfofim                  : 1;       /* [8] */
        unsigned int spi_inmar_rsvd1         : 6;       /* [14:9] */
        unsigned int mmcim                   : 1;       /* [15] */
        unsigned int spi_inmar_rsvd0         : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_inmar;

/* Define the union spi_slenr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ssef                    : 1;       /* [0] */
        unsigned int spi_slenr_rsvd          : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_slenr;

/* Define the union spi_twlr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int twl                     : 5;       /* [4:0] */
        unsigned int spi_twlr_rsvd           : 27;      /* [31:5] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_twlr;

/* Define the union spi_tlr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int tfl                     : 6;       /* [5:0] */
        unsigned int spi_tlr                 : 26;      /* [31:6] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_tlr;

/* Define the union spi_tfofier */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ctfofi                  : 1;       /* [0] */
        unsigned int spi_tfoficr             : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_tfofier;

/* Define the union spi_rwlr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int rwl                     : 5;       /* [4:0] */
        unsigned int spi_rwlr_rsvd           : 27;      /* [31:5] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_rwlr;

/* Define the union spi_rlr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int rfl                     : 6;       /* [5:0] */
        unsigned int spi_rlr_rsvd            : 26;      /* [31:6] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_rlr;

/* Define the union spi_rfficr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int crfufi                  : 1;       /* [0] */
        unsigned int spi_rfficr_rsvd0        : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_rfficr;

/* Define the union spi_wsr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dcfe_tfee               : 1;       /* [0] */
        unsigned int spi_wsr_rsvd3           : 3;       /* [3:1] */
        unsigned int rfne                    : 1;       /* [4] */
        unsigned int rffe                    : 1;       /* [5] */
        unsigned int spi_wsr_rsvd2           : 5;       /* [10:6] */
        unsigned int tfnf                    : 1;       /* [11] */
        unsigned int tfe                     : 1;       /* [12] */
        unsigned int spi_wsr_rsvd1           : 2;       /* [14:13] */
        unsigned int sbf                     : 1;       /* [15] */
        unsigned int spi_wsr_rsvd0           : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_wsr;

/* Define the union spi_msticr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cmmci                   : 1;       /* [0] */
        unsigned int spi_msticr_rsvd0        : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_msticr;

/* Define the union spi_id */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int id                      : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_id;

/* Define the union spi_rsvd */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int tx_fifo_th              : 6;       /* [5:0] */
        unsigned int spi_pready_en           : 1;       /* [6] */
        unsigned int spi_qspi_data_en        : 1;       /* [7] */
        unsigned int spi_qspi_data_cnt       : 3;       /* [10:8] */
        unsigned int spi_qspi_inv_byte_en    : 2;       /* [12:11] */
        unsigned int cfg_ssp_sclk_mask       : 1;       /* [13] */
        unsigned int cfg_spi_display_data_wait_en : 1;       /* [14] */
        unsigned int spi_rsvd                : 17;      /* [31:15] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_rsvd;

/* Define the union spi_rxoicr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int crfofi                  : 1;       /* [0] */
        unsigned int spi_rxoicr              : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_rxoicr;

/* Define the union spi_icr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cis                     : 1;       /* [0] */
        unsigned int spi_icr                 : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_icr;

/* Define the union spi_apb_cg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int spi_apb_cg              : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_apb_cg;


/*
 * DEFINE GLOBAL STRUCT
 */
typedef struct {
    u_spi_er                             spi_er;                              /* 0 */
    u_spi_ctra                           spi_ctra;                            /* 4 */
    u_spi_ctrb                           spi_ctrb;                            /* 8 */
    u_spi_enhctl                         spi_enhctl;                          /* c */
    u_spi_mcr                            spi_mcr;                             /* 10 */
    u_spi_brs                            spi_brs;                             /* 14 */
    u_spi_dcr                            spi_dcr;                             /* 18 */
    u_spi_drdl                           spi_drdl;                            /* 1c */
    u_spi_dtdl                           spi_dtdl;                            /* 20 */
    u_spi_rsdr                           spi_rsdr;                            /* 24 */
    u_spi_tder                           spi_tder;                            /* 28 */
    u_spi_drnm                           spi_drnm;                            /* 2c */
    unsigned int                         rsr0[35];                            /* 30 ~ b8 */
    u_spi_rainsr                         spi_rainsr;                          /* bc */
    u_spi_insr                           spi_insr;                            /* c0 */
    u_spi_inmar                          spi_inmar;                           /* c4 */
    u_spi_slenr                          spi_slenr;                           /* c8 */
    u_spi_twlr                           spi_twlr;                            /* cc */
    u_spi_tlr                            spi_tlr;                             /* d0 */
    u_spi_tfofier                        spi_tfofier;                         /* d4 */
    u_spi_rwlr                           spi_rwlr;                            /* d8 */
    u_spi_rlr                            spi_rlr;                             /* dc */
    u_spi_rfficr                         spi_rfficr;                          /* e0 */
    u_spi_wsr                            spi_wsr;                             /* e4 */
    u_spi_msticr                         spi_msticr;                          /* e8 */
    u_spi_id                             spi_id;                              /* ec */
    u_spi_rsvd                           spi_rsvd;                            /* f0 */
    u_spi_rxoicr                         spi_rxoicr;                          /* f4 */
    u_spi_icr                            spi_icr;                             /* f8 */
    u_spi_apb_cg                         spi_apb_cg;                          /* fc */
} qspi_display_regs_type_t;

#endif /* __QSPI_DISPLAY_REG_H__ */
