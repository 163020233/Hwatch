/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SOC KEY VALUE STORAGE IMPLEMENTATION
 */

#include "nv_config.h"
#include "nv.h"
#include "nv_storage.h"
#include "nv_store.h"
#include "nv_async_store.h"
#include "securec.h"
#include "nv_porting.h"
#include "common_def.h"
#include "systick.h"
#include "nv_notify.h"
#include "nv_storage_handle.h"

errcode_t uapi_nv_extra_init(kv_store_t store)
{
    if (store >= KV_STORE_MAX_NUM) {
        return ERRCODE_NV_INVALID_PARAM;
    }
    return nv_direct_ctrl_init(store);
}

errcode_t uapi_nv_extra_write_with_attr(kv_store_t store, uint16_t key, const uint8_t *kvalue, uint16_t kvalue_length,
    nv_key_attr_t *attr, nv_storage_completed_callback func)
{
    errcode_t ret;

    if ((store >= KV_STORE_MAX_NUM) || (kvalue == NULL) || (kvalue_length == 0U)) {
        return ERRCODE_NV_INVALID_PARAM;
    }

#if (CONFIG_NV_SUPPORT_ENCRYPT != NV_YES)
    if ((attr != NULL && attr->encrypted == true)) {
        nv_log_err("[NV] nv_direct_write_with_attr: encryption not support!\r\n");
        return ERRCODE_NV_ILLEGAL_OPERATION;
    }
#endif

#ifdef KV_DEBUG
    uint64_t start_time = uapi_systick_get_ms();
#endif

#if (CONFIG_NV_SUPPORT_ASYNCHRONOUS_STORE == NV_YES)
    ret = nv_async_write_with_attr(store, key, kvalue, kvalue_length, attr, func);
#else
    ret = nv_direct_write_with_attr(store, key, kvalue, kvalue_length, attr, func);
#endif

#ifdef KV_DEBUG
    uint64_t end_time = uapi_systick_get_ms();
    if (ret != ERRCODE_SUCC) {
        nv_log_err("[NV] write key fail! key_id= 0x%x, ret 0x%x\r\n", key, ret);
    }

    nv_log_debug("[NV] write key (key_id = 0x%x len = %d) ret = %d. takes %lld ms\r\n",
        key, kvalue_length, ret, (end_time - start_time));
    unused(start_time);
    unused(end_time);
#endif
    return ret;
}

errcode_t uapi_nv_extra_write(kv_store_t store, uint16_t key, const uint8_t *kvalue, uint16_t kvalue_length)
{
    errcode_t err = uapi_nv_extra_write_with_attr(store, key, kvalue, kvalue_length, NULL, NULL);
#ifdef CONFIG_ERRCODE_SUPPORT_REPORT
    uapi_errcode_invoke_report((uint8_t)ERRCODE_MODULE_NV, err);
#endif
    return err;
}

errcode_t uapi_nv_extra_write_force(kv_store_t store, uint16_t key, const uint8_t *kvalue, uint16_t kvalue_length)
{
    if ((store >= KV_STORE_MAX_NUM) || (kvalue == NULL) || (kvalue_length == 0)) {
        return ERRCODE_NV_INVALID_PARAM;
    }
#ifdef CONFIG_NV_SUPPORT_WRITE_FORCE
    return nv_direct_write_force(store, key, kvalue, kvalue_length);
#endif
    unused(store);
    unused(key);
    return ERRCODE_NOT_SUPPORT;
}

errcode_t uapi_nv_extra_update_key_attr(kv_store_t store, uint16_t key, nv_key_attr_t *attr, nv_storage_completed_callback func)
{
    if ((store >= KV_STORE_MAX_NUM) || (attr == NULL)) {
        return ERRCODE_NV_INVALID_PARAM;
    }
#ifdef CONFIG_NV_SUPPORT_UPDATE_ATTR
    return nv_direct_update_key_attr(store, key, attr, func);
#endif
    unused(store);
    unused(key);
    unused(func);
    return ERRCODE_NOT_SUPPORT;
}

errcode_t uapi_nv_extra_get_key_attr(kv_store_t store, uint16_t key, uint16_t *length, nv_key_attr_t *attr)
{
    if ((store >= KV_STORE_MAX_NUM) || (attr == NULL)) {
        return ERRCODE_NV_INVALID_PARAM;
    }
    return nv_direct_get_key_attr(store, key, length, attr);
}

errcode_t uapi_nv_extra_read_with_attr(kv_store_t store, uint16_t key, uint16_t kvalue_max_length, uint16_t *kvalue_length,
                                       uint8_t *kvalue, nv_key_attr_t *attr)
{
    errcode_t ret;
    if ((store >= KV_STORE_MAX_NUM) || (kvalue_length == NULL) || (kvalue == NULL) || (attr == NULL)) {
        return ERRCODE_NV_INVALID_PARAM;
    }

#ifdef KV_DEBUG
    uint64_t start_time = uapi_systick_get_ms();
#endif

#if (CONFIG_NV_SUPPORT_ASYNCHRONOUS_STORE == NV_YES)
    ret = nv_async_read_with_attr(store, key, kvalue_max_length, kvalue_length, kvalue, attr);
#else
    ret = nv_direct_get_key_data(store, key, kvalue_max_length, kvalue_length, kvalue, attr);
#endif

#ifdef KV_DEBUG
    uint64_t end_time = uapi_systick_get_ms();
    if (ret != ERRCODE_SUCC) {
        nv_log_err("[NV] read key fail! key_id= 0x%x, ret 0x%x\r\n", key, ret);
    }
    nv_log_debug("[NV] read key (key_id = 0x%x len = %d) ret = 0x%x. takes %lld ms\r\n",
        key, *kvalue_length, ret, (end_time - start_time));
    unused(start_time);
    unused(end_time);
#endif
    return ret;
}

errcode_t uapi_nv_extra_read(kv_store_t store, uint16_t key, uint16_t kvalue_max_length, uint16_t *kvalue_length, uint8_t *kvalue)
{
    nv_key_attr_t attr = {0};
    errcode_t err = uapi_nv_extra_read_with_attr(store, key, kvalue_max_length, kvalue_length, kvalue, &attr);
#ifdef CONFIG_ERRCODE_SUPPORT_REPORT
    uapi_errcode_invoke_report((uint8_t)ERRCODE_MODULE_NV, err);
#endif
    return err;
}

errcode_t uapi_nv_extra_delete_key(kv_store_t store, uint16_t key, bool completely)
{
#if (defined(CONFIG_NV_SUPPORT_DELETE_KEY) && (CONFIG_NV_SUPPORT_DELETE_KEY == NV_YES))
    if (store >= KV_STORE_MAX_NUM) {
        return ERRCODE_NV_INVALID_PARAM;
    }

    errcode_t ret;
#ifdef KV_DEBUG
    uint64_t start_time = uapi_systick_get_ms();
#endif
    ret = nv_direct_erase(store, key, completely);
#ifdef KV_DEBUG
    uint64_t end_time = uapi_systick_get_ms();
    if (ret != ERRCODE_SUCC) {
        nv_log_err("[NV] Delete key fail! key_id= 0x%x, ret 0x%x\r\n", key, ret);
    }
    nv_log_debug("[NV] Delete key (key_id = 0x%x) ret = 0x%x. takes %d ms\r\n",
        key, ret, (uint32_t)(end_time - start_time));
    unused(start_time);
    unused(end_time);
#endif
    return ret;
#else
    unused(store);
    unused(key);
    unused(completely);
    return ERRCODE_NOT_SUPPORT;
#endif /* (defined(CONFIG_NV_SUPPORT_DELETE_KEY) && (CONFIG_NV_SUPPORT_DELETE_KEY == NV_YES) */
}

bool uapi_nv_extra_is_stored(kv_store_t store, uint16_t key, uint16_t kvalue_length, const uint8_t *kvalue)
{
    if ((store >= KV_STORE_MAX_NUM) || (kvalue_length == 0) || (kvalue == NULL)) {
        return false;
    }

    errcode_t ret = nv_direct_stored(store, key, kvalue_length, kvalue);
    if (ret == ERRCODE_SUCC) {
        return true;
    } else {
        return false;
    }
}

errcode_t uapi_nv_extra_get_store_status(kv_store_t store, nv_store_status_t *status)
{
    if ((store >= KV_STORE_MAX_NUM) || (status == NULL)) {
        return ERRCODE_NV_INVALID_PARAM;
    }

#ifdef KV_DEBUG
    uint64_t start_time = uapi_systick_get_ms();
#endif
    errcode_t ret = nv_direct_get_store_status(store, status);
#ifdef KV_DEBUG
    uint64_t end_time = uapi_systick_get_ms();
    nv_log_debug("[NV] get store status take %lld ms\r\n", (end_time - start_time));
    unused(start_time);
    unused(end_time);
#endif
    return ret;
}

errcode_t uapi_nv_extra_backup(kv_store_t store, const nv_backup_mode_t *backup_mode)
{
#if (CONFIG_NV_SUPPORT_BACKUP_RESTORE == NV_YES)
    if ((store >= KV_STORE_MAX_NUM) || (backup_mode == NULL)) {
        return ERRCODE_NV_INVALID_PARAM;
    }
    return nv_direct_backup_keys(store, backup_mode);
#else
    unused(store);
    unused(backup_mode);
    nv_log_err("uapi_nv_extra_backup not sup\n");
    return ERRCODE_NOT_SUPPORT;
#endif
}

errcode_t uapi_nv_extra_set_restore_mode_all(kv_store_t store)
{
#if (CONFIG_NV_SUPPORT_BACKUP_RESTORE == NV_YES)
    if (store >= KV_STORE_MAX_NUM) {
        return ERRCODE_NV_INVALID_PARAM;
    }
    return nv_direct_set_restore_flag_all(store);
#else
    unused(store);
    return ERRCODE_NOT_SUPPORT;
#endif
}

errcode_t uapi_nv_extra_set_restore_mode_partitial(kv_store_t store, const nv_restore_mode_t *restore_mode)
{
#if (CONFIG_NV_SUPPORT_BACKUP_RESTORE == NV_YES)
    if ((store >= KV_STORE_MAX_NUM) || (restore_mode == NULL)) {
        return ERRCODE_NV_INVALID_PARAM;
    }
    return nv_direct_set_restore_flag_partitial(store, restore_mode);
#else
    unused(store);
    unused(restore_mode);
    return ERRCODE_NOT_SUPPORT;
#endif
}
errcode_t uapi_nv_extra_register_change_notify_proc(kv_store_t store, uint16_t min_key, uint16_t max_key, nv_changed_notify_func func)
{
    if ((store >= KV_STORE_MAX_NUM) || (min_key > max_key) || (func == NULL)) {
        return ERRCODE_NV_INVALID_PARAM;
    }

#if (CONFIG_NV_SUPPORT_CHANGE_NOTIFY == NV_YES)
    return nv_direct_add_func_to_notify_list(store, min_key, max_key, func);
#endif
    return ERRCODE_NOT_SUPPORT;
}

errcode_t uapi_nv_extra_flush(kv_store_t store)
{
    if (store >= KV_STORE_MAX_NUM) {
        return ERRCODE_NV_INVALID_PARAM;
    }
#if (CONFIG_NV_SUPPORT_ASYNCHRONOUS_STORE == NV_YES)
    return nv_async_flush(store);
#else
    unused(store);
    return ERRCODE_NOT_SUPPORT;
#endif
}