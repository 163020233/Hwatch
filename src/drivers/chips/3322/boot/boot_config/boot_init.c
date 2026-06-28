/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: boot Cmd
 *
 * Create: 2025-08-13
 */

#include "boot_init.h"

static uint32_t g_flashsize = FLASH_LENGTH;
static uint32_t g_erasesize = 0;

void set_flashsize(uint32_t addr)
{
#ifdef SUPPORT_FMC_NAND
    if ((addr >= NAND_BURN_START) && (addr < EMMC_BURN_START)) {
        g_flashsize = get_erasesize_incl_oob(NAND_FLASH_SIZE_FOR_BURN);
        return;
    }
#endif
#ifdef SUPPORT_EMMC_LOADERBOOT
    if (addr >= EMMC_BURN_START) {
        g_flashsize = (uint32_t)EMMC_SIZE_FOR_BURN;
        return;
    }
#endif
    unused(addr);
    g_flashsize = FLASH_LENGTH;
}

uint32_t get_flashsize(void)
{
    return g_flashsize;
}

void set_erasesize(uint32_t addr, uint32_t erase_size)
{
#ifdef SUPPORT_FMC_NAND
    if (NAND_BURN_START <= addr && addr < EMMC_BURN_START) {
        if (erase_size > get_erasesize_incl_oob(NAND_FLASH_SIZE_FOR_BURN)) {
            g_erasesize = get_erasesize_incl_oob(NAND_FLASH_SIZE_FOR_BURN);
        } else {
            g_erasesize = get_erasesize_incl_oob(erase_size);
        }
        return;
    }
#endif
#ifdef SUPPORT_EMMC_LOADERBOOT
    if (addr >= EMMC_BURN_START) {
        if (erase_size > (uint32_t)EMMC_SIZE_FOR_BURN) {
            g_erasesize = (uint32_t)EMMC_SIZE_FOR_BURN;
        } else {
            g_erasesize = erase_size;
        }
        return;
    }
#endif
    unused(addr);
    g_erasesize = erase_size;
}

uint32_t get_erasesize(void)
{
    return g_erasesize;
}