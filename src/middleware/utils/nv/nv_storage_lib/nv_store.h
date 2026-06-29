/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2022. All rights reserved.
 * Description: KV Storage Library store access interface
 */

#ifndef NV_STORE_H
#define NV_STORE_H

#include "nv_page.h"
#include "nv.h"
#include "errcode.h"
#include "nv_config.h"
#if defined(CONFIG_PARTITION_FEATURE_SUPPORT)
#include "partition_resource_id.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef uint8_t kv_store_t;

/** Store ids for each store, with a minimum hamming distance of 10 between each value */
#define KV_STORE_ID_SCPU    0xCB7E
#define KV_STORE_ID_ACPU    0x254D
#define KV_STORE_ID_BACKUP  0x34B2

#define KV_STORE_APPLICATION 0
#ifndef KV_STORE_MAX_NUM
#define KV_STORE_MAX_NUM 1
#endif

typedef struct {
    uint16_t store_id;
    uint8_t page_num;
#if defined(CONFIG_PARTITION_FEATURE_SUPPORT)
    partition_ids_t partition_id;
#endif
    uint32_t start_addr;
    uint32_t size;
#if defined(CONFIG_PARTITION_FEATURE_SUPPORT)
    partition_ids_t backup_partition_id;
#endif
    uint32_t backup_start_addr;
    uint32_t backup_size;

#if (CONFIG_NV_SUPPORT_KEY_INDEX_MAP == NV_YES)
    uint16_t hash_bucket_size;
#endif
} nv_store_cfg_t;

typedef struct {
    uint16_t kvalue_max_length;
    uint16_t kvalue_actual_length;
    uint8_t *kvalue;
} kv_store_key_data_t;

bool nv_store_is_ready(kv_store_t store);
nv_store_cfg_t *nv_get_store_cfg_table(void);
nv_store_cfg_t *nv_get_store_cfg(kv_store_t store);
uint16_t kv_store_get_id(kv_store_t store);
kv_store_t kv_store_convert_id(uint16_t store_id);
uint8_t kv_store_get_page_count(kv_store_t store);
errcode_t kv_store_get_page_handle(kv_store_t store, uint32_t page_index, kv_page_handle_t *page);
errcode_t kv_store_find_valid_key_from_location(kv_store_t store, kv_key_id key_id, kv_page_handle_t *page,
    kv_key_handle_t *key, kv_key_location start_location);
errcode_t kv_store_find_valid_key(kv_store_t store, kv_key_id key_id, kv_key_handle_t *key);
errcode_t kv_store_get_backup_key(kv_key_id key_id, kv_store_key_data_t *key_data, kv_attributes_t *attributes,
    kv_page_location page_location);
errcode_t kv_store_get_key_attr(kv_store_t store, kv_key_id key_id, uint16_t *len, kv_attributes_t *attributes);
errcode_t kv_store_get_key(kv_store_t store, kv_key_id key_id, kv_store_key_data_t *key_data,
                           kv_attributes_t *attributes);
errcode_t kv_store_get_status(kv_store_t store, nv_store_status_t *store_status);
errcode_t kv_store_find_write_page(kv_store_t store, uint32_t required_space, kv_page_handle_t *page,
                                   kv_page_status_t *page_status);
errcode_t kv_store_read_backup_key(kv_store_t store, kv_key_id key_id, kv_store_key_data_t *key_data, kv_attributes_t *attributes);
errcode_t kv_store_get_backup_key_attr(kv_store_t store, kv_key_id key_id, uint16_t *len,
    kv_attributes_t *attributes, kv_key_handle_t *backup_key);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* NV_STORE_H_ */
