/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Main boot for Second Stage Bootloader (SSB).
 * Author: HiSilicon
 * Create: 2021-03-09
 */

#ifndef MAIN_H
#define MAIN_H
#include "platform_core.h"
#include "chip_io.h"

typedef enum {
    PSRAM_DENSITY_16M = 0x5,
    PSRAM_DENSITY_64M = 0x6,
    PSRAM_DENSITY_32M = 0x7,

    PSRAM_DENSITY_MAX
} psram_density_t;

typedef enum {
    /* 0-15 for tee reserved */
    PMP_INDEX_ITCM_SELITEOS_ADDR = 13,          /* addr:0x5_0000 - 0x7_0000 */
    PMP_INDEX_DTCM_TEE_ADDR = 14,               /* addr:0x1000_0000 - 0x1001_0000 */

    /* 16-31 for ree reserved */
    PMP_INDEX_ITCM_STARTUP_ADDR = 16,           /* addr:0x1_0000 - 0x1_0400 */
    PMP_INDEX_SFC_CODE_ADDR = 18,               /* addr:0x2600_0000 - 0x2800_0000 */
    PMP_INDEX_BOOTROM_ADDR = 20,                /* addr:0x0 - 0x1_0000 */
    PMP_INDEX_ITCM_TEE_ADDR = 21,               /* addr:0x1_0000 - 0x2_0000 */
    PMP_INDEX_ITCM_BACKGROUND_ADDR = 22,        /* addr:0x0 - 0x8_0000 */
    PMP_INDEX_DTCM_BACKGROUND_ADDR = 23,        /* addr:0x1000_0000 - 0x1010_0000 */
    PMP_INDEX_L2RAM_SFC_PSRAM_ADDR = 24,        /* addr:0x2000_0000 - 0x3000_0000 */
    PMP_INDEX_SOC_REG_ADDR = 25,                /* addr:0x5000_0000 - 0x6000_0000 */
    PMP_INDEX_OTHER_CORE_SRAM_ADDR = 26,        /* addr:0x8000_0000 - 0x9000_0000 */
    PMP_INDEX_CPU_SYSTEM_REG_ADDR = 27,         /* addr:0xb000_0000 - 0xc000_0000 */
} pmp_addr_index_enum;

#define BOOT_BACKFLASH          0x39BB

typedef struct {
    /* efuse */
    uint32_t resv_0 : 1;        /* 688 bits */
    uint32_t flash_io_data_ds : 2;
    uint32_t flash_io_clk_ds : 2;
    uint32_t uart_io_ds : 2;
    uint32_t nand_bootup_flag : 2;          // 0x2: boot from nand, other value: boot from nor
    uint32_t resv_1 : 1;
    uint32_t flash_drive_en : 1;
    uint32_t uart_drive_en : 1;
    uint32_t chip_features_cfg_disable : 1;
    uint32_t chip_features_global_disable : 1;
    uint32_t disable_uart_load : 1;
    uint32_t ate_loader_disable : 1;
    uint32_t flashboot_support_hash : 1;
    uint32_t uart_wait_sle : 2; /* efuse uart timeout */
    uint32_t reboot_wait_hard_reset : 1;
    uint32_t wait_flash_poweron_sle : 1;
    uint32_t usb_dfu_en : 2;
    uint32_t pub_key_sle : 4;
    uint32_t secure_boot_enable_efuse : 4;
    uint32_t resv_2 : 1;
} bootup_efuse_context;

typedef struct {
    uint32_t flash_id;
    uint16_t boot_start_status;
    uint16_t uart_wait_timeout; /* uint:ms */
    uint16_t uart_bus;
    uint16_t curr_freq;
    uint32_t io_skip_loader : 1;
    uint32_t flash_has_pwr_history : 1;
    uint32_t get_efuse_fail : 1;
    uint32_t secure_boot_enble : 1;
    uint32_t chip_lifecycle : 4;
    uint32_t resv : 24;
    uint32_t start_systicks;
    bootup_efuse_context efuse;
} bootup_ctrl; /* share mem, only power reset miss */

typedef enum {
    /** 空魔术字 */
    DEFAULT_MAGIC_NUM = 0xFFFFFFFFUL,
    /** 进入Recovery魔术字 */
    RECOVERY_ENTER_MAGIC = 0x68767378UL,
    /** Recovery成功魔术字 */
    RECOVERY_SUCCESS_MAGIC = 0x68767975UL,
    /** Recovery失败应术字 */
    RECOVERY_FAILURE_MAGIC = 0x68767070UL,
    /** Recovery差分成功魔术字 */
    RECOVERY_DIFF_SUCCESS = 0x68707975UL,
    /** Recovery加载失败应术字 */
    RECOVERY_LOAD_FAIL = 0x68767670UL,
    /** Recovery已打印过ERRORCODE魔术字 */
    RECOVERY_ERROR_CODE_MAGIC = 0x68767E7EUL,
    /** 升级更新成功魔术字 */
    UPGRADE_SUCCESS_MAGIC = 0xABCDABCDUL,
} sharedata_magic;

typedef struct {
    sharedata_magic dloadmagic;
    int32_t updateflag;
} sharedata_info;

#define ROMBOOT_REBOOT_CNT_REG         (LP_CTL_ULP_B_RB_BASE + 0x4) /* Low 8bits */
#define ROMBOOT_FLASHBOOT_UP_CNT_REG   (LP_CTL_ULP_B_RB_BASE + 0x4) /* High 8bits */
#define ROMBOOT_FLASHBOOT_BACKUP_REG   (LP_CTL_ULP_B_RB_BASE + 0x8)

static __inline uint16_t boot_get_reboot_cnt(void)
{
    return reg16_getbits(ROMBOOT_REBOOT_CNT_REG, 0, 8); /* start bit0, len8 */
}

static __inline void boot_set_reboot_cnt(uint16_t reboot_cnt)
{
    reg16_setbits(ROMBOOT_REBOOT_CNT_REG, 0, 8, reboot_cnt); /* start bit0, len8 */
}

static __inline uint16_t boot_get_flashboot_up_cnt(void)
{
    return reg16_getbits(ROMBOOT_FLASHBOOT_UP_CNT_REG, 8, 8); /* start bit8, len8 */
}

static __inline void boot_set_flashboot_up_cnt(uint16_t up_cnt)
{
    reg16_setbits(ROMBOOT_FLASHBOOT_UP_CNT_REG, 8, 8, up_cnt); /* start bit8, len8 */
}

static __inline uint16_t boot_get_flashboot_backup_flag(void)
{
    return readw(ROMBOOT_FLASHBOOT_BACKUP_REG);
}

static __inline void boot_set_flashboot_backup_flag(uint16_t flag)
{
    writew(ROMBOOT_FLASHBOOT_BACKUP_REG, flag);
}

void tee_flash_online_decryption_config(uint32_t start_addr, uint32_t end_addr);

#endif
