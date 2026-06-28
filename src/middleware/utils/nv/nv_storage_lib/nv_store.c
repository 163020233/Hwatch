/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2022. All rights reserved.
 * Description: KV Storage Library store access module implementation
 */

#include "nv_store.h"
#include "nv_key.h"
#include "nv_page.h"
#include "nv_reset.h"
#include "nv_nvregion.h"
#include "securec.h"
#include "nv_porting.h"
#include "common_def.h"
#include "nv.h"
#if defined(CONFIG_NV_SUPPORT_KEY_INDEX_MAP) && (CONFIG_NV_SUPPORT_KEY_INDEX_MAP == NV_YES)
#include "nv_hash.h"
#endif

#define kv_page_base(a)               ((uint8_t *)(((uint32_t)(a)) & ~(KV_PAGE_SIZE - 1)))
#define kv_is_pointer_in_page(x, p)   ((((uint32_t)(x)) >= ((uint32_t) (p))) && \
                                       (((uint32_t)(x)) < (((uint32_t) (p)) + KV_PAGE_SIZE)))
#define kv_is_pointer_word_aligned(x) (((uint32_t)(x) & 0x3) == 0)
#define kv_bytes_to_words(x)          (((x)+3) >> 2) /* minimal number of words needed to store that many bytes */
#define kv_is_mem_in_page(x, y, p)    ((kv_is_pointer_in_page((x), (p))) && \
                                       (kv_is_pointer_in_page(((uint32_t)(x) + ((uint32_t)(y) - 1)), (p))))

__attribute__((weak)) nv_store_cfg_t *nv_get_store_cfg_table(void)
{
    static nv_store_cfg_t store_cfg[KV_STORE_MAX_NUM] = {
        [KV_STORE_APPLICATION] = {
            .store_id = KV_STORE_ID_ACPU,
            .page_num = KV_STORE_PAGES_ACPU,
#if defined(CONFIG_PARTITION_FEATURE_SUPPORT)
            .partition_id = PARTITION_NV_DATA,
#endif
            .start_addr = KV_STORE_START_ADDR,
            .size = KV_STORE_DATA_SIZE,
#if defined(CONFIG_PARTITION_FEATURE_SUPPORT)
#if defined(CONFIG_NV_SUPPORT_BACKUP_REGION) && (CONFIG_NV_SUPPORT_BACKUP_REGION == NV_YES)
            .backup_partition_id = PARTITION_NV_DATA_BACKUP,
#endif
#endif
            .backup_start_addr = KV_BACKUP_START_ADDR,
            .backup_size = KV_BACKUP_DATA_SIZE,

#if (CONFIG_NV_SUPPORT_KEY_INDEX_MAP == NV_YES)
            .hash_bucket_size = NV_HASH_RW_BACKET_SIZE,
#endif
        },
    };

    return store_cfg;
}

nv_store_cfg_t *nv_get_store_cfg(kv_store_t store)
{
    if (store >= KV_STORE_MAX_NUM) {
        return NULL;
    }
    nv_store_cfg_t *cfg = nv_get_store_cfg_table();
    return &cfg[store];
}

uint16_t kv_store_get_id(kv_store_t store)
{
    nv_store_cfg_t *cfg = nv_get_store_cfg(store);
    if (cfg == NULL) {
        return 0;
    }
    return cfg->store_id;
}

uint8_t kv_store_get_page_count(kv_store_t store)
{
    nv_store_cfg_t *cfg = nv_get_store_cfg(store);
    if (cfg == NULL) {
        return 0;
    }
    return cfg->page_num;
}

kv_store_t kv_store_convert_id(uint16_t store_id)
{
    for (uint8_t i = 0; i < KV_STORE_MAX_NUM; i++) {
        nv_store_cfg_t *cfg = nv_get_store_cfg((kv_store_t)i);
        if ((cfg != NULL) && (cfg->store_id == store_id)) {
            return (kv_store_t)i;
        }
    }
    return KV_STORE_MAX_NUM;
}

errcode_t kv_store_get_page_handle(kv_store_t store, uint32_t page_index, kv_page_handle_t *page)
{
    errcode_t res;
    kv_page_location page_location;

    (void)memset_s(page, sizeof(kv_page_handle_t), 0, sizeof(*page));

    res = kv_nvregion_find_page(store, (uint8_t)page_index, &page_location, &page->page_header);
    if (res != ERRCODE_SUCC) {
        return res;
    }
    page->page_location = page_location;
    return ERRCODE_SUCC;
}

errcode_t kv_store_find_valid_key_from_location(kv_store_t store, kv_key_id key_id, kv_page_handle_t *page,
    kv_key_handle_t *key, kv_key_location start_location)
{
#if defined(CONFIG_NV_SUPPORT_KEY_INDEX_MAP) && (CONFIG_NV_SUPPORT_KEY_INDEX_MAP == NV_YES)
    errcode_t res = nv_hash_key_find(store, key_id, key);
    if (res != ERRCODE_SUCC) {
        return ERRCODE_NV_KEY_NOT_FOUND;
    }

    /* 向下对齐KV_PAGE_SIZE取整,得到page_location */
    kv_page_location page_location = (key->key_location) & ~(KV_PAGE_SIZE - 1);
    page->page_location = page_location;
    res = kv_nvregion_find_page_by_page_location(page);
    return res;
#endif
    uint32_t page_index;
    uint32_t pages_in_store;
    kv_key_filter_t search_filter;

    /* We are looking for the first (and only) valid key in a store */
    search_filter.location = 0;
    search_filter.mask = 0xFFFF;
    search_filter.pattern = key_id;
    search_filter.state = KV_KEY_FILTER_STATE_VALID;
    search_filter.type = KV_KEY_FILTER_TYPE_ANY;
    pages_in_store = kv_store_get_page_count(store);
    for (page_index = 0; page_index < pages_in_store; page_index++) {
        if ((kv_store_get_page_handle(store, page_index, page) == ERRCODE_SUCC) &&
            ((page->page_location >= start_location) || (page->page_location + KV_PAGE_SIZE > start_location)) &&
            (kv_page_find_first_key_from_location(page, &search_filter, key, start_location) == ERRCODE_SUCC)) {
            return ERRCODE_SUCC;
        }
    }
    return ERRCODE_NV_KEY_NOT_FOUND;
}

errcode_t kv_store_find_valid_key(kv_store_t store, kv_key_id key_id, kv_key_handle_t *key)
{
    kv_page_handle_t page;
    return kv_store_find_valid_key_from_location(store, key_id, &page, key, 0);
}

static errcode_t kv_store_find_backup_key(kv_key_id key_id, kv_key_handle_t *key, kv_page_location page_location)
{
    kv_page_handle_t page;
    kv_key_filter_t search_filter;
    page.page_location = page_location;
    /* We are looking for the first (and only) valid key in a store */
    search_filter.location = 0;
    search_filter.mask = 0xFFFF;
    search_filter.pattern = key_id;
    search_filter.state = KV_KEY_FILTER_STATE_VALID;
    search_filter.type = KV_KEY_FILTER_TYPE_ANY;

    if (kv_page_find_first_key(&page, &search_filter, key) ==  ERRCODE_SUCC) {
        return ERRCODE_SUCC;
    }
    return ERRCODE_NV_KEY_NOT_FOUND;
}

errcode_t kv_store_get_key_attr(kv_store_t store, kv_key_id key_id, uint16_t *len, kv_attributes_t *attributes)
{
    errcode_t res = ERRCODE_FAIL;
    kv_key_handle_t key;

    res = kv_store_find_valid_key(store, key_id, &key);
    if (res != ERRCODE_SUCC) {
        return res;
    }

    /* Extract stored attributes from the key */
    *attributes = kv_key_attributes(&key);
    *len = key.header.length;
    return res;
}

errcode_t kv_store_get_backup_key_attr(kv_store_t store, kv_key_id key_id, uint16_t *len, kv_attributes_t *attributes,
    kv_key_handle_t *backup_key)
{
#if (CONFIG_NV_SUPPORT_BACKUP_RESTORE == NV_YES)
    errcode_t res = ERRCODE_FAIL;
    kv_page_location page_location;
    uint32_t page_count;

    if ((store >= KV_STORE_MAX_NUM) || (backup_key == NULL)) {
        return ERRCODE_NV_INVALID_PARAM;
    }
    if (!kv_store_backup_enabled(store)) {
        return ERRCODE_NV_KEY_NOT_FOUND;
    }

    page_count = kv_store_backup_page_count(store);
    for (uint32_t page_num = 0; page_num < page_count; page_num++) {
        res = kv_backup_get_page_location(store, page_num, &page_location);
        if (res != ERRCODE_SUCC) {
            return res;
        }

        res = kv_store_find_backup_key(key_id, backup_key, page_location);
        if (res == ERRCODE_SUCC) {
            break;
        }
    }

    if (res != ERRCODE_SUCC) {
        return res;
    }
    if ((attributes != NULL) && (len != NULL)) {
        *attributes = kv_key_attributes(backup_key);
        *len = backup_key->header.length;
    }

    return res;
#else
    unused(store);
    unused(key_id);
    unused(len);
    unused(attributes);
    unused(backup_key);
    return ERRCODE_SUCC;
#endif
}

errcode_t kv_store_read_backup_key(kv_store_t store, kv_key_id key_id,
                                   kv_store_key_data_t *key_data, kv_attributes_t *attributes)
{
#if (CONFIG_NV_SUPPORT_BACKUP_RESTORE == NV_YES)
    errcode_t res;
    kv_page_location page_location;
    uint32_t page_count;

    if ((store >= KV_STORE_MAX_NUM) || (key_data == NULL) || (attributes == NULL)) {
        return ERRCODE_NV_INVALID_PARAM;
    }
    if (!kv_store_backup_enabled(store)) {
        return ERRCODE_NV_KEY_NOT_FOUND;
    }

    page_count = kv_store_backup_page_count(store);
    for (uint32_t page_num = 0; page_num < page_count; page_num++) {
        res = kv_backup_get_page_location(store, page_num, &page_location);
        if (res != ERRCODE_SUCC) {
            return res;
        }

        res = kv_store_get_backup_key(key_id, key_data, attributes, page_location);
        if (res == ERRCODE_SUCC) {
            return res;
        }
    }

    return ERRCODE_NV_KEY_NOT_FOUND;
#else
    unused(store);
    unused(key_id);
    unused(key_data);
    unused(attributes);
    return ERRCODE_NV_KEY_NOT_FOUND;
#endif
}

errcode_t kv_store_get_key(kv_store_t store, kv_key_id key_id, kv_store_key_data_t *key_data,
                           kv_attributes_t *attributes)
{
    errcode_t res;
    kv_key_handle_t key;

    res = kv_store_find_valid_key(store, key_id, &key);
    if (res != ERRCODE_SUCC) {
        return res;
    }

    if (attributes != NULL) {
        *attributes = kv_key_attributes(&key);
    }

    /* Attempt to obtain key data from store */
    key_data->kvalue_actual_length = key.header.length;
    if (key_data->kvalue_max_length < key_data->kvalue_actual_length) {
        return ERRCODE_NV_GET_BUFFER_TOO_SMALL;
    }

    res = kv_key_read_data(&key, key_data->kvalue);
    return res;
}

errcode_t kv_store_get_backup_key(kv_key_id key_id, kv_store_key_data_t *key_data, kv_attributes_t *attributes,
    kv_page_location page_location)
{
    errcode_t res_b;
    kv_key_handle_t key;

    res_b = kv_store_find_backup_key(key_id, &key, page_location);
    if (res_b != ERRCODE_SUCC) {
        return res_b;
    }

    if (attributes != NULL) {
        *attributes = kv_key_attributes(&key);
    }

    /* Attempt to obtain key data from store */
    key_data->kvalue_actual_length = key.header.length;
    if (key_data->kvalue_max_length < key_data->kvalue_actual_length) {
        return ERRCODE_NV_GET_BUFFER_TOO_SMALL;
    }

    res_b = kv_key_read_data(&key, key_data->kvalue);
    return res_b;
}

errcode_t kv_store_get_status(kv_store_t store, nv_store_status_t *store_status)
{
    uint32_t pages_in_store;
    uint32_t page_index;

    if (store >= KV_STORE_MAX_NUM) {
        return ERRCODE_NV_INVALID_PARAM;
    }

    (void)memset_s(store_status, sizeof(nv_store_status_t), 0, sizeof(nv_store_status_t));

    pages_in_store = kv_store_get_page_count(store);
    for (page_index = 0; page_index < pages_in_store; page_index++) {
        kv_page_handle_t page;
        kv_page_status_t page_status;
        if (kv_store_get_page_handle(store, page_index, &page) == ERRCODE_SUCC) {
            kv_page_get_status(&page, &page_status);
            store_status->total_space += page_status.total_space;
            store_status->used_space += page_status.used_space;
            store_status->reclaimable_space += page_status.reclaimable_space;
            store_status->corrupted_space += page_status.corrupted_space;
            if (page_status.max_key_space > store_status->max_key_space) {
                store_status->max_key_space = page_status.max_key_space;
            }
        }
    }
    return ERRCODE_SUCC;
}

errcode_t kv_store_find_write_page(kv_store_t store, uint32_t required_space, kv_page_handle_t *page,
                                   kv_page_status_t *page_status)
{
    uint32_t pages_in_store;
    uint32_t page_index;
    kv_page_handle_t page_tmp;
    kv_page_status_t page_status_tmp;
    uint32_t mininal_used_times = 0;
    uint32_t need_defrag_page = (uint32_t)-1;
    uint32_t total_available_space = 0;

    pages_in_store = kv_store_get_page_count(store);

    for (page_index = 0; page_index < pages_in_store; page_index++) {
        errcode_t res = kv_store_get_page_handle(store, page_index, page);
        if (res != ERRCODE_SUCC) {
            return res;
        }
        kv_page_get_status_from_map(page, page_status);

        /* 如果有未使用的空间，直接返回该页，空间计算使用加法判断更保险 */
        if (page_status->total_space >= (page_status->used_space + required_space)) {
            return ERRCODE_SUCC;
        }

        /* 如果未找到有未使用空间够用的页，考虑到flash擦写平衡，在可换页的页中找到一个擦写次数最少的 */
        if (page_status->max_key_space >= required_space) {
            uint32_t used_times = kv_nvregion_get_use_times(page->page_location);
            if ((need_defrag_page == (uint32_t)-1) || (used_times < mininal_used_times)) {
                need_defrag_page = page_index;
                page_tmp = *page;
                page_status_tmp = *page_status;
                mininal_used_times = used_times;
            }
        }
        total_available_space += page_status->max_key_space;
    }

    if (need_defrag_page != (uint32_t)-1) {
        *page = page_tmp;
        *page_status = page_status_tmp;
        return ERRCODE_NV_DEFRAGMENTATION_NEEDED;
    }

    if (required_space < total_available_space) {
        return ERRCODE_NV_PAGE_REALIGN_NEEDED;
    }
    return ERRCODE_NV_NO_ENOUGH_SPACE;
}
