/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: UPG common functions for different chip
 */

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "errcode.h"
#include "common_def.h"
#include "upg_debug.h"
#include "partition.h"
#include "securec.h"
#include "upg_porting.h"
#include "hal_reboot.h"
#include "tcxo.h"
#include "cpu_utils.h"
#if (UPG_CFG_VERIFICATION_SUPPORT == YES)
#include "efuse.h"
#endif
#include "otp_map.h"
#include "upg_alloc.h"
#include "upg_config.h"
#include "upg_definitions_porting.h"
#include "upg_common.h"
#include "debug_print.h"
#include "cipher.h"
#include "upg_common_porting.h"

#define MS_ID_ADDR                      0xF8
#define CHIP_ID_ADDR                    OTP_CHIP_ID_START
#define TCXO_REBOOT_DELAY               500ULL
#define SSB_ANTI_ROLLBACK_MASK          0xFF0
#define SSB_ANTI_ROLLBACK_OFFSET        0x4
#define SELITEOS_ANTI_ROLLBACK_MASK     0xFFFF
#define UPG_FLASH_WDT_FEED_THRESHOLD    (0x100000)
#define UPG_TEE_IMAGE_NEED_UPGRADE      0x99

#define uapi_array_size(_array)  (sizeof(_array) / sizeof((_array)[0]))

static upg_image_partition_ids_map_t g_img_partition_map[] = {
#ifdef BUILD_APPLICATION_SSB
    {SSB_IMAGE_ID, PARTITION_SSB_IMAGE},
    {SELITEOS_IMAGE_ID, PARTITION_SELITEOS_IMAGE},
#else
    {SSB_IMAGE_ID, PARTITION_SSB_UPG_IMAGE},
    {SELITEOS_IMAGE_ID, PARTITION_SELITEOS_UPG_IMAGE},
#endif
    {RECOVERY_IMAGE_ID, PARTITION_RECOVERY_IMAGE},
    {APPLICATION_IMAGE_ID, PARTITION_ACPU_IMAGE},
    {BT_IMAGE_ID, PARTITION_BT_IMAGE},
    {DSP_MAIN_IMAGE_ID, PARTITION_DSP_MAIN_IMAGE},
    {DSP_OVERLAY_IMAGE_ID, PARTITION_DSP_OVERLAY_IMAGE},
};

/* 获取镜像ID和分区ID的map表
 * map: 镜像ID和分区ID的映射表
 * 返回map表中映射数量
 */
uint32_t upg_get_ids_map(upg_image_partition_ids_map_t **map)
{
    *map = g_img_partition_map;
    return uapi_array_size(g_img_partition_map);
}

/*
 * recovery/APP支持升级的镜像ID
 * 注意增删ID时同步更新升级ID的数量
 */
#if defined BUILD_APPLICATION_STANDARD && defined BUILD_RECOVERY_IMAGE
#define UPDATE_IMAGE_SET \
    { APPLICATION_IMAGE_ID, \
      BT_IMAGE_ID, \
      DSP_MAIN_IMAGE_ID, \
      DSP_OVERLAY_IMAGE_ID, \
      UPG_IMAGE_ID_NV, \
      UPG_IMAGE_ID_RES_INDEX, UPG_IMAGE_ID_RES_DATA }
#define UPDATE_IMAGE_SET_CNT 0x7 /* 镜像ID列表长度 */

#else
#define UPDATE_IMAGE_SET \
    { RECOVERY_IMAGE_ID, SSB_IMAGE_ID, SELITEOS_IMAGE_ID }
#define UPDATE_IMAGE_SET_CNT 0x3 /* 镜像ID列表长度 */

#endif

static uint32_t g_support_upg_id[] = UPDATE_IMAGE_SET;
static upg_image_collections_t g_upg_collect = { g_support_upg_id, UPDATE_IMAGE_SET_CNT };
/* 获取当前程序支持的升级镜像 */
upg_image_collections_t *uapi_upg_get_image_id_collection(void)
{
    return &g_upg_collect;
}

/* 获取升级包路径 */
char *upg_get_pkg_file_path(void)
{
    return UPG_FILE_NAME;
}

/* 获取资源索引路径 */
char *upg_get_res_file_index_path(void)
{
    return UPG_RES_INDEX_PATH;
}

/* 获取升级包所在目录 */
char *upg_get_pkg_file_dir(void)
{
    return UPG_FILE_PATH;
}

/*
 * 获取FOTA升级标记区的Flash起始地址，该地址为在flash上的相对地址，是相对flash基地址的偏移
 * start_address 返回升级标记区的起始地址
 */
errcode_t upg_get_upgrade_flag_flash_start_addr(uint32_t *start_address)
{
    errcode_t ret_val;
    partition_information_t info;

    ret_val = uapi_partition_get_info(PARTITION_FOTA_DATA, &info);
    if (ret_val != ERRCODE_SUCC) {
        return ret_val;
    }

    /* FOTA升级标记区在FOTA分区的最后 */
    *start_address = info.part_info.addr_info.addr + info.part_info.addr_info.size - UPG_FLASH_PAGE_SIZE;
    return ERRCODE_SUCC;
}

/*
 * 获取SSB SELITEOS 升级标记区的Flash起始地址，该地址为在flash上的相对地址，是相对flash基地址的偏移
 * start_address 返回升级标记区的起始地址
 */
errcode_t upg_get_store_flag_flash_start_addr(uint32_t *start_address)
{
    errcode_t ret_val;
    partition_information_t info;

    ret_val = uapi_partition_get_info(PARTITION_FOTA_DATA, &info);
    if (ret_val != ERRCODE_SUCC) {
        return ret_val;
    }

    /* FOTA升级标记区在FOTA分区的最后 */
    *start_address = info.part_info.addr_info.addr + info.part_info.addr_info.size - sizeof(fota_store_flag_area_t);
    return ERRCODE_SUCC;
}

errcode_t upg_get_slave_chip_upgrade_flag_flash_start_addr(uint32_t *start_address)
{
    errcode_t ret_val;
    partition_information_t info;

    ret_val = uapi_partition_get_info(PARTITION_FOTA_DATA, &info);
    if (ret_val != ERRCODE_SUCC) {
        return ret_val;
    }
    *start_address = info.part_info.addr_info.addr +
        info.part_info.addr_info.size - UPG_FLASH_PAGE_SIZE + sizeof(fota_upgrade_flag_area_t);
    return ERRCODE_SUCC;
}

/*
 * 获取FOTA升级进度恢复标记区的Flash起始地址，该地址为在flash上的相对地址，是相对flash基地址的偏移
 * start_address 返回标记区的起始地址
 * size 返回包含标记区和flag区的总长度
 */
errcode_t upg_get_progress_status_start_addr(uint32_t *start_address, uint32_t *size)
{
    partition_information_t info;
    errcode_t ret_val = uapi_partition_get_info(PARTITION_FOTA_DATA, &info);
    if (ret_val != ERRCODE_SUCC) {
        return ret_val;
    }

    *start_address = info.part_info.addr_info.addr + info.part_info.addr_info.size - UPG_UPGRADE_FLAG_LENGTH;
    *size = UPG_UPGRADE_FLAG_LENGTH;

    return ERRCODE_SUCC;
}

/*
 * 获取在Flash上预留的FOTA分区的地址和长度，该地址为在flash上的相对地址，是相对flash基地址的偏移
 * start_address 返回FOTA分区的起始地址
 * size          返回FOTA分区大小（包含升级标记区和缓存区、状态区）
 */
errcode_t upg_get_fota_partiton_area_addr(uint32_t *start_address, uint32_t *size)
{
    partition_information_t info;
    errcode_t ret_val = uapi_partition_get_info(PARTITION_FOTA_DATA, &info);
    if (ret_val != ERRCODE_SUCC) {
        return ret_val;
    }
    *start_address = info.part_info.addr_info.addr;
    *size = info.part_info.addr_info.size;
    return ERRCODE_SUCC;
}

/*
 * 重启
 */
void upg_reboot(void)
{
    /* 重启前睡眠500ms */
    uapi_tcxo_delay_ms((uint64_t)TCXO_REBOOT_DELAY);
    cpu_utils_set_system_status_by_cause(REBOOT_CAUSE_UPG_COMPLETION);
    hal_reboot_chip();
}

/*
 * 防止看门狗超时，踢狗
 */
void upg_watchdog_kick(void)
{
    uapi_watchdog_kick();
}

uint32_t upg_get_flash_base_addr(void)
{
    return FLASH_START;
}

uint32_t upg_get_flash_size(void)
{
    return UPG_FLASH_SIZE;
}

// 升级读flash接口需支持跨页读取
errcode_t upg_flash_read(const uint32_t flash_offset, const uint32_t size, uint8_t *ram_data)
{
    errcode_t ret = ERRCODE_FAIL;
    ret = uapi_sfc_read(0, flash_offset, ram_data, size);
    return ret;
}

STATIC errcode_t upg_erase_before_write(const uint32_t flash_offset, uint32_t size)
{
    uint32_t erase_offset = flash_offset;
    uint32_t erase_size = size;
    uint32_t fst_page_tail_size = UPG_FLASH_PAGE_SIZE - (flash_offset + UPG_FLASH_PAGE_SIZE) % UPG_FLASH_PAGE_SIZE;
    bool not_page_start = flash_offset % UPG_FLASH_PAGE_SIZE != 0 ? true : false;
    bool not_mult_page = fst_page_tail_size >= size ? true : false;
    if (not_page_start && not_mult_page) {
        return ERRCODE_SUCC;
    }

    if (not_page_start) {
        erase_size -= fst_page_tail_size;
        erase_offset += fst_page_tail_size;
    }

    return upg_flash_erase(erase_offset, erase_size);
}

// 升级写flash接口需支持跨页写入和写前擦功能
errcode_t upg_flash_write(const uint32_t flash_offset, uint32_t size, const uint8_t *ram_data, bool do_erase)
{
    if (size == 0) {
        return ERRCODE_SUCC;
    }
    errcode_t ret = ERRCODE_FAIL;
    uint8_t *cmp_data = upg_malloc(size);
    if (cmp_data == NULL) {
        return ERRCODE_FAIL;
    }

    if (do_erase) {
        if (upg_erase_before_write(flash_offset, size) != ERRCODE_SUCC) {
            goto erase_end;
        }
    }

    uint32_t irq_status = osal_irq_lock();
    ret = uapi_sfc_write(0, flash_offset, ram_data, size);
    if (ret != ERRCODE_SUCC) {
        goto write_end;
    }

    ret = uapi_sfc_read(0, flash_offset, cmp_data, size);
    if (ret != ERRCODE_SUCC) {
        goto write_end;
    }

write_end:
    osal_irq_restore(irq_status);
    if (memcmp(cmp_data, ram_data, size) != 0) {
        ret = ERRCODE_FAIL;
    }
erase_end:
    upg_free(cmp_data);
    return ret;
}

errcode_t upg_flash_erase(const uint32_t flash_offset, const uint32_t size)
{
    errcode_t ret = ERRCODE_FAIL;
    uint32_t irq_status = osal_irq_lock();

    for (uint32_t i = 0; i < size; i += UPG_FLASH_PAGE_SIZE) {
        uint32_t erase_offset = flash_offset + i;
        if (uapi_sfc_erase(0, erase_offset, UPG_FLASH_PAGE_SIZE) != ERRCODE_SUCC) {
            goto end;
        }
        if (i > 0 && !(i % UPG_FLASH_WDT_FEED_THRESHOLD)) {
            upg_watchdog_kick();
            upg_log_info("[UPG] flash is erasing [%d] \r\n", ((size - i) / UPG_FLASH_WDT_FEED_THRESHOLD + 1));
        }
    }

    ret = ERRCODE_SUCC;
end:
    osal_irq_restore(irq_status);
    return ret;
}

#if (UPG_CFG_VERIFICATION_SUPPORT == YES)
/*
 * 获取校验用的root_public_key
 */
uint8_t *upg_get_root_public_key(void)
{
#if (UPG_CFG_DIRECT_FLASH_ACCESS == YES)
    /* 使用Upgrader_External_Public_Key校验Key Area的签名  */
    uint32_t upgrader_address = 0;
    partition_information_t info;
    errcode_t ret_val = uapi_partition_get_info(PARTITION_UPGRADER_PRIMARY, &info);
    if (ret_val != ERRCODE_SUCC) {
        return NULL;
    }
    upgrader_address = info.part_info.addr_info.addr;
    image_key_area *upgrader_key = (image_key_area *)(uintptr_t)(upgrader_address + upg_get_flash_base_addr());
    return upgrader_key->ext_pulic_key_area;
#else
    upg_package_header_t *pkg_header = NULL;
    errcode_t ret;
    ret = upg_get_package_header(&pkg_header);
    if (ret != ERRCODE_SUCC || pkg_header == NULL) {
        upg_log_err("upg_get_package_header fail");
        return NULL;
    }
    static uint8_t public_key[PUBLIC_KEY_LEN];
    memcpy_s(public_key, sizeof(public_key), (pkg_header->key_area.fota_external_public_key), PUBLIC_KEY_LEN);
    upg_free(pkg_header);
    return public_key;
#endif
}

STATIC errcode_t check_fota_msid(const uint32_t msid_ext, const uint32_t mask_msid_ext)
{
    unused(msid_ext);
    unused(mask_msid_ext);
    uint32_t msid = 0;
    errcode_t ret = efuse_port_read_buffer_by_reload((uint8_t *)&msid, MS_ID_ADDR, sizeof(uint32_t));
    if (ret != ERRCODE_SUCC) {
        upg_log_err("[UPG] get msid failed. ret = 0x%x \r\n", ret);
    }
    upg_log_info("[UPG] msid = 0x%x \r\n", msid);
    if ((msid_ext & mask_msid_ext) != (msid & mask_msid_ext)) {
        upg_log_err("[UPG] upg verify: msid is wrong!\r\n");
    }

    return ERRCODE_SUCC; // msid地址未确定，先返回成功
}

/*
 * 检查FOTA升级包中的信息与板端是否匹配
 * pkg_header 升级包包头指针
 * 检查成功，返回 ERRCODE_SUCC
 */
errcode_t upg_check_fota_information(const upg_package_header_t *pkg_header)
{
    upg_key_area_data_t *key_area = (upg_key_area_data_t *)&(pkg_header->key_area);
    upg_fota_info_data_t *fota_info = (upg_fota_info_data_t *)&(pkg_header->info_area);
    errcode_t ret;

    ret = check_fota_msid(key_area->msid_ext, key_area->mask_msid_ext);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    ret = check_fota_msid(fota_info->msid_ext, fota_info->mask_msid_ext);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    return ERRCODE_SUCC;
}
#endif

errcode_t upg_get_efuse_addr(uint32_t image_id, uint32_t *start_addr)
{
    switch (image_id) {
        case SSB_IMAGE_ID:
            *start_addr = OTP_SSB_ROLLBACK_VERSION;
            break;
        case SELITEOS_IMAGE_ID:
            *start_addr = OTP_SELITOS_ROLLBACK_VERSION;
            break;
        case RECOVERY_IMAGE_ID:
        case APPLICATION_IMAGE_ID:
        case BT_IMAGE_ID:
        case DSP_MAIN_IMAGE_ID:
        case DSP_OVERLAY_IMAGE_ID:
            *start_addr = OTP_NOT_SUPPORT_ANTI_ROLLBACK;
            break;
        default:
            return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}

errcode_t upg_get_board_rollback_version(uint32_t image_id, uint32_t *anti_rollback_ver)
{
    uint32_t efuse_offset;
    errcode_t ret = upg_get_efuse_addr(image_id, &efuse_offset);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    ret = efuse_port_read_buffer_by_reload((uint8_t *)anti_rollback_ver, efuse_offset, sizeof(uint32_t));

    if (image_id == SSB_IMAGE_ID) {
        *anti_rollback_ver = (*anti_rollback_ver & SSB_ANTI_ROLLBACK_MASK) >> SSB_ANTI_ROLLBACK_OFFSET;
    } else if (image_id == SELITEOS_IMAGE_ID) {
        *anti_rollback_ver = *anti_rollback_ver & SELITEOS_ANTI_ROLLBACK_MASK;
    } else if (efuse_offset == OTP_NOT_SUPPORT_ANTI_ROLLBACK) {
        *anti_rollback_ver = 0;
    }

    return ret;
}

errcode_t upg_get_board_version_mask(uint32_t image_id, uint32_t *key_mask, uint32_t *code_mask)
{
    unused(image_id);
    *key_mask = 0x0;
    *code_mask = 0xFFFFFFFF;
    return ERRCODE_SUCC;
}

errcode_t upg_get_board_version(uint32_t image_id, uint32_t *key_ver, uint32_t *code_ver)
{
    unused(image_id);
    upg_package_header_t     *pkg_header = NULL;
    upg_image_hash_node_t *img_hash_table = NULL;
    upg_image_header_t *img_header = NULL;
    errcode_t ret = upg_get_package_header(&pkg_header);
    if (ret != ERRCODE_SUCC || pkg_header == NULL) {
        upg_log_err("upg_get_package_header fail\r\n");
        return ret;
    }
    ret = upg_get_pkg_image_hash_table(pkg_header, &img_hash_table);
    if (ret != ERRCODE_SUCC || img_hash_table == NULL) {
        upg_log_err("upg_get_pkg_image_hash_table fail\r\n");
        upg_free(pkg_header);
        return ret;
    }
    upg_fota_info_data_t *fota_info = (upg_fota_info_data_t *)&(pkg_header->info_area);
    for (uint32_t i = 0; i < fota_info->image_num; i++) {
        ret = upg_get_pkg_image_header((const upg_image_hash_node_t *)&(img_hash_table[i]), &img_header);
        if (ret != ERRCODE_SUCC || img_header == NULL) {
            upg_log_err("upg_get_pkg_image_header fail");
            upg_free(pkg_header);
            upg_free(img_hash_table);
            return ret;
        }
        if (img_header->image_id == image_id) {
            break;
        }
        upg_free(img_header);
        img_header = NULL;
    }
    *key_ver = pkg_header->key_area.fota_key_version_ext;
    *code_ver = img_header->version_ext;
    upg_free(pkg_header);
    upg_free(img_hash_table);
    upg_free(img_header);
    return ret;
}

errcode_t upg_set_board_rollback_version(uint32_t image_id, uint32_t *anti_rollback_ver)
{
#if BUILD_APPLICATION_STANDARD
    unused(image_id);
    unused(anti_rollback_ver);
    return ERRCODE_SUCC;
#else
    if (!(image_id == SSB_IMAGE_ID || image_id == SELITEOS_IMAGE_ID)) {
        return ERRCODE_SUCC;
    }
    uint32_t efuse_offset;
    errcode_t ret = upg_get_efuse_addr(image_id, &efuse_offset);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    uint32_t version = *anti_rollback_ver;
    if (image_id == SSB_IMAGE_ID) {
        version = (version  << SSB_ANTI_ROLLBACK_OFFSET) & SSB_ANTI_ROLLBACK_MASK;
    } else if (image_id == SELITEOS_IMAGE_ID) {
        version = version & SELITEOS_ANTI_ROLLBACK_MASK;
    }
    return uapi_efuse_write_buffer(efuse_offset, (uint8_t *)&version, sizeof(uint32_t));
#endif
}

/* 本次支持升级的镜像ID集合，若为空则无镜像限制 */
bool upg_img_in_set(uint32_t img_id)
{
    upg_image_collections_t *collect = uapi_upg_get_image_id_collection();
    if (collect == NULL || collect->img_ids_cnt == 0) {
        return true;
    }

    for (uint32_t i = 0; i < collect->img_ids_cnt; i++) {
        if (collect->img_ids[i] == img_id) {
            return true;
        }
    }
    return false;
}

/* 启动HASH计算 */
errcode_t upg_start_hash(uint32_t key_alg, uint32_t *handle)
{
    errcode_t ret;

    if (key_alg != UPG_KEY_ALG_HASH256_ONLY_ON_ECC && key_alg != UPG_KEY_ALG_ECC256_HASH256) {
        return ERRCODE_FAIL;
    }

    ret = uapi_drv_cipher_sha256_start(handle);
    if (ret != ERRCODE_SUCC) {
        upg_log_err("[UPG] uapi_drv_cipher_sha256_start fail ret = 0x%x\r\n", ret);
        return ret;
    }
    return ERRCODE_SUCC;
}

/* 更新HASH */
errcode_t upg_update_hash(uint32_t handle, const uint8_t *src, uint32_t length)
{
    errcode_t ret = uapi_drv_cipher_sha256_update(handle, src, length);
    if (ret != ERRCODE_SUCC) {
        upg_log_err("[UPG] uapi_drv_cipher_sha256_update fail ret = 0x%x\r\n", ret);
        return ret;
    }
    return ERRCODE_SUCC;
}

/* 完成HASH计算并返回HASH值 */
errcode_t upg_complete_hash(uint32_t handle, uint8_t *hash, uint32_t hash_len)
{
    errcode_t ret;

    if (hash_len < SHA_256_LENGTH) {
        return ERRCODE_INVALID_PARAM;
    }

    ret = uapi_drv_cipher_sha256_finish(handle, hash);
    if (ret != ERRCODE_SUCC) {
        uapi_drv_cipher_sha_destroy(handle);
        upg_log_err("[UPG] uapi_drv_cipher_sha256_finish failed, ret = 0x%x\n", ret);
        return ret;
    }

    ret = uapi_drv_cipher_sha_destroy(handle);
    if (ret != ERRCODE_SUCC) {
        upg_log_err("[UPG] uapi_drv_cipher_sha_destroy failed, ret = 0x%x\n", ret);
    }

    return ret;
}

/* 获取指定的校验算法 */
upg_key_alg_t upg_get_specific_verify_key_alg(void)
{
    return UPG_KEY_ALG_INVAILD;
}

errcode_t upg_detect_upgrade_image(void)
{
    upg_package_header_t *pkg_header = NULL;
    upg_image_hash_node_t *img_hash_table = NULL;
    upg_image_header_t *img_header = NULL;
    uint32_t flag_start_address = 0;
    uint32_t firmware_num = 0;
    uint32_t flash_offset = 0;

    errcode_t ret = upg_get_store_flag_flash_start_addr(&flag_start_address);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    ret = upg_get_package_header(&pkg_header);
    if (ret != ERRCODE_SUCC || pkg_header == NULL) {
        return ret;
    }

    ret = upg_get_pkg_image_hash_table(pkg_header, &img_hash_table);
    if (ret != ERRCODE_SUCC || img_hash_table == NULL) {
        upg_free(pkg_header);
        return ret;
    }

    upg_fota_info_data_t *fota_info = (upg_fota_info_data_t *)&(pkg_header->info_area);
    for (uint32_t i = 0; i < fota_info->image_num; i++) {
        ret = upg_get_pkg_image_header((const upg_image_hash_node_t *)&(img_hash_table[i]), &img_header);
        if (ret != ERRCODE_SUCC || img_header == NULL) {
            upg_free(pkg_header);
            upg_free(img_hash_table);
            return ret;
        }
        if (img_header->image_id == SSB_IMAGE_ID || img_header->image_id == SELITEOS_IMAGE_ID) {
            firmware_num++;
            /* 将新的镜像长度写入 store flag 区 */
            flash_offset = (uint32_t)flag_start_address + offsetof(fota_store_flag_area_t, new_image_len[i]);
            ret = upg_flash_write(flash_offset, sizeof(uint32_t), (uint8_t *)&img_header->new_image_len, false);
            if (ret != ERRCODE_SUCC) { goto free_res; }

            /* 将需要升级的镜像 id 写入 store flag 区 */
            flash_offset = (uint32_t)flag_start_address + offsetof(fota_store_flag_area_t, image_id[i]);
            ret = upg_flash_write(flash_offset, sizeof(uint32_t), (uint8_t *)&img_header->image_id, false);
            if (ret != ERRCODE_SUCC) { goto free_res; }

            /* 将镜像的地址偏移写入 store flag 区 */
            flash_offset = (uint32_t)flag_start_address + offsetof(fota_store_flag_area_t, image_offset[i]);
            ret = upg_flash_write(flash_offset, sizeof(uint32_t), (uint8_t *)&img_header->image_offset, false);
            if (ret != ERRCODE_SUCC) { goto free_res; }

            /* 将新的 key version 写入 store flag 区 */
            flash_offset = (uint32_t)flag_start_address + offsetof(fota_store_flag_area_t, key_ver[i]);
            ret = upg_flash_write(flash_offset, sizeof(uint32_t), (uint8_t *)&pkg_header->key_area.fota_key_version_ext, false);
            if (ret != ERRCODE_SUCC) { goto free_res; }

            /* 将新的 code version 写入 store flag 区 */
            flash_offset = (uint32_t)flag_start_address + offsetof(fota_store_flag_area_t, code_ver[i]);
            ret = upg_flash_write(flash_offset, sizeof(uint32_t), (uint8_t *)&img_header->version_ext, false);
            if (ret != ERRCODE_SUCC) { goto free_res; }
        }
        upg_free(img_header);
        img_header = NULL;
    }

    if (firmware_num > 0) {
        uint8_t need_upgrade = UPG_TEE_IMAGE_NEED_UPGRADE;
        /* 写入 need_flag 标记，ssb 阶段读取到该标志开始升级 */
        flash_offset = (uint32_t)flag_start_address + offsetof(fota_store_flag_area_t, need_upgrade);
        ret = upg_flash_write(flash_offset, sizeof(uint8_t), (uint8_t *)&need_upgrade, false);
    }

    /* 写入firmware_num */
    flash_offset = (uint32_t)flag_start_address + offsetof(fota_store_flag_area_t, firmware_num);
    ret = upg_flash_write(flash_offset, sizeof(uint32_t), (uint8_t *)&firmware_num, false);

free_res:
    upg_free(img_header);
    upg_free(pkg_header);
    upg_free(img_hash_table);
    return ret;
}