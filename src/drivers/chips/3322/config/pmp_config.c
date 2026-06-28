/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description:  chip pmp init config file.
 */
#include "pmp_config.h"
#include "stdbool.h"
#include "drv_pmp.h"
#include "memory_config.h"

#if (CORE == BT)
static const pmp_conf_t g_region_attr[] = {
    { //  BROM1
        .idx = 0, // 0 means pmpaddr1
        .addr = 0xA0000,
        .size = 0x20000,
        .conf.pmp_attr = PMP_ATTR_NO_BUFFERABLE_AND_NO_CACHEABLE,
        .conf.rwx_permission = (rwx_permission_t)PMPCFG_READ_ONLY_EXECUTE,
        .conf.addr_match = PMPCFG_ADDR_MATCH_NAPOT,
        .conf.lock = true,
    },
    { //  BROM2
        .idx = 1, // 1 means pmpaddr2
        .addr = 0xC0000,
        .size = 0x40000,
        .conf.pmp_attr = PMP_ATTR_NO_BUFFERABLE_AND_NO_CACHEABLE,
        .conf.rwx_permission = (rwx_permission_t)PMPCFG_READ_ONLY_EXECUTE,
        .conf.addr_match = PMPCFG_ADDR_MATCH_NAPOT,
        .conf.lock = true,
    },
    { //  BRAM
        .idx = 2, // 2 means pmpaddr2
        .addr = 0x0,
        .size = 0x400,
        .conf.pmp_attr = PMP_ATTR_NO_BUFFERABLE_AND_NO_CACHEABLE,
        .conf.rwx_permission = (rwx_permission_t)PMPCFG_READ_ONLY_NEXECUTE,
        .conf.addr_match = PMPCFG_ADDR_MATCH_NAPOT,
        .conf.lock = true,
    },
    { //  BRAM
        .idx = 3, // 3 means pmpaddr3
        .addr = 0x0,
        .size = 0x100000,
        .conf.pmp_attr = PMP_ATTR_NO_BUFFERABLE_AND_NO_CACHEABLE,
        .conf.rwx_permission = (rwx_permission_t)PMPCFG_RW_EXECUTE,
        .conf.addr_match = PMPCFG_ADDR_MATCH_NAPOT,
        .conf.lock = true,
    },
    { // comm ram
        .idx = 4, // 4 means pmpaddr4
        .addr = 0x83000000,
        .size = 0x20000,
        .conf.pmp_attr = PMP_ATTR_NO_BUFFERABLE_AND_NO_CACHEABLE,
        .conf.rwx_permission = (rwx_permission_t)PMPCFG_RW_NEXECUTE,
        .conf.addr_match = PMPCFG_ADDR_MATCH_NAPOT,
        .conf.lock = true,
    },
#ifdef CONFIG_BT_SUPPORT_BT_SAMPLE
    { // L2M 背景属性
        .idx = 5,
        .addr = 0x2000000,
        .size = 0x0400000,
        .conf.pmp_attr = PMP_ATTR_NO_BUFFERABLE_AND_NO_CACHEABLE,
        .conf.rwx_permission = (rwx_permission_t)PMPCFG_READ_ONLY_EXECUTE,
        .conf.addr_match = PMPCFG_ADDR_MATCH_NAPOT,
        .conf.lock = true,
    },
#endif
};
#else
static const pmp_conf_t g_region_attr[] = {
    // pmp 16 recovery: tee used itcm first 2k for startup.s, app: unused
#if defined(USE_PSRAM_INSTEAD_NORFLASH_FOR_CODETEXT)
    { // psram for npu heap, non-cache, 2MB
        .idx = 17,
        .addr = 0x22800000,
        .size = 0x00200000,
        .conf.pmp_attr = PMP_ATTR_NO_BUFFERABLE_AND_NO_CACHEABLE,
        .conf.rwx_permission = (rwx_permission_t)PMPCFG_RW_NEXECUTE,
        .conf.addr_match = PMPCFG_ADDR_MATCH_NAPOT,
        .conf.lock = true,
    },
#endif
    // pmp 17 reserved
    { //  norflash with Cache
        .idx = 18, // 18 means pmpaddr18
        .addr = 0x26000000,
        .size = 0x2000000,
        .conf.pmp_attr = PMP_ATTR_WRITEBACK_RALLOCATE,
        .conf.rwx_permission = (rwx_permission_t)PMPCFG_READ_ONLY_EXECUTE,
        .conf.addr_match = PMPCFG_ADDR_MATCH_NAPOT,
        .conf.lock = true,
    },
#ifndef BUILD_RECOVERY_IMAGE
    {
        .idx = 19,
        .addr = PSRAM_APP_SYN_LOAD_ADDR,
        .size = PSRAM_APP_SYN_LOAD_LENGTH,
        .conf.pmp_attr = PMP_ATTR_NO_BUFFERABLE_AND_NO_CACHEABLE,
        .conf.rwx_permission = (rwx_permission_t)PMPCFG_RW_NEXECUTE,
        .conf.addr_match = PMPCFG_ADDR_MATCH_NAPOT,
        .conf.lock = true,
    },
#endif
    // pmp 19 reserved
    // pmp 20 bootrom PMPCFG_NO_ACCESS
#ifdef SUPPORT_GPU_QSPI
    { // QSPI_DISPLAY REG 属性
        .idx = 21,
        .addr = 0x24000000,
        .size = 0x00000100,
        .conf.pmp_attr = PMP_ATTR_DEVICE_NO_BUFFERABLE,
        .conf.rwx_permission = (rwx_permission_t)PMPCFG_RW_NEXECUTE,
        .conf.addr_match = PMPCFG_ADDR_MATCH_NAPOT,
        .conf.lock = true,
    },
#elif defined(USE_PSRAM_INSTEAD_NORFLASH_FOR_CODETEXT)
    { // psram code text 属性
        .idx = 21,
        .addr = 0x22800000,
        .size = 0x00800000,
        .conf.pmp_attr = PMP_ATTR_WRITEBACK_RALLOCATE,
        .conf.rwx_permission = (rwx_permission_t)PMPCFG_READ_ONLY_EXECUTE,
        .conf.addr_match = PMPCFG_ADDR_MATCH_NAPOT,
        .conf.lock = true,
    },
#endif
    { // itcm + bootrom  背景属性
        .idx = 22,
        .addr = 0x00000000,
        .size = 0x00080000,
        .conf.pmp_attr = PMP_ATTR_NO_BUFFERABLE_AND_NO_CACHEABLE,
        .conf.rwx_permission = (rwx_permission_t)PMPCFG_READ_ONLY_EXECUTE,
        .conf.addr_match = PMPCFG_ADDR_MATCH_NAPOT,
        .conf.lock = true,
    },
    { // dtcm 背景属性  23
        .idx = 23,
        .addr = 0x10000000,
        .size = 0x00100000,
        .conf.pmp_attr = PMP_ATTR_NO_BUFFERABLE_AND_NO_CACHEABLE,
        .conf.rwx_permission = (rwx_permission_t)PMPCFG_RW_NEXECUTE,
        .conf.addr_match = PMPCFG_ADDR_MATCH_NAPOT,
        .conf.lock = true,
    },
    { // psram with Cache, 4MB
        .idx = 24,
        .addr = 0x22000000,
        .size = 0x400000,
        .conf.pmp_attr = PMP_ATTR_WRITEBACK_RWALLOCATE,
        .conf.rwx_permission = (rwx_permission_t)PMPCFG_RW_NEXECUTE,
        .conf.addr_match = PMPCFG_ADDR_MATCH_NAPOT,
        .conf.lock = true,
    },
    { // psram with Cache, 2MB/4MB
        .idx = 25,
        .addr = 0x22400000,
#if defined(USE_PSRAM_INSTEAD_NORFLASH_FOR_CODETEXT)
        .size = 0x00400000,
#elif defined(CONFIG_MAP_DEMO)
        .size = 0x00400000,
#else
        .size = 0x200000,
#endif
        .conf.pmp_attr = PMP_ATTR_WRITEBACK_RWALLOCATE,
        .conf.rwx_permission = (rwx_permission_t)PMPCFG_RW_NEXECUTE,
        .conf.addr_match = PMPCFG_ADDR_MATCH_NAPOT,
        .conf.lock = true,
    },
    { // sfc control register
        .idx = 26,
        .addr = 0x28000000,
        .size = 0x2000,
        .conf.pmp_attr = PMP_ATTR_DEVICE_NO_BUFFERABLE,
        .conf.rwx_permission = (rwx_permission_t)PMPCFG_RW_NEXECUTE,
        .conf.addr_match = PMPCFG_ADDR_MATCH_NAPOT,
        .conf.lock = true,
    },
    { // L2M 背景属性
        .idx = 27,
        .addr = 0x20000000,
        .size = 0x00400000,
        .conf.pmp_attr = PMP_ATTR_NO_BUFFERABLE_AND_NO_CACHEABLE,
        .conf.rwx_permission = (rwx_permission_t)PMPCFG_RW_NEXECUTE,
        .conf.addr_match = PMPCFG_ADDR_MATCH_NAPOT,
        .conf.lock = true,
    },
    { // soc register
        .idx = 28,
        .addr = 0x50000000,
        .size = 0x10000000,
        .conf.pmp_attr = PMP_ATTR_DEVICE_NO_BUFFERABLE,
        .conf.rwx_permission = (rwx_permission_t)PMPCFG_RW_NEXECUTE,
        .conf.addr_match = PMPCFG_ADDR_MATCH_NAPOT,
        .conf.lock = true,
    },
    { // shareram, mcu访问bt的tcm的地址访问
        .idx = 29,
        .addr = 0x80000000,
        .size = 0x4000000,
        .conf.pmp_attr = PMP_ATTR_NO_BUFFERABLE_AND_NO_CACHEABLE,
        .conf.rwx_permission = (rwx_permission_t)PMPCFG_RW_NEXECUTE,
        .conf.addr_match = PMPCFG_ADDR_MATCH_NAPOT,
        .conf.lock = true,
    },
    { // clic的寄存器
        .idx = 30,
        .addr = 0xb0000000,
        .size = 0x10000000,
        .conf.pmp_attr = PMP_ATTR_DEVICE_NO_BUFFERABLE,
        .conf.rwx_permission = (rwx_permission_t)PMPCFG_RW_NEXECUTE,
        .conf.addr_match = PMPCFG_ADDR_MATCH_NAPOT,
        .conf.lock = true,
    },
    { // psram背景属性，
        .idx = 31,
        .addr = 0x22000000,
        .size = 0x01000000,
        .conf.pmp_attr = PMP_ATTR_NO_BUFFERABLE_AND_NO_CACHEABLE,
        .conf.rwx_permission = (rwx_permission_t)PMPCFG_RW_NEXECUTE,
        .conf.addr_match = PMPCFG_ADDR_MATCH_NAPOT,
        .conf.lock = true,
    }
};
#endif

uint8_t g_m2b_ram_pmp_flag = 1;
void pmp_mcu_to_btram_config(uint8_t rwable)
{
    if (g_m2b_ram_pmp_flag == rwable) {
        return;
    }
    pmp_conf_t exc_region_attr[] = {
        {
            .idx = 29, // 29 means pmpaddr29
            .addr = 0x80000000,
            .size = 0x4000000,
            .conf.pmp_attr = PMP_ATTR_NO_BUFFERABLE_AND_NO_CACHEABLE,
            .conf.rwx_permission = (rwx_permission_t)PMPCFG_RW_NEXECUTE,
            .conf.addr_match = PMPCFG_ADDR_MATCH_NAPOT,
            .conf.lock = true,
        },
    };
    if (rwable == 0) {
        exc_region_attr[0].addr = 0x83000000;
        exc_region_attr[0].size = 0x20000;
        g_m2b_ram_pmp_flag = 0;
    } else {
        g_m2b_ram_pmp_flag = 1;
    }
    uapi_pmp_config(exc_region_attr, sizeof(exc_region_attr) / sizeof(pmp_conf_t));
}

void pmp_norflash_config(uint8_t readable)
{
    const pmp_conf_t region_attr[] = {
        // pmp 17 reserved
        {
            //  norflash with Cache
            .idx = 18,  // 18 means pmpaddr18
            .addr = 0x26000000,
            .size = 0x2000000,
            .conf.pmp_attr = PMP_ATTR_WRITEBACK_RALLOCATE,
            .conf.rwx_permission =
                (readable ? (rwx_permission_t)PMPCFG_READ_ONLY_EXECUTE : (rwx_permission_t)PMPCFG_NO_ACCESS),
            .conf.addr_match = PMPCFG_ADDR_MATCH_NAPOT,
            .conf.lock = true,
        },
    };
    uapi_pmp_config(region_attr, sizeof(region_attr) / sizeof(pmp_conf_t));
}

void pmp_init(void)
{
    uapi_pmp_config(g_region_attr, sizeof(g_region_attr) / sizeof(pmp_conf_t));
}
