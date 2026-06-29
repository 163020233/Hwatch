/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 * Description:  massdata reader module.
 */

#ifndef MASSDATA_READER_H
#define MASSDATA_READER_H

#include <stdint.h>
#include "massdata_buffer.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup middleware_utils_dfx_massdata massdata
 * @ingroup  middleware_utils_dfx_massdata
 * @{
 */

typedef enum {
    MASSDATA_READER_RET_OK,
    MASSDATA_READER_RET_BUSY,
    MASSDATA_READER_RET_ERROR,
    MASSDATA_READER_RET_TIMEDOUT,
    MASSDATA_READER_RET_ERROR_OVERFLOW_ON_DISCARDING,
    MASSDATA_READER_RET_THERE_IS_NO_NEXT_MESSAGE,
    MASSDATA_READER_RET_ERROR_IN_PARAMS,
    MASSDATA_READER_RET_ERROR_TRYING_TO_DISCARD_WHEN_EMPTY,
    MASSDATA_READER_RET_ERROR_CORRUPT_SHARED_MEMORY,
} massdata_reader_ret_t;

typedef enum {
    MASS_DISABLED,
    MASS_ENABLED_UART,
    MASS_ENABLED_FILE,
    MASS_STATE_MAX,
} massdata_state_t;

typedef void (*massdata_reader_callback)(uint8_t *, uint32_t, uint8_t *, uint32_t);

/**
 * @brief  Set the callback of reading mass data buffer
 * @param  callback massdata_reader_callback
 */
void massdata_register_reader_callback(massdata_reader_callback callback);


void uapi_massdata_set_state(uint8_t state);

/**
 * @brief  Find the position of next mass data in the buffer.
 * @return MASSDATA_READER_RET_OK or an error code
 */
massdata_reader_ret_t massdata_reader_has_next(massdata_memory_region_section_t *mmsec);

/**
 * @brief  Get the next mass data in the buffer.
 * @return MASSDATA_READER_RET_OK or an error code
 */
massdata_reader_ret_t massdata_reader_get_next(massdata_memory_region_section_t mmsec, uint8_t **p1,
    uint32_t *len1, uint8_t **p2, uint32_t *len2);

/**
 * @brief  Discard previous mess data.
 * @return MASSDATA_READER_RET_OK or an error code
 */
massdata_reader_ret_t massdata_reader_discard(massdata_memory_region_section_t mmsec, uint32_t len);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* MASSDATA_READER_H */
