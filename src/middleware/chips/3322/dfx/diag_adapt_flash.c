/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 * Description: diag adapt flash
 * This file should be changed only infrequently and with great care.
 */
#include "diag_adapt_flash.h"
#include "non_os.h"
#include "common_def.h"
#include "diag.h"
#include "debug_print.h"
#include "partition.h"
#include "dfx_adapt_layer.h"
#if CORE == APPS
#include "sfc_porting.h"
#include "sfc_v2.h"
#endif

#define ERASE_LENGTH_4K         0x1000

static errcode_t diag_adapt_flash_get_addr(uint8_t flash_op_type, uint32_t *addr, uint32_t *len)
{
    errcode_t ret = ERRCODE_SUCC;
    partition_ids_t partition_id;
    partition_information_t partition_info;
    *addr = 0;
    *len = 0;
    switch (flash_op_type) {
        case FLASH_OP_TYPE_DUMP_INFO:
            partition_id = PARTITION_DUMP_INFO;
            break;
        case FLASH_OP_TYPE_LOG_FILE:
            partition_id = PARTITION_LOG_FILE;
            break;
        case FLASH_OP_TYPE_DOTTING:
        default:
            partition_id = PARTITION_MAX_CNT;
            break;
    }
    ret = uapi_partition_get_info(partition_id, &partition_info);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    *addr = partition_info.part_info.addr_info.addr;
    *len = partition_info.part_info.addr_info.size;
    return ret;
}

uint32_t diag_adapt_flash_get_region_size(uint8_t opt_type)
{
    uint32_t flash_addr = 0;
    uint32_t length = 0;

    if (diag_adapt_flash_get_addr(opt_type, &flash_addr, &length) != ERRCODE_SUCC) {
        return 0;
    }
    return length;
}

uint32_t dfx_flash_get_region_size(uint8_t opt_type)
{
    return diag_adapt_flash_get_region_size(opt_type);
}

/* flash 读写 */
int32_t diag_adapt_flash_read(uint8_t opt_type, uint32_t offset, uint8_t *buf, uint32_t size)
{
    errcode_t ret;
    uint32_t flash_addr = 0;
    uint32_t length = 0;
    if (diag_adapt_flash_get_addr(opt_type, &flash_addr, &length) != ERRCODE_SUCC) {
        return -1;
    }

    if (offset >= length) {
        return 0;
    }

    uint32_t remain_len = length - offset;
    uint32_t read_len = uapi_min(remain_len, size);
    flash_addr += offset;
    ret = uapi_sfc_read(0, flash_addr, buf, read_len);
    if (ret == ERRCODE_SUCC) {
        return (int32_t)read_len;
    } else {
        return 0;
    }
}

int32_t diag_adapt_flash_write(uint8_t opt_type, uint32_t offset, const uint8_t *buf, uint32_t size,
    bool do_erase)
{
    unused(do_erase);
    int32_t ret = 0;
    uint32_t flash_addr = 0;
    uint32_t length = 0;

    if (diag_adapt_flash_get_addr(opt_type, &flash_addr, &length) != ERRCODE_SUCC) {
        return -1;
    }

    if (offset >= length) {
        return 0;
    }

    uint32_t remain_len = length - offset;
    uint32_t write_len = uapi_min(remain_len, size);
    flash_addr += offset;
    ret = (int32_t)uapi_sfc_write(0, flash_addr, (uint8_t *)buf, write_len);
    if (ret == ERRCODE_SUCC) {
        return (int32_t)write_len;
    } else {
        return 0;
    }
}

/*
 * flash擦除数据接口
 */
errcode_t diag_adapt_flash_erase(uint8_t opt_type, const uint32_t offset, const uint32_t size)
{
    uint32_t flash_addr = 0;
    uint32_t length = 0;

    if (diag_adapt_flash_get_addr(opt_type, &flash_addr, &length) != ERRCODE_SUCC) {
        return ERRCODE_INVALID_PARAM;
    }

    if (offset >= length) {
        return 0;
    }

    uint32_t remain_len = length - offset;
    uint32_t erase_size = uapi_min(remain_len, size);
    if (erase_size % ERASE_LENGTH_4K != 0) {
        erase_size += (ERASE_LENGTH_4K - erase_size % ERASE_LENGTH_4K);
    }
    flash_addr += offset;
    errcode_t ret = uapi_sfc_erase(0, flash_addr, erase_size);
    return ret;
}