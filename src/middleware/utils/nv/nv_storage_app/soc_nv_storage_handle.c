/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2016-2022. All rights reserved.
 * Description: SOC KEY VALUE STORAGE IMPLEMENTATION
 */

#include "nv.h"
#include "nv_storage.h"
#include "nv_store.h"
#include "nv_upg.h"
#include "securec.h"
#include "nv_porting.h"
#include "common_def.h"
#include "nv_reset.h"
#include "nv_async_store.h"
#include "osal_semaphore.h"
#include "nv_update.h"
#include "nv_notify.h"
#include "nv_task_adapt.h"
#include "nv_storage_handle.h"
#if defined(CONFIG_NV_SUPPORT_KEY_INDEX_MAP) && (CONFIG_NV_SUPPORT_KEY_INDEX_MAP == NV_YES)
#include "nv_hash.h"
#endif
osal_semaphore nv_sem = {0};
#define MAX_BINARY_VAL 1

static bool g_nv_store_inited[KV_STORE_MAX_NUM] = {false};

bool nv_store_is_ready(kv_store_t store)
{
    return g_nv_store_inited[store];
}

STATIC kv_attributes_t nv_helper_convert_key_attr(const nv_key_attr_t *attr)
{
    kv_attributes_t attribute = KV_ATTRIBUTE_NORMAL;
    bool is_support_crypt = false;

    if (attr == NULL) {
        return attribute;
    }

#if (CONFIG_NV_SUPPORT_ENCRYPT == NV_YES)
    is_support_crypt = attr->encrypted;
#endif

    if (attr->permanent && is_support_crypt) {
        attribute = KV_ATTRIBUTE_ENCRYPTED | KV_ATTRIBUTE_PERMANENT | KV_ATTRIBUTE_NON_UPGRADE;
    } else if (attr->permanent) {
        attribute = KV_ATTRIBUTE_PERMANENT | KV_ATTRIBUTE_NON_UPGRADE;
    } else if (is_support_crypt) {
        attribute = KV_ATTRIBUTE_ENCRYPTED | KV_ATTRIBUTE_NON_UPGRADE;
    } else if (attr->non_upgrade) {
        attribute = KV_ATTRIBUTE_NON_UPGRADE;
    }
    return attribute;
}

STATIC void nv_helper_rev_convert_key_attr(kv_attributes_t attribute, nv_key_attr_t *attr)
{
    if (((uint32_t)attribute & KV_ATTRIBUTE_PERMANENT) != 0) {
        attr->permanent = true;
        attr->non_upgrade = true;
    }
#if (CONFIG_NV_SUPPORT_ENCRYPT == NV_YES)
    if (((uint32_t)attribute & KV_ATTRIBUTE_ENCRYPTED) != 0) {
        attr->encrypted = true;
        attr->non_upgrade = true;
    }
#endif
    if (((uint32_t)attribute & KV_ATTRIBUTE_NON_UPGRADE) != 0) {
        attr->non_upgrade = true;
    }
}

static uint16_t nv_storage_max_key_space(const nv_key_attr_t *attr)
{
    if (attr == NULL) {
        return NV_NORMAL_KVALUE_MAX_LEN;
    }
    return (attr->encrypted ? NV_ENCRYPTED_KVALUE_MAX_LEN : NV_NORMAL_KVALUE_MAX_LEN);
}

#if (CONFIG_NV_SUPPORT_CHANGE_NOTIFY == NV_YES)
static nv_direct_ctrl_t g_nv_direct_ctrl[KV_STORE_MAX_NUM] = {0};
nv_direct_ctrl_t *nv_direct_get_nv_ctrl(kv_store_t store)
{
    return &g_nv_direct_ctrl[store];
}
#endif

errcode_t nv_direct_get_key_attr(kv_store_t store, uint16_t key_id, uint16_t *len, nv_key_attr_t *attr)
{
    kv_attributes_t get_attribute = 0;
    errcode_t ret;

    if ((len == NULL) || (attr == NULL)) {
        return ERRCODE_NV_INVALID_PARAM;
    }
    if (!nv_store_is_ready(store)) {
        return ERRCODE_NV_NOT_INIT;
    }

    if (osal_sem_down_timeout(&nv_sem, 0xFFFFFFFFU) != ERRCODE_SUCC) {
        return ERRCODE_NV_SEM_WAIT_ERR;
    }

    *len = 0;
    (void)memset_s(attr, sizeof(nv_key_attr_t), 0, sizeof(nv_key_attr_t));
    ret = kv_store_get_key_attr(store, key_id, len, &get_attribute);
    if (ret == ERRCODE_SUCC) {
        nv_helper_rev_convert_key_attr(get_attribute, attr);
    }

    osal_sem_up(&nv_sem);
    return ret;
}
errcode_t nv_direct_write_with_attr(kv_store_t store, uint16_t key_id, const uint8_t *kvalue,
                                    uint16_t kvalue_length, nv_key_attr_t *attr,
                                    nv_storage_completed_callback func)
{
    errcode_t ret;
    flash_task_node task_node = {0};
    kv_attributes_t attribute;
    (void)func;

    if (!nv_store_is_ready(store)) {
        return ERRCODE_NV_NOT_INIT;
    }
    if (kvalue_length > nv_storage_max_key_space(attr)) {
        return ERRCODE_NV_NO_ENOUGH_SPACE;
    }

    if (osal_sem_down_timeout(&nv_sem, 0xFFFFFFFFU) != ERRCODE_SUCC) {
        return ERRCODE_NV_SEM_WAIT_ERR;
    }

    attribute = nv_helper_convert_key_attr(attr);

    task_node.state_code = FLASH_TASK_READY;
    task_node.data.kv.key = key_id;
    task_node.data.kv.kvalue = kvalue;
    task_node.data.kv.kvalue_length = kvalue_length;
    task_node.data.kv.write_type = (attr == NULL) ? NV_OP_WRITE_NO_ATTR : NV_OP_WRITE_WITH_ATTR;
    task_node.data.kv.attribute = attribute;

    ret = kv_update_write_key(store, &task_node);
    osal_sem_up(&nv_sem);

    if (ret != ERRCODE_SUCC) {
        return ret;
    }

#if (CONFIG_NV_SUPPORT_CHANGE_NOTIFY == NV_YES)
    nv_change_notify(store, key_id);
#endif
    return ERRCODE_SUCC;
}

#ifdef CONFIG_NV_SUPPORT_WRITE_FORCE
errcode_t nv_direct_write_force(kv_store_t store, uint16_t key_id, const uint8_t *kvalue, uint16_t kvalue_length)
{
    errcode_t ret;
    flash_task_node task_node = {0};

    if (!nv_store_is_ready(store)) {
        return ERRCODE_NV_NOT_INIT;
    }
    if (kvalue_length > NV_NORMAL_KVALUE_MAX_LEN) {
        return ERRCODE_NV_NO_ENOUGH_SPACE;
    }
    if (osal_sem_down_timeout(&nv_sem, 0xFFFFFFFFU) != ERRCODE_SUCC) {
        return ERRCODE_NV_SEM_WAIT_ERR;
    }

    task_node.state_code = FLASH_TASK_READY;
    task_node.data.kv.key = key_id;
    task_node.data.kv.kvalue = kvalue;
    task_node.data.kv.kvalue_length = kvalue_length;
    task_node.data.kv.write_type = NV_OP_WRITE_FORCE;
    ret = kv_update_write_key(store, &task_node);
    osal_sem_up(&nv_sem);
#if (CONFIG_NV_SUPPORT_CHANGE_NOTIFY == NV_YES)
    if (ret == ERRCODE_SUCC) {
        nv_change_notify(store, key_id);
    }
#endif
    return ret;
}
#endif

errcode_t nv_direct_get_key_data(kv_store_t store, uint16_t key_id, uint16_t kvalue_max_length,
                                 uint16_t *kvalue_length, uint8_t *kvalue, nv_key_attr_t *attr)
{
    kv_attributes_t data_attribute = 0;
    kv_store_key_data_t key_data;
    errcode_t ret_val;

    if (!nv_store_is_ready(store)) {
        return ERRCODE_NV_NOT_INIT;
    }

    if (osal_sem_down_timeout(&nv_sem, 0xFFFFFFFFU) != ERRCODE_SUCC) {
        return ERRCODE_NV_SEM_WAIT_ERR;
    }

    key_data.kvalue_max_length = kvalue_max_length;
    key_data.kvalue_actual_length = 0;
    key_data.kvalue = kvalue;

    ret_val = kv_store_get_key(store, key_id, &key_data, &data_attribute);

#if (CONFIG_NV_SUPPORT_BACKUP_RESTORE == NV_YES)
    bool is_do_backup = false;
    if (ret_val != ERRCODE_SUCC) {
        if (kv_store_read_backup_key(store, key_id, &key_data, &data_attribute) == ERRCODE_SUCC) {
            is_do_backup = true;
            ret_val = ERRCODE_SUCC;
        }
    }
#endif

    (void)memset_s(attr, sizeof(nv_key_attr_t), 0, sizeof(nv_key_attr_t));
    if (ret_val == ERRCODE_SUCC) {
        nv_helper_rev_convert_key_attr(data_attribute, attr);
        *kvalue_length = key_data.kvalue_actual_length;
    }

    osal_sem_up(&nv_sem);

#if (CONFIG_NV_SUPPORT_BACKUP_RESTORE == NV_YES)
    if (is_do_backup) {
        (void)nv_direct_write_with_attr(store, key_id, key_data.kvalue,
                                        key_data.kvalue_actual_length, attr, NULL);
    }
#endif

    return ret_val;
}

#ifdef CONFIG_NV_SUPPORT_UPDATE_ATTR
errcode_t nv_direct_update_key_attr(kv_store_t store, uint16_t key_id, nv_key_attr_t *attr, nv_storage_completed_callback func)
{
    errcode_t ret_val;
    flash_task_node task_node = {0};
    nv_key_attr_t new_attr;
    unused(func);

    if (!nv_store_is_ready(store)) {
        return ERRCODE_NV_NOT_INIT;
    }

    memcpy_s(&new_attr, sizeof(nv_key_attr_t), attr, sizeof(nv_key_attr_t));
    if (osal_sem_down_timeout(&nv_sem, 0xFFFFFFFF) != ERRCODE_SUCC) {
        return ERRCODE_NV_SEM_WAIT_ERR;
    }

    task_node.state_code = FLASH_TASK_READY;
    task_node.data.kv_attribute.key = key_id;
    task_node.data.kv_attribute.attribute = (uint8_t)nv_helper_convert_key_attr(&new_attr);
    ret_val = kv_update_modify_attribute(store, &task_node);
    osal_sem_up(&nv_sem);
#if (CONFIG_NV_SUPPORT_CHANGE_NOTIFY == NV_YES)
    if (ret_val == ERRCODE_SUCC) {
        nv_change_notify(store, key_id);
    }
#endif
    return ret_val;
}
#endif

#if (defined(CONFIG_NV_SUPPORT_DELETE_KEY) && (CONFIG_NV_SUPPORT_DELETE_KEY == NV_YES))
errcode_t nv_direct_erase(kv_store_t store, uint16_t key_id, bool completely)
{
    errcode_t ret_val;
    flash_task_node task_node = {0};

    if (!nv_store_is_ready(store)) {
        return ERRCODE_NV_NOT_INIT;
    }

    if (osal_sem_down_timeout(&nv_sem, 0xFFFFFFFFU) != ERRCODE_SUCC) {
        return ERRCODE_NV_SEM_WAIT_ERR;
    }

    task_node.data.kv_erase.key = key_id;
    task_node.data.kv_erase.completely = completely;
    ret_val = kv_update_erase_key(store, &task_node);

    osal_sem_up(&nv_sem);

#if (CONFIG_NV_SUPPORT_CHANGE_NOTIFY == NV_YES)
    if (ret_val == ERRCODE_SUCC) {
        nv_change_notify(store, key_id);
    }
#endif

    return ret_val;
}
#endif

errcode_t nv_direct_get_store_status(kv_store_t store, nv_store_status_t *status)
{
    errcode_t ret_val;

    if (!nv_store_is_ready(store)) {
        return ERRCODE_NV_NOT_INIT;
    }

    if (osal_sem_down_timeout(&nv_sem, 0xFFFFFFFFU) != ERRCODE_SUCC) {
        return ERRCODE_NV_SEM_WAIT_ERR;
    }

    (void)memset_s(status, sizeof(nv_store_status_t), 0, sizeof(nv_store_status_t));
    ret_val = kv_store_get_status(store, status);

    osal_sem_up(&nv_sem);
    return ret_val;
}

errcode_t nv_direct_stored(kv_store_t store, uint16_t key_id, uint16_t kvalue_length, const uint8_t *kvalue)
{
    errcode_t ret = ERRCODE_SUCC;
    uint8_t *stored_kvalue;
    kv_attributes_t attribute = 0;
    kv_store_key_data_t key_data;

    if (!nv_store_is_ready(store)) {
        return ERRCODE_NV_NOT_INIT;
    }
    if (kvalue_length > NV_NORMAL_KVALUE_MAX_LEN) {
        return ERRCODE_NV_NO_ENOUGH_SPACE;
    }

    stored_kvalue = (uint8_t *)kv_malloc(kvalue_length);
    if (stored_kvalue == NULL) {
        return ERRCODE_MALLOC;
    }

    if (osal_sem_down_timeout(&nv_sem, 0xFFFFFFFFU) != ERRCODE_SUCC) {
        kv_free(stored_kvalue);
        return ERRCODE_NV_SEM_WAIT_ERR;
    }

    key_data.kvalue_max_length = kvalue_length;
    key_data.kvalue_actual_length = 0;
    key_data.kvalue = stored_kvalue;

    if (kv_store_get_key(store, key_id, &key_data, &attribute) != ERRCODE_SUCC) {
        ret = ERRCODE_FAIL;
    } else if ((key_data.kvalue_actual_length != kvalue_length) ||
               (memcmp(stored_kvalue, kvalue, kvalue_length) != 0)) {
        ret = ERRCODE_FAIL;
    }

    osal_sem_up(&nv_sem);
    kv_free(stored_kvalue);
    return ret;
}

#if (CONFIG_NV_SUPPORT_BACKUP_RESTORE == NV_YES)
errcode_t nv_direct_backup_keys(kv_store_t store, const nv_backup_mode_t *backup_mode)
{
    errcode_t ret;

    if (!nv_store_is_ready(store)) {
        return ERRCODE_NV_NOT_INIT;
    }
#if (CONFIG_NV_SUPPORT_ASYNCHRONOUS_STORE == NV_YES)
    /* 保证备份前，NV消息队列中数据写入flash */
    ret = uapi_nv_flush();
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
#endif
    if (!kv_store_backup_enabled(store)) {
        return ERRCODE_NOT_SUPPORT;
    }

    if (osal_sem_down_timeout(&nv_sem, 0xFFFFFFFFU) != ERRCODE_SUCC) {
        return ERRCODE_NV_SEM_WAIT_ERR;
    }

    ret = kv_restore_set_region_flag(store, backup_mode->region_mode);
    if (ret != ERRCODE_SUCC) {
        osal_sem_up(&nv_sem);
        return ret;
    }

    ret = kv_backup_keys(store, backup_mode);
    osal_sem_up(&nv_sem);
    return ret;
}

errcode_t nv_direct_set_restore_flag_all(kv_store_t store)
{
    nv_reset_mode_t nv_reset_mode = {0};

    if (!nv_store_is_ready(store)) {
        return ERRCODE_NV_NOT_INIT;
    }
    if (!kv_store_backup_enabled(store)) {
        return ERRCODE_NOT_SUPPORT;
    }

    nv_reset_mode.mode = RESET_MODE_A;
    return kv_enable_restore_flag(store, &nv_reset_mode);
}

errcode_t nv_direct_set_restore_flag_partitial(kv_store_t store, const nv_restore_mode_t *nv_restore_mode)
{
    if (!nv_store_is_ready(store)) {
        return ERRCODE_NV_NOT_INIT;
    }
    if (!kv_store_backup_enabled(store)) {
        return ERRCODE_NOT_SUPPORT;
    }
    nv_reset_mode_t nv_reset_mode = {0};
    if (memcpy_s(nv_reset_mode.region_flag, KEY_ID_REGION_MAX_NUM, nv_restore_mode, KEY_ID_REGION_MAX_NUM) != EOK) {
        return ERRCODE_MEMCPY;
    }
    nv_reset_mode.mode = RESET_MODE_B;
    return kv_enable_restore_flag(store, &nv_reset_mode);
}
#endif /* CONFIG_NV_SUPPORT_BACKUP_RESTORE */

#if (CONFIG_NV_SUPPORT_CHANGE_NOTIFY == NV_YES)
errcode_t nv_direct_add_func_to_notify_list(kv_store_t store, uint16_t min_key,
                                            uint16_t max_key, nv_changed_notify_func func)
{
    nv_direct_ctrl_t *nv_ctrl = nv_direct_get_nv_ctrl(store);

    if (osal_sem_down_timeout(&nv_sem, 0xFFFFFFFFU) != ERRCODE_SUCC) {
        return ERRCODE_NV_SEM_WAIT_ERR;
    }

    nv_changed_proc_t *notify_list = nv_ctrl->nv_change_notify_list;
    if ((!nv_store_is_ready(store)) || (notify_list == NULL)) {
        osal_sem_up(&nv_sem);
        return ERRCODE_NV_NOT_INIT;
    }

    if (nv_ctrl->notify_registered_nums >= nv_ctrl->notify_regitser_max_nums) {
        osal_sem_up(&nv_sem);
        return ERRCODE_NV_NOTIFY_LIST_FULL;
    }

    if (!nv_change_notify_segment_is_valid(nv_ctrl, min_key, max_key)) {
        osal_sem_up(&nv_sem);
        return ERRCODE_NV_NOTIFY_SEGMENT_ERR;
    }

    uint8_t new_index = nv_ctrl->notify_registered_nums;
    notify_list[new_index].min_key = min_key;
    notify_list[new_index].max_key = max_key;
    notify_list[new_index].func = func;
    nv_ctrl->notify_registered_nums++;

    osal_sem_up(&nv_sem);
    return ERRCODE_SUCC;
}
#endif /* CONFIG_NV_SUPPORT_CHANGE_NOTIFY */

errcode_t nv_direct_ctrl_init(kv_store_t store)
{
    if (g_nv_store_inited[store]) {
        return ERRCODE_SUCC;
    }
    if (nv_sem.sem == NULL) {
        if (osal_sem_binary_sem_init(&nv_sem, MAX_BINARY_VAL) != ERRCODE_SUCC) {
            return ERRCODE_FAIL;
        }
    }
#if defined(CONFIG_NV_SUPPORT_KEY_INDEX_MAP) && (CONFIG_NV_SUPPORT_KEY_INDEX_MAP == NV_YES)
    (void)nv_hash_init(store);
#endif
    (void)kv_update_init(store);

#if (CONFIG_NV_SUPPORT_CHANGE_NOTIFY == NV_YES)
    nv_direct_ctrl_t *nv_ctrl = nv_direct_get_nv_ctrl(store);
    (void)memset_s(nv_ctrl, sizeof(nv_direct_ctrl_t), 0, sizeof(nv_direct_ctrl_t));
    (void)nv_direct_notify_list_init(store);
#endif

#if (CONFIG_NV_SUPPORT_BACKUP_RESTORE == NV_YES)
    (void)kv_update_backup_init(store);
#endif

#if (CONFIG_NV_SUPPORT_ASYNCHRONOUS_STORE == NV_YES)
    (void)nv_async_init(store, kv_store_get_page_count(store));
#endif

    g_nv_store_inited[store] = true;
    nv_log_info("nv init success! store=%d\r\n", store);

#if (CONFIG_NV_SUPPORT_BACKUP_RESTORE == NV_YES)
    if (kv_store_backup_enabled(store)) {
        (void)kv_restore_all_keys(store);
    }
#endif
    return ERRCODE_SUCC;
}
