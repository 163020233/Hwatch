/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: CA_CTRL reg header file.
 *
 * Create: 2025-01-20
 */

#ifndef HAL_COMMON_REG_H
#define HAL_COMMON_REG_H

#define LOW_POWER_EN (0x008)
#define SEC_CRG (0x350)
#define VOTE_CLK_EN(n) (0x400 + 0x10 * (n))
#define TRNG_VOTE_CLK_EN  VOTE_CLK_EN(0)
#define KM_VOTE_CLK_EN  VOTE_CLK_EN(1)
#define SPACC_VOTE_CLK_EN  VOTE_CLK_EN(2)
#define PKE_VOTE_CLK_EN  VOTE_CLK_EN(3)
#define VOTE_CFG(n) (0x404 + 0x10 * (n))
#define TRNG_VOTE_CFG VOTE_CFG(0)
#define KM_VOTE_CFG VOTE_CFG(1)
#define SPACC_VOTE_CFG VOTE_CFG(2)
#define PKE_VOTE_CFG VOTE_CFG(3)
#define VOTE_STATUS(n)  (0x408 + 0x10 * (n))
#define TRNG_VOTE_STATUS  VOTE_STATUS(0)
#define KM_VOTE_STATUS  VOTE_STATUS(1)
#define SPACC_VOTE_STATUS  VOTE_STATUS(2)
#define PKE_VOTE_STATUS  VOTE_STATUS(3)
#define CPU_TYPE    (0x500)

/* Define the union VOTE_CLK_EN */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    vote_clk_en    : 1; /* [0] vote enable */
        unsigned int    reserved_0    : 31; /* [1:31] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} vote_clk_en;

/* Define the union VOTE_CFG */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    tee_vote    : 1; /* [0] tee vote clk enable. */
        unsigned int    ree_vote    : 1; /* [1] ree vote clk enable. */
        unsigned int    reserved_0    : 30; /* [31:2] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} vote_cfg;

/* Define the union VOTE_STATUS */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    clk_gate_status    : 1; /* [0] whether current module has clock. */
        unsigned int    clk_gate_handshake    : 1; /* [1] whether current module is handshake. */
        unsigned int    reserved_0    : 30; /* [31:2] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} vote_status;

/* Define the union U_CPU_TYPE */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    cpu_type    : 8; /* [7:0] cpu type. */
        unsigned int    reserved_0    : 24; /* [31:8] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} cpu_type_cfg;

/* Define the union U_SEC_CFG_CTRL */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned short    soft_rst_pke_n    : 1; /* [0] sec soft reset ctrl */
        unsigned short    cfg_pke_clken    : 1; /* [1] pke clock gate ctrl */
        unsigned short    cfg_km_clken    : 1; /* [2] km clock gate ctrl */
        unsigned short    cfg_hash_clken    : 1; /* [3] hash clock gate ctrl */
        unsigned short    cfg_sce_clken    : 1; /* [4] symc clock gate ctrl */
        unsigned short    cfg_spacc_clken    : 1; /* [5] spacc clock gate ctrl */
        unsigned short    reserved_0    : 10; /* [15:6] */
    } bits;

    /* Define an unsigned member */
    unsigned short    u16;
} sec_cfg_ctrl;
#endif  /* HAL_COMMON_REG_H */