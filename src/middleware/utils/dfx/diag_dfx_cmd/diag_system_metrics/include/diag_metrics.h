/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description:  diag metrics
 * This file should be changed only infrequently and with great care.
 */

#ifndef __DIAG_METRICS_H__
#define __DIAG_METRICS_H__

#include "errcode.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Struct for diag storage information
 */
typedef struct {
    uint32_t total_size;     /*!< Total size of the storage */
    uint32_t used_size;      /*!< Used size of the storage */
    uint32_t free_size;      /*!< Free size of the storage */
} diag_storage_info_t;

/**
 * @brief Struct for diag spp rate information
 */
#pragma pack(4)
typedef struct {
    uint32_t window_sec;     // Time window in seconds
    uint32_t tx_bps;         // Average upload bit rate
    uint32_t rx_bps;         // Average download bit rate
    uint64_t tx_bytes_total; // Total upload bytes since startup/reset
    uint64_t rx_bytes_total; // Total download bytes since startup/reset
} diag_spp_rate_info_t;
#pragma pack()

/**
 * @brief Initialize the OS metrics
 *
 * OS metrics are collected periodically.
 *
 * Default periodic interval is 10 seconds.
 *
 * @return ERRCODE_SUCC if successful, appropriate error code otherwise.
 */
errcode_t diag_os_metrics_init(void);

/**
 * @brief Deinitialize the OS metrics
 *
 * @return ERRCODE_SUCC if successful, appropriate error code otherwise.
 */
errcode_t diag_os_metrics_deinit(void);

/**
 * @brief Initialize the connectivity metrics (bt, wifi, etc)
 *
 * Rssi metrics are collected periodically.
 *
 * Default periodic interval is 10 seconds.
 *
 * @return ERRCODE_SUCC if successful, appropriate error code otherwise.
 */
errcode_t diag_connectivity_metrics_init(void);

/**
 * @brief Deinitialize the connectivity metrics (bt, wifi, etc)
 *
 * @return ERRCODE_SUCC if successful, appropriate error code otherwise.
 */
errcode_t diag_connectivity_metrics_deinit(void);

/**
 * @brief Initialize the temperature metrics
 *
 * Temperature metrics are collected periodically.
 *
 * Default periodic interval is 10 seconds.
 *
 * @return ERRCODE_SUCC if successful, appropriate error code otherwise.
 */
errcode_t diag_sensor_metrics_init(void);

/**
 * @brief Deinitialize the sensor metrics
 *
 * @return ERRCODE_SUCC if successful, appropriate error code otherwise.
 */
errcode_t diag_sensor_metrics_deinit(void);

/**
 * @brief Record the received bytes for SPP rate metrics
 * @param bytes Number of bytes received
 */
void diag_spp_rate_on_rx(uint32_t bytes);

/**
 * @brief Record the transmitted bytes for SPP rate metrics
 * @param bytes Number of bytes transmitted
 */
void diag_spp_rate_on_tx(uint32_t bytes);

#ifdef __cplusplus
}
#endif

#endif /* __DIAG_METRICS_H__ */