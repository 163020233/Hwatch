/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description:  AT framework adapter
 */

#include <soc_osal.h>
#include <at.h>
#include <at_config.h>
#include <at_product.h>
#include <dut.h>
#include <debug_print.h>
#include "app_at_uart.h"
#ifdef CONFIG_NV_FEATURE_SUPPORT
#include "nv.h"
#include "key_id.h"
#endif
#include "app_at_init.h"

#define AT_CMD_ATTR_USER_MASK 0xFF00
#define AT_CMD_PARAM_MAX 32

static dut_cmd_level_t g_dut_cmd_lvl = DUT_CMD_LEVEL_WHITELIST;

STATIC void at_base_api_queue_create(uint32_t msg_count, uint32_t msg_size, unsigned long *queue_id)
{
    osal_msg_queue_create(NULL, (uint16_t)msg_count, queue_id, 0, (uint16_t)msg_size);
}

STATIC uint32_t at_base_api_msg_queue_write(unsigned long queue_id, void *msg_ptr,
    uint32_t msg_size, uint32_t timeout)
{
    return (uint32_t)osal_msg_queue_write_copy(queue_id, msg_ptr, msg_size, timeout);
}

STATIC uint32_t at_base_api_msg_queue_read(unsigned long queue_id, void *buf_ptr,
    uint32_t *buf_size, uint32_t timeout)
{
    return (uint32_t)osal_msg_queue_read_copy(queue_id, buf_ptr, buf_size, timeout);
}

STATIC void at_base_api_task_pause(void)
{
    osal_yield();
}

STATIC void* at_base_api_malloc(uint32_t size)
{
    return osal_kmalloc(size, OSAL_GFP_ATOMIC);
}

STATIC void at_base_api_free(void *addr)
{
    osal_kfree(addr);
}

STATIC void at_output(const char* str)
{
    at_uart_printf(str);
}

static void dut_set_cmd_permission(dut_cmd_level_t lvl)
{
    if (lvl >= DUT_CMD_LEVEL_BUTT) {
        g_dut_cmd_lvl = DUT_CMD_LEVEL_WHITELIST;
    } else {
        g_dut_cmd_lvl = lvl;
    }
}

dut_cmd_level_t dut_get_cmd_permission(void)
{
    return g_dut_cmd_lvl;
}

errcode_t dut_init(void)
{
    uint8_t ftm_flag = 0;
    uint16_t kvalue_len = 0;

#ifdef CONFIG_NV_FEATURE_SUPPORT
    errcode_t nv_op = uapi_nv_read(NV_ID_FTM_CONFIG, sizeof(uint8_t), &kvalue_len, &ftm_flag);
    if (nv_op != ERRCODE_SUCC) {
        ftm_flag = 0;
    }
#endif

    if (ftm_flag == 1) {
        dut_set_cmd_permission(DUT_CMD_LEVEL_DUT);
    } else {
        dut_set_cmd_permission(DUT_CMD_LEVEL_WHITELIST);
    }

    return ERRCODE_SUCC;
}

#ifdef CONFIG_AT_SUPPORT_CMD_ATTR
STATIC bool at_base_api_cmd_attr(uint16_t attr)
{
    uint16_t mask = attr & AT_CMD_ATTR_USER_MASK;
    dut_cmd_level_t lvl = dut_get_cmd_permission();
    if (mask <= (uint16_t)lvl) {
        return true;
    }
    return false;
}
#endif

void app_at_init(void)
{
    dut_init();
    at_base_api_t base_api = {
        .msg_queue_create_func = at_base_api_queue_create,
        .msg_queue_write_func = at_base_api_msg_queue_write,
        .msg_queue_read_func = at_base_api_msg_queue_read,
        .task_pause_func = at_base_api_task_pause,
        .malloc_func = at_base_api_malloc,
        .free_func = at_base_api_free,
#ifdef CONFIG_AT_SUPPORT_CMD_ATTR
        .cmd_attr_func = at_base_api_cmd_attr,
#endif
    };
    uapi_at_base_api_register(base_api);

    at_channel_id_t id = AT_UART_PORT;
    uapi_at_channel_write_register(id, at_output);
}

