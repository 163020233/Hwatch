/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All
 * rights reserved.
 * Description: demo TA
 *
 */

#include "los_printf.h"
#include "seliteos_memory_config.h"
#include "boxa_header.h"
#include "ta_manager.h"

static TEE_Result TA_CreateEntryPoint(void)
{
    /* Nothing to do */
    PRINT_INFO("%s!\n", __func__);
    return TEE_SUCCESS;
}

static void TA_DestroyEntryPoint(void)
{
    /* Nothing to do */
    PRINT_INFO("%s!\n", __func__);
}

static TEE_Result TA_OpenSessionEntryPoint(uint32_t param_types,
                                           TEE_Param params[4], void **session)
{
    (void)param_types;
    (void)params;
    (void)session;
    PRINT_INFO("%s!\n", __func__);
    /* Nothing to do */
    return TEE_SUCCESS;
}

static void TA_CloseSessionEntryPoint(void *session)
{
    (void)session;
    /* Nothing to do */
    PRINT_INFO("%s!\n", __func__);
}

static uint8_t g_hello_boxa[] = "Hello boxA!\n";
static TEE_Result TA_InvokeCommandEntryPoint(void *session, uint32_t command,
                                             uint32_t param_types,
                                             TEE_Param params[4])
{
    (void)session;
    (void)param_types;
    (void)params;
    switch (command) {
        case TA_HELLO_TEST:
            LOS_Printf("Hello world!\n");
            LOS_Printf("%s", g_hello_boxa);
            return TEE_SUCCESS;
        default:
            PRINT_ERR("Command ID 0x%x is not supported\n", command);
            return TEE_ERROR_NOT_IMPLEMENTED;
    }
}

static const struct ta_ops ta_op SECTION(".ta_array_ops") = {
    .create_entry_point = TA_CreateEntryPoint,
    .destroy_entry_point = TA_DestroyEntryPoint,
    .open_session_entry_point = TA_OpenSessionEntryPoint,
    .close_session_entry_point = TA_CloseSessionEntryPoint,
    .invoke_command_entry_point = TA_InvokeCommandEntryPoint,
};

ta_register(.name = "hello_world", .uuid = TA_UUID, .ops = (struct ta_ops *)&ta_op,
            .data_addr = BOXA_DATA_START, .heap_size = BOXA_HEAP_SIZE,
            .data_size = BOXA_DATA_SIZE,
            .stack_addr = BOXA_STACK_START,
            .stack_size = BOXA_STACK_SIZE,
            .text_rodata_addr = BOXA_TEXT_RODATA_START,
            .text_rodata_size = BOXA_TEXT_RODATA_SIZE);
