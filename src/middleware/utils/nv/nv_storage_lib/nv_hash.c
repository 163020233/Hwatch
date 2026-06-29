/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: NV Key Write And Read Used Hash
 */

#include "nv_config.h"
#if defined(CONFIG_NV_SUPPORT_KEY_INDEX_MAP) && (CONFIG_NV_SUPPORT_KEY_INDEX_MAP == NV_YES)
#include "cstl_hash.h"
#include "nv_key.h"
#include "nv_porting.h"
#include "nv_nvregion.h"
#include "nv_storage_handle.h"
#include "nv_hash.h"

static CstlHash *g_nv_hash_table[KV_STORE_MAX_NUM] = {0};
static bool g_nv_hash_table_inited[KV_STORE_MAX_NUM] = {0};

void nv_hash_table_build_complete(kv_store_t store)
{
    if (store >= KV_STORE_MAX_NUM) {
        return;
    }

    if (!g_nv_hash_table_inited[store]) {
        g_nv_hash_table_inited[store] = true;
    }
}

/* 把绝对地址转换为绝对地址 */
static bool set_key_location_offset(kv_store_t store, uint32_t abs_location, uint32_t *relative_location)
{
    kv_nvregion_area_info_t area;

    if ((store >= KV_STORE_MAX_NUM) || (relative_location == NULL)) {
        return false;
    }

    if (kv_nvregion_get_area_from_store(store, &area) != ERRCODE_SUCC) {
        return false;
    }

    if (abs_location < area.nv_addr) {
        return false;
    }

    if (abs_location >= (area.nv_addr + area.nv_size)) {
        return false;
    }

    *relative_location = abs_location - area.nv_addr;
    return true;
}

/* 从相对地址还原绝对地址 */
static bool get_key_location_offset(kv_store_t store, uint32_t relative_location, uint32_t *abs_location)
{
    kv_nvregion_area_info_t area;

    if ((store >= KV_STORE_MAX_NUM) || (abs_location == NULL)) {
        return false;
    }

    if (kv_nvregion_get_area_from_store(store, &area) != ERRCODE_SUCC) {
        return false;
    }

    *abs_location = area.nv_addr + relative_location;
    return true;
}

static bool handle_convert_hash_value(kv_store_t store, const kv_key_handle_t *handle, nv_key_hash_t *value)
{
    if ((store >= KV_STORE_MAX_NUM) || (handle == NULL) || (value == NULL)) {
        return false;
    }
    memset_s(value, sizeof(*value), 0, sizeof(*value));
    uint32_t relative_location;
    if (!set_key_location_offset(store, handle->key_location, &relative_location)) {
        return false;
    }
    value->key_location = relative_location;

    return true;
}

static CstlHash *nv_hash_get_table(kv_store_t store)
{
    if (store >= KV_STORE_MAX_NUM) {
        return NULL;
    }
    return g_nv_hash_table[store];
}

bool nv_hash_init(kv_store_t store)
{
    if (store >= KV_STORE_MAX_NUM) {
        return false;
    }
    if (g_nv_hash_table[store] != NULL) {
        return false;
    }
    CstlDupFreeFuncPair value_func = {NULL, kv_free};
    nv_store_cfg_t *cfg = nv_get_store_cfg(store);
    if (cfg == NULL) {
        return false;
    }
    g_nv_hash_table[store] = CstlHashCreate(cfg->hash_bucket_size, NULL, NULL, NULL, &value_func);
    if (g_nv_hash_table[store] == NULL) {
        return false;
    }
    return true;
}

/* 查看是否存在key */
bool nv_hash_key_exist(kv_store_t store, kv_key_id key_id)
{
    CstlHash *table = nv_hash_get_table(store);
    if (table == NULL) {
        return false;
    }

    uintptr_t value;
    return (CstlHashAt(table, key_id, &value) == CSTL_OK);
}

/* 哈希查找到value, 返回kv_key_handle_t */
errcode_t nv_hash_key_find(kv_store_t store, kv_key_id key_id, kv_key_handle_t *out_key)
{
    CstlHash *table = nv_hash_get_table(store);
    if (table == NULL) {
        return ERRCODE_NV_NOT_INIT;
    }

    uintptr_t value;
    if (CstlHashAt(table, key_id, &value) != CSTL_OK) {
        return ERRCODE_NV_KEY_NOT_FOUND;
    }

    nv_key_hash_t *cur_key = (nv_key_hash_t *)value;
    uint32_t key_location;
    if (!get_key_location_offset(store, cur_key->key_location, &key_location)) {
        return ERRCODE_NV_KEY_NOT_FOUND;
    }

    return kv_key_get_handle_from_location(key_location, out_key);
}

/*  插入或者更新key */
bool nv_hash_key_put(kv_store_t store, kv_key_id key_id, const kv_key_handle_t *key_handle)
{
    CstlHash *table = nv_hash_get_table(store);
    if (table == NULL || key_handle == NULL) {
        return false;
    }

    if (nv_hash_key_exist(store, key_id)) {
        (void)CstlHashErase(table, key_id);
    }

    nv_key_hash_t *cur_key = (nv_key_hash_t *)kv_malloc(sizeof(nv_key_hash_t));
    if (cur_key == NULL) {
        return false;
    }

    if (memset_s(cur_key, sizeof(nv_key_hash_t), 0, sizeof(nv_key_hash_t)) != EOK) {
        kv_free(cur_key);
        return false;
    }

    if (!handle_convert_hash_value(store, key_handle, cur_key)) {
        kv_free(cur_key);
        return false;
    }

    return (CstlHashInsert(table, key_id, sizeof(key_id),
                           (uintptr_t)cur_key, sizeof(nv_key_hash_t)) == CSTL_OK);
}

/* 删除指定key */
void nv_hash_key_erase(kv_store_t store, kv_key_id key_id)
{
    CstlHash *table = nv_hash_get_table(store);
    if (table == NULL) {
        return;
    }

    (void)CstlHashErase(table, key_id);
}

void nv_hash_key_clear(kv_store_t store)
{
    osal_kthread_lock();
    (void)CstlHashClear(g_nv_hash_table[store]);
    osal_kthread_unlock();
}

size_t nv_hash_key_num(kv_store_t store)
{
    return CstlHashSize(g_nv_hash_table[store]);
}

/* 遍历哈希表，获取指定页的所有keyId */
bool nv_hash_collect_keys_by_page(kv_store_t store, kv_page_location page_location,
                                  uint16_t **out_keys, size_t *out_count)
{
    CstlHash *table = nv_hash_get_table(store);
    if (table == NULL || out_keys == NULL || out_count == NULL) {
        return false;
    }

    *out_keys = NULL;
    *out_count = 0;

    size_t cnt = 0;
    uint32_t key_location;
    nv_key_hash_t *info;
    CstlHashIterator iter = CstlHashIterBegin(table);
    CstlHashIterator end = CstlHashIterEnd(table);

    while (iter != end) {
        uintptr_t value = CstlHashIterValue(table, iter);
        info = (nv_key_hash_t *)value;
        if ((info != NULL) && get_key_location_offset(store, info->key_location, &key_location)) {
            if ((key_location > page_location) && ((key_location - page_location) < KV_PAGE_SIZE)) {
                cnt++;
            }
        }
        iter = CstlHashIterNext(table, iter);
    }

    if (cnt == 0) {
        return true;
    }

    uint16_t *keys = (uint16_t *)kv_malloc(cnt * sizeof(uint16_t));
    if (keys == NULL) {
        return false;
    }
    (void)memset_s(keys, cnt * sizeof(uint16_t), 0, cnt * sizeof(uint16_t));

    size_t idx = 0;
    iter = CstlHashIterBegin(table);
    while (iter != end) {
        uintptr_t value = CstlHashIterValue(table, iter);
        info = (nv_key_hash_t *)value;
        if ((info != NULL) && get_key_location_offset(store, info->key_location, &key_location)) {
            if ((key_location > page_location) && ((key_location - page_location) < KV_PAGE_SIZE)) {
                uintptr_t key_id = CstlHashIterKey(table, iter);
                keys[idx++] = (uint16_t)key_id;
            }
        }
        iter = CstlHashIterNext(table, iter);
    }

    *out_keys = keys;
    *out_count = idx;
    return true;
}

/*
 * 完整性校验失败时（且未擦除），设置key为 invalid
 */
void nv_hash_table_init_validate_wrong(kv_key_handle_t *key)
{
    kv_store_t store;
    if (key == NULL) {
        return;
    }
    if (kv_nvregion_get_store_from_location(key->key_location, &store) != ERRCODE_SUCC) {
        return;
    }
    CstlHash *table = nv_hash_get_table(store);
    if (table == NULL) {
        return;
    }

    if (g_nv_hash_table_inited[store] || kv_key_is_erased(key)) {
        return;
    }

    (void)kv_key_set_invalid(key);
}

/*
 * 完整性校验成功时的索引映射处理：
 * - 若哈希中已存在相同 key_id，则将当前 key 标记为 invalid，并增加可回收空间
 * - 将当前 key 放入哈希索引
 */
void nv_hash_table_init_validate_correct(kv_key_handle_t *key, nv_page_status_map_t *status_map, uint32_t key_size)
{
    kv_store_t store;
    kv_page_handle_t cur_page = {0};
    uint32_t cur_seq;

    if (key == NULL || status_map == NULL) {
        return;
    }

    if (kv_nvregion_get_store_from_location(key->key_location, &store) != ERRCODE_SUCC) {
        return;
    }
    if (g_nv_hash_table_inited[store]) {
        return;
    }

    cur_page.page_location = key->key_location & ~(KV_PAGE_SIZE - 1);
    if (kv_nvregion_find_page_by_page_location(&cur_page) != ERRCODE_SUCC) {
        return;
    }
    cur_seq = cur_page.page_header.sequence_number;

    if (nv_hash_key_exist(store, key->header.key_id)) {
        kv_key_handle_t old_key = {0};
        kv_page_handle_t old_page = {0};
        uint32_t old_seq;

        if (nv_hash_key_find(store, key->header.key_id, &old_key) != ERRCODE_SUCC) {
            return;
        }

        old_page.page_location = old_key.key_location & ~(KV_PAGE_SIZE - 1);
        if (kv_nvregion_find_page_by_page_location(&old_page) != ERRCODE_SUCC) {
            return;
        }
        old_seq = old_page.page_header.sequence_number;

        if (cur_seq <= old_seq) {
            if (kv_key_set_invalid(key) != ERRCODE_SUCC) {
                return;
            }
            status_map->reclaimable_space += (uint16_t)key_size;
            return;
        }

        kv_nvregion_map_t *nv_map = kv_nvregion_get_map_from_store(store);
        uint32_t old_page_num = kv_nvregion_get_page_number(old_page.page_location);
        if (kv_key_set_invalid(&old_key) != ERRCODE_SUCC) {
            return;
        }
        nv_map->page_status_map[old_page_num].reclaimable_space += (uint16_t)kv_key_flash_size(&old_key);
    }

    osal_kthread_lock();
    (void)nv_hash_key_put(store, key->header.key_id, key);
    osal_kthread_unlock();
}
#endif