/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provide CAN-FD driver source \n
 *
 * History: \n
 * 2023-02-23, Create file. \n
 */

#include <securec.h>
#include "common_def.h"
#include "hal_can.h"
#include "can.h"

#define CAN_FD_RX_FIFO_MAX_NUM  0x2

#define can_get_75_percent(val) (((val) >> 2) * 3)
#define can_get_80_percent(val) (((val) << 2) / 5)
#define can_get_85_percent(val) ((val) * 0.85)

static bool g_can_all_bus_not_init = true;

static can_callback_t g_can_callback[CONFIG_CAN_BUS_NUM] = {NULL};

static bool g_can_inited[CAN_BUS_MAX] = {false};

static void can_callback_handler(uint8_t int_sts,
                                 hal_can_msg_t *received_msg, uint8_t *data, uint32_t length)
{
    uint8_t bus = 0;
    if (g_can_callback[bus] != NULL) {
        g_can_callback[bus](int_sts, (can_msg_attr_t *)received_msg, data, length);
    }
}

static uint32_t can_get_seg1(uint32_t ticks, uint8_t point)
{
    uint32_t res = 0;
    switch (point) {
        case CAN_SAMPLE_PERCENT_75:
            res = can_get_75_percent(ticks);
            break;
        case CAN_SAMPLE_PERCENT_80:
            res = can_get_80_percent(ticks);
            break;
        case CAN_SAMPLE_PERCENT_85:
            res = can_get_85_percent(ticks);
            break;
        default:
            res = can_get_85_percent(ticks);
            break;
    }
    return res;
}

errcode_t uapi_can_init(uint8_t bus, can_global_config_t *global_config, can_filter_list_t *std_filter,
                        can_filter_list_t *ex_filter)
{
    if (bus >= CONFIG_CAN_BUS_NUM || global_config->std_baud_rate > CAN_BPS_MAX
        || global_config->fd_baud_rate > CAN_FD_BPS_MAX) {
        return ERRCODE_INVALID_PARAM;
    }
    /* 允许传入空指针 */
    if ((std_filter != NULL && std_filter->filter_num >= CONFIG_STANDARD_CAN_FILTER_NUM)
        || (ex_filter != NULL && ex_filter->filter_num >= CONFIG_EXTENDED_CAN_FILTER_NUM)) {
        return ERRCODE_INVALID_PARAM;
    }
    if (g_can_inited[bus] == true) {
        return ERRCODE_SUCC;
    }
    can_porting_pwr_on();
    if (g_can_all_bus_not_init == true) {
        hal_can_register_int_callback(can_callback_handler);
    }
    uint32_t stdseg = can_porting_get_seg_ticks(global_config->std_baud_rate);
    uint32_t stdseg1 = can_get_seg1(stdseg, global_config->point);
    uint32_t fdseg = can_porting_get_seg_ticks(global_config->fd_baud_rate);
    uint32_t fdseg1 = can_get_seg1(fdseg, global_config->point);
    hal_can_global_config_t hal_config = {
        .mode = global_config->can_mode,
        .nseg1 = stdseg1,
        .nseg2 = stdseg - stdseg1,
        .dseg1 = fdseg1,
        .dseg2 = fdseg - fdseg1,
    };
    errcode_t ret = hal_can_init(&hal_config,
                                 (hal_can_filter_list_t *)std_filter, (hal_can_filter_list_t *)ex_filter);
    can_porting_regitser_irq();
    if (ret == ERRCODE_SUCC) {
        g_can_inited[bus] = true;
        g_can_all_bus_not_init = false;
        can_port_lock_init();
    }
    return ret;
}

void uapi_can_deinit(uint8_t bus)
{
    if (bus >= CONFIG_CAN_BUS_NUM || g_can_inited[bus] == false) {
        return;
    }
    uapi_can_callback_unregister(bus);
    can_porting_unregitser_irq();
    hal_can_deinit();
    g_can_inited[bus] = false;
    for (uint8_t i = 0; i < CONFIG_CAN_BUS_NUM; i++) {
        if (g_can_inited[i] == true) {
            return;
        }
    }
    hal_can_unregister_int_callback();
    g_can_all_bus_not_init = true;
}

errcode_t uapi_can_send_msg(uint8_t bus, can_msg_attr_t *can_msg, uint8_t *data, uint8_t length)
{
    if (bus >= CONFIG_CAN_BUS_NUM || can_msg == NULL) {
        return ERRCODE_INVALID_PARAM;
    }
    if (g_can_inited[bus] == false) {
        return ERRCODE_CAN_FD_NOT_INIT;
    }
    if ((can_msg->fdf == 0 && length > 0x8) || length > 0x40) {
        return ERRCODE_INVALID_PARAM;
    }
    uint32_t lock_sts = can_port_get_lock();
    errcode_t ret = hal_can_send_msg((hal_can_msg_t *)can_msg, data, length);
    can_port_restore_lock(lock_sts);
    return ret;
}

errcode_t uapi_can_receive_msg(uint8_t bus, uint8_t fifo_index,
                               can_msg_attr_t *receive_msg, uint8_t *data, uint32_t *length)
{
    if (bus >= CONFIG_CAN_BUS_NUM) {
        return ERRCODE_INVALID_PARAM;
    }
    if (g_can_inited[bus] == false) {
        return ERRCODE_CAN_FD_NOT_INIT;
    }
    return hal_can_receive_msg(fifo_index, (hal_can_msg_t *)receive_msg, data, length);
}

errcode_t uapi_can_callback_register(uint8_t bus, can_callback_t rx_callback)
{
    if (bus >= CONFIG_CAN_BUS_NUM) {
        return ERRCODE_INVALID_PARAM;
    }
    g_can_callback[bus] = rx_callback;
    return ERRCODE_SUCC;
}

errcode_t uapi_can_callback_unregister(uint8_t bus)
{
    if (bus >= CONFIG_CAN_BUS_NUM) {
        return ERRCODE_INVALID_PARAM;
    }
    g_can_callback[bus] = NULL;
    return ERRCODE_SUCC;
}

can_controller_sts_t uapi_can_get_controller_sts(uint8_t bus)
{
    unused(bus);
    if (g_can_inited[bus] == false) {
        return CAN_BUS_NOT_INIT;
    }
    return hal_can_get_controller_sts();
}

uint8_t uapi_can_get_transmit_error_count(uint8_t bus)
{
    unused(bus);
    if (g_can_inited[bus] == false) {
        return 0;
    }
    return hal_can_get_transmit_error_count();
}

uint8_t uapi_can_get_receive_error_count(uint8_t bus)
{
    unused(bus);
    if (g_can_inited[bus] == false) {
        return 0;
    }
    return hal_can_get_receive_error_count();
}

uint8_t uapi_can_get_rx_sts(uint8_t bus)
{
    if (g_can_inited[bus] == false) {
        return 0xFF;
    }
    return hal_can_get_rx_sts();
}

errcode_t uapi_can_fd_config_tdc(uint8_t bus, uint8_t enable, uint8_t offset)
{
    if (g_can_inited[bus] == false) {
        return ERRCODE_CAN_FD_NOT_INIT;
    }
    return hal_can_fd_config_tdc(enable, offset);
}