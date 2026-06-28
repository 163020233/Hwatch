/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: Provides udma register operation api \n
 *
 * History: \n
 * 2025-6-25， Create file. \n
 */

#ifndef HAL_UDMAC_REGS_OP_H
#define HAL_UDMAC_REGS_OP_H

#include <stdint.h>
#include <stdbool.h>
#include "common_def.h"
#include "errcode.h"
#include "hal_udma_regs_def.h"
#include "udma_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

static inline void hal_udma_test_logic_en(uint32_t en, udma_all_reg_t *udma_regs)
{
    integration_cfg_t test_logic = {0};
    test_logic.d32 = ((udma_all_reg_t *)udma_regs)->integration_cfg.d32;
    test_logic.b.int_test_en = en;
    ((udma_all_reg_t *)udma_regs)->integration_cfg.d32 = test_logic.d32;
}

static inline bool hal_udma_test_status(udma_all_reg_t *udma_regs)
{
    udma_test_status_t status = {0};
    status.d32 = ((udma_all_reg_t *)udma_regs)->udma_test_status.d32;
    return (bool)status.b.test_status;
}

static inline bool hal_udma_stall_status(udma_all_reg_t *udma_regs)
{
    stall_status_t status = {0};
    status.d32 = ((udma_all_reg_t *)udma_regs)->stall_status.d32;
    return (bool)status.b.udma_stall_status;
}

static inline bool hal_udma_request_status(udma_channel_t ch, udma_all_reg_t *udma_regs)
{
    udma_req_status_t status = {0};
    status.d32 = ((udma_all_reg_t *)udma_regs)->udma_req_status.d32 & (1UL << ch);
    return (bool)status.b.udma_req_status;
}

static inline bool hal_udma_single_request_status(udma_channel_t ch, udma_all_reg_t *udma_regs)
{
    udma_sreq_status_t status = {0};
    status.d32 = ((udma_all_reg_t *)udma_regs)->udma_sreq_status.d32 & (1UL << ch);
    return (bool)status.b.udma_sreq_status;
}

static inline void hal_udma_done_set(udma_channel_t ch, udma_all_reg_t *udma_regs)
{
    udma_done_set_t done_set = {0};
    done_set.b.udma_done_set = 1UL << ch;
    ((udma_all_reg_t *)udma_regs)->udma_done_set.d32 = done_set.d32;
}

static inline bool hal_udma_done_status(udma_channel_t ch, udma_all_reg_t *udma_regs)
{
    udma_done_set_t status = {0};
    status.d32 = ((udma_all_reg_t *)udma_regs)->udma_done_set.d32 & (1UL << ch);
    return (bool)status.b.udma_done_set;
}

static inline void hal_udma_done_clr(udma_channel_t ch, udma_all_reg_t *udma_regs)
{
    udma_done_clr_t done_clr = {0};
    done_clr.b.udma_done_clr = 1UL << ch;
    ((udma_all_reg_t *)udma_regs)->udma_done_clr.d32 = done_clr.d32;
}

static inline void hal_udma_active_set(udma_channel_t ch, udma_all_reg_t *udma_regs)
{
    udma_active_set_t active_set = {0};
    active_set.b.udma_active_set = 1UL << ch;
    ((udma_all_reg_t *)udma_regs)->udma_active_set.d32 = active_set.d32;
}

static inline bool hal_udma_active_set_get(udma_channel_t ch, udma_all_reg_t *udma_regs)
{
    udma_active_set_t status = {0};
    status.d32 = ((udma_all_reg_t *)udma_regs)->udma_active_set.d32 & (1UL << ch);
    return (bool)status.b.udma_active_set;
}

static inline void hal_udma_active_set_clear(udma_channel_t ch, udma_all_reg_t *udma_regs)
{
    udma_active_clr_t active_clr = {0};
    active_clr.b.udma_active_clr = 1UL << ch;
    ((udma_all_reg_t *)udma_regs)->udma_active_clr.d32 = active_clr.d32;
}

static inline void hal_udma_ch_err_set(udma_all_reg_t *udma_regs)
{
    err_set_t ch_err = {0};
    ch_err.b.err_set = 1UL;
    ((udma_all_reg_t *)udma_regs)->err_set.d32 = ch_err.d32;
}

static inline void hal_udma_ch_alt_set(udma_channel_t ch, udma_all_reg_t *udma_regs)
{
    chnl_pri_alt_set_t select_alt = {0};
    select_alt.d32 = ((udma_all_reg_t *)udma_regs)->chnl_pri_alt_set.d32;
    select_alt.b.chnl_pri_alt_set |= (1UL << ch);
    ((udma_all_reg_t *)udma_regs)->chnl_pri_alt_set.d32 = select_alt.d32;
}

/**
 * @brief Set the UDMA enable
 * @param en Enable value to set
 * @param udma_regs Pointer to the UDMA registers structure
 */
void hal_udma_cfg_set_en(uint32_t en, udma_all_reg_t *udma_regs);

/**
 * @brief Set the UDMA control base pointer
 * @param ch_ctr Control base pointer value to set
 * @param udma_regs Pointer to the UDMA registers structure
 */
void hal_udma_ctrl_set(uint32_t ch_ctr, udma_all_reg_t *udma_regs);

/**
 * @brief Enable a specific UDMA channel
 * @param ch UDMA channel ID
 * @param udma_regs Pointer to the UDMA registers structure
 */
void hal_udma_ch_en_set(udma_channel_t ch, udma_all_reg_t *udma_regs);

/**
 * @brief Disable a specific UDMA channel
 * @param ch UDMA channel ID
 * @param udma_regs Pointer to the UDMA registers structure
 */
void hal_udma_ch_en_clr(udma_channel_t ch, udma_all_reg_t *udma_regs);

/**
 * @brief Check whether a specific UDMA channel is enabled
 * @param ch UDMA channel ID
 * @return true if the UDMA channel is enabled or false otherwise
 */
bool hal_udma_ch_is_en(udma_channel_t ch, udma_all_reg_t *udma_regs);

/**
 * @brief Send a software request to a specific UDMA channel,
 *        then this udma channel will start transfer
 * @param ch UDMA channel ID
 * @param udma_regs Pointer to the UDMA registers structure
 */
void hal_udma_ch_sw_request(udma_channel_t ch, udma_all_reg_t *udma_regs);

/**
 * @brief Set the priority of a specific UDMA channel
 * @param ch UDMA channel ID
 * @param udma_regs Pointer to the UDMA registers structure
 */
void hal_udma_ch_priority_set(udma_channel_t ch, udma_all_reg_t *udma_regs);

/**
 * @brief Clear the priority of a specific UDMA channel
 * @param ch UDMA channel ID
 * @param udma_regs Pointer to the UDMA registers structure
 */
void hal_udma_ch_priority_clr(udma_channel_t ch, udma_all_reg_t *udma_regs);

/**
 * @brief Set a specific UDMA channel whether responds to single requests
 * @param ch UDMA channel ID
 * @param udma_regs Pointer to the UDMA registers structure
 */
void hal_udma_ch_sreq_response_set(udma_channel_t ch, udma_all_reg_t *udma_regs);

/**
 * @brief Set a specific UDMA channel not to respond to single requests
 * @param ch UDMA channel ID
 * @param udma_regs Pointer to the UDMA registers structure
 */
void hal_udma_ch_sreq_response_clr(udma_channel_t ch, udma_all_reg_t *udma_regs);

/**
 * @brief Clear the error status of a specific UDMA channel
 * @param udma_regs Pointer to the UDMA registers structure
 */
void hal_udma_ch_err_clr(udma_all_reg_t *udma_regs);

/**
 * @brief Check whether a specific UDMA channel has an error
 * @param udma_regs Pointer to the UDMA registers structure
 * @return true if the UDMA channel has an error, false otherwise
 */
bool hal_udma_ch_is_err(udma_all_reg_t *udma_regs);

/**
 * @brief Set the channel to use the primary data structure
 * @param ch UDMA channel ID
 * @param udma_regs Pointer to the UDMA registers structure
 */
void hal_udma_ch_alt_clr(udma_channel_t ch, udma_all_reg_t *udma_regs);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif