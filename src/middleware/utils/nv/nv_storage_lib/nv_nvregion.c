/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2022. All rights reserved.
 * Description: KV Storage Library non-volatile region access module implementation
 */

#include "nv_config.h"
#include "nv_page.h"
#include "nv_store.h"
#include "nv_porting.h"
#include "common_def.h"
#include "nv_nvregion.h"
#if defined(CONFIG_NV_SUPPORT_KEY_INDEX_MAP) && (CONFIG_NV_SUPPORT_KEY_INDEX_MAP == NV_YES)
#include "nv_hash.h"
#endif
#if defined(CONFIG_PARTITION_FEATURE_SUPPORT)
#include "partition.h"
#endif

static kv_nvregion_map_t g_kv_nvregion_map[KV_STORE_MAX_NUM] = {0};
static kv_nvregion_area_t g_kv_nvregion_area[KV_STORE_MAX_NUM] = {0};

kv_nvregion_map_t* kv_nvregion_get_map(void)
{
    return &g_kv_nvregion_map[KV_STORE_APPLICATION];
}

kv_nvregion_map_t *kv_nvregion_get_map_from_store(kv_store_t store)
{
    if (store >= KV_STORE_MAX_NUM) {
        return NULL;
    }
    return &g_kv_nvregion_map[store];
}

errcode_t kv_nvregion_get_area_from_store(kv_store_t store, kv_nvregion_area_info_t *nvregion_area)
{
    if ((store >= KV_STORE_MAX_NUM) || (nvregion_area == NULL)) {
        return ERRCODE_NV_INVALID_PARAM;
    }
    nvregion_area->nv_addr = g_kv_nvregion_area[store].nv_data_addr;
    nvregion_area->nv_size = g_kv_nvregion_area[store].nv_data_size;
    return ERRCODE_SUCC;
}

errcode_t kv_nvregion_get_store_from_location(uint32_t location, kv_store_t *store)
{
    if (store == NULL) {
        return ERRCODE_NV_INVALID_PARAM;
    }

    for (uint32_t i = 0; i < KV_STORE_MAX_NUM; i++) {
        kv_nvregion_area_t *area = &g_kv_nvregion_area[i];
        if ((area->nv_data_addr != 0U) && (area->nv_data_size != 0U) &&
            (location >= area->nv_data_addr) &&
            (location < (area->nv_data_addr + area->nv_data_size))) {
            *store = (kv_store_t)i;
            return ERRCODE_SUCC;
        }
    }
    return ERRCODE_NV_INVALID_PARAM;
}

kv_nvregion_map_t *kv_nvregion_get_map_from_location(uint32_t location)
{
    kv_store_t store;
    if (kv_nvregion_get_store_from_location(location, &store) != ERRCODE_SUCC) {
        return NULL;
    }
    return kv_nvregion_get_map_from_store(store);
}

static void kv_nvregion_clear_map_entry(kv_page_header_t *map_entry)
{
    (void)memset_s(map_entry, sizeof(kv_page_header_t), 0, sizeof(kv_page_header_t));
}

errcode_t kv_nvregion_is_valid_map_entry(const kv_page_header_t *map_entry)
{
    if ((map_entry->details.store_id != 0) && (map_entry->details.store_id != 0xFFFF) &&
        (~map_entry->inverted_details_word == *(uint32_t *)(uintptr_t)&map_entry->details) &&
        (~map_entry->inverted_sequence_number == map_entry->sequence_number)) {
        return ERRCODE_SUCC;
    }
    return ERRCODE_FAIL;
}

errcode_t kv_nvregion_get_area_from_location(uint32_t location, kv_nvregion_area_info_t *nvregion_area)
{
    kv_store_t store;
    if ((nvregion_area == NULL) ||
        (kv_nvregion_get_store_from_location(location, &store) != ERRCODE_SUCC)) {
        return ERRCODE_NV_INVALID_PARAM;
    }

    nvregion_area->nv_addr = g_kv_nvregion_area[store].nv_data_addr;
    nvregion_area->nv_size = g_kv_nvregion_area[store].nv_data_size;
    return ERRCODE_SUCC;
}

static bool kv_nvregion_is_defrag_map(kv_nvregion_map_t *nvregion_map, kv_page_header_t *new_entry)
{
    for (uint16_t i = 0; i < nvregion_map->num_entries; i++) {
        kv_page_header_t *existing_entry = &nvregion_map->entries[i];
        if ((existing_entry->details.store_id == new_entry->details.store_id) &&
            (existing_entry->details.page_index == new_entry->details.page_index)) {
            if (existing_entry->sequence_number <= new_entry->sequence_number) {
                /* An existing map entry is older than the new one, use the new one */
                /* Two entries have the same sequence number.  This is unexpected. */
                /* For the moment use the new entry, but it could be better to try and determine based on content */
                /* e.g. amount of free space, number of erased keys, corrupted keys */
                continue;
            } else {
                /* An existing map entry is more recent than the new one */
                return true;
            }
        }
    }
    return false;
}

STATIC void kv_nvregion_free_resource(kv_nvregion_map_t *nvregion_map)
{
    if (nvregion_map->entries != NULL) {
        kv_free(nvregion_map->entries);
        nvregion_map->entries = NULL;
    }
    if (nvregion_map->page_status_map != NULL) {
        kv_free(nvregion_map->page_status_map);
        nvregion_map->page_status_map = NULL;
    }
}

STATIC errcode_t kv_nvregion_init_map(kv_store_t store)
{
    kv_nvregion_area_info_t nvregion_area;
    kv_nvregion_map_t *nvregion_map;
    errcode_t ret = kv_nvregion_get_area_from_store(store, &nvregion_area);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    uint32_t num_entries = nvregion_area.nv_size / KV_PAGE_SIZE;
    if (num_entries == 0) {
        return ERRCODE_NV_INVALID_PARAM;
    }

    /* Obtain details of NV memory region */
    if (((nvregion_area.nv_addr & (KV_PAGE_SIZE - 1U)) != 0U) ||
        ((nvregion_area.nv_size & (KV_PAGE_SIZE - 1U)) != 0U) ||
        ((nvregion_area.nv_size & ~(KV_PAGE_SIZE - 1U)) == 0U)) {
        /* NV Data address or size not aligned to a page boundry */
        return ERRCODE_FAIL;
    }

    nvregion_map = kv_nvregion_get_map_from_store(store);
    if (nvregion_map == NULL) {
        return ERRCODE_NV_INVALID_PARAM;
    }

    if (num_entries != nvregion_map->num_entries) {
        kv_nvregion_free_resource(nvregion_map);
    }

    if (nvregion_map->entries == NULL) {
        nvregion_map->entries = (kv_page_header_t *)kv_zalloc(num_entries * sizeof(kv_page_header_t));
    }

    if (nvregion_map->page_status_map == NULL) {
        nvregion_map->page_status_map =
            (nv_page_status_map_t *)kv_zalloc(num_entries * sizeof(nv_page_status_map_t));
    }

    if ((nvregion_map->entries == NULL) || (nvregion_map->page_status_map == NULL)) {
        kv_nvregion_free_resource(nvregion_map);
        return ERRCODE_MALLOC;
    }

    nvregion_map->num_entries = num_entries;
    return ERRCODE_SUCC;
}

static errcode_t kv_nvregion_build_map(kv_store_t store)
{
    kv_nvregion_area_info_t nvregion_area;
    (void)kv_nvregion_get_area_from_store(store, &nvregion_area);
    kv_nvregion_map_t *nvregion_map = kv_nvregion_get_map_from_store(store);

    uint32_t nv_page = nvregion_area.nv_addr;
    for (uint32_t i = 0; i < nvregion_map->num_entries; i++) {
        kv_page_header_t *entry = &nvregion_map->entries[i];
        (void)kv_key_helper_copy_flash((uint32_t)(uintptr_t)entry, nv_page, sizeof(kv_page_header_t));
        nv_page += KV_PAGE_SIZE;
    }
    uint32_t page_location = nvregion_area.nv_addr;
    for (uint32_t i = 0; i < nvregion_map->num_entries; i++) {
        kv_page_header_t *new_entry = &nvregion_map->entries[i];
        if ((kv_nvregion_is_valid_map_entry(new_entry) == ERRCODE_SUCC) &&
            (new_entry->details.store_id != KV_STORE_ID_BACKUP)) {
            if (new_entry->details.ver == 0) {
                g_nv_header_magic = KV_KEY_NO_MAGIC;
            }
            if (!kv_nvregion_is_defrag_map(nvregion_map, new_entry)) {
                kv_page_handle_t page;
                page.page_location = (kv_page_location)(uintptr_t)page_location;
                kv_page_read_status_to_map(&page, &(nvregion_map->page_status_map[i]));
            } else {
                kv_nvregion_clear_map_entry(new_entry);
            }
        } else {
            kv_nvregion_clear_map_entry(new_entry);
        }

        page_location += KV_PAGE_SIZE;
    }

#if defined(CONFIG_NV_SUPPORT_KEY_INDEX_MAP) && (CONFIG_NV_SUPPORT_KEY_INDEX_MAP == NV_YES)
    nv_hash_table_build_complete(store);
    nv_log_debug("[NV] store=%d, key_num=%d, g_nv_header_magic=0x%x\r\n", store, nv_hash_key_num(store), g_nv_header_magic);
#else
    nv_log_debug("[NV] store=%d g_nv_header_magic=0x%x\r\n", store, g_nv_header_magic);
#endif
    return ERRCODE_SUCC;
}

errcode_t kv_nvregion_init_store_area(kv_store_t store,
                                      uint32_t nv_addr, uint32_t nv_size,
                                      uint32_t nv_backup_addr, uint32_t nv_backup_size)
{
    g_kv_nvregion_area[store].nv_data_addr = nv_addr;
    g_kv_nvregion_area[store].nv_data_size = nv_size;
    g_kv_nvregion_area[store].nv_backup_addr = nv_backup_addr;
    g_kv_nvregion_area[store].nv_backup_size = nv_backup_size;
    return ERRCODE_SUCC;
}

#if defined(CONFIG_PARTITION_FEATURE_SUPPORT)
STATIC errcode_t nv_read_partition_addr(partition_ids_t partition_nv_id, uint32_t *start_address, uint32_t *size)
{
    /* 获取分区表标记 */
    errcode_t ret;
    partition_information_t info;

    ret = uapi_partition_get_info(partition_nv_id, &info);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    *start_address = info.part_info.addr_info.addr + FLASH_PHYSICAL_ADDR_START;
    *size = info.part_info.addr_info.size;

    return ERRCODE_SUCC;
}
#endif

static errcode_t nv_resolve_store_region(kv_store_t store, uint32_t *data_addr, uint32_t *data_size,
                                         uint32_t *backup_addr, uint32_t *backup_size)
{
    nv_store_cfg_t *cfg = nv_get_store_cfg(store);

    if (cfg == NULL || data_addr == NULL || data_size == NULL ||
        backup_addr == NULL || backup_size == NULL) {
        return ERRCODE_NV_INVALID_PARAM;
    }

#if defined(CONFIG_PARTITION_FEATURE_SUPPORT)
    errcode_t ret = nv_read_partition_addr(cfg->partition_id, data_addr, data_size);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

#if (CONFIG_NV_SUPPORT_BACKUP_RESTORE == NV_YES)
#if defined(CONFIG_NV_SUPPORT_BACKUP_REGION) && (CONFIG_NV_SUPPORT_BACKUP_REGION == NV_YES)
    ret = nv_read_partition_addr(cfg->backup_partition_id, backup_addr, backup_size);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
#endif
#endif
#endif
    if (*data_addr != 0 && *data_size != 0) {
        *data_size = cfg->size;
        *backup_addr = *data_addr + *data_size;
        *backup_size = cfg->backup_size;
    } else {
        *data_addr = cfg->start_addr;
        *data_size = cfg->size;
        *backup_addr = cfg->backup_start_addr;
        *backup_size = cfg->backup_size;
    }
    return ERRCODE_SUCC;
}

errcode_t kv_nvregion_init(kv_store_t store)
{
    uint32_t data_addr = 0;
    uint32_t data_size = 0;
    uint32_t backup_addr = 0;
    uint32_t backup_size = 0;
    errcode_t ret = nv_resolve_store_region(store, &data_addr, &data_size, &backup_addr, &backup_size);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    return kv_nvregion_init_store_area(store, data_addr, data_size, backup_addr, backup_size);
}

kv_nvregion_area_t *nv_get_region_area(void)
{
    return &g_kv_nvregion_area[KV_STORE_APPLICATION];
}

kv_nvregion_area_t *kv_nvregion_get_store_area(kv_store_t store)
{
    if (store >= KV_STORE_MAX_NUM) {
        return NULL;
    }
    return &g_kv_nvregion_area[store];
}

errcode_t kv_nvregion_scan(kv_store_t store)
{
    errcode_t res;
    res = kv_nvregion_init_map(store);
    if (res != ERRCODE_SUCC) {
        nv_log_err("kv_nvregion_init_map,ret = %x\n", res);
        return res;
    }
    return kv_nvregion_build_map(store);
}

errcode_t kv_nvregion_create_page(kv_store_t store, uint8_t page_index)
{
    errcode_t res;
    kv_page_location page_location;
    kv_nvregion_map_t *nvregion_map;
    uint16_t store_id;

    if (store >= KV_STORE_MAX_NUM) {
        return ERRCODE_NV_INVALID_PARAM;
    }

    nvregion_map = kv_nvregion_get_map_from_store(store);
    if ((nvregion_map == NULL) || (nvregion_map->entries == NULL)) {
        return ERRCODE_NV_NOT_INIT;
    }

    store_id = kv_store_get_id(store);
    if (store_id == 0U) {
        return ERRCODE_INVALID_PARAM;
    }

    res = kv_nvregion_find_unused_page(store, &page_location);
    if (res != ERRCODE_SUCC) {
        return res;
    }

    res = kv_nvregion_erase_page(page_location);
    if (res != ERRCODE_SUCC) {
        return res;
    }

    res = kv_nvregion_write_page(page_location, store_id, page_index);
    if (res != ERRCODE_SUCC) {
        return res;
    }

    return kv_nvregion_scan(store);
}

errcode_t kv_nvregion_find_page(kv_store_t store, uint8_t page_index,
                                kv_page_location *location, kv_page_header_t *header)
{
    if (store >= KV_STORE_MAX_NUM) {
        return ERRCODE_NV_INVALID_PARAM;
    }
    
    kv_nvregion_map_t *nvregion_map;
    kv_nvregion_area_info_t nvregion_area;
    uint16_t store_id;
    errcode_t res;

    nvregion_map = kv_nvregion_get_map_from_store(store);
    if (nvregion_map == NULL) {
        return ERRCODE_NV_INVALID_PARAM;
    }
    if (nvregion_map->entries == NULL) {
        return ERRCODE_NV_NOT_INIT;
    }
    store_id = kv_store_get_id(store);
    if (store_id == 0) {
        return ERRCODE_NV_INVALID_PARAM;
    }
    res = kv_nvregion_get_area_from_store(store, &nvregion_area);
    if (res != ERRCODE_SUCC) {
        return res;
    }

    for (uint32_t i = 0; i < nvregion_map->num_entries; i++) {
        kv_page_header_t *map_entry = &nvregion_map->entries[i];
        if ((map_entry->details.store_id == store_id) && (map_entry->details.page_index == page_index)) {
            if (location != NULL) {
                uint32_t nv_page = nvregion_area.nv_addr;
                nv_page += KV_PAGE_SIZE * i;
                *location = (kv_page_location)(uintptr_t)nv_page;
            }
            if (header != NULL) {
                (void)memcpy_s(header, sizeof(kv_page_header_t), map_entry, sizeof(kv_page_header_t));
            }
            return ERRCODE_SUCC;
        }
    }
    return ERRCODE_NV_PAGE_NOT_FOUND;
}

errcode_t kv_nvregion_find_unused_page(kv_store_t store, kv_page_location *location)
{
    if (store >= KV_STORE_MAX_NUM) {
        return ERRCODE_NV_INVALID_PARAM;
    }
    
    kv_nvregion_map_t *nvregion_map;
    kv_nvregion_area_info_t nvregion_area;
    errcode_t res;
    
    nvregion_map = kv_nvregion_get_map_from_store(store);
    if (nvregion_map == NULL) {
        return ERRCODE_NV_INVALID_PARAM;
    }
    if (nvregion_map->entries == NULL) {
        return ERRCODE_NV_NOT_INIT;
    }

    res = kv_nvregion_get_area_from_store(store, &nvregion_area);
    if (res != ERRCODE_SUCC) {
        return res;
    }

    for (uint32_t i = 0; i < nvregion_map->num_entries; i++) {
        kv_page_header_t *map_entry = &nvregion_map->entries[i];

        if ((map_entry->details.store_id == 0) && (map_entry->details.page_index == 0) &&
            (map_entry->inverted_details_word == 0) &&
            (map_entry->sequence_number == 0) && (map_entry->inverted_sequence_number == 0)) {
            if (location != NULL) {
                uint32_t nv_page = nvregion_area.nv_addr;
                nv_page += KV_PAGE_SIZE * i;
                *location = (kv_page_location)(uintptr_t)nv_page;
            }
            return ERRCODE_SUCC;
        }
    }
    return ERRCODE_NV_PAGE_NOT_FOUND;
}

#if defined(CONFIG_NV_SUPPORT_KEY_INDEX_MAP) && (CONFIG_NV_SUPPORT_KEY_INDEX_MAP == NV_YES)
errcode_t kv_nvregion_find_page_by_page_location(kv_page_handle_t *page_handle)
{
    kv_store_t store;
    kv_nvregion_area_info_t area;
    kv_nvregion_map_t *map;
    uint32_t page_index;

    if (page_handle == NULL) {
        return ERRCODE_NV_PAGE_NOT_FOUND;
    }

    if (kv_nvregion_get_area_from_location((uint32_t)page_handle->page_location, &area) != ERRCODE_SUCC) {
        return ERRCODE_NV_PAGE_NOT_FOUND;
    }
    if (kv_nvregion_get_store_from_location((uint32_t)page_handle->page_location, &store) != ERRCODE_SUCC) {
        return ERRCODE_NV_PAGE_NOT_FOUND;
    }

    map = kv_nvregion_get_map_from_store(store);
    if (map == NULL || map->entries == NULL) {
        return ERRCODE_NV_NOT_INIT;
    }

    page_index = ((uint32_t)page_handle->page_location - area.nv_addr) / KV_PAGE_SIZE;
    if (page_index >= map->num_entries) {
        return ERRCODE_NV_PAGE_NOT_FOUND;
    }

    (void)memcpy_s(&page_handle->page_header, sizeof(kv_page_header_t),
                   &map->entries[page_index], sizeof(kv_page_header_t));
    return ERRCODE_SUCC;
}
#endif

errcode_t kv_nvregion_erase_page(kv_page_location page_location)
{
    uint32_t nv_page_addr = (uint32_t)(uintptr_t)page_location;
    kv_nvregion_area_info_t nvregion_area;
    if (kv_nvregion_get_area_from_location(nv_page_addr, &nvregion_area) != ERRCODE_SUCC) {
        return ERRCODE_NV_INVALID_PARAM;
    }
    return kv_key_erase_flash(nv_page_addr, KV_PAGE_SIZE);
}

errcode_t kv_nvregion_write_page(kv_page_location page_location, uint16_t store_id, uint8_t page_index)
{
    kv_page_header_t page_header;

    page_header.details.store_id = store_id;
    page_header.details.page_index = page_index;
    page_header.details.ver = (g_nv_header_magic == KV_KEY_MAGIC) ? 1U : 0U;
    page_header.inverted_details_word = ~*(uint32_t *)(uintptr_t)&page_header.details;
    page_header.sequence_number = 0;
    page_header.inverted_sequence_number = ~page_header.sequence_number;

    return kv_key_write_flash((uint32_t)(uintptr_t)page_location,
                              sizeof(kv_page_header_t),
                              (uint8_t *)&page_header);
}

uint32_t kv_nvregion_get_page_number(kv_page_location page_location)
{
    uint32_t location = (uint32_t)(uintptr_t)page_location;
    kv_nvregion_area_info_t nvregion_area;

    if (kv_nvregion_get_area_from_location(location, &nvregion_area) != ERRCODE_SUCC) {
        return (uint32_t)-1;
    }

    return (location - nvregion_area.nv_addr) / KV_PAGE_SIZE;
}

uint32_t kv_nvregion_get_use_times(kv_page_location page_location)
{
    uint32_t page_number = kv_nvregion_get_page_number(page_location);
    if (page_number == (uint32_t)-1) {
        return (uint32_t)-1;
    }

    kv_nvregion_map_t *nvregion_map = kv_nvregion_get_map_from_location((uint32_t)(uintptr_t)page_location);
    if (nvregion_map == NULL) {
        return (uint32_t)-1;
    }

    return nvregion_map->entries[page_number].sequence_number;
}