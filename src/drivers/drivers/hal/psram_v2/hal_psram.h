/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: psram hal header file \n
 *
 * History: \n
 * 2024-05-14, Create file. \n
 */
#ifndef HAL_PSRAM_H
#define HAL_PSRAM_H

#include "psram_porting.h"

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

void hal_psram_init(psram_id_t id);
void hal_psram_phy_init(psram_id_t id);
void hal_psram_dmac_init(void);
void hal_psram_axi_init(psram_id_t id);
void hal_psram_freq_flag_set(uint32_t value);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif