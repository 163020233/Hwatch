/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2025. All rights reserved.
 * Description: update (SSB).
 * Author: HiSilicon
 * Create: 2023-10-31
 */
#include "upg_definitions.h"
#include "partition.h"
#include "sfc_v2.h"
#include "platform_types.h"
#include "debug_print.h"
#include "upg_common.h"
#include "chip_io.h"
#include "hal_fapc.h"
#include "secure_verify_boot.h"
#include "km.h"
#include "security_km.h"
#include "main.h"

#define UPG_FLAG_DATA_LENGTH         0x1000
#define UPG_FLAG_OFFSET_OF_FOTA_DATA (FOTA_FLASH_REGION_LENGTH - UPG_FLAG_DATA_LENGTH)
#define UPG_UNFINISHED_FLAG 0xFFFFFFFF

#define NOT_START_FLAG              0xFF
#define STARTED_FLAG                0x0F
#define FINISHED_FLAG               0x00
#define UPG_NEED_UPGRADE_FLAG       0x99
#define UPG_NOT_NEED_UPGRADE_FLAG   0x66
#define UPG_FINISH_HALF_FLAG        0xFFFF
#define UPG_FINISH_ALL_FLAG         0
#define UPG_ABNORMAL_FLAG           0x5a5a5a5a
#define UPG_FLASH_PAGE_SIZE         FLASH_PAGE_SIZE /* Bytes per sector */

#define UPG_MEM_SIZE0 0x200
#define UPG_MEM_SIZE1 0x1000
#define UPG_MEM_SIZE2 0x1800
#define UPG_MEM_NUM_2  2
#define UPG_MEM_NUM_5  5

typedef struct {
    uint16_t used;
    uint16_t size;
    uint8_t data[UPG_MEM_SIZE0];
} upg_mem0_t;
typedef struct {
    uint16_t used;
    uint16_t size;
    uint8_t data[UPG_MEM_SIZE1];
} upg_mem1_t;
typedef struct {
    uint16_t used;
    uint16_t size;
    uint8_t data[UPG_MEM_SIZE2];
} upg_mem2_t;

static upg_mem0_t g_upg_mem0[UPG_MEM_NUM_5] = {{0}};
static upg_mem1_t g_upg_mem1[UPG_MEM_NUM_5] = {{0}};
static upg_mem2_t g_upg_mem2[UPG_MEM_NUM_2] = {{0}};

typedef struct fota_store_flag_area {
    uint32_t firmware_num;
    uint8_t  firmware_flag[UPG_FIRMWARE_MAX_NUM][UPG_FLAG_RETYR_TIMES];
    uint16_t update_result;
    uint32_t complete_flag;
    uint32_t new_image_len[UPG_FIRMWARE_MAX_NUM];
    uint32_t image_id[UPG_FIRMWARE_MAX_NUM];
    uint32_t image_offset[UPG_FIRMWARE_MAX_NUM];
    uint8_t  need_upgrade;
    uint32_t key_ver[UPG_FIRMWARE_MAX_NUM];
    uint32_t code_ver[UPG_FIRMWARE_MAX_NUM];
} fota_store_flag_area_t;

void upg_set_efuse_protect(bool need_open)
{
    if (need_open) {
        reg16_clrbit(TEE_CTL_RB_TZPC_PROT_CFG_REG, TEE_CTL_RB_EFUSE_TZPC_PROT_CFG_OFFSET);
    } else {
        reg16_setbit(TEE_CTL_RB_TZPC_PROT_CFG_REG, TEE_CTL_RB_EFUSE_TZPC_PROT_CFG_OFFSET);
    }
}

static bool upg_need_upgrade(fota_upgrade_flag_area_t *upg_flag_info)
{
    bool ret = false;
    uint32_t fota_address = 0;
    partition_information_t info;
    sfc_info_t flash_info = {0};

    uapi_sfc_get_flash_info(0, &flash_info);
    errcode_t ret_val = uapi_partition_get_info(PARTITION_FOTA_DATA, &info);
    if (ret_val != ERRCODE_SUCC || info.type != PARTITION_BY_ADDRESS) {
        return false;
    }
    fota_address = info.part_info.addr_info.addr;
    fota_address += UPG_FLAG_OFFSET_OF_FOTA_DATA;
    non_os_enter_critical();
    ret_val = uapi_sfc_read(0, fota_address, (uint8_t *)(upg_flag_info), sizeof(fota_upgrade_flag_area_t));
    if (ret_val != ERRCODE_SUCC) {
        goto end;
    }

    if (!(upg_flag_info->head_magic == UPG_HEAD_MAGIC &&
          upg_flag_info->head_end_magic == UPG_END_MAGIC &&
          upg_flag_info->complete_flag != 0)) {
        /* 不需要升级直接返回 */
        PRINT("No need to upgrade...\r\n");
        goto end;
    }
    ret = true;
end:
    non_os_exit_critical();
    return ret;
}

static bool upg_cat1_need_upgrade(void)
{
#define UPG_CAT1_START_MAGIC            0x5A5A5A5A
    uint32_t start_addr = 0;
    uint32_t value = 0;
    upg_get_slave_chip_upgrade_flag_flash_start_addr(&start_addr);
    uint32_t ret_val = uapi_sfc_read(0, start_addr, (uint8_t *)&value, sizeof(uint32_t));
    if (ret_val == ERRCODE_SUCC && value == UPG_CAT1_START_MAGIC) {
        return true;
    }
    return false;
}

core_images_e ssb_get_jump_image(void)
{
    // cat1需要升级, 跳转Recovery
    if (upg_cat1_need_upgrade()) {
        return CORE_IMAGES_RECOVERY;
    }
    fota_upgrade_flag_area_t upg_flag_info;
    // 不需要升级，跳转APP
    if (!upg_need_upgrade(&upg_flag_info)) {
        return CORE_IMAGES_APPS;
    }
    // APP升级未完成，跳转APP
    if (upg_flag_info.complete_flag == UPG_UNFINISHED_FLAG) {
        return CORE_IMAGES_APPS;
    }
    // recovery升级未完成，跳转Recovery
    return CORE_IMAGES_RECOVERY;
}

/*
 * 获取SSB SELITEOS 升级标记区的Flash起始地址，该地址为在flash上的相对地址，是相对flash基地址的偏移
 * start_address 返回升级标记区的起始地址
 */
static errcode_t boot_get_store_flag_flash_start_addr(uint32_t *start_address)
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

/* 获取 boot 升级标记结构到RAM中 */
errcode_t boot_alloc_and_get_upgrade_flag(fota_store_flag_area_t **upg_flag)
{
    uint32_t start_addr = 0;
    errcode_t ret_val;

    ret_val = boot_get_store_flag_flash_start_addr(&start_addr);
    if (ret_val != ERRCODE_SUCC) {
        return ret_val;
    }

    *upg_flag = (fota_store_flag_area_t*)upg_malloc(sizeof(fota_store_flag_area_t));
    if (*upg_flag == NULL) {
        return ERRCODE_MALLOC;
    }

    ret_val = upg_flash_read(start_addr, sizeof(fota_store_flag_area_t), (uint8_t *)(*upg_flag));
    if (ret_val != ERRCODE_SUCC) {
        upg_free(*upg_flag);
        *upg_flag = NULL;
        return ret_val;
    }

    return ERRCODE_SUCC;
}

/*
 * 获取升级包中的升级镜像的升级标记状态(NOT_STARTED/STARTED/RETRY/FINISHED)
 * firmware_index为升级包中的固件（除NV之外的镜像）的序号
 * 如升级包中包含 固件0、固件1、固件2、NV、固件3，其中固件3的序号为3而不是4
 * 如为NV镜像，firmware_index参数可忽略
 */
static upg_image_status_t boot_get_image_update_status(fota_store_flag_area_t *upg_flag,
    uint32_t firmware_index)
{
    uint8_t *firmware_flag;
    uint8_t finished_flag[UPG_FLAG_RETYR_TIMES] = { FINISHED_FLAG, FINISHED_FLAG, FINISHED_FLAG };
    uint8_t init_flag[UPG_FLAG_RETYR_TIMES] = { NOT_START_FLAG, NOT_START_FLAG, NOT_START_FLAG };
    uint32_t i;

    firmware_flag = upg_flag->firmware_flag[firmware_index];

    if (memcmp(firmware_flag, finished_flag, UPG_FLAG_RETYR_TIMES) == 0) {
        /* 3个flag都是0x00, 处理完成状态 */
        return UPG_IMAGE_STATUS_FINISHED;
    } else if (memcmp(firmware_flag, init_flag, UPG_FLAG_RETYR_TIMES) == 0) {
        /* 3个flag都是0xFF, 未开始处理状态 */
        return UPG_IMAGE_STATUS_NOT_STARTED;
    } else {
        /* 获取第一个非0的flag，即当前正在处理的第i次重试的flag */
        for (i = 0; i < UPG_FLAG_RETYR_TIMES; i++) {
            if (firmware_flag[i] != 0) {
                break;
            }
        }

        if (firmware_flag[i] == STARTED_FLAG) {
            return UPG_IMAGE_STATUS_STARTED;
        } else if (firmware_flag[i] == NOT_START_FLAG) {
            return UPG_IMAGE_STATUS_RETRY;
        } else {
            return UPG_IMAGE_STATUS_INVALID;
        }
    }
}

static void boot_get_package_info(fota_store_flag_area_t *upg_flag, upg_package_info_t *pkg_info)
{
    int32_t firmware_index;
    uint32_t firmware_num;

    pkg_info->total_new_fw_size = 0;
    pkg_info->finished_fw_size = 0;

    firmware_num = upg_flag->firmware_num;

    for (firmware_index = 0; firmware_index < (int32_t)firmware_num; firmware_index++) {
        pkg_info->total_new_fw_size += upg_flag->new_image_len[firmware_index];

        upg_image_status_t status;
        status = boot_get_image_update_status(upg_flag, (uint32_t)firmware_index);
        if (status == UPG_IMAGE_STATUS_FINISHED) {
            pkg_info->finished_fw_size += upg_flag->new_image_len[firmware_index];
        }
    }

    return;
}

static errcode_t boot_get_firmware_flag_address(uint32_t firmware_index, uint32_t current_loop, uint32_t *flag_addr)
{
    uint32_t fota_flag_addr = 0;
    uint32_t addr;

    errcode_t ret = boot_get_store_flag_flash_start_addr(&fota_flag_addr);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    addr = fota_flag_addr + offsetof(fota_store_flag_area_t, firmware_flag[firmware_index]);
    addr += (firmware_index * UPG_FLAG_RETYR_TIMES) + (current_loop * (uint32_t)sizeof(uint8_t));

    *flag_addr = addr;
    return ERRCODE_SUCC;
}

/*
 * 设置指定固件的升级标记
 * firmware_index为升级包中的固件（除NV之外的镜像）的序号
 * 如升级包中包含 固件0、固件1、固件2、NV、固件3，其中固件3的序号为3而不是4
 */
static errcode_t boot_set_firmware_update_status(fota_store_flag_area_t *upg_flag,
    uint32_t firmware_index, upg_image_status_switch_t switch_status)
{
    uint32_t write_len = 0;
    uint32_t flag_addr = 0;
    uint8_t *firmware_flag;
    uint32_t i;

    firmware_flag = upg_flag->firmware_flag[firmware_index];

    /* 获取第一个非0的flag, 即第i次重试的flag */
    for (i = 0; i < UPG_FLAG_RETYR_TIMES; i++) {
        if (firmware_flag[i] != 0) {
            break;
        }
    }

    if (i >= UPG_FLAG_RETYR_TIMES) {
        return ERRCODE_SUCC;
    }

    errcode_t ret = boot_get_firmware_flag_address(firmware_index, i, &flag_addr);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    if ((switch_status == UPG_IMAGE_STATUS_SWITCH_TO_STARTED) && (firmware_flag[i] == NOT_START_FLAG)) {
        /* 当前flag为NOT_START, 要切换成STARTED */
        write_len = 1;
        firmware_flag[i] = STARTED_FLAG;
    } else if ((switch_status == UPG_IMAGE_STATUS_SWITCH_TO_RETRY) && (firmware_flag[i] == STARTED_FLAG)) {
        /* 当前flag为STARTED, 要切换成RETRY, 即当前flag置为0 */
        write_len = 1;
        firmware_flag[i] = FINISHED_FLAG;
    } else if (switch_status == UPG_IMAGE_STATUS_SWITCH_TO_FINISHED) {
        /* 要切换成FINISH，即全部flag置为0 */
        write_len = UPG_FLAG_RETYR_TIMES - i;
        (void)memset_s(&firmware_flag[i], write_len, FINISHED_FLAG, write_len);
    } else {
        /* 其他情况不做处理 */
        return ERRCODE_SUCC;
    }

    ret = upg_flash_write(flag_addr, write_len, (uint8_t *)(&(firmware_flag[i])), false);
    if (ret != ERRCODE_SUCC) {
        PRINT("[UPG] upg_flash_write upgrader flag fail. ret = 0x%x\r\n", ret);
        return ret;
    }

    if (i >= UPG_FLAG_RETYR_TIMES - 1 && firmware_flag[i] == FINISHED_FLAG) {
        PRINT("[UPG] retry times all failed\r\n");
        upg_set_temporary_result(UPG_RESULT_RETRY_ALL_FAILED);
        return ERRCODE_UPG_RETRY_ALL_FAIL;
    }
    return ERRCODE_SUCC;
}

errcode_t boot_get_partition_area_addr(uint32_t *start_address, uint32_t *size, partition_ids_t ids)
{
    partition_information_t info;
    errcode_t ret_val = uapi_partition_get_info(ids, &info);
    if (ret_val != ERRCODE_SUCC) {
        return ret_val;
    }
    *start_address = info.part_info.addr_info.addr;
    *size = info.part_info.addr_info.size;
    return ERRCODE_SUCC;
}

/*
 * 读取升级包中的数据到buffer中
 * read_offset 相对升级包开头的偏移
 * buffer      读取数据buffer指针
 * read_len    输入buffer的长度，输出实际读到的数据长度
 */
errcode_t boot_read_pkg_data(uint32_t read_offset, uint8_t *buffer, uint32_t *read_len, partition_ids_t ids)
{
    errcode_t ret_val;
    uint32_t start_addr = 0;
    uint32_t size = 0;
    uint32_t actual_len;

    ret_val = boot_get_partition_area_addr(&start_addr, &size, ids);
    if (ret_val != ERRCODE_SUCC) {
        return ret_val;
    }

    if (read_offset >= size || *read_len == 0) {
        return ERRCODE_UPG_INVALID_PARAMETER;
    }

    actual_len = ((read_offset + *read_len) > size) ? (size - read_offset) : *read_len;
    start_addr += read_offset;

    ret_val = upg_flash_read(start_addr, actual_len, (uint8_t *)buffer);
    if (ret_val != ERRCODE_SUCC) {
        return ret_val;
    }

    *read_len = actual_len;

    return ERRCODE_SUCC;
}

/* 执行指定固件的更新任务 */
static errcode_t boot_perform_image_task(fota_store_flag_area_t *upg_flag, uint32_t firmware_index)
{
    uint32_t read_offset = 0;
    errcode_t ret = ERRCODE_SUCC;
    uint32_t write_offset = 0;
    uint32_t app_address = 0;
    uint32_t app_size = 0;
    partition_ids_t partition_id;
    errcode_t ret_val = upg_get_partition_info(upg_flag->image_id[firmware_index], &app_address, &app_size);
    if (ret_val != ERRCODE_SUCC) { return ret_val; }

    if (upg_flag->image_id[firmware_index] == SSB_IMAGE_ID) {
        partition_id = PARTITION_SSB_UPG_IMAGE;
    } else if (upg_flag->image_id[firmware_index] == SELITEOS_IMAGE_ID) {
        partition_id = PARTITION_SELITEOS_UPG_IMAGE;
    } else {
        return ERRCODE_FAIL;
    }

    uint32_t read_len = 0;
    uint32_t tmp_len = 0;

    uint8_t *buffer = (uint8_t *)upg_malloc(UPG_FLASH_PAGE_SIZE);
    if (buffer == NULL) { return ERRCODE_MALLOC; }
    while (read_len < upg_flag->new_image_len[firmware_index]) {
        tmp_len = ((upg_flag->new_image_len[firmware_index] - read_len) > UPG_FLASH_PAGE_SIZE) ?
            UPG_FLASH_PAGE_SIZE : (upg_flag->new_image_len[firmware_index] - read_len);
        ret = boot_read_pkg_data(read_offset + read_len, (uint8_t *)buffer, &tmp_len, partition_id);
        if (ret != ERRCODE_SUCC) {
            upg_free(buffer);
            return ret;
        }
        ret = upg_write_new_image_data(write_offset + read_len,
            (uint8_t *)buffer, &tmp_len, upg_flag->image_id[firmware_index], true);
        if (ret != ERRCODE_SUCC || tmp_len == 0) {
            upg_free(buffer);
            return ret;
        }
        read_len += tmp_len;
        // 踢狗
        upg_watchdog_kick();
    }

    upg_free(buffer);
    buffer = NULL;

    return ret;
}

static errcode_t boot_anti_rollback_version_verify(uint32_t image_id, uint32_t key_ver, uint32_t code_ver)
{
    if (!(image_id == SSB_IMAGE_ID || image_id == SELITEOS_IMAGE_ID)) {
        return ERRCODE_UPG_INVALID_PARAMETER;
    }

    uint32_t otp_ver = 0;

    errcode_t ret = upg_get_board_rollback_version(image_id, &otp_ver);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    uint32_t board_key_mask;
    uint32_t board_code_mask;

    ret = upg_get_board_version_mask(image_id, &board_key_mask, &board_code_mask);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    PRINT("[UPG] upg verify: old_key_ver[0x%x]->key_ver[0x%x], old_code_ver[0x%x]->code_ver[0x%x]\r\n",
        (otp_ver & board_key_mask), (key_ver & board_key_mask),
        (otp_ver & board_code_mask), (code_ver & board_code_mask));

    if (((key_ver & board_key_mask) < (otp_ver & board_key_mask)) ||
        ((code_ver & board_code_mask) < (otp_ver & board_code_mask))) {
        return ERRCODE_UPG_VERSION_ERROR;
    }
    return ERRCODE_SUCC;
}

static errcode_t boot_anti_rollback_version_update(uint32_t image_id, uint32_t key_ver, uint32_t code_ver)
{
    if (!(image_id == SSB_IMAGE_ID || image_id == SELITEOS_IMAGE_ID)) {
        return ERRCODE_UPG_INVALID_PARAMETER;
    }

    uint32_t board_key_mask = 0;
    uint32_t board_code_mask = 0;
    uint32_t new_ver;
    uint32_t old_ver = 0;

    errcode_t ret = upg_get_board_rollback_version(image_id, &old_ver);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    // 获取更新后的板端的掩码
    ret = upg_get_board_version_mask(image_id, &board_key_mask, &board_code_mask);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    if ((code_ver & board_code_mask) < (old_ver & board_code_mask) ||
        (key_ver & board_key_mask) < (old_ver & board_key_mask)) {
        return ERRCODE_UPG_VERSION_ERROR;
    }

    new_ver = (code_ver & board_code_mask) | (key_ver & board_key_mask);
    return upg_set_board_rollback_version(image_id, &new_ver);
}

static errcode_t boot_perform_upgrade_task(uint32_t firmware_index,
    upg_image_status_t status, fota_store_flag_area_t *upg_flag)
{
    errcode_t ret;
    upg_image_status_switch_t switch_status;

    if (status == UPG_IMAGE_STATUS_NOT_STARTED) {
        ret = upg_flash_erase_metadata_pages();
        if (ret != ERRCODE_SUCC) {
            PRINT("[UPG] upg_flash_erase_metadata_pages fail.\r\n");
            goto end;
        }
    }

    ret = boot_perform_image_task(upg_flag, firmware_index);
end:
    if (ret == ERRCODE_SUCC) {
        /* 升级成功，设置升级标记为FINISH */
        switch_status = UPG_IMAGE_STATUS_SWITCH_TO_FINISHED;
        (void)boot_anti_rollback_version_update(upg_flag->image_id[firmware_index],
            upg_flag->key_ver[firmware_index],
            upg_flag->code_ver[firmware_index]);
    } else {
        /* 升级不成功，设置升级标记为RETRY */
        switch_status = UPG_IMAGE_STATUS_SWITCH_TO_RETRY;
    }

    PRINT("[UPG] switch status = 0x%x\r\n", (uint32_t)switch_status);
    errcode_t status_ret = boot_set_firmware_update_status(upg_flag, firmware_index, switch_status);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    return status_ret;
}

static errcode_t boot_process_update_image_tasks(fota_store_flag_area_t *upg_flag)
{
    int32_t fw_idx;
    errcode_t ret = ERRCODE_SUCC;

    for (fw_idx = 0; fw_idx < (int32_t)upg_flag->firmware_num; fw_idx++) {
        upg_image_status_t status = boot_get_image_update_status(upg_flag, (uint32_t)fw_idx);
        /* 如果该Image已经完成，处理下一个image */
        if (status == UPG_IMAGE_STATUS_FINISHED) {
            PRINT("[UPG] The image has finished. image_id = 0x%x\r\n", upg_flag->image_id[fw_idx]);
            continue;
        }

        /* 设置升级标记为STARTED */
        ret = boot_set_firmware_update_status(upg_flag, (uint32_t)fw_idx, UPG_IMAGE_STATUS_SWITCH_TO_STARTED);
        if (ret != ERRCODE_SUCC) {
            PRINT("[UPG] boot_set_firmware_update_status fail\r\n");
            goto ret_free;
        }
        PRINT("[UPG] start perform update image : 0x%x\r\n", upg_flag->image_id[fw_idx]);
        ret = boot_perform_upgrade_task((uint32_t)fw_idx, status, upg_flag);
        PRINT("[UPG] perform update image over. ret = 0x%x\r\n", ret);

        if (ret != ERRCODE_SUCC) {
            goto ret_free;
        }
        upg_watchdog_kick();
    }
ret_free:
    return ret;
}

static errcode_t boot_process_update(fota_store_flag_area_t *upg_flag)
{
    errcode_t ret_val = ERRCODE_SUCC;
    PRINT("[UPG] update firmware number = 0x%x\r\n", upg_flag->firmware_num);

    ret_val = boot_process_update_image_tasks(upg_flag);
    if (ret_val != ERRCODE_SUCC) {
        PRINT("[UPG] upg_process_update_image_tasks fail, ret = 0x%x\r\n", ret_val);
    }

    return ret_val;
}

static bool boot_check_image_update_complete(const fota_store_flag_area_t *upg_flag)
{
    uint32_t firmware_index;
    const uint8_t *firmware_flag;
    uint8_t finish_flag[UPG_FLAG_RETYR_TIMES] = {FINISHED_FLAG, FINISHED_FLAG, FINISHED_FLAG};
    for (firmware_index = 0; firmware_index < upg_flag->firmware_num; firmware_index++) {
        firmware_flag = upg_flag->firmware_flag[firmware_index];
        /* 存在升级包未完成升级 */
        if (memcmp(firmware_flag, finish_flag, UPG_FLAG_RETYR_TIMES) != 0) {
            return false;
        }
    }
    return true;
}

static void boot_write_complete_flag(uint32_t fota_flag_addr, uint32_t complete_flag)
{
    if (complete_flag != UPG_ABNORMAL_FLAG) {
        uint32_t flag_addr = fota_flag_addr + offsetof(fota_store_flag_area_t, complete_flag);
        PRINT("[UPG] write complete: 0x%x\r\n", complete_flag);
        if (upg_flash_write(flag_addr, sizeof(uint32_t), (uint8_t *)&(complete_flag), false) != ERRCODE_SUCC) {
            return;
        }
    }
}

/* 设置升级完成标记 */
static void boot_set_complete_flag(errcode_t result, bool direct_finish)
{
    uint32_t fota_flag_addr = 0;
    uint32_t complete_flag = UPG_ABNORMAL_FLAG;
    bool image_complete = false;

    errcode_t ret = boot_get_store_flag_flash_start_addr(&fota_flag_addr);
    if (ret != ERRCODE_SUCC) {
        return;
    }

    fota_store_flag_area_t upg_flag;
    ret = upg_flash_read(fota_flag_addr, sizeof(fota_store_flag_area_t), (uint8_t *)(&upg_flag));
    if (ret != ERRCODE_SUCC) {
        PRINT("[UPG] upg_flash_read flag fail\r\n");
        return;
    }

    do {
        /*
         * 1. 当前程序中升级流程返回失败，直接结束升级流程（如整包校验失败），
         *    则升级流程失败，结束升级，将flag置成 UPG_FINISH_ALL_FLAG；
         */
        if (direct_finish && (upg_flag.complete_flag != UPG_FINISH_ALL_FLAG)) {
            complete_flag = UPG_FINISH_ALL_FLAG;
            /* 保存升级结果至升级标记区内 */
            upg_set_update_result(upg_get_temporary_result(), false);
            break;
        }

        /*
         * 2. 当前程序中升级流程返回成功，
         *    即当前程序中支持升级的所有镜像成功升级，但存在还需要升级的镜像，则将flag置成 UPG_FINISH_HALF_FLAG；
         */
        image_complete = boot_check_image_update_complete(&upg_flag);
        if (result == ERRCODE_SUCC && !image_complete) {
            if (upg_flag.complete_flag != UPG_FINISH_HALF_FLAG) {
                complete_flag = UPG_FINISH_HALF_FLAG;
            }
        } else if (image_complete && (upg_flag.complete_flag != UPG_FINISH_ALL_FLAG)) {
            /*
             * 3. 若升级包中所有镜像成功升级，将flag置成 UPG_FINISH_ALL_FLAG；
             */
            complete_flag = UPG_FINISH_ALL_FLAG;
            if (result == ERRCODE_SUCC) {
                upg_set_temporary_result(UPG_RESULT_UPDATE_SUCCESS);
            }
            /* 保存升级结果至升级标记区内 */
            upg_set_update_result(upg_get_temporary_result(), false);
        } else if (upg_get_temporary_result() == UPG_RESULT_RETRY_ALL_FAILED &&
            (upg_flag.complete_flag != UPG_FINISH_ALL_FLAG)) {
            /*
             * 4. 当前程序中升级流程返回失败，但最后一个已处理的镜像的3个flag全0，即已经重试最大次数，仍然失败，
             *    则升级流程失败，结束升级，将flag置成 UPG_FINISH_ALL_FLAG；
             */
            complete_flag = UPG_FINISH_ALL_FLAG;
            upg_set_update_result(upg_get_temporary_result(), false);
        }
    } while (0);
    /* 升级标记需要修改 */
    boot_write_complete_flag(fota_flag_addr, complete_flag);
}

static errcode_t upg_verify_new_image(uint32_t image_id, bool exist_ssb_image)
{
    errcode_t ret = ERRCODE_FAIL;
    partition_information_t  upg_store_info;
    switch (image_id) {
        case SSB_IMAGE_ID:
            (void)uapi_partition_get_info(PARTITION_SSB_UPG_IMAGE, &upg_store_info);
            return secure_verify_image(OEM_FLASH_BOOT_UPG_TYPE, upg_store_info.part_info.addr_info.addr + FLASH_START, \
                                       ROOT_PUBLIC_KEY_START);
        case SELITEOS_IMAGE_ID:
            (void)uapi_partition_get_info(PARTITION_SELITEOS_UPG_IMAGE, &upg_store_info);
            tee_flash_online_decryption_config((upg_store_info.part_info.addr_info.addr + FLASH_START + FLASH_SIGN_HEADER), \
                                               (upg_store_info.part_info.addr_info.addr + FLASH_START + UPG_SELITEOS_FLASH_REGION_LENGTH));
            ret = memcpy_s((void *)(uintptr_t)UPG_SELITEOS_DTCM_ORIGIN, UPG_SELITEOS_DTCM_LENGTH,
                           (void *)(uintptr_t)(upg_store_info.part_info.addr_info.addr + FLASH_START), UPG_SELITEOS_DTCM_LENGTH);
            if (ret != ERRCODE_SUCC) {
                PRINT("[upg] memcpy_s error, SELITEOS_FLASH_REGION_START = %#x, sec_ret = %d", \
                      upg_store_info.part_info.addr_info.addr + FLASH_START, ret);
                return ret;
            }
            if (exist_ssb_image) {
                return secure_verify_image(OEM_TEE_FIRMWARE_UPG_TYPE, UPG_SELITEOS_DTCM_ORIGIN, \
                                           upg_store_info.part_info.addr_info.addr + FLASH_START - UPG_FLASHBOOT_FLASH_REGION_LENGTH);
            } else {
                return secure_verify_image(OEM_TEE_FIRMWARE_UPG_TYPE, UPG_SELITEOS_DTCM_ORIGIN, SSB_FLASH_REGION_START);
            }
        default:
            PRINT("[UPG] image id check error! id == %#x\r\n", image_id);
            return ret;
    }
    return ret;
}

static errcode_t boot_check_need_upgrade(fota_store_flag_area_t *upg_flag_info)
{
    uint32_t fota_address = 0;
    partition_information_t info;
    bool exist_ssb_image = false;
    errcode_t ret_val = uapi_partition_get_info(PARTITION_FOTA_DATA, &info);
    if (ret_val != ERRCODE_SUCC || info.type != PARTITION_BY_ADDRESS) {
        return ret_val;
    }

    fota_address = info.part_info.addr_info.addr;
    fota_upgrade_flag_area_t upg_flag_info_pri;
    fota_address += UPG_FLAG_OFFSET_OF_FOTA_DATA;
    ret_val = uapi_sfc_read(0, fota_address, (uint8_t *)(&upg_flag_info_pri), sizeof(fota_upgrade_flag_area_t));
    if (ret_val != ERRCODE_SUCC) {
        return ret_val;
    }

    /* 判断升级区有没有升级包 */
    if (!(upg_flag_info_pri.head_magic == UPG_HEAD_MAGIC &&
          upg_flag_info_pri.head_end_magic == UPG_END_MAGIC &&
          upg_flag_info->complete_flag != 0 &&
          upg_flag_info->need_upgrade == UPG_NEED_UPGRADE_FLAG)) {
        /* 不需要升级直接返回 */
        return ERRCODE_UPG_NOT_NEED_TO_UPDATE;
    }

    for (uint8_t i = 0; i < upg_flag_info->firmware_num; i++) {
        if (upg_flag_info->firmware_num > 1) {
            exist_ssb_image = true;
        }
        ret_val = boot_anti_rollback_version_verify(upg_flag_info->image_id[i],
            upg_flag_info->key_ver[i], upg_flag_info->code_ver[i]);
        if (ret_val != ERRCODE_SUCC) {
            PRINT("[UPG] verify %#x image version fail! ret=%#x\r\n", upg_flag_info->image_id[i], ret_val);
            return ret_val;
        }
        ret_val = upg_verify_new_image(upg_flag_info->image_id[i], exist_ssb_image);
        if (ret_val != ERRCODE_SUCC) {
            PRINT("[UPG] verify %#x image signature fail! ret=%#x\r\n", upg_flag_info->image_id[i], ret_val);
            return ret_val;
        }
        if (upg_flag_info->image_id[i] == SSB_IMAGE_ID) {
            ret_val = upg_image_backups_update();
            if (ret_val != ERRCODE_SUCC) {
                PRINT("[UPG] verify %#x image backup fail! ret=%#x\r\n", upg_flag_info->image_id[i], ret_val);
                return ret_val;
            }
        }
    }

    return ERRCODE_SUCC;
}

/* 开始升级 */
errcode_t boot_upg_start(void)
{
    fota_store_flag_area_t   *upg_flag_info = NULL;
    errcode_t                ret = 0;
    upg_package_info_t       *pkg_info = NULL;
    bool                     direct_finish = true;
    uint32_t                 flag_start_address = 0;
    uint32_t                 flash_offset = 0;
    uint8_t                  need_upgrade = UPG_NEED_UPGRADE_FLAG;
    partition_information_t  upg_store_info;

    if (upg_is_inited() == false) {
        ret = ERRCODE_UPG_NOT_INIT;
        goto end;
    }

    ret = upg_get_store_flag_flash_start_addr(&flag_start_address);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    ret = boot_alloc_and_get_upgrade_flag(&upg_flag_info);
    if (ret != ERRCODE_SUCC || upg_flag_info == NULL) {
        goto end;
    }

    ret = boot_check_need_upgrade(upg_flag_info);
    if (ret != ERRCODE_SUCC) {
        goto not_need_update;
    }

    pkg_info = &upg_get_ctx()->package_info;
    boot_get_package_info(upg_flag_info, pkg_info);
    if (ret != ERRCODE_SUCC) {
        upg_set_temporary_result(UPG_RESULT_VERIFY_HEAD_FAILED);
        goto end;
    }

    ret = boot_process_update(upg_flag_info);
    if (ret == ERRCODE_SUCC) {
        /* 擦除升级存储区的 ssb 镜像数据 */
        (void)uapi_partition_get_info(PARTITION_SSB_UPG_IMAGE, &upg_store_info);
        (void)uapi_sfc_erase(SFC_ID_0, upg_store_info.part_info.addr_info.addr,
                             upg_store_info.part_info.addr_info.size);
        /* 擦除升级存储区的 SeLiteOS 镜像数据 */
        (void)uapi_partition_get_info(PARTITION_SELITEOS_UPG_IMAGE, &upg_store_info);
        (void)uapi_sfc_erase(SFC_ID_0, upg_store_info.part_info.addr_info.addr,
                             upg_store_info.part_info.addr_info.size);
    }
    direct_finish = false;
end:
    need_upgrade = UPG_NOT_NEED_UPGRADE_FLAG;
    flash_offset = (uint32_t)flag_start_address + offsetof(fota_store_flag_area_t, need_upgrade);
    (void)upg_flash_write(flash_offset, sizeof(uint32_t), (uint8_t *)(&need_upgrade), false);
    /* 更新complete_flag */
    boot_set_complete_flag(ret, direct_finish);
not_need_update:
    upg_free(upg_flag_info);
    return ret;
}

void *upg_malloc_func(const uint32_t size)
{
    if (size > UPG_MEM_SIZE2) {
        PRINT("  == malloc size too big size == %#x\n", size);
        return NULL;
    }

    if (size <= UPG_MEM_SIZE0) {
        for (int i = 0; i < UPG_MEM_NUM_5; i++) {
            if (g_upg_mem0[i].used != 1) {
                g_upg_mem0[i].used = 1;
                g_upg_mem0[i].size = size;
                return (void *)g_upg_mem0[i].data;
            }
        }
    }

    if (size <= UPG_MEM_SIZE1) {
        for (int i = 0; i < UPG_MEM_NUM_5; i++) {
            if (g_upg_mem1[i].used != 1) {
                g_upg_mem1[i].used = 1;
                g_upg_mem1[i].size = size;
                return (void *)g_upg_mem1[i].data;
            }
        }
    }

    if (size <= UPG_MEM_SIZE2) {
        for (int i = 0; i < UPG_MEM_NUM_2; i++) {
            if (g_upg_mem2[i].used != 1) {
                g_upg_mem2[i].used = 1;
                g_upg_mem2[i].size = size;
                return (void *)g_upg_mem2[i].data;
            }
        }
    }

    return NULL;
}

void upg_free_func(void *ptr)
{
    for (int i = 0; i < UPG_MEM_NUM_5; i++) {
        if (g_upg_mem0[i].data == ptr) {
            g_upg_mem0[i].used = 0;
            return;
        }
    }
 
    for (int i = 0; i < UPG_MEM_NUM_5; i++) {
        if (g_upg_mem1[i].data == ptr) {
            g_upg_mem1[i].used = 0;
            return;
        }
    }
 
    for (int i = 0; i < UPG_MEM_NUM_2; i++) {
        if (g_upg_mem2[i].data == ptr) {
            g_upg_mem2[i].used = 0;
            return;
        }
    }
    PRINT("[upg] free mem failed, addr = 0x%x\n", ptr);
}

static void upg_putc(const char c)
{
    printf("%c", c);
}

void boot_upg_init(void)
{
    upg_func_t upg_func = {0};
    upg_func.malloc = upg_malloc_func;
    upg_func.free = upg_free_func;
    upg_func.serial_putc = upg_putc;
    (void)uapi_upg_init(&upg_func);
    PRINT("[UPG] boot upg init success\r\n");
}

void boot_upg_handle(void)
{
    errcode_t ret = ERRCODE_SUCC;

    boot_upg_init();
    ret = boot_upg_start();
    if (ret == ERRCODE_SUCC) {
        PRINT("[UPG] upgrade success, reset now! \r\n");
        upg_reboot();
    } else if (ret == ERRCODE_UPG_NOT_NEED_TO_UPDATE) {
        PRINT("[UPG] ssb or seliteos not need to upgrade...\r\n");
    } else {
        PRINT("[UPG] upgrade fail! \r\n");
    }
}
