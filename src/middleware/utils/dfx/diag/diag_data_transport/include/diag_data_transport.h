/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: diag data transport
 * This file should be changed only infrequently and with great care.
 */

#ifndef DIAG_DATA_TRANSPORT_H
#define DIAG_DATA_TRANSPORT_H
#include "errcode.h"
#include "diag_data_store.h"
#include "bts_le_gap.h"

/**
 * @brief Diag data transport init callback prototype
 *
 * @param[in] userdata User data
 *
 * @return ERRCODE_SUCC on success, appropriate error code otherwise.
 */
typedef errcode_t(*diag_data_transport_init_t)(void *userdata);

/**
 * @brief Diag data transport deinit callback prototype
 */
typedef void(*diag_data_transport_deinit_t)(void);

/**
 * @brief Diag data transport connect callback prototype
 *
 * @return ERRCODE_SUCC on success, appropriate error code otherwise.
 */
typedef errcode_t(*diag_data_transport_connect_t)(void);

/**
 * @brief Diag data transport disconnect callback prototype
 */
typedef void(*diag_data_transport_disconnect_t)(void);

/**
 * @brief Diag data transport is connected callback prototype
 */
typedef bool(*diag_data_transport_is_connected_t)(void);

/**
 * @brief Diag data transport data send callback prototype
 *
 * @param[in] item Data item to be sent
 *
 * @return ERRCODE_SUCC on success, appropriate error code otherwise.
 */
typedef errcode_t(*diag_data_transport_data_send_t)(diag_data_item_t *item);

/**
 * @brief Diag data transport data send callback prototype
 *
 * @param[in] buf Bytes to be sent
 *
 * @return ERRCODE_SUCC on success, appropriate error code otherwise.
 */
typedef errcode_t(*diag_data_transport_data_send_buf_t)(const uint8_t *buf, uint16_t len);

/**
 * @brief Diag data transport configurations
 */
typedef struct {
    /** Diag data transport callback functions */
    struct {
        /** Transport init function */
        diag_data_transport_init_t init;
        /** Transport deinit function */
        diag_data_transport_deinit_t deinit;
        /** Transport connect function */
        diag_data_transport_connect_t connect;
        /** Transport disconnect function */
        diag_data_transport_disconnect_t disconnect;
        /** Function to check if transport is connected */
        diag_data_transport_is_connected_t is_connected;
        /** Function to send data */
        diag_data_transport_data_send_t data_send;
        /** Function to send data with buffer */
        diag_data_transport_data_send_buf_t data_send_buf;
    } callbacks;
    /** User data */
    void *userdata;
} diag_data_transport_config_t;

#if (CONFIG_DFX_DATA_TRANSPORT_BT == DFX_YES)
/* Default configurations for BT SPP */
extern diag_data_transport_config_t g_default_diag_data_transport_bt;
#endif

#if (CONFIG_DFX_DATA_TRANSPORT_BLE == DFX_YES)
extern diag_data_transport_config_t g_default_diag_data_transport_ble;
#endif

/**
 * @brief Register Diag data transport.
 *
 * This function should be used only when default transport needs to be overridden.
 *
 * @note Call dfx_report_enable()
 * after registering your own transport to enable Diag report.
 *
 * @param[in] config Configurations of type \ref diag_data_transport_config_t
 *
 * @return ERRCODE_SUCC on success, appropriate error code otherwise
 */
errcode_t diag_data_transport_register(diag_data_transport_config_t *config);

/**
 * @brief Unregister Diag data transport.
 *
 * @note This API does not disable Diag report.
 * Call dfx_report_disable() to turn off Diag report.
 */
void diag_data_transport_unregister(void);

/**
 * @brief Set SPP connection state
 *
 * @param state SPP connect state \ref ux_spp_connect_state_t
 */
void diag_set_bt_conn_state(uint8_t state);

/**
 * @brief Set BLE connection state for diag transport
 *
 * @param state BLE connect state \ref gap_ble_conn_state_t
 */
void diag_set_ble_conn_state(gap_ble_conn_state_t state);

static inline uint32_t align_up4(uint32_t x)
{
    return (x + 3u) & ~3u;
}

#endif