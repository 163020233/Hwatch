/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: sec_storage uapi interface adapter header for SELiteOS BOX.
 */

#ifndef SEC_STORAGE_ADAPTER_H
#define SEC_STORAGE_ADAPTER_H

#include "sec_storage.h"

errcode_t box_uapi_sec_storage_write(uint16_t sec_storage_id, const uint8_t *kvalue, uint16_t kvalue_length);

errcode_t box_uapi_sec_storage_read(uint16_t sec_storage_id, uint16_t kvalue_max_length, uint16_t *kvalue_length,
                                    uint8_t *kvalue);

errcode_t box_uapi_sec_storage_delete(uint16_t sec_storage_id);

#endif
