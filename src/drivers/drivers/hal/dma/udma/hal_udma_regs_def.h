/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: Provides udma register \n
 *
 * History: \n
 * 2025-6-13， Create file. \n
 */
#ifndef HAL_UDMA_REGS_DEF_H
#define HAL_UDMA_REGS_DEF_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_udma_regs_def uDMA Regs Definition
 * @ingroup  drivers_hal_udma
 * @{
 */


/**
 * @brief  Enables the integration test logic.
 *         0: disables the integration test logic;
 *         1: integration test logic controls the status of udma_active、udma_done、udma_err.
 */
typedef union {
    struct {
        uint32_t int_test_en             : 1;
        uint32_t reserved                : 31;
    } b;

    uint32_t d32;
} integration_cfg_t;

/**
 * @brief The status of udma_test_status Register.
 *        0: controller include the integration test logic;
 *        1: controller does not include the integration test logic.
 */
typedef union {
    struct {
        uint32_t test_status             : 1;
        uint32_t reserved                : 31;
    } b;

    uint32_t d32;
} udma_test_status_t;

/**
 * @brief The status of udma_stall Register.
 *        0: udma_stall is LOW;
 *        1: udma_stall is HIGH.
 */
typedef union {
    struct {
        uint32_t udma_stall_status       : 1;
        uint32_t reserved                : 31;
    } b;

    uint32_t d32;
} stall_status_t;

/**
 * @brief The channel status of request signal Register: udma_req.
 *        0: udma_req[C] is LOW;
 *        1: udma_req[C] is HIGH.
 */
typedef union {
    struct {
        uint32_t udma_req_status         : 8;
        uint32_t reserved                : 24;
    } b;

    uint32_t d32;
} udma_req_status_t;

/**
 * @brief The channel status of single request signals Register: udma_sreq.
 *        0: udma_sreq[C] is LOW;
 *        1: udma_sreq[C] is HIGH.
 */
typedef union {
    struct {
        uint32_t udma_sreq_status         : 8;
        uint32_t reserved                : 24;
    } b;

    uint32_t d32;
} udma_sreq_status_t;

/**
 * @brief Return the channel status of udma_done or set the channel udma_done.
 *        w0: No effect;
 *        w1: Sets udma_done[C] HIGH;
 *        r0: udma_done[C] is LOW;
 *        r1: udma_done[C] is HIGH.
 */
typedef union {
    struct {
        uint32_t udma_done_set           : 8;
        uint32_t reserved                : 24;
    } b;

    uint32_t d32;
} udma_done_set_t;

/**
 * @brief Set the channel udma_done.
 *        w0: No effect;
 *        w1: Sets udma_done[C] LOW.
 */
typedef union {
    struct {
        uint32_t udma_done_clr           : 8;
        uint32_t reserved                : 24;
    } b;

    uint32_t d32;
} udma_done_clr_t;

/**
 * @brief Return the channel status of udma_active or set the channel udma_active.
 *        w0: No effect;
 *        w1: Sets udma_active[C] HIGH;
 *        r0: udma_active[C] is LOW;
 *        r1: udma_active[C] is HIGH.
 */
typedef union {
    struct {
        uint32_t udma_active_set         : 8;
        uint32_t reserved                : 24;
    } b;

    uint32_t d32;
} udma_active_set_t;

/**
 * @brief Set the channel udma_active.
 *        w0: No effect;
 *        w1: Sets udma_active[C] LOW.
 */
typedef union {
    struct {
        uint32_t udma_active_clr         : 8;
        uint32_t reserved                : 24;
    } b;

    uint32_t d32;
} udma_active_clr_t;

/**
 * @brief Return the channel status of udma_err or set the channel udma_err.
 *        w0: No effect;
 *        w1: Sets udma_err HIGH.
 */
typedef union {
    struct {
        uint32_t err_set                 : 2;
        uint32_t reserved                : 30;
    } b;

    uint32_t d32;
} err_set_t;

/**
 * @brief Return the channel status of udma_err or set the channel udma_err.
 *        w0: No effect;
 *        w1: Sets udma_err LOW;
 *        r0: udma_err is LOW;
 *        r1: udma_err is HIGH.
 */
typedef union {
    struct {
        uint32_t err_clr                 : 1;
        uint32_t reserved                : 31;
    } b;

    uint32_t d32;
} chnl_err_clr_t;

/**
* @brief The status of the controller.
*/
typedef union {
    struct {
        /**
         * @brief Enable status of the controller
         *        0: controller is disabled；
         *        1：controller is enabled.
         */
        uint32_t master_enable           : 1;
        uint32_t reserved0               : 3;
        /**
         * @brief  Current state of the control state machine:
         *        b0000: idle；
         *        b0001: reading channel controller data;
         *        b0010: reading source data end pointer;
         *        b0011: reading destination data end pointer;
         *        b0100: reading source data;
         *        b0101: writing destination data;
         *        b0110: waiting for DMA request to clear;
         *        b0111: writing channel controller data;
         *        b1000: stalled;
         *        b1001: done;
         *        b1010: peripheral scatter-gather transition;
         *        b1011-b1111: undefined.
         */
        uint32_t state                   : 4;
        uint32_t reserved1               : 8;
        /**
         * @brief uDMA chnls_minusl。
         *        b00000: controller configured to use 1 DMA channel;
         *        b00001: controller configured to use 2 DMA channels;
         *        ...
         *        b00111: controller configured to use 8 DMA channels.
         */
        uint32_t chnls_minusl            : 5;
        uint32_t reserved2               : 11;
    } b;

    uint32_t d32;
} udma_status_t;

/**
* @brief Sets the AHB-Lite protection:
*        bit[2]: Controls HPROT[3] to indicate if a cacheable access is occurring;
*        bit[1]: Controls HPROT[2] to indicate if a cacheable access is occurring;
*        bit[0]: Controls HPROT[1] to indicate if a cacheable access is occurring;
*/
typedef union {
    struct {
        uint32_t chnl_prot_ctrl          : 3;
        uint32_t reserved                : 29;
    } b;

    uint32_t d32;
} udma_ahb_prt_t;

/**
* @brief Channel control data base pointer.
*        1 channel, bit[4:0] must be 0;
*        2 channels, bit[5:0] must be 0;
*        3-4 channels, bit[6:0] must be 0;
*        5-8 channels, bit[7:0] must be 0;
*        9-16 channels, bit[8:0] must be 0;
*        17-32 channels, bit[9:0] must be 0.
*/
typedef union {
    struct {
        uint32_t ctrl_base_ptr           : 32;
    } b;

    uint32_t d32;
} ctrl_base_ptr_t;

/** @brief Channel alternate control data base pointer.
*          1 channel, bit[4:0] must be 0;
*          2 channels, bit[5:0] must be 0;
*          3-4 channels, bit[6:0] must be 0;
*          5-8 channels, bit[7:0] must be 0;
*          9-16 channels, bit[8:0] must be 0;
*          17-32 channels, bit[9:0] must be 0.
*/
typedef union {
    struct {
        uint32_t alt_ctrl_base_ptr       : 32;
    } b;

    uint32_t d32;
} alt_ctrl_base_ptr_t;

/**
* @brief Channel wait on request status.
*        r0: bit[C] udma_waitonreq is LOW;
*        r1: bit[C] udma_waitonreq is HIGH.
*/
typedef union {
    struct {
        uint32_t udma_waitonreq          : 8;
        uint32_t reserved                : 24;
    } b;

    uint32_t d32;
} udma_waitonreq_status_t;

/**
* @brief Channel software request.
*        w0: Does not create a uDMA request for channel[C];
*        w1: Creates a uDMA request for channel[C].
*/
typedef union {
    struct {
        uint32_t chnl_sw_request         : 8;
        uint32_t reserved                : 24;
    } b;

    uint32_t d32;
} chnl_sw_request_t;

/**
* @brief  Returns the channel control data structure status, or selects the alternate data structure for the
*        corresponding uDMA channel.
*        r0: Channel[C] is using primary data structure;
*        r1: Channel[C] is using alternate data structure;
*        w0: No effect;
*        w1: Selects the alternate data structure for channel[C].
*/
typedef union {
    struct {
        uint32_t chnl_pri_alt_set        : 8;
        uint32_t reserved                : 24;
    } b;

    uint32_t d32;
} chnl_pri_alt_set_t;

/**
* @brief Set the bit[C] to select the primary data structure for the corresponding uDMA channel.
*        w0: No effect;
*        w1: Selects the primary data structure for channel[C].
*/
typedef union {
    struct {
        uint32_t chnl_pri_alt_clr        : 8;
        uint32_t reserved                : 24;
    } b;

    uint32_t d32;
} chnl_pri_alt_clr_t;

/**
* @brief Returns the useburst status, or disables udma_sreq[C] from generating uDMA requests.
*        r0: uDMA channel[C] responds to udma_sreq[C];
*        r1: uDMA channel[C] dose not respond to dma_sreq[C];
*        w0: No effect;
*        w1: Disables udma_sreq[C] from generating uDMA requests.
*/
typedef union {
    struct {
        uint32_t chnl_useburst_set       : 8;
        uint32_t reserved                : 24;
    } b;

    uint32_t d32;
} chnl_useburst_set_t;

/**
* @brief  Set the bit[C] to enable udma_sreq[C] to generate requests.
*        w0: No effect;
*        w1: Enables udma_sreq[C] to generate uDMA requests.
*/
typedef union {
    struct {
        uint32_t chnl_useburst_clr       : 8;
        uint32_t reserved                : 24;
    } b;

    uint32_t d32;
} chnl_useburst_clr_t;

/**
* @brief Returns the enable status of the channels, or enables the corresponding channels.
*        r0: uDMA channel[C] is disabled;
*        r1: uDMA channel[C] is enabled;
*        w0: No effect;
*        w1: Enables channel[C].
*/
typedef union {
    struct {
        uint32_t chnl_enable_set         : 8;
        uint32_t reserved                : 24;
    } b;

    uint32_t d32;
} chnl_enable_set_t;

/**
* @brief Disables responds to channel.
*        w0: No effect;
*        w1: Disables uDMA channel[C].
*/
typedef union {
    struct {
        uint32_t chnl_enable_clr         : 8;
        uint32_t reserved                : 24;
    } b;

    uint32_t d32;
} chnl_enable_clr_t;

/**
* @brief Returns the request mask status of udma_req[C] and udma_sreq[C], or disables the
*        corresponding channel from generating uDMA requests.
*        r0: External requests are enabled for channel[C];
*        r1: External requests are disabled for channel[C];
*        w0: No effect;
*        w1: Disables udma_req[C] and udma_sreq[C] from generating uDMA requests.
*/
typedef union {
    struct {
        uint32_t chnl_req_mask_set       : 8;
        uint32_t reserved                : 24;
    } b;

    uint32_t d32;
} chnl_req_mask_set_t;

/**
* @brief Set the bit[C] to enable uDMA requests for the channel corresponding to
*        udma_req[C] and udma_sreq[C].
*        w0: No effect;
*        w1: Enables udma_req[C] or udma_sreq[C] to generating uDMA requests.
*/
typedef union {
    struct {
        uint32_t chnl_req_mask_clr       : 8;
        uint32_t reserved                : 24;
    } b;

    uint32_t d32;
} chnl_req_mask_clr_t;

/**
* @brief Returns the channel priority mask status, or sets the channel priority to high.
*        r0: uDMA channel[C] is using the default priority level;
*        r1: uDMA channel[C] is using a high priority level;
*        w0: No effect;
*        w1: Channel[C] uses the high priority level.
*/
typedef union {
    struct {
        uint32_t chnl_priority_set       : 8;
        uint32_t reserved                : 24;
    } b;

    uint32_t d32;
} chnl_priority_set_t;

/**
* @brief Set the bit[C] to select the default priority level for the specified uDMA channel
*        w0: No effect;
*        w1: Channel[C] uses the default priority level.
*/
typedef union {
    struct {
        uint32_t chnl_priority_clr       : 8;
        uint32_t reserved                : 24;
    } b;

    uint32_t d32;
} chnl_priority_clr_t;

/**
* @brief Enable for the controller.
*        w0: Disables the controller;
*        w1: Enables the controller.
*/
typedef union {
    struct {
        uint32_t master_enable_set       : 1;
        uint32_t reserved                : 31;
    } b;

    uint32_t d32;
} udma_cfg_t;

/**
 * @brief uDMA registers configuration uDMA.
 */
typedef struct {
    volatile integration_cfg_t                    integration_cfg;                     /* 0 */
    volatile udma_test_status_t                   udma_test_status;                    /* 4 */
    volatile stall_status_t                       stall_status;                        /* 8 */
    volatile udma_req_status_t                    udma_req_status;                     /* c */
    volatile udma_sreq_status_t                   udma_sreq_status;                    /* 10 */
    volatile uint32_t                             reserved0;                           /* 14 */
    volatile udma_done_set_t                      udma_done_set;                       /* 18 */
    volatile udma_done_clr_t                      udma_done_clr;                       /* 1c */
    volatile udma_active_set_t                    udma_active_set;                     /* 20 */
    volatile udma_active_clr_t                    udma_active_clr;                     /* 24 */
    volatile err_set_t                            err_set;                             /* 28 */
    volatile chnl_err_clr_t                       chnl_err_clr;                        /* 2c */
    volatile uint32_t                             reserved1[884];                      /* 30 ~ dfc */
    volatile udma_status_t                        udma_status;                         /* e00 */
    volatile udma_ahb_prt_t                       udma_ahb_prt;                        /* e04 */
    volatile ctrl_base_ptr_t                      ctrl_base_ptr;                       /* e08 */
    volatile alt_ctrl_base_ptr_t                  alt_ctrl_base_ptr;                   /* e0c */
    volatile udma_waitonreq_status_t              udma_waitonreq_status;               /* e10 */
    volatile chnl_sw_request_t                    chnl_sw_request;                     /* e14 */
    volatile chnl_pri_alt_set_t                   chnl_pri_alt_set;                    /* e18 */
    volatile chnl_pri_alt_clr_t                   chnl_pri_alt_clr;                    /* e1c */
    volatile chnl_useburst_set_t                  chnl_useburst_set;                   /* e20 */
    volatile chnl_useburst_clr_t                  chnl_useburst_clr;                   /* e24 */
    volatile chnl_enable_set_t                    chnl_enable_set;                     /* e28 */
    volatile chnl_enable_clr_t                    chnl_enable_clr;                     /* e2c */
    volatile chnl_req_mask_set_t                  chnl_req_mask_set;                   /* e30 */
    volatile chnl_req_mask_clr_t                  chnl_req_mask_clr;                   /* e34 */
    volatile chnl_priority_set_t                  chnl_priority_set;                   /* e38 */
    volatile chnl_priority_clr_t                  chnl_priority_clr;                   /* e3c */
    volatile udma_cfg_t                           udma_cfg;                            /* e40 */
} udma_all_reg_t;

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif