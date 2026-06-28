/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description: NON OS Utils
 * Author:
 * Create:  2018-10-15
 */

#include "product.h"
#include "non_os.h"
#include "core.h"
#include "panic.h"
#include "chip_io.h"
#include "arch_barrier.h"
#if ((ARCH == RISCV31) || (ARCH == RISCV32) || (ARCH == RISCV70))
#include "arch_encoding.h"
#endif

#include "non_os_utils.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/** Variable used to store the nesting of critical sections.
 * If it reaches 0 interrupts are disabled, when it reaches 0 interrupts are enabled again.  */
static volatile uint16_t g_non_os_critical_nesting = 0;

static critical_statistic_handler g_cs_stat_handler = NULL;
static critical_record_handler g_cs_record_handler = NULL;

static volatile uint32_t g_lock_stat = 0;

static void non_os_panic(uint32_t code)
{
    panic(PANIC_CRITICLA, code);
}
void non_os_register_critical_statistic(critical_statistic_handler handler)
{
    if (handler != NULL) {
        g_cs_stat_handler = handler;
    }
}

void non_os_unregister_critical_statistic(void)
{
    g_cs_stat_handler = NULL;
}

void non_os_register_critical_record(critical_record_handler handler)
{
    if (handler != NULL) {
        g_cs_record_handler = handler;
    }
}

void non_os_unregister_critical_record(void)
{
    g_cs_record_handler = NULL;
}

void non_os_enter_critical(void)
{
    uint32_t lock_stat = get_int_status();
    int_enter_lock();  //lint !e40 !e718 !e746
    if (!g_non_os_critical_nesting && (g_cs_stat_handler != NULL)) {
        g_cs_stat_handler(CRITICAL_ENTER, (uint32_t)(uintptr_t)__builtin_return_address(0));
    }

    if (g_cs_record_handler != NULL) {
        uint32_t temp_lr = 0;
        get_temp_lr(temp_lr);
        g_cs_record_handler(CRITICAL_ENTER, temp_lr, g_non_os_critical_nesting);
    }

    if (g_non_os_critical_nesting == 0) {
        g_lock_stat = lock_stat;
    }

    g_non_os_critical_nesting++;
    if (!g_non_os_critical_nesting) {  // panic we have not overflowed.
        non_os_panic(0x63);
        return;  //lint !e527  unreachable code
    }

    dsb();
    isb();
}

void non_os_exit_critical(void)
{
    // panic if we are trying to exit critical when no enter critical has been called
    if (!g_non_os_critical_nesting) {
        non_os_panic(0x6f);
        return;  //lint !e527  unreachable code
    }
    g_non_os_critical_nesting--;

    if (g_cs_record_handler != NULL) {
        uint32_t temp_lr = 0;
        get_temp_lr(temp_lr);
        g_cs_record_handler(CRITICAL_EXIT, temp_lr, g_non_os_critical_nesting);
    }

    if (g_non_os_critical_nesting == 0) {
        if (g_cs_stat_handler != NULL) {
            g_cs_stat_handler(CRITICAL_EXIT, (uint32_t)(uintptr_t)__builtin_return_address(0));
        }
        if (g_lock_stat == 0) {
            int_exit_lock();
        }
        dsb();
        isb();
    }
}

void non_os_critical_section_init(void)
{
    int_enter_lock();  /*lint !e40*/
    g_non_os_critical_nesting = 0;
    int_exit_lock();  /*lint !e40*/
}

bool non_os_is_in_critical_section(void)
{
    return g_non_os_critical_nesting > 0;
}


static const mem_pool_hook_ops *g_mempool_ops = NULL;

void mem_pool_ops_hook_reg(const mem_pool_hook_ops *ops)
{
    g_mempool_ops = ops;
}

void *mem_pool_malloc_hook(uint32_t pool_id, uint32_t size)
{
    if (g_mempool_ops != NULL) {
        return g_mempool_ops->mem_pool_alloc(pool_id, size);
    }
    return NULL;
}

bool mem_pool_mfree_hook(uint32_t pool_id, void *addr)
{
    if (g_mempool_ops != NULL) {
        return g_mempool_ops->mem_pool_free(pool_id, addr);
    }
    return false;
}

static const mem_dfx_hook_ops *g_mem_dfx_ops = NULL;

void mem_dfx_ops_hook_reg(const mem_dfx_hook_ops *ops)
{
    g_mem_dfx_ops = ops;
}

void mem_alloc_dfx_hook(void *addr, uint32_t size, uintptr_t caller_ra)
{
    if (g_mem_dfx_ops == NULL) { return; }

    if (g_mem_dfx_ops->mem_alloc_proc != NULL) {
        g_mem_dfx_ops->mem_alloc_proc(addr, size, caller_ra, 0);
    }
    return;
}

void mem_free_dfx_hook(void *addr)
{
    if (g_mem_dfx_ops == NULL) { return; }

    if (g_mem_dfx_ops->mem_free_proc != NULL) {
        g_mem_dfx_ops->mem_free_proc(addr);
    }
    return;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
