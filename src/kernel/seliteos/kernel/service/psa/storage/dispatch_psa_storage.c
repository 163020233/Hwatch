/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PSA storage function entry adapter source for SELiteOS.
 */

#include "los_base.h"
#include "los_mem.h"
#include "pmp_mgr.h"
#include "psa/psa_storage_common.h"
#include "sec_storage.h"
#include "dispatch_psa_storage.h"

#define MIN(x, y)             (x) > (y) ? (y) : (x)
#define MAX_PSA_MALLOC_LENGTH 3012

static bool is_psa_storage_id(uint32_t sec_storage_id)
{
    // the sec_storage_id PSA storage can use is the upper of the kernel sec_storage_id
    if (sec_storage_id > 0 && sec_storage_id < PSA_STORAGE_ID_RANGE) {
        return true;
    }

    return false;
}

int32_t dispatch_psa_storage_read(uint32_t cmd, void *arg)
{
    volatile_sec int32_t ret = PSA_INTERNAL_FAILURE;
    psa_storage_read_arg_t *local_arg = arg;
    uint8_t *data_buffer = NULL;
    uint16_t data_buffer_length = 0;
    uint32_t data_buffer_offset = 0;
    uint32_t copy_data_length = 0;
    uint16_t data_length = 0;

    // local_arg content校验
    los_chk_return((cmd != CMD_PSA_STORAGE_READ_HEADER && cmd != CMD_PSA_STORAGE_READ_DATA), PSA_ERROR_INVALID_ARGUMENT,
                   "cmd is invalid!\n");
    los_chk_return((local_arg->data == NULL && local_arg->data_size != 0), PSA_ERROR_INVALID_ARGUMENT, "data is invalid!\n");
    los_chk_return(local_arg-> data_size != 0 &&
                   check_is_not_in_tee_range((uintptr_t)local_arg->data, local_arg->data_size) != LOS_SUCCESS,
                   PSA_ERROR_INVALID_ARGUMENT, "data range is invalid!\n");
    los_chk_return(!is_psa_storage_id((uint32_t)local_arg->sec_storage_id), PSA_ERROR_INVALID_ARGUMENT,
                   "sec_storage_id is invalid!\n");

    // need to check data_length from remote_call
    ret = uapi_sec_storage_get_key_length(local_arg->sec_storage_id, &data_length);
    los_chk_return(psa_get_wrap_errcode(ret) != PSA_INTERNAL_SUCCESS, psa_get_wrap_errcode(ret),
        "uapi_sec_storage_get_key_length failed!\n");
    los_chk_return(data_length > MAX_PSA_MALLOC_LENGTH, PSA_ERROR_INVALID_ARGUMENT,
        "uapi_sec_storage_get_key_length failed!\n");

    data_buffer = los_kmalloc(data_length);
    los_chk_return(data_buffer == NULL, PSA_ERROR_INSUFFICIENT_MEMORY, "los_kmalloc failed!\n");

    ret = uapi_sec_storage_read(local_arg->sec_storage_id, data_length, &data_buffer_length, data_buffer);
    los_chk_goto_with_ret(ret, psa_get_wrap_errcode(ret) != PSA_INTERNAL_SUCCESS, free_buffer,
        psa_get_wrap_errcode(ret), "uapi_sec_storage_read failed!\n");
    los_chk_goto_with_ret(ret, data_buffer_length > MAX_PSA_MALLOC_LENGTH ||
        data_buffer_length < sizeof(struct psa_storage_info_t), free_buffer, PSA_ERROR_INVALID_ARGUMENT,
        "uapi_sec_storage_read failed!\n");

    // the length of data in nv storage is sizeof(struct psa_storage_info_t) + data_size
    copy_data_length = cmd == CMD_PSA_STORAGE_READ_DATA ? data_buffer_length - sizeof(struct psa_storage_info_t)
                                                        : sizeof(struct psa_storage_info_t);
    los_chk_goto_with_ret(ret, local_arg->data_offset > copy_data_length, free_buffer, PSA_ERROR_INVALID_ARGUMENT,
        "data_offset is greater than sec_storage_id.size!\n");

    if (local_arg->data_size == 0 || local_arg->data_offset == copy_data_length) {
        ret = PSA_INTERNAL_SUCCESS;
        local_arg->data_length = 0;
        goto free_buffer;
    }

    /* if cmd == CMD_PSA_STORAGE_READ_DATA, get the min(data_size, sec_storage_id.size-data_offset);
    else the data_size eqaul to sec_storage_id.size-data_offset */
    data_buffer_offset =
        cmd == CMD_PSA_STORAGE_READ_DATA ? sizeof(struct psa_storage_info_t) + local_arg->data_offset : 0;
    copy_data_length = MIN(local_arg->data_size, copy_data_length - local_arg->data_offset);

    ret = memcpy_sec(local_arg->data, local_arg->data_size, data_buffer + data_buffer_offset, copy_data_length);
    los_chk_goto(ret != PSA_INTERNAL_SUCCESS, free_buffer, "memcpy_sec failed!\n");

    local_arg->data_length = copy_data_length;
free_buffer:
    los_kfree(data_buffer);
    return ret;
}

int32_t dispatch_psa_storage_write(uint32_t cmd, void *arg)
{
    volatile_sec int32_t ret = PSA_INTERNAL_FAILURE;
    psa_storage_write_arg_t *local_arg = arg;
    uint8_t *data_buffer = NULL;
    uint32_t data_buffer_length = 0;
    struct psa_storage_info_t *psa_storage_header = NULL;
    sec_storage_attr_t sec_storage_attr = {0};

    // local_arg content校验
    los_chk_return(cmd != CMD_PSA_STORAGE_WRITE, PSA_ERROR_INVALID_ARGUMENT, "cmd is invalid!\n");
    los_chk_return((local_arg->data == NULL && local_arg->data_length != 0), PSA_ERROR_INVALID_ARGUMENT,
                   "data is invalid!\n");
    los_chk_return(local_arg->data_length != 0 &&
                   check_is_not_in_tee_range((uintptr_t)local_arg->data, local_arg->data_length) != LOS_SUCCESS,
                   PSA_ERROR_INVALID_ARGUMENT, "data range is invalid!\n");
    los_chk_return(!is_psa_storage_id((uint32_t)local_arg->sec_storage_id), PSA_ERROR_INVALID_ARGUMENT,
                   "sec_storage_id is invalid!\n");
    los_chk_return(local_arg->data_length + sizeof(struct psa_storage_info_t) < local_arg->data_length ||
                       local_arg->data_length + sizeof(struct psa_storage_info_t) > MAX_PSA_MALLOC_LENGTH,
                   PSA_ERROR_INVALID_ARGUMENT, "data_length is invalid!\n");

    data_buffer_length = local_arg->data_length + sizeof(struct psa_storage_info_t);
    data_buffer = los_kmalloc(data_buffer_length);
    los_chk_return(data_buffer == NULL, PSA_ERROR_INSUFFICIENT_MEMORY, "los_kmalloc failed!\n");

    if (local_arg->data_length != 0) {
        ret = memcpy_sec(data_buffer + sizeof(struct psa_storage_info_t), local_arg->data_length, local_arg->data,
            local_arg->data_length);
        los_chk_goto(ret != PSA_INTERNAL_SUCCESS, free_buffer, "memcpy_sec failed!\n");
    }

    psa_storage_header = (struct psa_storage_info_t *)data_buffer;
    psa_storage_header->capacity = local_arg->data_length;
    psa_storage_header->size = local_arg->data_length;
    psa_storage_header->flags = local_arg->flags;
    sec_storage_attr.permanent = local_arg->flags & PSA_STORAGE_FLAG_WRITE_ONCE;

    ret = uapi_sec_storage_write_with_attr((uint16_t)local_arg->sec_storage_id, data_buffer, data_buffer_length,
                                           sec_storage_attr);
    los_chk_goto_with_ret(ret, psa_get_wrap_errcode(ret) != PSA_INTERNAL_SUCCESS, free_buffer,
        psa_get_wrap_errcode(ret), "uapi_sec_storage_write_with_attr failed!\n");
    ret = PSA_INTERNAL_SUCCESS;

free_buffer:
    los_kfree(data_buffer);
    return ret;
}

int32_t dispatch_psa_storage_remove(uint32_t cmd, void *arg)
{
    volatile_sec int32_t ret = PSA_INTERNAL_FAILURE;
    uint32_t sec_storage_id = (uint32_t)(*(uint64_t *)arg);
    uint8_t *data_buffer = NULL;
    uint16_t data_buffer_length = 0;
    uint16_t data_length = 0;

    // psa remove key need to check whether the sec_storage_id is valid or not
    los_chk_return(cmd != CMD_PSA_STORAGE_REMOVE, PSA_ERROR_INVALID_ARGUMENT, "cmd is invalid!\n");
    los_chk_return(!is_psa_storage_id(sec_storage_id), PSA_ERROR_INVALID_ARGUMENT, "sec_storage_id is invalid!\n");

    ret = uapi_sec_storage_get_key_length(sec_storage_id, &data_length);
    los_chk_return(psa_get_wrap_errcode(ret) != PSA_INTERNAL_SUCCESS, psa_get_wrap_errcode(ret),
                   "uapi_sec_storage_get_key_length failed!\n");
    los_chk_return(data_length > MAX_PSA_MALLOC_LENGTH, PSA_ERROR_INVALID_ARGUMENT,
                   "uapi_sec_storage_get_key_length failed!\n");

    data_buffer = los_kmalloc(data_length);
    los_chk_return(data_buffer == NULL, PSA_ERROR_INSUFFICIENT_MEMORY, "los_kmalloc failed!\n");

    // Need to read the data by tee nv api, to ensure the data hasn't been tampered
    ret = uapi_sec_storage_read(sec_storage_id, data_length, &data_buffer_length, data_buffer);
    los_chk_goto_with_ret(ret, psa_get_wrap_errcode(ret) != PSA_INTERNAL_SUCCESS, free_buffer,
        psa_get_wrap_errcode(ret), "uapi_sec_storage_read failed!\n");

    ret = uapi_sec_storage_delete(sec_storage_id);
    los_chk_goto_with_ret(ret, psa_get_wrap_errcode(ret) != PSA_INTERNAL_SUCCESS, free_buffer,
        psa_get_wrap_errcode(ret), "uapi_sec_storage_delete failed!\n");
    ret = PSA_INTERNAL_SUCCESS;

free_buffer:
    los_kfree(data_buffer);
    return ret;
}
