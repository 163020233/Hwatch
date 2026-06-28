/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2016-2022. All rights reserved.
 * Description: SOC KEY VALUE STORAGE IMPLEMENTATION
 */

#include "nv.h"
#include "nv_storage.h"
#include "nv_store.h"
#include "nv_reset.h"
#include "securec.h"
#include "nv_porting.h"
#include "common_def.h"
#include "systick.h"
#include "nv_async_store.h"
#include "nv_update.h"
#include "nv_notify.h"
#include "nv_task_adapt.h"
#include "nv_storage_handle.h"

void uapi_nv_init(void)
{
    (void)uapi_nv_extra_init(KV_STORE_APPLICATION);
}

errcode_t uapi_nv_write_with_attr(uint16_t key, const uint8_t *kvalue, uint16_t kvalue_length, nv_key_attr_t *attr,
                                  nv_storage_completed_callback func)
{
    return uapi_nv_extra_write_with_attr(KV_STORE_APPLICATION, key, kvalue, kvalue_length, attr, func);
}

errcode_t uapi_nv_write(uint16_t key, const uint8_t *kvalue, uint16_t kvalue_length)
{
    return uapi_nv_extra_write(KV_STORE_APPLICATION, key, kvalue, kvalue_length);
}

errcode_t uapi_nv_write_force(uint16_t key, const uint8_t *kvalue, uint16_t kvalue_length)
{
    return uapi_nv_extra_write_force(KV_STORE_APPLICATION, key, kvalue, kvalue_length);
}

errcode_t uapi_nv_update_key_attr(uint16_t key, nv_key_attr_t *attr, nv_storage_completed_callback func)
{
    return uapi_nv_extra_update_key_attr(KV_STORE_APPLICATION, key, attr, func);
}

errcode_t uapi_nv_get_key_attr(uint16_t key, uint16_t *length, nv_key_attr_t *attr)
{
    return uapi_nv_extra_get_key_attr(KV_STORE_APPLICATION, key, length, attr);
}

errcode_t uapi_nv_read_with_attr(uint16_t key, uint16_t kvalue_max_length, uint16_t *kvalue_length, uint8_t *kvalue,
                                 nv_key_attr_t *attr)
{
    return uapi_nv_extra_read_with_attr(KV_STORE_APPLICATION, key, kvalue_max_length, kvalue_length, kvalue, attr);
}

errcode_t uapi_nv_read(uint16_t key, uint16_t kvalue_max_length, uint16_t *kvalue_length, uint8_t *kvalue)
{
    return uapi_nv_extra_read(KV_STORE_APPLICATION, key, kvalue_max_length, kvalue_length, kvalue);
}

errcode_t uapi_nv_delete_key(uint16_t key, bool completely)
{
    return uapi_nv_extra_delete_key(KV_STORE_APPLICATION, key, completely);
}

bool uapi_nv_is_stored(uint16_t key, uint16_t kvalue_length, const uint8_t *kvalue)
{
    return uapi_nv_extra_is_stored(KV_STORE_APPLICATION, key, kvalue_length, kvalue);
}

errcode_t uapi_nv_get_store_status(nv_store_status_t *status)
{
    return uapi_nv_extra_get_store_status(KV_STORE_APPLICATION, status);
}

errcode_t uapi_nv_backup(const nv_backup_mode_t *backup_mode)
{
    return uapi_nv_extra_backup(KV_STORE_APPLICATION, backup_mode);
}

errcode_t uapi_nv_set_restore_mode_all(void)
{
    return uapi_nv_extra_set_restore_mode_all(KV_STORE_APPLICATION);
}

errcode_t uapi_nv_set_restore_mode_partitial(const nv_restore_mode_t *restore_mode)
{
    return uapi_nv_extra_set_restore_mode_partitial(KV_STORE_APPLICATION, restore_mode);
}

errcode_t uapi_nv_register_change_notify_proc(uint16_t min_key, uint16_t max_key, nv_changed_notify_func func)
{
    return uapi_nv_extra_register_change_notify_proc(KV_STORE_APPLICATION, min_key, max_key, func);
}

errcode_t uapi_nv_flush(void)
{
    return uapi_nv_extra_flush(KV_STORE_APPLICATION);
}
