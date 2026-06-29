/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: NV Async Store Used Hash
 */

#ifndef NV_HASH_H
#define NV_HASH_H

#include "nv.h"
#include "nv_store.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef struct nv_hash_table {
    uint32_t key_location;          /* Key相对于flash的位置 */
} nv_key_hash_t;

bool nv_hash_init(kv_store_t store);
void nv_hash_table_build_complete(kv_store_t store);
bool nv_hash_key_exist(kv_store_t store, kv_key_id key_id);
errcode_t nv_hash_key_find(kv_store_t store, kv_key_id key_id, kv_key_handle_t *out_key);
bool nv_hash_key_put(kv_store_t store, kv_key_id key_id, const kv_key_handle_t *key_handle);
void nv_hash_key_erase(kv_store_t store, kv_key_id key_id);
void nv_hash_key_clear(kv_store_t store);
size_t nv_hash_key_num(kv_store_t store);

/* 遍历哈希表取出哈希表中一页的所有keyId，返回数组 */
bool nv_hash_collect_keys_by_page(kv_store_t store, kv_page_location page_location, uint16_t **outKeys, size_t *outCount);

void nv_hash_table_init_validate_wrong(kv_key_handle_t *key);
void nv_hash_table_init_validate_correct(kv_key_handle_t *key, nv_page_status_map_t *status_map, uint32_t key_size);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* NV_HASH_H */