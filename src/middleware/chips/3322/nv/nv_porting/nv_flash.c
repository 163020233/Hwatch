/*================================================================
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
*
* Description: NV on different flash
* Author: @CompanyNameTag
* History:
* 2023-10-11， Create file.
================================================================*/

#include "memory_config.h"
#include "nv_porting.h"
#include "sfc_v2.h"
#include "soc_osal.h"
#include "std_def.h"

#if ((CONFIG_NV_SUPPORT_FILESYSTEM == NV_YES))
#define NV_FLASH_OFFSET     (NV_STATR_ADDR - FLASH_START)
#define NV_FILE_PATH        "user/nv/3322_all_nv.bin"
#endif

#define FLASH_WORD_ALIGN 4

static uint32_t flash_read_data_align(const uint32_t *addr, uint32_t *length, uint8_t *first_cover_count,
                                      uint8_t *last_cover_count)
{
    uint32_t dest_addr;
    /* Check if the address is 4 bytes aligned. */
    if (((*addr) & 0x3) != 0) {
        /* The actual send address requires 4 byte alignment. */
        dest_addr = ((*addr) & 0xFFFFFFFC);

        /* Fill the first 4 bytes of data. */
        *first_cover_count = (*addr) & 0x3;
        (*length) += (*first_cover_count);
    } else {
        dest_addr = (*addr);
    }
    /* Check if the last 4 bytes are aligned. */
    if (((*length) & 0x3) != 0) {
        /* Fill the first 4 bytes of data. */
        *last_cover_count = FLASH_WORD_ALIGN - (uint8_t)((*length) & 0x3);
        (*length) += (*last_cover_count);
    }
    return dest_addr;
}

errcode_t kv_flash_read(const uint32_t flash_offset, const uint32_t size, uint8_t *ram_data)
{
    errcode_t ret = ERRCODE_SUCC;
    uint32_t read_size = size;
    uint8_t first_cover_count = 0;
    uint8_t last_cover_count = 0;
    uint32_t dest_addr = flash_read_data_align(&flash_offset, &read_size, &first_cover_count, &last_cover_count);
    uint8_t *align_read_buff = (uint8_t *)kv_malloc(read_size);
    if (align_read_buff == NULL) {
        return ERRCODE_MALLOC;
    }

#if ((CONFIG_NV_SUPPORT_FILESYSTEM == NV_YES))
    int len = 0;
    uint32_t file_offset = dest_addr - NV_FLASH_OFFSET;
    len = kv_file_read(NV_FILE_PATH, file_offset, (uint8_t*)align_read_buff, size);
    if (len < 0) {
        nv_log_err("[NV] error:read nv file fail! file_offset: %#x\n", file_offset);
    }
#else
    uint32_t mstatus = osal_irq_lock();
    if (uapi_sfc_read(0, dest_addr, align_read_buff, read_size) != ERRCODE_SUCC) {
        ret = ERRCODE_FAIL;
    }
    osal_irq_restore(mstatus);
#endif

    if (memcpy_s((uint8_t *)ram_data, size, (uint8_t *)(align_read_buff + first_cover_count), size) != EOK) {
        ret = ERRCODE_FAIL;
    }
    kv_free(align_read_buff);
    return ret;
}

errcode_t kv_flash_write(const uint32_t flash_offset, uint32_t size, const uint8_t *ram_data, bool do_erase)
{
    (void)do_erase;
    errcode_t ret = ERRCODE_SUCC;

    uint32_t write_size = size;
    uint8_t first_cover_count = 0;
    uint8_t last_cover_count = 0;
    uint32_t dest_addr = flash_read_data_align(&flash_offset, &write_size, &first_cover_count, &last_cover_count);

    uint8_t *align_write_buff = (uint8_t *)kv_malloc(write_size);
    if (align_write_buff == NULL) {
        return ERRCODE_MALLOC;
    }

#if ((CONFIG_NV_SUPPORT_FILESYSTEM == NV_YES))
    int len = 0;
    uint32_t file_offset = dest_addr - NV_FLASH_OFFSET;

    len = kv_file_read(NV_FILE_PATH, file_offset, (uint8_t*)align_write_buff, write_size);
    if (len < 0) {
        nv_log_err("[NV] read fail! file_offset: %#x\n", file_offset);
    }

    if (memcpy_s((uint8_t *)(align_write_buff + first_cover_count), write_size, (uint8_t *)ram_data, size) != EOK) {
        ret = ERRCODE_FAIL;
    }

    uint32_t result = kv_file_write(NV_FILE_PATH, file_offset, align_write_buff, write_size);
    if (result == write_size) {
        nv_log_info("[NV] Write successful: %d bytes written.\n", result);
    } else {
        nv_log_err("[NV] Write failed or partial write. Result: %d\n", result);
    }

    kv_free(align_write_buff);
#else
    uint32_t mstatus = osal_irq_lock();
    if (uapi_sfc_read(0, dest_addr, align_write_buff, write_size) != ERRCODE_SUCC) {
        ret = ERRCODE_FAIL;
        goto flash_err;
    }
    osal_irq_restore(mstatus);
    if (memcpy_s((uint8_t *)(align_write_buff + first_cover_count), write_size, (uint8_t *)ram_data, size) != EOK) {
        ret = ERRCODE_FAIL;
    }

    mstatus = osal_irq_lock();
    if (uapi_sfc_write(0, dest_addr, (uint8_t *)align_write_buff, write_size) !=
        ERRCODE_SUCC) {
        ret = ERRCODE_FAIL;
    }
flash_err:
    osal_irq_restore(mstatus);
    kv_free(align_write_buff);
#endif
    return ret;
}

errcode_t kv_flash_erase(const uint32_t flash_offset, uint32_t size)
{
    UNUSED(size);
    errcode_t ret = ERRCODE_SUCC;
#if ((CONFIG_NV_SUPPORT_FILESYSTEM == NV_YES))
    UNUSED(flash_offset);
#else
    uint32_t mstatus = osal_irq_lock();

    ret = uapi_sfc_erase(0, flash_offset, size);
    if (ret != ERRCODE_SUCC) {
        ret = ERRCODE_FAIL;
    }
    osal_irq_restore(mstatus);
#endif
    return ret;
}