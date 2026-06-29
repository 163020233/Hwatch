/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All
 * rights reserved.
 *
 * filename:libteec.c
 * Author: @CompanyNameTag
 * Create:2024-04-01
 * Description: GP API
 *
 */

#include "tee_client_api.h"
#include "debug_print.h"
#include "seliteos_memory_config.h"
#include "tcxo.h"
#include "los_memory.h"
#include "tesvec_ioctl.h"
#include "ree_tee_comm.h"

TEEC_Result ree_tee_communication(struct tee_msg_arg *arg, uint32_t param_nums)
{
    gp_comm_arg_t local_arg = {.arg = arg, .param_nums = param_nums};

    return ree_tee_comm_unify_entry(CMD_GP_COMM, &local_arg);
}

TEEC_Result TEEC_InitializeContext(const char *name, TEEC_Context *context)
{
    (void)name;
    if (context == NULL) {
        PRINT("param error!\n");
        return TEEC_ERROR_BAD_PARAMETERS;
    }

    PRINT("TEEC_InitializeContext!\n");
    context->is_inited = true;
    return TEEC_SUCCESS;
}

void TEEC_FinalizeContext(TEEC_Context *context)
{
    if (context == NULL) {
        return;
    }

    PRINT("TEEC_FinalizeContext!\n");
    context->is_inited = false;
    return;
}

static TEEC_Result pre_temref_process(TEEC_Operation *operation,
                                      struct tee_msg_param *param,
                                      uint32_t param_type, int i)
{
    TEEC_TempMemoryReference tmpref = operation->params[i].tmpref;
    if (tmpref.buffer == NULL || tmpref.size > MAX_TEMP_BUFFER_LEN) {
        return TEEC_ERROR_BAD_PARAMETERS;
    }

    param[i].attr =
        param_type - TEEC_MEMREF_TEMP_INPUT + TEE_MSG_ATTR_TYPE_TMEM_INPUT;
    param[i].u.tmem.buf_ptr = (uintptr_t)tmpref.buffer;
    param[i].u.tmem.size = tmpref.size;

    return TEEC_SUCCESS;
}

static TEEC_Result pre_memref_process(TEEC_Context *ctx,
                                      TEEC_Operation *operation,
                                      struct tee_msg_param *param,
                                      uint32_t param_type, int i)
{
    uint32_t inout = TEEC_MEM_INPUT | TEEC_MEM_OUTPUT;
    uint32_t flags = 0;
    TEEC_SharedMemory *shm = operation->params[i].memref.parent;
    (void)param_type;
    if (!shm) {
        return TEEC_ERROR_BAD_PARAMETERS;
    }

    if (shm->ctx != ctx) {
        return TEEC_ERROR_BAD_STATE;
    }

    flags = shm->flags;
    if (flags == inout) {
        param[i].attr = TEE_MSG_ATTR_TYPE_RMEM_INOUT;
    } else if (flags == TEEC_MEM_INPUT) {
        param[i].attr = TEE_MSG_ATTR_TYPE_RMEM_INPUT;
    } else {
        param[i].attr = TEE_MSG_ATTR_TYPE_RMEM_OUTPUT;
    }

    param[i].u.rmem.shm_ref = (uint64_t)(uintptr_t)shm;
    param[i].u.rmem.size = shm->size;
    param[i].u.rmem.offs = 0;

    return TEEC_SUCCESS;
}

static TEEC_Result pre_partial_memref_process(TEEC_Context *ctx,
                                              TEEC_Operation *operation,
                                              struct tee_msg_param *param,
                                              uint32_t param_type, int i)
{
    TEEC_RegisteredMemoryReference memref = operation->params[i].memref;
    TEEC_SharedMemory *shm = memref.parent;

    if (shm->ctx != ctx) {
        return TEEC_ERROR_BAD_STATE;
    }

    param[i].attr =
        param_type - TEEC_MEMREF_PARTIAL_INPUT + TEE_MSG_ATTR_TYPE_RMEM_INPUT;
    // judge overflow and buffer size is enough
    if (memref.offset + memref.size < memref.offset ||
        memref.offset + memref.size > shm->size) {
        PRINT("offs is %d, size is %d, shm.size is %d!\n", param[i].u.rmem.offs,
              param[i].u.rmem.size, shm->size);
        return TEEC_ERROR_BAD_PARAMETERS;
    }

    param[i].u.rmem.shm_ref = (uint64_t)(uintptr_t)shm;
    param[i].u.rmem.size = operation->params[i].memref.size;
    param[i].u.rmem.offs = operation->params[i].memref.offset;

    return TEEC_SUCCESS;
}

static TEEC_Result pre_process_operation(TEEC_Context *ctx,
                                         TEEC_Operation *operation,
                                         struct tee_msg_param *param)
{
    TEEC_Result res = TEEC_SUCCESS;
    uint32_t param_type = 0;
    uint32_t size =
        sizeof(struct tee_msg_param) * TEEC_CONFIG_PAYLOAD_REF_COUNT;

    if (!operation) {
        (void)memset_s(param, size, 0, size);
        return TEEC_SUCCESS;
    }

    for (int i = 0; i < TEEC_CONFIG_PAYLOAD_REF_COUNT; ++i) {
        param_type = TEEC_PARAM_TYPE_GET(operation->paramTypes, i);
        switch (param_type) {
            case TEEC_NONE:
                param[i].attr = param_type;
                break;
            case TEEC_VALUE_INOUT:
            case TEEC_VALUE_INPUT:
            case TEEC_VALUE_OUTPUT:
                param[i].attr = param_type;
                param[i].u.value.a = operation->params[i].value.a;
                param[i].u.value.b = operation->params[i].value.b;
                break;
            case TEEC_MEMREF_TEMP_INOUT:
            case TEEC_MEMREF_TEMP_INPUT:
            case TEEC_MEMREF_TEMP_OUTPUT:
                res = pre_temref_process(operation, param, param_type, i);
                break;
            case TEEC_MEMREF_WHOLE:
                res = pre_memref_process(ctx, operation, param, param_type, i);
                break;
            case TEEC_MEMREF_PARTIAL_INPUT:
            case TEEC_MEMREF_PARTIAL_OUTPUT:
            case TEEC_MEMREF_PARTIAL_INOUT:
                res = pre_partial_memref_process(ctx, operation, param,
                                                 param_type, i);
                break;
            default:
                return TEEC_ERROR_BAD_PARAMETERS;
        }

        if (res != TEEC_SUCCESS) {
            return res;
        }
    }

    return res;
}

static void post_process_operation(TEEC_Operation *operation,
                                   struct tee_msg_param *param)
{
    uint32_t param_type = 0;

    if (!operation) {
        return;
    }

    for (int i = 0; i < TEEC_CONFIG_PAYLOAD_REF_COUNT; ++i) {
        param_type = TEEC_PARAM_TYPE_GET(operation->paramTypes, i);
        switch (param_type) {
            case TEEC_VALUE_INOUT:
            case TEEC_VALUE_OUTPUT:
                operation->params[i].value.a = param[i].u.value.a;
                operation->params[i].value.b = param[i].u.value.b;
                break;
            case TEEC_MEMREF_WHOLE:
                // memref is shared memory， output is already inside this
                // memory
                operation->params[i].memref.size = param[i].u.rmem.size;
                break;
            case TEEC_MEMREF_PARTIAL_OUTPUT:
            case TEEC_MEMREF_PARTIAL_INOUT:
                operation->params[i].memref.size = param[i].u.rmem.size;
                break;
            case TEEC_MEMREF_TEMP_INOUT:
            case TEEC_MEMREF_TEMP_OUTPUT:
                // tmpref will be change in tee
                operation->params[i].memref.size = param[i].u.tmem.size;
                break;
            default:
                break;
        }
    }
}

static TEEC_Result process_connect_data(uint32_t connection_method,
                                        uint32_t *connection_data,
                                        TEEC_UUID *clint_uuid)
{
    TEEC_Result ret = TEEC_SUCCESS;

    switch (connection_method) {
        case TEEC_LOGIN_PUBLIC:
            break;
        case TEEC_LOGIN_USER:
            break;
        case TEEC_LOGIN_APPLICATION:
            break;
        case TEEC_LOGIN_USER_APPLICATION:
            break;
        case TEEC_LOGIN_GROUP:
        case TEEC_LOGIN_GROUP_APPLICATION:
            if (connection_data == NULL) {
                return TEEC_ERROR_BAD_PARAMETERS;
            }

            if (memcpy_s(clint_uuid, sizeof(TEEC_UUID), connection_data,
                         sizeof(uint32_t)) != EOK) {
                return TEEC_ERROR_BAD_STATE;
            }

            break;
        default:
            break;
    }

    return ret;
}

TEEC_Result TEEC_OpenSession(TEEC_Context *context, TEEC_Session *session,
                             const TEEC_UUID *destination,
                             uint32_t connectionMethod,
                             const void *connectionData,
                             TEEC_Operation *operation, uint32_t *returnOrigin)
{
    TEEC_Result ret = TEEC_SUCCESS;
    int res = 0;
    struct tee_msg_arg *arg = NULL;
    uint32_t ret_origin = TEEC_ORIGIN_COMMS;
    uint32_t arg_size =
        sizeof(struct tee_msg_arg) +
        sizeof(struct tee_msg_param) *
            (TEEC_CONFIG_PAYLOAD_REF_COUNT + TEEC_CONFIG_META_COUNT);
    TEEC_UUID clint_uuid = {0};

    if (!context || !context->is_inited || !session || !destination) {
        ret = TEEC_ERROR_BAD_PARAMETERS;
        goto err_exit2;
    }

    ret = process_connect_data(connectionMethod, (uint32_t *)connectionData, &clint_uuid);
    if (ret != TEEC_SUCCESS) {
        goto err_exit2;
    }
    
    arg = (struct tee_msg_arg *)calloc(1, arg_size);
    if (arg == NULL) {
        ret = TEEC_ERROR_OUT_OF_MEMORY;
        goto err_exit2;
    }

    arg->num_params = TEEC_CONFIG_PAYLOAD_REF_COUNT + TEEC_CONFIG_META_COUNT;
    arg->params[0].attr = TEE_MSG_ATTR_TYPE_VALUE_INPUT | TEE_MSG_ATTR_META;
    arg->params[1].attr = TEE_MSG_ATTR_TYPE_VALUE_INPUT | TEE_MSG_ATTR_META;
    res = memcpy_s(&arg->params[0].u.value, sizeof(struct tee_msg_param_value),
                   destination, sizeof(TEEC_UUID));
    if (res != EOK) {
        ret = TEEC_ERROR_OUT_OF_MEMORY;
        goto err_exit;
    }

    arg->params[1].u.value.c = connectionMethod;
    res = memcpy_s(&arg->params[1].u.value, sizeof(struct tee_msg_param_value),
                   &clint_uuid, sizeof(TEEC_UUID));
    if (res != EOK) {
        ret = TEEC_ERROR_OUT_OF_MEMORY;
        goto err_exit;
    }

    // 处理operation里的param, 如果operation为NULL, 则param置0
    ret = pre_process_operation(context, operation,
                                arg->params + TEEC_CONFIG_META_COUNT);
    if (ret != TEEC_SUCCESS) {
        goto err_exit;
    }

    PRINT("TEEC_OpenSession!\n");
    arg->cmd = TEE_MSG_CMD_OPEN_SESSION;
    ret = ree_tee_communication(arg, TEEC_CONFIG_META_COUNT +
                                         TEEC_CONFIG_PAYLOAD_REF_COUNT);
    if (ret != TEEC_SUCCESS) {
        ret_origin = arg->ret_origin != 0 ? arg->ret_origin : TEEC_ORIGIN_COMMS;
        goto err_exit;
    }

    ret_origin = arg->ret_origin;
    ret = arg->ret;
    if (ret == TEEC_SUCCESS) {
        session->ctx = context;
        session->session_id = arg->session;
    }

    post_process_operation(operation, arg->params + TEEC_CONFIG_META_COUNT);

err_exit:
    (void)free(arg);
err_exit2:
    if (returnOrigin) {
        *returnOrigin = ret_origin;
    }

    return ret;
}

static void start_time_stamp(uint64_t *start_time)
{
#if (GP_BENCHMARK == 1)
    *start_time = uapi_tcxo_get_us();
#else
    (void)start_time;
#endif
}

static void end_time_stamp(uint64_t start_time)
{
#if (GP_BENCHMARK == 1)
    uint64_t end_time = uapi_tcxo_get_us();
    PRINT("invoke command cost time %d!\n", (uint32_t)(end_time - start_time));
#else
    (void)&start_time;
#endif
}

TEEC_Result TEEC_InvokeCommand(TEEC_Session *session, uint32_t commandID,
                               TEEC_Operation *operation,
                               uint32_t *returnOrigin)
{
    TEEC_Result ret = TEEC_SUCCESS;
    struct tee_msg_arg *arg = NULL;
    uint32_t ret_origin = TEEC_ORIGIN_COMMS;
    uint64_t start_time = 0;
    uint32_t arg_size =
        sizeof(struct tee_msg_arg) +
        sizeof(struct tee_msg_param) * TEEC_CONFIG_PAYLOAD_REF_COUNT;

    if (!session) {
        ret = TEEC_ERROR_BAD_PARAMETERS;
        return ret;
    }

    arg = (struct tee_msg_arg *)calloc(1, arg_size);
    if (arg == NULL) {
        ret = TEEC_ERROR_OUT_OF_MEMORY;
        goto err_exit2;
    }

    start_time_stamp(&start_time);
    // 处理operation里的param, 如果operation为NULL, 则param置0
    ret = pre_process_operation(session->ctx, operation, arg->params);
    if (ret != TEEC_SUCCESS) {
        goto err_exit;
    }

    PRINT("TEEC_InvokeCommand!\n");
    arg->num_params = TEEC_CONFIG_PAYLOAD_REF_COUNT;
    arg->cmd = TEE_MSG_CMD_INVOKE_COMMAND;
    arg->func = commandID;
    arg->session = session->session_id;
    ret = ree_tee_communication(arg, TEEC_CONFIG_PAYLOAD_REF_COUNT);
    if (ret != TEEC_SUCCESS) {
        ret_origin = arg->ret_origin != 0 ? arg->ret_origin : TEEC_ORIGIN_COMMS;
        goto err_exit;
    }

    ret = arg->ret;
    ret_origin = arg->ret_origin;
    post_process_operation(operation, arg->params);
    end_time_stamp(start_time);

err_exit:
    (void)free(arg);
err_exit2:
    if (returnOrigin) {
        *returnOrigin = ret_origin;
    }

    return ret;
}

void TEEC_CloseSession(TEEC_Session *session)
{
    struct tee_msg_arg *arg = NULL;

    if (!session) {
        return;
    }

    PRINT("TEEC_CloseSession!\n");

    arg = (struct tee_msg_arg *)calloc(1, sizeof(struct tee_msg_arg));
    if (arg == NULL) {
        return;
    }

    arg->session = session->session_id;
    arg->cmd = TEE_MSG_CMD_CLOSE_SESSION;
    if (ree_tee_communication(arg, 0) != TEEC_SUCCESS || arg->ret != TEEC_SUCCESS) {
        PRINT("%s failed!\n", __func__);
    }

    (void)free(arg);
}

TEEC_Result TEEC_AllocateSharedMemory(TEEC_Context *ctx, TEEC_SharedMemory *shm)
{
    uint32_t size = 0;

    if (ctx == NULL || !ctx->is_inited || shm == NULL) {
        return TEEC_ERROR_BAD_PARAMETERS;
    }

    if (!shm->flags || (shm->flags & ~(TEEC_MEM_INPUT | TEEC_MEM_OUTPUT))) {
        return TEEC_ERROR_BAD_PARAMETERS;
    }

    size = shm->size;
    if (size == 0) {
        size = TEEC_SHARED_MEMORY_CONST_SIZE;
    }

    if (size > TEEC_CONFIG_SHAREDMEM_MAX_SIZE) {
        PRINT("The max size of SharedMemory is %#x!\n",
              TEEC_CONFIG_SHAREDMEM_MAX_SIZE);
        return TEEC_ERROR_BAD_PARAMETERS;
    }

    shm->buffer = (void *)ree_tee_shared_memory_alloc(size);
    if (shm->buffer == NULL) {
        return TEEC_ERROR_OUT_OF_MEMORY;
    }

    shm->alloced_size = size;
    shm->ctx = ctx;
    return TEEC_SUCCESS;
}

void TEEC_ReleaseSharedMemory(TEEC_SharedMemory *shm)
{
    if (!shm) {
        return;
    }

    if (shm->buffer) {
        (void)ree_tee_shared_memory_free(shm->buffer);
    }

    shm->buffer = NULL;
    shm->size = 0;
    shm->alloced_size = 0;
    shm->ctx = NULL;
}
