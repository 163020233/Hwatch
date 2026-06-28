/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: svr dynload implementation.
 * Author:
 * Create: 2024-10-30
 */

#include "svr_dynload.h"
#include "stdint.h"
#include "los_ld_elflib.h"
#include "los_memory.h"
#include "osal_adapt.h"
#include "osal_addr.h"
#include "memory_config.h"
#include "ohos_init.h"
#include "common_def.h"
#include "drv_pmp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

void *g_dyn_handle = NULL;
char *g_dyn_path = NULL;

static const pmp_conf_t g_synload_region_attr[] = {
    {
        .idx = 19,
        .addr = PSRAM_APP_SYN_LOAD_ADDR,
        .size = PSRAM_APP_SYN_LOAD_LENGTH,
        .conf.pmp_attr = PMP_ATTR_NO_BUFFERABLE_AND_NO_CACHEABLE,
        .conf.rwx_permission = (rwx_permission_t)PMPCFG_READ_ONLY_EXECUTE,
        .conf.addr_match = PMPCFG_ADDR_MATCH_NAPOT,
        .conf.lock = true,
    }
};

static const pmp_conf_t g_synload_region_attr_rw[] = {
    {
        .idx = 19,
        .addr = PSRAM_APP_SYN_LOAD_ADDR,
        .size = PSRAM_APP_SYN_LOAD_LENGTH,
        .conf.pmp_attr = PMP_ATTR_NO_BUFFERABLE_AND_NO_CACHEABLE,
        .conf.rwx_permission = (rwx_permission_t)PMPCFG_RW_NEXECUTE,
        .conf.addr_match = PMPCFG_ADDR_MATCH_NAPOT,
        .conf.lock = true,
    }
};

void dynload_update_permision_to_rx(void)
{
    uapi_pmp_config(g_synload_region_attr, sizeof(g_synload_region_attr) / sizeof(pmp_conf_t));
}

#if defined(LOSCFG_KERNEL_DYNLOAD) && defined(LOSCFG_DYNLOAD_DYN_FROM_FS)
static void dynload_update_permision_to_rw(void)
{
    uapi_pmp_config(g_synload_region_attr_rw, sizeof(g_synload_region_attr_rw) / sizeof(pmp_conf_t));
}
#endif

static void init(void)
{
#if defined(LOSCFG_KERNEL_DYNLOAD) && defined(LOSCFG_DYNLOAD_DYN_FROM_FS)
    uint8_t *dyn_pool = NULL;
    dyn_pool =  (uint8_t *)PSRAM_APP_SYN_LOAD_ADDR;
    (void)osal_pool_mem_init((void *)dyn_pool, PSRAM_APP_SYN_LOAD_LENGTH);
    osal_dyn_mem_pool_set(dyn_pool);
#endif
}

APP_FEATURE_INIT_PRI(init, LAYER_INIT_LEVEL_4);

void dynload_load_library(const char* path)
{
#if defined(LOSCFG_KERNEL_DYNLOAD) && defined(LOSCFG_DYNLOAD_DYN_FROM_FS)
    if ((g_dyn_handle != NULL) && (g_dyn_path != NULL) && (strcmp(path, g_dyn_path) == 0)) {
        return;
    }
    dynload_unload_library();
    g_dyn_handle = osal_so_load(path);
    g_dyn_path = strdup(path);
#else
    unused(path);
#endif
}

void dynload_unload_library(void)
{
#if defined(LOSCFG_KERNEL_DYNLOAD) && defined(LOSCFG_DYNLOAD_DYN_FROM_FS)
    dynload_update_permision_to_rw();
    if (g_dyn_handle != NULL) {
        osal_module_unload(g_dyn_handle);
        g_dyn_handle = NULL;
    }
    if (g_dyn_path != NULL) {
        free(g_dyn_path);
        g_dyn_path = NULL;
    }
#endif
}

void* dynload_get_symbol(char* name)
{
#if defined(LOSCFG_KERNEL_DYNLOAD) && defined(LOSCFG_DYNLOAD_DYN_FROM_FS)
    if (g_dyn_handle != NULL) {
        return osal_find_sym_by_name(g_dyn_handle, name);
    }
#else
    unused(name);
#endif
    return NULL;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif