/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides IPC source \n
 *
 * History: \n
 * 2023-04-01, Create file. \n
 */
#include <stdint.h>
#include "securec.h"
#ifdef IPC_USE_OS_DELAY
#include "cmsis_os2.h"
#endif
#include "common_def.h"
#include "systick.h"
#include "ipc_porting.h"
#include "hal_ipc.h"
#include "ipc_ring.h"

typedef struct ipc_ring_element_header {
    uint8_t msg_id;
    uint8_t priority;
    uint16_t cur_msg_len;
}ipc_ring_element_header_t;

hal_ipc_funcs_t *g_ipc_hal_funcs = NULL;

static volatile uint64_t g_send_start_ms = 0;
static volatile uint64_t g_send_cur_ms = 0;
static volatile uint64_t g_send_end_ms = 0;

static ipc_memory_manage_t *g_ipc_tx_mem_manage = g_ipc_memory_init_tx_config;
static ipc_memory_manage_t *g_ipc_rx_mem_manage = g_ipc_memory_init_rx_config;

ipc_mailbox_ring_header_t *g_ipc_ring_mailbox_tx_pointer[CORE_IPC_TX_PIPE_NUM] = {NULL};
ipc_mailbox_ring_header_t *g_ipc_ring_mailbox_rx_pointer[CORE_IPC_RX_PIPE_NUM] = {NULL};

static ipc_rx_callback_t g_ipc_msg_handlers[IPC_MSG_ID_MAX];

static uint8_t g_databuf[IPC_MESSAGE_MAX_LENGTH] = {0};

STATIC errcode_t ipc_mailbox_init(void)
{
    uint32_t total_len;
    uint32_t start_addr;
    uint32_t mailbox_header_size = (uint32_t)sizeof(ipc_mailbox_ring_header_t);

    for (int i = 0; i < CORE_IPC_TX_PIPE_NUM; i++) {
        start_addr = g_ipc_tx_mem_manage[i].start_addr;
        total_len = g_ipc_tx_mem_manage[i].total_len;
        if (start_addr == 0 || total_len <= mailbox_header_size) {
            return ERRCODE_IPC_INVALID_PARAMETER;
        }
        (void)memset_s((void *)(uintptr_t)start_addr, total_len, 0, total_len);
        g_ipc_ring_mailbox_tx_pointer[i] = (ipc_mailbox_ring_header_t *)(uintptr_t)start_addr;
        g_ipc_ring_mailbox_tx_pointer[i]->src_core = g_ipc_tx_mem_manage[i].src_core;
        g_ipc_ring_mailbox_tx_pointer[i]->dst_core = g_ipc_tx_mem_manage[i].dst_core;
        g_ipc_ring_mailbox_tx_pointer[i]->channel = g_ipc_tx_mem_manage[i].channel;
        g_ipc_ring_mailbox_tx_pointer[i]->total_size = (uint32_t)(total_len - sizeof(ipc_mailbox_ring_header_t));
        g_ipc_ring_mailbox_tx_pointer[i]->read = 0;
        g_ipc_ring_mailbox_tx_pointer[i]->write = 0;
        g_ipc_ring_mailbox_tx_pointer[i]->buffer_empty = 1;
    }

    for (int i = 0; i < CORE_IPC_RX_PIPE_NUM; i++) {
        start_addr = g_ipc_rx_mem_manage[i].start_addr;
        total_len = g_ipc_rx_mem_manage[i].total_len;
        if (start_addr == 0 || total_len <= mailbox_header_size) {
            return ERRCODE_IPC_INVALID_PARAMETER;
        }
        g_ipc_ring_mailbox_rx_pointer[i] = (ipc_mailbox_ring_header_t *)(uintptr_t)start_addr;
    }
    return ERRCODE_SUCC;
}

STATIC void ipc_mailbox_deinit(void)
{
    for (int i = 0; i < CORE_IPC_TX_PIPE_NUM; i++) {
        (void)memset_s((void *)(uintptr_t)(g_ipc_memory_init_tx_config[i].start_addr),
                       g_ipc_memory_init_tx_config[i].total_len,
                       0, g_ipc_memory_init_tx_config[i].total_len);
    }
    for (int i = 0; i < CORE_IPC_RX_PIPE_NUM; i++) {
        g_ipc_ring_mailbox_rx_pointer[i] = NULL;
    }
}

STATIC void ipc_msg_handler_clear(void)
{
    (void)memset_s(g_ipc_msg_handlers, sizeof(ipc_rx_callback_t) * IPC_MSG_ID_MAX,
                   0, sizeof(ipc_rx_callback_t) * IPC_MSG_ID_MAX);
}

STATIC ipc_mailbox_ring_header_t *ipc_get_tx_mailbox(uint8_t dst_core, uint8_t channel)
{
    for (int i = 0; i < CORE_IPC_TX_PIPE_NUM; i++) {
        if (g_ipc_ring_mailbox_tx_pointer[i]->dst_core == dst_core &&
            g_ipc_ring_mailbox_tx_pointer[i]->channel == channel) {
            return g_ipc_ring_mailbox_tx_pointer[i];
        }
    }
    return NULL;
}

STATIC ipc_mailbox_ring_header_t *ipc_get_rx_mailbox(uint8_t src_core, uint8_t channel)
{
    for (int i = 0; i < CORE_IPC_RX_PIPE_NUM; i++) {
        if (g_ipc_ring_mailbox_rx_pointer[i]->src_core == src_core &&
            g_ipc_ring_mailbox_rx_pointer[i]->channel == channel) {
            return g_ipc_ring_mailbox_rx_pointer[i];
        }
    }
    return NULL;
}

STATIC errcode_t ipc_copy_data_to_ring_buffer(ipc_mailbox_ring_header_t *mailbox, uint16_t write,
                                              uint8_t *databuf, uint32_t length)
{
    uint8_t *write_ptr;
    uint8_t *databuf_tmp;
    uint32_t end_index;
    uint32_t ring_end_index;
    uint32_t current_length;

    if (length == 0) {
        return ERRCODE_SUCC;
    }
    write_ptr = mailbox->ring_buffer;
    databuf_tmp = databuf;
    write_ptr += write;
    end_index = write + length - 1;
    ring_end_index = mailbox->total_size - 1;
    if (end_index > ring_end_index) {
        current_length = ring_end_index - write + 1;
        if (memcpy_s(write_ptr, current_length, databuf_tmp, current_length) != EOK) {
            return ERRCODE_FAIL;
        }
        databuf_tmp += current_length;
        current_length = length - current_length;
        if (memcpy_s(mailbox->ring_buffer, current_length, databuf_tmp, current_length) != EOK) {
            return ERRCODE_FAIL;
        }
    } else {
        if (memcpy_s(write_ptr, length, databuf_tmp, length) != EOK) {
            return ERRCODE_FAIL;
        }
    }
    return ERRCODE_SUCC;
}

STATIC errcode_t ipc_set_msg_into_mailbox(ipc_mailbox_ring_header_t *mailbox, ipc_ring_element_header_t *current_msg,
                                          uint8_t *databuf)
{
    uint32_t status = ipc_port_irq_lock();
    uint16_t write = mailbox->write;
    uint32_t read = mailbox->read;
    uint32_t total_length = mailbox->total_size;
    uint32_t full_length = current_msg->cur_msg_len + (uint32_t)sizeof(ipc_ring_element_header_t);
    uint32_t remained_size = total_length - ((write - read + total_length) % total_length);
    errcode_t ret;

    if (remained_size == mailbox->total_size) {
        remained_size = mailbox->buffer_empty == 1 ? remained_size : 0;
    }
    if (full_length > remained_size) {
        g_ipc_hal_funcs->set_core_int(mailbox->dst_core, mailbox->channel);
        ipc_port_irq_restore(status);
        return ERRCODE_IPC_ASYNC_BUF_OVERFLOW;
    }
    ret = ipc_copy_data_to_ring_buffer(mailbox, write,
                                       (uint8_t *)(void *)current_msg,
                                       (uint32_t)sizeof(ipc_ring_element_header_t));
    if (ret != ERRCODE_SUCC) {
        ipc_port_irq_restore(status);
        return ret;
    }
    write = (write + (uint32_t)sizeof(ipc_ring_element_header_t)) % mailbox->total_size;
    ret = ipc_copy_data_to_ring_buffer(mailbox, write, databuf, current_msg->cur_msg_len);
    if (ret != ERRCODE_SUCC) {
        ipc_port_irq_restore(status);
        return ret;
    }
    mailbox->write = (uint16_t)((mailbox->write + full_length) % mailbox->total_size);
    if (mailbox->buffer_empty == 1) {
        mailbox->buffer_empty = 0;
    }
    g_ipc_hal_funcs->set_core_int(mailbox->dst_core, mailbox->channel);
    ipc_port_irq_restore(status);
    return ERRCODE_SUCC;
}

STATIC errcode_t ipc_get_data_from_ringbuffer(ipc_mailbox_ring_header_t *mailbox, uint16_t read,
                                              uint8_t *databuf, uint32_t length)
{
    uint8_t *read_ptr;
    uint8_t *databuf_tmp;
    uint32_t end_index;
    uint32_t ring_end_index;
    uint32_t current_length;

    if (length == 0) {
        return ERRCODE_SUCC;
    }
    read_ptr = mailbox->ring_buffer;
    databuf_tmp = databuf;
    read_ptr += read;
    end_index = read + length - 1;
    ring_end_index = mailbox->total_size - 1;
    if (end_index > ring_end_index) {
        current_length = ring_end_index - read + 1;
        if (memcpy_s(databuf_tmp, current_length, read_ptr, current_length) != EOK) {
            return ERRCODE_FAIL;
        }
        databuf_tmp += current_length;
        current_length = length - current_length;
        if (memcpy_s(databuf_tmp, current_length, mailbox->ring_buffer, current_length) != EOK) {
            return ERRCODE_FAIL;
        }
    } else {
        if (memcpy_s(databuf_tmp, length, read_ptr, length) != EOK) {
            return ERRCODE_FAIL;
        };
    }
    return ERRCODE_SUCC;
}

STATIC errcode_t ipc_get_msg_from_mailbox(ipc_mailbox_ring_header_t *mailbox, ipc_ring_element_header_t *current_msg,
                                          uint8_t *databuf)
{
    uint32_t status = ipc_port_irq_lock();
    uint16_t read = mailbox->read;
    errcode_t ret = ipc_get_data_from_ringbuffer(mailbox, read, (uint8_t *)current_msg,
                                                 (uint32_t)sizeof(ipc_ring_element_header_t));
    uint32_t length;

    if (ret != ERRCODE_SUCC) {
        ipc_port_irq_restore(status);
        return ret;
    }
    read = (read + (uint32_t)sizeof(ipc_ring_element_header_t)) % mailbox->total_size;
    ret = ipc_get_data_from_ringbuffer(mailbox, read, databuf, current_msg->cur_msg_len);
    if (ret != ERRCODE_SUCC) {
        ipc_port_irq_restore(status);
        return ret;
    }
    length = current_msg->cur_msg_len + (uint32_t)sizeof(ipc_ring_element_header_t);
    mailbox->read = (uint16_t)((mailbox->read + length) % mailbox->total_size);
    if (mailbox->write == mailbox->read) {
        mailbox->buffer_empty = 1;
    }
    ipc_port_irq_restore(status);
    return ERRCODE_SUCC;
}

errcode_t ipc_ring_receive_msg(uint8_t src_core, uint8_t channel)
{
    errcode_t ret = ERRCODE_SUCC;
    ipc_mailbox_ring_header_t *mailbox = ipc_get_rx_mailbox(src_core, channel);
    ipc_ring_element_header_t current_msg = {
        .msg_id = 0,
        .priority = 0,
        .cur_msg_len = 0,
    };
    if (mailbox == NULL) {
        return ERRCODE_IPC_UNDEFINED_ACTION;
    }
    while (mailbox->read != mailbox->write || mailbox->buffer_empty == 0) {
        ret = ipc_get_msg_from_mailbox(mailbox, &current_msg, g_databuf);
        if (ret != ERRCODE_SUCC) {
            break;
        }
        if (g_ipc_msg_handlers[current_msg.msg_id] == NULL) {
            continue;
        }
        g_ipc_msg_handlers[current_msg.msg_id](g_databuf, current_msg.cur_msg_len);
    }
    return ret;
}

errcode_t ipc_ring_init(void)
{
    errcode_t ret = ipc_mailbox_init();
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    ipc_msg_handler_clear();
    ipc_port_register_hal_funcs();
    g_ipc_hal_funcs = hal_ipc_get_funcs();
    if (g_ipc_hal_funcs == NULL) {
        return ERRCODE_IPC_NOT_REGISTER_HAL_FUNCS;
    }
    ipc_port_register_irq();
    g_ipc_hal_funcs->register_callback(ipc_ring_receive_msg);
    g_ipc_hal_funcs->init();
    g_ipc_hal_funcs->enable_all_int();

    return ERRCODE_SUCC;
}

errcode_t ipc_ring_deinit(void)
{
    ipc_msg_handler_clear();
    ipc_port_unregister_irq();
    g_ipc_hal_funcs->disable_all_int();
    g_ipc_hal_funcs->deinit();
    g_ipc_hal_funcs->unregister_callback();
    g_ipc_hal_funcs = NULL;
    ipc_port_unregister_hal_funcs();
    ipc_mailbox_deinit();
    return ERRCODE_SUCC;
}

errcode_t ipc_ring_send_msg_async(ipc_msg_info_t *msg_info)
{
#if defined(CONFIG_PM_IPC_MEM_POWER_OFF)
    ipc_port_memory_power_on();
#endif
    ipc_mailbox_ring_header_t *mailbox = ipc_get_tx_mailbox(msg_info->dst_core, msg_info->channel);
    ipc_ring_element_header_t current_msg;
    errcode_t ret;

    if (mailbox == NULL) {
        return ERRCODE_IPC_UNDEFINED_ACTION;
    }
    current_msg.msg_id = msg_info->msg_id;
    current_msg.priority = msg_info->priority;
    current_msg.cur_msg_len = (uint16_t)msg_info->buf_len;

    ret = ipc_set_msg_into_mailbox(mailbox, &current_msg, msg_info->buf_addr);
    return ret;
}

errcode_t ipc_ring_send_msg_sync(ipc_msg_info_t *msg_info)
{
    errcode_t ret = ERRCODE_FAIL;
    g_send_start_ms = uapi_systick_get_ms();
    g_send_end_ms = g_send_start_ms + CONFIG_IPC_SEND_MSG_MAX_WAIT_TIME_MS;
    g_send_cur_ms = g_send_start_ms;
    while (g_send_cur_ms <= g_send_end_ms) {
        ret = ipc_ring_send_msg_async(msg_info);
        if (ret == ERRCODE_SUCC) {
            return ret;
        }
#ifdef IPC_USE_OS_DELAY
        osDelay(DELAY_NEXT_SEND);
#endif
        g_send_cur_ms = uapi_systick_get_ms();
    }
    return ret;
}

errcode_t ipc_ring_register_rx_handler(ipc_rx_handler_info_t *handler_info)
{
    uint8_t msg_id = handler_info->msg_id;
    if (msg_id >= IPC_MSG_ID_MAX) {
        return ERRCODE_IPC_UNDEFINED_ACTION;
    }
    if (handler_info->cb == NULL) {
        return ERRCODE_IPC_INVALID_PARAMETER;
    }
    g_ipc_msg_handlers[msg_id] = handler_info->cb;

    return ERRCODE_SUCC;
}
