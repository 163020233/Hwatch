/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: sec_storage uapi interface adapter source for SELiteOS BOX.
 */

#include "sec_storage_adapter.h"
#include <stdio.h>
#include "sec_storage_ioctl.h"

errcode_t box_uapi_sec_storage_write(uint16_t sec_storage_id, const uint8_t *kvalue, uint16_t kvalue_length)
{
    sec_storage_write_t arg = {.sec_storage_id = sec_storage_id, .kvalue = kvalue, .kvalue_length = kvalue_length};

    if (kvalue == NULL) {
        return ERRCODE_INVALID_PARAM;
    }

    return LOS_SEC_STORAGE_IOCTL(CMD_SEC_STORAGE_WRITE, &arg);
}

errcode_t box_uapi_sec_storage_read(uint16_t sec_storage_id, uint16_t kvalue_max_length, uint16_t *kvalue_length,
                                    uint8_t *kvalue)
{
    errcode_t ret = ERRCODE_SUCC;

    sec_storage_read_t arg = {
        .sec_storage_id = sec_storage_id,
        .kvalue_max_length = kvalue_max_length,
        .kvalue = kvalue,
    };

    if (kvalue == NULL || kvalue_length == NULL) {
        return ERRCODE_INVALID_PARAM;
    }

    arg.kvalue_length = *kvalue_length;
    ret = LOS_SEC_STORAGE_IOCTL(CMD_SEC_STORAGE_READ, &arg);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    *kvalue_length = arg.kvalue_length;

    return ret;
}

errcode_t box_uapi_sec_storage_delete(uint16_t sec_storage_id)
{
    errcode_t ret = ERRCODE_SUCC;
    uint16_t arg = sec_storage_id;

    ret = LOS_SEC_STORAGE_IOCTL(CMD_SEC_STORAGE_DELETE, &arg);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    return ret;
}
