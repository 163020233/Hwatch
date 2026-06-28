/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Main boot for Standard Application Core
 * Author:
 * Create: 2021-03-09
 */

#ifndef MAIN_H
#define MAIN_H

#include "platform_core.h"
#include "chip_io.h"

typedef struct {
    /* efuse */
    uint32_t resv_0 : 1;        /* 688 bits */
    uint32_t flash_io_data_ds : 2;
    uint32_t flash_io_clk_ds : 2;
    uint32_t uart_io_ds : 2;
    uint32_t nand_bootup_flag : 2;          // 0x2: boot from nand, other value: boot from nor
    uint32_t sec_veto_enable : 1;
    uint32_t flash_drive_en : 1;
    uint32_t uart_drive_en : 1;
    uint32_t chip_features_cfg_disable : 1;
    uint32_t chip_features_global_disable : 1;
    uint32_t disable_uart_load : 1;
    uint32_t usb_phy_trim_switch : 1;
    uint32_t flashboot_support_hash : 1;
    uint32_t uart_wait_sle : 2; /* efuse uart timeout */
    uint32_t reboot_wait_hard_reset : 1;
    uint32_t wait_flash_poweron_sle : 1;
    uint32_t usb_dfu_en : 2;
    uint32_t pub_key_sle : 3;
    uint32_t fnpll_en : 1;
    uint32_t secure_boot_enable_efuse : 4;
    uint32_t fnpll_sel : 1;
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

#endif