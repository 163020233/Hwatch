/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: NV Storage Library Restore factory settings interface
 */
#include "nv.h"
#include "nv_store.h"
#include "nv_page.h"
#include "nv_storage.h"
#include "nv_nvregion.h"
#include "nv_porting.h"
#include "nv_notify.h"
#include "nv_task_adapt.h"
#include "nv_update.h"
#include "common_def.h"
#include "nv_storage_handle.h"
#include "nv_reset.h"

#if (CONFIG_NV_SUPPORT_BACKUP_RESTORE == NV_YES)

STATIC bool g_reset_region_flag[KV_STORE_MAX_NUM][KEY_ID_REGION_MAX_NUM] = {0};

#define RESET_MAX_BINARY_VAL 1
#define assert_r(x)        ((void)0)
#define NV_RESTORE_MAX_KEY 4060

bool kv_store_backup_enabled(kv_store_t store)
{
    kv_nvregion_area_t *area = kv_nvregion_get_store_area(store);
    if (area == NULL) {
        return false;
    }
    return ((area->nv_backup_addr != 0U) && (area->nv_backup_size >= KV_PAGE_SIZE));
}

uint32_t kv_store_backup_page_count(kv_store_t store)
{
    kv_nvregion_area_t *area = kv_nvregion_get_store_area(store);
    if (area == NULL) {
        return 0U;
    }
    return area->nv_backup_size / KV_PAGE_SIZE;
}

errcode_t kv_backup_get_page_location(kv_store_t store, uint32_t page_num, kv_page_location *page_location)
{
    kv_nvregion_area_t *area;
    uint32_t page_count;

    if ((store >= KV_STORE_MAX_NUM) || (page_location == NULL)) {
        return ERRCODE_NV_INVALID_PARAM;
    }
    if (!kv_store_backup_enabled(store)) {
        return ERRCODE_NOT_SUPPORT;
    }

    area = kv_nvregion_get_store_area(store);
    page_count = kv_store_backup_page_count(store);
    if (page_num >= page_count) {
        return ERRCODE_NV_INVALID_PARAM;
    }

    *page_location = (kv_page_location)(uintptr_t)(area->nv_backup_addr + page_num * KV_PAGE_SIZE);
    return ERRCODE_SUCC;
}

errcode_t kv_region_is_enable(kv_store_t store, uint16_t key_id)
{
    uint16_t region_id = key_id / REGION_KEY_NUMS;
    if ((region_id < KEY_ID_REGION_MAX_NUM) &&
        g_reset_region_flag[store][region_id]) {
        return ERRCODE_SUCC;
    }
    return ERRCODE_FAIL;
}

STATIC errcode_t kv_backup_delete_one_page_repeat_key(kv_store_t store, kv_page_handle_t *page)
{
    errcode_t ret;
    kv_key_handle_t key;
    errcode_t key_found = kv_page_find_first_key(page, NULL, &key);

    while (key_found == ERRCODE_SUCC) {
        if (kv_key_is_valid(&key) &&
            (kv_region_is_enable(store, key.header.key_id) == ERRCODE_SUCC)) {
            ret = kv_key_set_invalid(&key);
            if (ret != ERRCODE_SUCC) {
                return ret;
            }
        }
        key_found = kv_page_find_next_key(page, NULL, &key);
    }
    return ERRCODE_SUCC;
}

STATIC errcode_t kv_backup_copy_section_process(uint8_t *buff, uint32_t src_location, uint32_t dest_location,
    uint32_t len)
{
    errcode_t ret = kv_key_helper_copy_flash((uint32_t)(uintptr_t)buff, src_location, len);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    ret = kv_key_write_flash(dest_location, len, buff);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    return ret;
}

errcode_t kv_backup_delete_repeat_key(kv_store_t store)
{
    errcode_t ret;
    kv_page_handle_t page;
    uint32_t page_count = kv_store_backup_page_count(store);

    for (uint32_t page_index = 0; page_index < page_count; page_index++) {
        ret = kv_backup_get_page_location(store, page_index, &page.page_location);
        if (ret != ERRCODE_SUCC) {
            return ret;
        }
        ret = kv_backup_delete_one_page_repeat_key(store, &page);
        if (ret != ERRCODE_SUCC) {
            return ret;
        }
    }

    return ERRCODE_SUCC;
}

STATIC errcode_t kv_backup_dragpage_switch_unused_page(kv_page_handle_t dragpage, kv_page_handle_t unused_page)
{
    /* 将换页页内容转移到unused page */
    errcode_t ret;
    uint32_t page_head_size = (uint32_t)sizeof(kv_page_header_t);

    /* 将换页页的页头转移到unused page */
    kv_page_header_t head_buffer;
    ret = kv_backup_copy_section_process((uint8_t *)(uintptr_t)&head_buffer,
        (uint32_t)dragpage.page_location, (uint32_t)unused_page.page_location, page_head_size);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    kv_key_handle_t key;
    uint16_t key_size;
    uint32_t write_position = (uint32_t)((uintptr_t)unused_page.page_location + page_head_size);
    /* 将换页页的有效的key转移到unused page */
    errcode_t key_found = kv_page_find_first_key(&dragpage, NULL, &key);
    while (key_found == ERRCODE_SUCC) {
        if (kv_key_is_valid(&key)) {
            key_size = kv_key_flash_size(&key);
            uint8_t *kv_buffer = (uint8_t *)kv_zalloc(key_size);
            if (kv_buffer == NULL) {
                return ERRCODE_MALLOC;
            }

            ret = kv_backup_copy_section_process(kv_buffer, (uint32_t)key.key_location, write_position, key_size);
            if (ret != ERRCODE_SUCC) {
                kv_free(kv_buffer);
                return ret;
            }

            kv_free(kv_buffer);
            write_position += key_size;
        }
        key_found = kv_page_find_next_key(&dragpage, NULL, &key);
    }

    return ERRCODE_SUCC;
}

errcode_t kv_backup_copy_unused_page_to_dragpage(uint32_t dragpage_location, uint32_t unused_page_location)
{
    errcode_t ret;
    uint32_t remain_len = KV_PAGE_SIZE - sizeof(kv_page_header_t);
    uint32_t cur_unused_page_location = unused_page_location + sizeof(kv_page_header_t);
    uint32_t cur_dragpage_location = dragpage_location + sizeof(kv_page_header_t);

    /* 分块拷贝换页页内容到备份区被换页的页上,减小内存占用 */
    uint8_t *page_buffer = (uint8_t *)kv_malloc(NV_KEY_DATA_CHUNK_LEN);
    if (page_buffer == NULL) {
        return ERRCODE_MALLOC;
    }

    while (remain_len >= NV_KEY_DATA_CHUNK_LEN) {
        memset_s(page_buffer, NV_KEY_DATA_CHUNK_LEN, 0, NV_KEY_DATA_CHUNK_LEN);
        ret = kv_backup_copy_section_process(page_buffer,
            cur_unused_page_location, cur_dragpage_location, NV_KEY_DATA_CHUNK_LEN);
        if (ret != ERRCODE_SUCC) {
            kv_free(page_buffer);
            return ret;
        }
        cur_dragpage_location += NV_KEY_DATA_CHUNK_LEN;
        cur_unused_page_location += NV_KEY_DATA_CHUNK_LEN;
        remain_len -= NV_KEY_DATA_CHUNK_LEN;
    }

    /* 处理剩余数据 */
    if (remain_len > 0) {
        memset_s(page_buffer, NV_KEY_DATA_CHUNK_LEN, 0, NV_KEY_DATA_CHUNK_LEN);
        ret = kv_backup_copy_section_process(page_buffer,
            cur_unused_page_location, cur_dragpage_location, remain_len);
        if (ret != ERRCODE_SUCC) {
            kv_free(page_buffer);
            return ret;
        }
    }
    kv_free(page_buffer);

    /* 其次拷贝工作区换页页的页头内容:先拷贝页内容，后拷贝页头是为了更好的处理换页时掉电异常场景 */
    kv_page_header_t head_buffer;
    ret = kv_backup_copy_section_process((uint8_t *)&head_buffer,
        unused_page_location, dragpage_location, sizeof(kv_page_header_t));
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    return ERRCODE_SUCC;
}

/* 备份区换页处理机制 */
STATIC errcode_t kv_backup_dragpage_process(kv_store_t store, kv_page_handle_t dragpage)
{
    errcode_t ret;
    kv_page_handle_t unused_page;
    kv_page_location unused_page_location = 0;

    /* 1、去找工作区可以换页的页，然后将可以换页的页地址给传出 */
    ret = kv_nvregion_find_unused_page(store, &unused_page_location);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    /* 2、将工作区内可以换页的页的内容擦除 */
    ret = kv_nvregion_erase_page(unused_page_location);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    unused_page.page_location = unused_page_location;
    /* 3、将备份区要换页的内容拷贝到工作区可以换页的页上 */
    ret = kv_backup_dragpage_switch_unused_page(dragpage, unused_page);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    /* 4、将备份区要换页的页擦除内容 */
    ret = kv_key_erase_flash((uint32_t)(uintptr_t)dragpage.page_location, KV_PAGE_SIZE);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    /* 5、将换页页的内容拷贝到备份区被擦除的页上 */
    ret =  kv_backup_copy_unused_page_to_dragpage((uint32_t)(uintptr_t)dragpage.page_location,
        (uint32_t)(uintptr_t)unused_page.page_location);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    /* 6、擦除工作区换页页 */
    ret = kv_nvregion_erase_page(unused_page_location);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    return ERRCODE_SUCC;
}

errcode_t kv_backup_find_write_position(kv_store_t store, uint16_t required_space, uint32_t *write_position)
{
    errcode_t ret;
    kv_page_handle_t page = {0};
    kv_page_status_t page_status = {0};
    kv_page_handle_t dragpage = {0};
    uint32_t page_count;

    if ((store >= KV_STORE_MAX_NUM) || (write_position == NULL)) {
        return ERRCODE_NV_INVALID_PARAM;
    }
    if (!kv_store_backup_enabled(store)) {
        return ERRCODE_NOT_SUPPORT;
    }

    page_count = kv_store_backup_page_count(store);
    for (uint32_t page_index = 0; page_index < page_count; page_index++) {
        ret = kv_backup_get_page_location(store, page_index, &page.page_location);
        if (ret != ERRCODE_SUCC) {
            return ret;
        }

        kv_page_get_status(&page, &page_status);
        if ((page_status.total_space - page_status.used_space) >= required_space) {
            *write_position = page_status.first_writable_location;
            return ERRCODE_SUCC;
        }

        /* 找可换页页 */
        if (page_status.max_key_space >= required_space) {
            dragpage = page;
        }
    }

    /* 进行备份区换页 */
    ret = kv_backup_dragpage_process(store, dragpage);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    /* 将备份区整理好的页可以写的位置传出 */
    kv_page_get_status(&dragpage, &page_status);
    if ((page_status.total_space - page_status.used_space) >= required_space) {
        *write_position = page_status.first_writable_location;
        return ERRCODE_SUCC;
    }
    return ERRCODE_NV_NO_ENOUGH_SPACE;
}

errcode_t kv_backup_write_one_page_key(kv_store_t store, kv_page_handle_t *page)
{
    errcode_t ret;
    kv_key_handle_t key;
    uint16_t key_size;
    uint32_t write_position = 0;
    errcode_t key_found = kv_page_find_first_key(page, NULL, &key);

    while (key_found == ERRCODE_SUCC) {
        if (kv_key_is_valid(&key) &&
            (kv_region_is_enable(store, key.header.key_id) == ERRCODE_SUCC)) {
            key_size = kv_key_flash_size(&key);
            uint8_t *kv_buffer = (uint8_t *)kv_malloc(key_size);
            if (kv_buffer == NULL) {
                return ERRCODE_MALLOC;
            }

            ret = kv_backup_find_write_position(store, key_size, &write_position);
            if (ret != ERRCODE_SUCC) {
                kv_free(kv_buffer);
                return ret;
            }

            ret = kv_backup_copy_section_process(kv_buffer, (uint32_t)key.key_location, write_position, key_size);
            kv_free(kv_buffer);
            if (ret != ERRCODE_SUCC) {
                return ret;
            }
        }
        key_found = kv_page_find_next_key(page, NULL, &key);
    }

    return ERRCODE_SUCC;
}

errcode_t kv_backup_write_key(kv_store_t store)
{
    errcode_t ret;
    kv_page_handle_t page;
    uint8_t page_num = kv_store_get_page_count(store);

    for (uint32_t page_index = 0; page_index < page_num; page_index++) {
        ret = kv_store_get_page_handle(store, (uint8_t)page_index, &page);
        if (ret != ERRCODE_SUCC) {
            return ret;
        }
        ret = kv_backup_write_one_page_key(store, &page);
        if (ret != ERRCODE_SUCC) {
            return ret;
        }
    }

    return ERRCODE_SUCC;
}

STATIC errcode_t kv_read_backup_key(uint16_t key_id, uint16_t *kvalue_length, uint8_t *kvalue, nv_key_attr_t *attr,
    kv_page_location page_location)
{
    kv_attributes_t backup_attribute = 0;
    if (kvalue_length == NULL || kvalue == NULL || attr == NULL) {
        return ERRCODE_NV_INVALID_PARAM;
    }

    kv_store_key_data_t key_data = {*kvalue_length, 0, kvalue};
    errcode_t ret_val = kv_store_get_backup_key(key_id, &key_data, &backup_attribute, page_location);
    if (ret_val == ERRCODE_SUCC) {
        memset_s(attr, sizeof(nv_key_attr_t), 0, sizeof(nv_key_attr_t));
        if (((uint32_t)backup_attribute & KV_ATTRIBUTE_PERMANENT) != 0) {
            attr->permanent = true;
            attr->non_upgrade = true;
        }
#if (CONFIG_NV_SUPPORT_ENCRYPT == NV_YES)
        if (((uint32_t)backup_attribute & KV_ATTRIBUTE_ENCRYPTED) != 0) {
            attr->encrypted = true;
            attr->non_upgrade = true;
        }
#endif
        if (((uint32_t)backup_attribute & KV_ATTRIBUTE_NON_UPGRADE) != 0) {
            attr->non_upgrade = true;
        }
    }
    *kvalue_length = key_data.kvalue_actual_length;
    return ret_val;
}

errcode_t kv_start_restore_all_keys(kv_store_t store)
{
    errcode_t ret = ERRCODE_SUCC;
    uint16_t klength = 0;
    uint8_t *kvalue;
    nv_key_attr_t attr;
    kv_key_handle_t key;
    kv_page_handle_t page;
    errcode_t key_found;
    uint32_t page_count;

    if (!kv_store_backup_enabled(store)) {
        return ERRCODE_NOT_SUPPORT;
    }

    page_count = kv_store_backup_page_count(store);
    for (uint32_t page_num = 0; page_num < page_count; page_num++) {
        ret = kv_backup_get_page_location(store, page_num, &page.page_location);
        if (ret != ERRCODE_SUCC) {
            return ret;
        }

        key_found = kv_page_find_first_key(&page, NULL, &key);
        while (key_found == ERRCODE_SUCC) {
            if (kv_region_is_enable(store, key.header.key_id) == ERRCODE_SUCC && kv_key_is_valid(&key)) {
                kvalue = (uint8_t *)kv_malloc(key.header.length);
                klength = key.header.length;
                (void)kv_read_backup_key(key.header.key_id, &klength, kvalue, &attr, page.page_location);
                ret = nv_direct_write_with_attr(store, key.header.key_id, kvalue, klength, &attr, NULL);
                kv_free(kvalue);
            }
            key_found = kv_page_find_next_key(&page, NULL, &key);
        }
    }
    return ret;
}

errcode_t kv_enable_restore_flag(kv_store_t store, const nv_reset_mode_t *nv_reset_mode)
{
    nv_key_attr_t attr = {0};
    attr.non_upgrade = true;

    if ((store >= KV_STORE_MAX_NUM) || (nv_reset_mode == NULL)) {
        return ERRCODE_NV_INVALID_PARAM;
    }
 
    return nv_direct_write_with_attr(store, NV_ID_RESTORE_ENABLE,
        (uint8_t *)(uintptr_t)nv_reset_mode, sizeof(nv_reset_mode_t), &attr, NULL);
}

STATIC errcode_t kv_disable_restore_flag(kv_store_t store)
{
    nv_reset_mode_t nv_reset_mode = {0};
    nv_key_attr_t attr = {0};
    attr.non_upgrade = true;

    return nv_direct_write_with_attr(store, NV_ID_RESTORE_ENABLE,
        (uint8_t *)(uintptr_t)&nv_reset_mode, sizeof(nv_reset_mode_t), &attr, NULL);
}

STATIC bool kv_is_restore_flag(uint16_t key_id)
{
    if (key_id == NV_ID_RESTORE_ENABLE) {
        return true;
    }
    return false;
}

errcode_t kv_restore_set_region_flag(kv_store_t store, const bool *flag)
{
    if (flag == NULL) {
        if (memset_s(g_reset_region_flag[store], sizeof(g_reset_region_flag[store]),
                     true, sizeof(g_reset_region_flag[store])) != EOK) {
            return ERRCODE_MEMSET;
        }
    } else {
        if (memcpy_s(g_reset_region_flag[store], sizeof(g_reset_region_flag[store]),
                     flag, sizeof(g_reset_region_flag[store])) != EOK) {
            return ERRCODE_MEMCPY;
        }
    }

    return ERRCODE_SUCC;
}

STATIC errcode_t kv_restore_invalid_one_page_region_keys(kv_store_t store, kv_page_handle_t *page)
{
    errcode_t ret;
    kv_key_handle_t key;
    errcode_t key_found = kv_page_find_first_key(page, NULL, &key);

    while (key_found == ERRCODE_SUCC) {
        if ((kv_key_is_valid(&key) && !kv_is_restore_flag(key.header.key_id)) &&
            (kv_region_is_enable(store, key.header.key_id) == ERRCODE_SUCC)) {
            ret = kv_key_set_invalid(&key);
            if (ret != ERRCODE_SUCC) {
                return ret;
            }
        }
        key_found = kv_page_find_next_key(page, NULL, &key);
    }

    return ERRCODE_SUCC;
}

errcode_t kv_restore_invalid_region_keys(kv_store_t store)
{
    errcode_t ret;
    kv_page_handle_t page;
    uint8_t page_num = kv_store_get_page_count(store);

    for (uint32_t page_index = 0; page_index < page_num; page_index++) {
        ret = kv_store_get_page_handle(store, (uint8_t)page_index, &page);
        if (ret != ERRCODE_SUCC) {
            return ret;
        }
        ret = kv_restore_invalid_one_page_region_keys(store, &page);
        if (ret != ERRCODE_SUCC) {
            return ret;
        }
    }

    return ERRCODE_SUCC;
}
errcode_t kv_backup_keys(kv_store_t store, const nv_backup_mode_t *backup_flag)
{
    errcode_t ret;

    if ((store >= KV_STORE_MAX_NUM) || (backup_flag == NULL)) {
        return ERRCODE_NV_INVALID_PARAM;
    }
    if (!kv_store_backup_enabled(store)) {
        return ERRCODE_NOT_SUPPORT;
    }

    ret = kv_restore_set_region_flag(store, backup_flag->region_mode);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    ret = kv_backup_delete_repeat_key(store);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    return kv_backup_write_key(store);
}


errcode_t kv_restore_all_keys(kv_store_t store)
{
    errcode_t ret;
    nv_reset_mode_t nv_reset_mode = {0};
    uint16_t flag_length = 0;
    bool *flag = NULL;
    nv_key_attr_t attr = {0};

    if (store >= KV_STORE_MAX_NUM) {
        return ERRCODE_NV_INVALID_PARAM;
    }
    if (!kv_store_backup_enabled(store)) {
        return ERRCODE_NOT_SUPPORT;
    }

    ret = nv_direct_get_key_data(store, NV_ID_RESTORE_ENABLE,
        sizeof(nv_reset_mode_t), &flag_length, (uint8_t *)(uintptr_t)&nv_reset_mode, &attr);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    if (nv_reset_mode.mode == RESET_MODE_A) {
        flag = NULL;
    } else if (nv_reset_mode.mode == RESET_MODE_B) {
        flag = nv_reset_mode.region_flag;
    } else {
        return ERRCODE_NV_ILLEGAL_OPERATION;
    }

    nv_log_debug("nv start restore all keys. store=%d mode=%d\r\n", store, nv_reset_mode.mode);

    /* 设定要恢复出厂的标记, 参数为NULL，表示全部恢复 */
    ret = kv_restore_set_region_flag(store, flag);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    /* 将工作区需要恢复的region区数据置为无效 */
    ret = kv_restore_invalid_region_keys(store);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    ret = kv_start_restore_all_keys(store);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    (void)kv_disable_restore_flag(store);
    nv_log_debug("nv restore all keys success\r\n");
    return ret;
}
#endif /* #if (CONFIG_NV_SUPPORT_BACKUP_RESTORE == NV_YES) */