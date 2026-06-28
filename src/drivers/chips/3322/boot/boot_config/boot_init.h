/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Common Boot for Standard Application Core
 *
 * Create: 2021-03-09
 */

#ifndef _BOOT_INIT_H_
#define _BOOT_INIT_H_
#include "soc_reg.h"
#include "memory_config_flash.h"
#include "boot_def.h"
#ifdef SUPPORT_EMMC_LOADERBOOT
#include "mmc.h"
#endif
#ifdef SUPPORT_FMC_NAND
#include "nand/nand_fmc.h"
#endif

#define FLASH_MEM_SIZE              get_flashsize()

#define KERNEL_START_ADDR           0x90110000

#define BOOT_HEAD_ADDR              0x200000
#define LOAD_BOOT_ADDR              0xA20000

#define BOOT_PORTING_RESET_REG      GLB_CTL_M_RB_SYS_SOFT_WDT_RST_REG
#define BOOT_PORTING_RESET_VALUE    GLB_CTL_M_RB_CFG_GLBM_SOFT_RST_SYS_N_OFFSET


#define STATUS_DEBUG_REG_ADDR       GLB_CTL_M_RB_GLB_GP_M_REG0_REG   // gp reg0

#define HIBURN_CODELOADER_UART              UART_BUS_2
#define HIBURN_UART_TX                      CHIP_FIXED_TX_PIN   // 当前驱动没有用到，只有检查
#define HIBURN_UART_RX                      CHIP_FIXED_RX_PIN   // 当前驱动没有用到，只有检查
#define UART_ROMBOOT_INT_TIMEOUT_DEFAULT    32

#define FLASHBOOT_MAX_SIZE          0x10000
#define SRAM_START_ADDR             0x00100000  /* SRAM 起始地址 */
#define LOADY_MAX_ADDR              0x1000000   /* loady 命令可存储的最大地址(不包括该地址) */

#define RAM_MIN_ADDR                BOOT_RAM_MIN  /* SRAM 可设置的最小起始地址 */
#define RAM_MAX_ADDR                BOOT_RAM_MAX

#define LOAD_MAX_RAM_ADDR           (RAM_MAX_ADDR - 0x40)
#define LOAD_MIN_SIZE               0x0030      /* 0x20(hash len) + 0x10(tail len) */
#define LOAD_MAX_SIZE               get_flashsize()

#define RESET_DELAY_MS              100

#define REPET_TIMES_2               2
#define REPET_TIMES_3               3
#define REBOOT_MAX_CNT              6
#define BOOT_STEP                   13

#ifdef UT_TEST
#define STATIC
#else
#define STATIC static
#endif

#define ADDR_RANGE_LENGTH 0x10000000
#define SFC_BURN_START FLASH_START
#define NAND_BURN_START (SFC_BURN_START + ADDR_RANGE_LENGTH)
#define EMMC_BURN_START (NAND_BURN_START + 4 * ADDR_RANGE_LENGTH)

#if defined(SUPPORT_FMC_NAND) || defined(SUPPORT_EMMC_LOADERBOOT)
#define SET_FLASH_SIZE(addr) set_flashsize(addr)
#define SET_ERASE_SIZE(addr, erasesize) set_erasesize(addr, erasesize)
#define GET_ERASE_SIZE get_erasesize()
void set_flashsize(uint32_t addr);
void set_erasesize(uint32_t addr, uint32_t erase_size);
uint32_t get_erasesize(void);
#endif

#ifdef SUPPORT_FMC_NAND
#define SET_NAND_WRITE_POS set_nand_write_pos()
#endif

uint32_t get_flashsize(void);

void start_fastboot(void);
void irq_handler(void);

#endif
