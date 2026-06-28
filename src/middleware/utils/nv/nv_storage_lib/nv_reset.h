/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: NV Storage Library Restore factory settings interface
 */

#ifndef NV_RESET_H_
#define NV_RESET_H_

#include "nv.h"
#include "nv_key.h"
#include "nv_store.h"
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define RESET_MODE_A        1
#define RESET_MODE_B        2

typedef struct {
    uint8_t mode;
    bool region_flag[KEY_ID_REGION_MAX_NUM];
} nv_reset_mode_t;

bool kv_store_backup_enabled(kv_store_t store);
uint32_t kv_store_backup_page_count(kv_store_t store);
errcode_t kv_backup_get_page_location(kv_store_t store, uint32_t page_num, kv_page_location *page_location);
errcode_t kv_backup_keys(kv_store_t store, const nv_backup_mode_t *backup_flag);
errcode_t kv_restore_all_keys(kv_store_t store);
errcode_t kv_enable_restore_flag(kv_store_t store, const nv_reset_mode_t *nv_reset_mode);
errcode_t kv_backup_copy_unused_page_to_dragpage(uint32_t dragpage_location, uint32_t unused_page_location);

errcode_t kv_restore_set_region_flag(kv_store_t store, const bool *flag);
errcode_t kv_backup_find_write_position(kv_store_t store, uint16_t required_space, uint32_t *write_position);

errcode_t kv_backup_delete_repeat_key(kv_store_t store);
errcode_t kv_backup_write_key(kv_store_t store);
errcode_t kv_backup_write_one_page_key(kv_store_t store, kv_page_handle_t *page);
#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* NV_RESET_H_ */

