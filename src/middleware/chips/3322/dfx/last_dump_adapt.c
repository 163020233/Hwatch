/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: last dump
 * This file should be changed only infrequently and with great care.
 */
#include "diag.h"
#include "pmp_config.h"
#include "last_dump.h"
#include "last_dump_st.h"
#ifdef DUMP_MEM_SUPPORT
#include "diag_cmd_id.h"
#endif
#include "memory_config.h"
#include "platform_types.h"
#include "dfx_adapt_layer.h"
#include "last_dump_adapt.h"

#define LAST_DUMP_HEAP_SIZE (4 * 1024)

typedef struct {
    char *name;
    uintptr_t start_addr;
    uintptr_t end_addr;
} diag_reg_dump_t;

typedef struct {
    char *name;
    uintptr_t start_addr;
    uint32_t len;
} diag_mem_dump_t;


#ifdef DUMP_REG_SUPPORT
static const diag_reg_dump_t g_reg_dump_info[] = {
    { "PMU1_CTL", 0x57004000, 0x570047e0 },      // PMU1_CTL
    { "PMU2_CTL", 0x57008000, 0x57008490 },      // PMU2_CTL
    { "ULP_AON_CTL", 0x5702c000, 0x5702c418 },   // ULP_AON_CTL
};

static void dfx_dump_reg(void)
{
    uint16_t count = (uint16_t)(sizeof(g_reg_dump_info) / sizeof(diag_reg_dump_t));
    for (uint8_t i = 0; i < count; i++) {
        dfx_last_dump_data(g_reg_dump_info[i].name, g_reg_dump_info[i].start_addr,
                           g_reg_dump_info[i].end_addr - g_reg_dump_info[i].start_addr);
        uapi_watchdog_kick();
    }
}
#endif /* DUMP_REG_SUPPORT */

#ifdef DUMP_MEM_SUPPORT
static uint8_t g_err_cores_num = 0x5a;
static const diag_mem_dump_t g_bt_mem_dump_info[] = {
    { "BT_RAM_ORIGIN",         BT_RAM_ORIGIN_APP_MAPPING,      BT_RAM_ORIGIN_APP_MAPPING_LENGTH },
    { "SHARED_MEM",            SHARED_MEM_START,               SHARED_MEM_LENGTH },
    { "BCPU_TRACE_MEM_REGION", BCPU_TRACE_MEM_REGION_START,    BCPU_TRACE_MEM_REGION_LENGTH },
};
static const diag_mem_dump_t g_dsp_mem_dump_info[] = {
    { "SHARED_MEM",            SHARED_MEM_START,               SHARED_MEM_LENGTH },
    { "DSP_OCRAM",             DSP_OCRAM_ACORE_PHYS_ADDR,      DSP_OCRAM_PHYS_SIZE },
    { "DSP_DTCM",              DSP_DTCM_MAP_ADDR,              DSP_DTCM_SIZE },
    { "DSP_ITCM",              DSP_ITCM_MAP_ADDR,              DSP_ITCM_SIZE },
};
static const diag_mem_dump_t g_app_mem_dump_info[] = {
    { "APP_ITCM_ORIGIN",       (REE_ITCM_ORIGIN + 0x800),     (REE_ITCM_LENGTH - 0x800) },
    { "APP_DTCM_ORIGIN",       REE_DTCM_ORIGIN,               REE_DTCM_LENGTH },
    { "SHARED_MEM",            SHARED_MEM_START,              SHARED_MEM_LENGTH },
    { "MCPU_TRACE_MEM_REGION", MCPU_TRACE_MEM_REGION_START,   MCPU_TRACE_MEM_REGION_LENGTH },
};
static const diag_mem_dump_t* g_mem_dump_info_ptr[] = {
    g_bt_mem_dump_info,
    g_dsp_mem_dump_info,
    g_app_mem_dump_info,
};

static void dfx_dump_aligned_4bytes_data(const char *name, uintptr_t addr, uint32_t size)
{
    last_dump_data_ind_t ind = { 0 };
    uint8_t *addr_tmp = (uint8_t *)addr;
    uint8_t *data_addr;
    uint32_t offset = 0;
    uint8_t *data[2];
    uint16_t len[2];
    uint32_t size_tmp = size;
    if (strncpy_s(ind.name, LAST_DUMP_NAME_LEN, name, strlen(name)) != EOK) {
        dfx_log_err("dfx_last_dump_data(%s) error \r\n", name);
        return;
    }

    data_addr = (uint8_t *)malloc(LAST_DUMP_HEAP_SIZE);
    if (data_addr == NULL) {
        dfx_log_err("dsp itcm malloc fail!\r\n");
        return;
    }

    ind.total_size = size_tmp;
    data[0] = (uint8_t *)&ind;
    len[0] = (uint16_t)sizeof(last_dump_data_ind_t);
    while (size_tmp != 0) {
#if defined(CONFIG_WATCHDOG_SUPPORT_ULP_WDT) && (CONFIG_WATCHDOG_SUPPORT_ULP_WDT == 1)
        ulp_wdt_kick();
#endif
        uint16_t per_size = (uint16_t)uapi_min(LAST_DUMP_HEAP_SIZE, size_tmp);
        for (uint16_t i = 0; i < per_size >> 2; i++) { /* 2：uint32_t needs to be shifted right by 2 */
            *((uint32_t*)data_addr + i) = *((uint32_t*)addr_tmp + i);
        }
        data[1] = (uint8_t *)data_addr;
        len[1] = per_size;
        ind.offset = offset;
        ind.size = per_size;
        (void)uapi_diag_report_packets_critical(DIAG_CMD_ID_LAST_DUMP, NULL, data, len, 2); /* pkt_cnt 为2 */
        addr_tmp += per_size;
        size_tmp -= per_size;
        offset += per_size;
        uapi_watchdog_kick();
    }
    free(data_addr);
    last_dump_data_ind_finish_t *finish_ind = (last_dump_data_ind_finish_t *)(uintptr_t)&ind;
    finish_ind->start_address = addr;
    data[0] = (uint8_t *)finish_ind;
    len[0] = (uint16_t)sizeof(last_dump_data_ind_finish_t);
    (void)uapi_diag_report_packets_critical(DIAG_CMD_ID_LAST_DUMP_FINISH, NULL, data, len, 1); /* pkt_cnt 为1 */
}

static void dfx_dump_mem(diag_mem_dump_t *mem_ptr, uint8_t num)
{
    for (uint8_t i = 0; i < num; i++) {
        if (mem_ptr[i].start_addr == DSP_ITCM_MAP_ADDR) {
            dfx_dump_aligned_4bytes_data(mem_ptr[i].name, mem_ptr[i].start_addr, mem_ptr[i].len);
        } else {
            dfx_last_dump_data(mem_ptr[i].name, mem_ptr[i].start_addr, mem_ptr[i].len);
        }
        uapi_watchdog_kick();
    }
}

static uint32_t dfx_get_mem_dump_num(cores_t cpu_type)
{
    uint32_t num = 0;
    if (cpu_type == CORES_BT_CORE) {
        num = (uint32_t)(sizeof(g_bt_mem_dump_info) / sizeof(diag_mem_dump_t));
    } else if (cpu_type == CORES_PROTOCOL_CORE) {
        num = (uint32_t)(sizeof(g_dsp_mem_dump_info) / sizeof(diag_mem_dump_t));
    } else {
        num = (uint32_t)(sizeof(g_app_mem_dump_info) / sizeof(diag_mem_dump_t));
    }
    return num;
}
#endif /* DUMP_MEM_SUPPORT */

void dfx_last_dump(void)
{
    pmp_mcu_to_btram_config(1);
    uint32_t file_num = 0;
#ifdef DUMP_MEM_SUPPORT
    uint32_t num = 0;
    cores_t cpu_num = get_crash_core_type();
    if (cpu_num > CORES_APPS_CORE) {
        g_err_cores_num = cpu_num;
        cpu_num = CORES_APPS_CORE;
    }
    diag_mem_dump_t* mem_ptr = g_mem_dump_info_ptr[(uint8_t)cpu_num];
    num = dfx_get_mem_dump_num(cpu_num);
    file_num += num;
#endif
#ifdef DUMP_REG_SUPPORT
    file_num += (uint32_t)(sizeof(g_reg_dump_info) / sizeof(diag_reg_dump_t));
#endif

#if (defined(DUMP_MEM_SUPPORT) || defined(DUMP_REG_SUPPORT))
    dfx_last_dump_start(file_num);
#endif
#ifdef DUMP_MEM_SUPPORT
    dfx_dump_mem(mem_ptr, (uint8_t)num);
#endif
#ifdef DUMP_REG_SUPPORT
    dfx_dump_reg();
#endif

#if (defined(DUMP_MEM_SUPPORT) || defined(DUMP_REG_SUPPORT))
    dfx_last_dump_end(file_num);
#endif
    unused(file_num);
}
