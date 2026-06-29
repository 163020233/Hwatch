/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: addr
 * Author: FBB OS Team
 * Create: 2021-12-16
 */

#include <string.h>
#include <los_memory.h>
#include <los_config.h>
#include <linux/kernel.h>

#ifdef HW_LITEOS_OPEN_VERSION_NUM
#include <securec.h>
#endif
#include "soc_osal.h"
#include "osal_inner.h"
static void *g_osal_heap = NULL;

void osal_mem_specified_addr_register(void *addr)
{
    g_osal_heap = addr;
}

#ifdef ENABLE_MEM_POOL_ALLOC
#include "mem_pool.h"
#endif
// 针对已rom化的代码，实现内存节点记录
#if defined(LOSCFG_MEM_DFX_SHOW_CALLER_RA_PATCH)
#include "memory_info.h"
#endif

static unsigned int mem_alloc_dfx_preproc(unsigned int osal_gfp_flag, unsigned int ra)
{
    unsigned int caller_ra = (osal_gfp_flag > 0x0F) ? osal_gfp_flag : ra;
#if defined(LOSCFG_MEM_DFX_SHOW_CALLER_RA)
    LOS_SaveCallerRa((UINTPTR)caller_ra);
#elif defined(LOSCFG_MEM_DFX_SHOW_CALLER_RA_PATCH)
    // 针对已rom化的代码，实现内存节点记录
    saveCallerRa((UINTPTR)caller_ra);
#endif
    return caller_ra;
}

static void mem_alloc_dfx_postproc(void *addr, unsigned long size, unsigned int ra)
{
#if !defined(ENABLE_PLAT_MEM_DFX)
    osal_unused(addr);
    osal_unused(size);
    osal_unused(ra);
#else
    LOS_MemAllocDfxHook(addr, size, ra);  // call mem alloc dfx hook function
#endif
}

static void mem_free_dfx_proc(void *addr)
{
#if !defined(ENABLE_PLAT_MEM_DFX)
    osal_unused(addr);
#else
    LOS_MemFreeDfxHook(addr); // mem fee dfx
#endif
}

void *osal_kmalloc(unsigned long size, unsigned int osal_gfp_flag)
{
    void *addr = NULL;
    /* mem calloc dfx pre-process */
    unsigned int ra = mem_alloc_dfx_preproc(osal_gfp_flag, (UINTPTR)__builtin_return_address(0));

#ifdef ENABLE_MEM_POOL_ALLOC
    addr = mem_pool_mem_alloc(GLOBAL_MEM_POOL_ID_DEFAULT, size);
    if (addr == NULL)
#endif
    {
#ifdef OSAL_USE_SPECIFIED_HEAP
        addr = LOS_MemAlloc((void*)g_osal_heap, size);
#else
        addr = LOS_MemAlloc((void*)m_aucSysMem0, size);
#endif
    }

    /* mem calloc dfx post-process */
    mem_alloc_dfx_postproc(addr, size, ra);
    return addr;
}

void *osal_kzalloc(unsigned long size, unsigned int osal_gfp_flag)
{
    void *addr = NULL;
    /* mem calloc dfx pre-process */
    unsigned int ra = mem_alloc_dfx_preproc(osal_gfp_flag, (UINTPTR)__builtin_return_address(0));

#ifdef ENABLE_MEM_POOL_ALLOC
    addr = mem_pool_mem_alloc(GLOBAL_MEM_POOL_ID_DEFAULT, size);
    if (addr == NULL)
#endif
    {
#ifdef OSAL_USE_SPECIFIED_HEAP
        addr = LOS_MemAlloc((void*)g_osal_heap, size);
#else
        addr = LOS_MemAlloc((void*)m_aucSysMem0, size);
#endif
    }

    if (addr != NULL) {
        memset_s(addr, size, 0, size);
    }

    mem_alloc_dfx_postproc(addr, size, ra); /* mem calloc dfx post-process */
    return addr;
}

void *osal_kmalloc_align(unsigned int size, unsigned int osal_gfp_flag, unsigned int boundary)
{
    void *addr = NULL;
    /* mem calloc dfx pre-process */
    unsigned int ra = mem_alloc_dfx_preproc(osal_gfp_flag, (UINTPTR)__builtin_return_address(0));

#ifdef OSAL_USE_SPECIFIED_HEAP
    addr = LOS_MemAllocAlign((void*)g_osal_heap, size, boundary);
#else
    addr = LOS_MemAllocAlign((void*)m_aucSysMem0, size, boundary);
#endif

    /* mem calloc dfx post-process */
    mem_alloc_dfx_postproc(addr, size, ra);
    return addr;
}

void *osal_kzalloc_align(unsigned int size, unsigned int osal_gfp_flag, unsigned int boundary)
{
    /* mem calloc dfx pre-process */
    unsigned int ra = mem_alloc_dfx_preproc(osal_gfp_flag, (UINTPTR)__builtin_return_address(0));

#ifdef OSAL_USE_SPECIFIED_HEAP
    void *addr = LOS_MemAllocAlign((void*)g_osal_heap, size, boundary);
#else
    void *addr = LOS_MemAllocAlign((void*)m_aucSysMem0, size, boundary);
#endif

    if (addr != NULL) {
        memset_s(addr, size, 0, size);
    }

    /* mem calloc dfx post-process */
    mem_alloc_dfx_postproc(addr, size, ra);
    return addr;
}

void *osal_krealloc(void *ptr, unsigned long size, unsigned int osal_gfp_flag)
{
    void *addr = NULL;
    /* mem calloc dfx pre-process */
    unsigned int ra = mem_alloc_dfx_preproc(osal_gfp_flag, (UINTPTR)__builtin_return_address(0));

#ifdef OSAL_USE_SPECIFIED_HEAP
    addr = LOS_MemRealloc((void*)g_osal_heap, ptr, size);
#else
    addr = LOS_MemRealloc((void*)m_aucSysMem0, ptr, size);
#endif

    /* mem calloc dfx post-process */
    mem_alloc_dfx_postproc(addr, size, ra);
    return addr;
}

void osal_kfree(void *addr)
{
    mem_free_dfx_proc(addr);  // mem fee dfx

#ifdef ENABLE_MEM_POOL_ALLOC
    if (mem_pool_mem_free(GLOBAL_MEM_POOL_ID_DEFAULT, addr) == true) {
        return;
    }
#endif
#ifdef OSAL_USE_SPECIFIED_HEAP
    (void)LOS_MemFree((void*)g_osal_heap, (void*)addr);
#else
    (void)LOS_MemFree((void*)m_aucSysMem0, (void*)addr);
#endif
}

int osal_pool_mem_init_ext(void *pool, unsigned int size, unsigned int attr)
{
    return (int)LOS_MemPoolInit(pool, size, attr);
}

int osal_pool_mem_init(void *pool, unsigned int size)
{
    return (int)LOS_MemInit(pool, size);
}

void *osal_pool_mem_alloc(void *pool, unsigned int size)
{
#ifdef LOSCFG_MEM_DFX_SHOW_CALLER_RA
    LOS_SaveCallerRa((UINTPTR)__builtin_return_address(0));
#endif
    return LOS_MemAlloc(pool, size);
}

void *osal_pool_mem_alloc_align(void *pool, unsigned int size, unsigned int boundary)
{
#ifdef LOSCFG_MEM_DFX_SHOW_CALLER_RA
    LOS_SaveCallerRa((UINTPTR)__builtin_return_address(0));
#endif
    return LOS_MemAllocAlign(pool, size, boundary);
}

void osal_pool_mem_free(void *pool, const void *addr)
{
    (void)LOS_MemFree(pool, (void*)addr);
}

int osal_pool_mem_deinit(void *pool)
{
#ifndef TINY_KERNEL
#ifdef LOSCFG_MEM_MUL_POOL
    unsigned int ret = LOS_MemDeInit(pool);
    if (ret != LOS_OK) {
        osal_log("MemDeInit fail! ret=%#x.\n", ret);
        return OSAL_FAILURE;
    }
#endif
#endif
    return OSAL_SUCCESS;
}

void *osal_vmalloc(unsigned long size)
{
    void *addr = NULL;

#ifdef HW_LITEOS_OPEN_VERSION_NUM
    /* mem calloc dfx pre-process */
    unsigned int ra = mem_alloc_dfx_preproc(0, (UINTPTR)__builtin_return_address(0));

#ifdef ENABLE_MEM_POOL_ALLOC
    addr = mem_pool_mem_alloc(GLOBAL_MEM_POOL_ID_DEFAULT, size);
    if (addr == NULL)
#endif
    {
#ifdef OSAL_USE_SPECIFIED_HEAP
        addr = LOS_MemAlloc((void*)g_osal_heap, size);
#else
        addr = LOS_MemAlloc((void*)m_aucSysMem0, size);
#endif
    }
    /* mem calloc dfx post-process */
    mem_alloc_dfx_postproc(addr, size, ra);
#else
    addr = LOS_VMalloc(size);
#endif
    return addr;
}

void *osal_vzalloc(unsigned long size)
{
    void *addr = NULL;
#ifdef HW_LITEOS_OPEN_VERSION_NUM
    /* mem calloc dfx pre-process */
    unsigned int ra = mem_alloc_dfx_preproc(0, (UINTPTR)__builtin_return_address(0));

#ifdef ENABLE_MEM_POOL_ALLOC
    addr = mem_pool_mem_alloc(GLOBAL_MEM_POOL_ID_DEFAULT, size);
    if (addr == NULL)
#endif
    {
#ifdef OSAL_USE_SPECIFIED_HEAP
        addr = LOS_MemAlloc((void*)g_osal_heap, size);
#else
        addr = LOS_MemAlloc((void*)m_aucSysMem0, size);
#endif
    }
    /* mem calloc dfx post-process */
    mem_alloc_dfx_postproc(addr, size, ra);
#else
    addr = LOS_VMalloc(size);
#endif
    if (addr != NULL) {
        memset_s(addr, size, 0, size);
    }
    return addr;
}

void osal_vfree(void *addr)
{
    mem_free_dfx_proc(addr);  // mem fee dfx

#ifdef HW_LITEOS_OPEN_VERSION_NUM
#ifdef ENABLE_MEM_POOL_ALLOC
    if (mem_pool_mem_free(GLOBAL_MEM_POOL_ID_DEFAULT, addr) == true) {
        return;
    }
#endif
#ifdef OSAL_USE_SPECIFIED_HEAP
    (void)LOS_MemFree((void*)g_osal_heap, (void*)addr);
#else
    (void)LOS_MemFree((void*)m_aucSysMem0, (void*)addr);
#endif
#else
    LOS_VFree((void *)addr);
#endif
}

void *osal_ioremap(unsigned long phys_addr, unsigned long size)
{
#ifdef HW_LITEOS_OPEN_VERSION_NUM
    return ioremap_nocache(phys_addr, size);
#else
    return ioremap(phys_addr, size);
#endif
}

void *osal_ioremap_nocache(unsigned long phys_addr, unsigned long size)
{
    return ioremap_nocache(phys_addr, size);
}
void *osal_ioremap_wc(unsigned long phys_addr, unsigned long size)
{
    return ioremap_nocache(phys_addr, size);
}

void *osal_ioremap_cached(unsigned long phys_addr, unsigned long size)
{
    return ioremap_cached(phys_addr, size);
}

void osal_iounmap(void *addr, unsigned long size)
{
    return iounmap(addr);
}

/*
* copy success: return 0;
* copy fail : return remain bytes
*/
unsigned long osal_copy_from_user(void *to, const void *from, unsigned long n)
{
    if (to == NULL || from == NULL) {
        return n;
    }
#ifdef HW_LITEOS_OPEN_VERSION_NUM
    errno_t ret = memcpy_s(to, n, from, n);
    return (ret == EOK) ? 0 : n;
#else
    int ret = LOS_CopyToKernel(to, n, from, n);
    if (ret != 0) {
        return n;
    }
    return 0;
#endif
}
/*
* copy success: return 0;
* copy fail : return remain bytes
*/
unsigned long osal_copy_to_user(void *to, const void *from, unsigned long n)
{
    if (to == NULL || from == NULL) {
        return n;
    }
#ifdef HW_LITEOS_OPEN_VERSION_NUM
    errno_t ret = memcpy_s(to, n, from, n);
    return (ret == EOK) ? 0 : n;
#else
    int ret = LOS_CopyFromKernel(to, n, from, n);
    if (ret != 0) {
        return n;
    }
    return 0;
#endif
}

void *osal_phys_to_virt(unsigned long addr)
{
    return (void *)(UINTPTR)addr;
}

unsigned long osal_virt_to_phys(const void *virt_addr)
{
    return (unsigned long)(UINTPTR)virt_addr;
}

int osal_access_ok(int type, const void *addr, unsigned long size)
{
    osal_log("not support in liteos!\n");
    return 1;
}
