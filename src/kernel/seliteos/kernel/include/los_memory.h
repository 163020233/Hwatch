/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: LiteOS Mem Module HeadFile
 * Author: Huawei LiteOS Team
 * Create: 2022-06-01
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --------------------------------------------------------------------------- */

/**
 * @defgroup los_memory  Dynamic memory
 * @ingroup kernel
 */

#ifndef LOS_MEMORY_H
#define LOS_MEMORY_H

#include "los_base.h"
#include "los_toolchain.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define OS_MEM_ALIGN(p, alignSize)   (((UINTPTR)(p) + (alignSize) - (UINTPTR)1) & \
                                       ~((UINTPTR)((alignSize) - (UINTPTR)1)))
#define OS_MEM_ALIGN_SIZE   (UINT32)(sizeof(UINTPTR))

#ifdef LOSCFG_KERNEL_MEM_ALLOC
/**
 * @ingroup los_memory
 * The start address of exception interaction dynamic memory pool address, when the exception
 * interaction feature not support, m_aucSysMem0 equals to m_aucSysMem1.
 */
#define m_aucSysMem0 m_aucSysMem1

/**
 * @ingroup los_memory
 * The start address of system dynamic memory pool address.
 */
extern UINT8 *m_aucSysMem1;
#endif

/**
 * @ingroup los_memory
 * @brief Deinitialize dynamic memory.
 *
 * @par Description:
 * This API is used to deinitialize the dynamic memory of a doubly linked list.
 *
 * @param pool          [IN] Starting address of memory.
 *
 * @retval #LOS_NOK    The dynamic memory fails to be deinitialized.
 * @retval #LOS_OK     The dynamic memory is successfully deinitialized.
 * @par Dependency:
 * <ul>
 * <li>los_memory.h: the header file that contains the API declaration.</li>
 * </ul>
 */
extern UINT32 LOS_MemDeInit(const VOID *pool);

/**
 * @ingroup los_memory
 * Memory pool extern information structure
 */
typedef struct {
    UINT32 totalUsedSize;
    UINT32 totalFreeSize;
    UINT32 maxFreeNodeSize;
    UINT32 usedNodeNum;
    UINT32 freeNodeNum;
#ifdef LOSCFG_MEM_TASK_STAT
    UINT32 usageWaterLine;  /**< this structure member is defined only when LOSCFG_MEM_TASK_STAT is defined. */
#endif
} LOS_MEM_POOL_STATUS;

/**
 * @ingroup los_memory
 * The attributes to config memory pool.
 * Bit 0 is used to config the memory pool containing slab.
 * Bit 1 is used to config the memory pool lock type.
 * The other 30 bits are reserved now.
 */
#define LOS_MEM_INIT_ATTR_SLAB_POS 0U
#define LOS_MEM_INIT_ATTR_SLAB_MSK 1U /* << LOS_MEM_INIT_ATTR_SLAB_POS */

/**
 * @ingroup los_memory
 * Memory management lock type for critical protection.
 * Bit 0 is used to config the lock by using mutex.
 * Bit 1 is used to config the lock by using disable interrupt.
 */
#define LOS_MEM_INIT_ATTR_LOCK_POS 1U
#define LOS_MEM_INIT_ATTR_LOCK_MSK 0x2U /* << LOS_MEM_INIT_ATTR_LOCK_POS */

 /**
 * @ingroup los_memory
 * @brief Initialize dynamic memory.
 *
 * @par Description:
 * This API is used to initialize the dynamic memory of a doubly linked list.
 * @attention
 * <ul>
 * <li>The size parameter value should match the following two conditions :
 * 1) Be less than or equal to the Memory pool size;
 * 2) Be greater than the size of OS_MEM_MIN_POOL_SIZE.</li>
 * <li>Call this API when dynamic memory needs to be initialized during the startup of Huawei LiteOS.</li>
 * <li>The parameter input must be OS_MEM_ALIGN_SIZE byte-aligned.</li>
 * <li>The init area [pool, pool + size] should not conflict with other pools.</li>
 * </ul>
 *
 * @param pool         [IN] Starting address of memory.
 * @param size         [IN] Memory size.
 * @param attr         [IN] Attributes to config memory pool, see LOS_MEM_LOCK_USE_MUX above.
 *
 * @retval #LOS_NOK    The dynamic memory fails to be initialized.
 * @retval #LOS_OK     The dynamic memory is successfully initialized.
 * @par Dependency:
 * <ul>
 * <li>los_memory.h: the header file that contains the API declaration.</li>
 * </ul>
 */
extern UINT32 LOS_MemPoolInit(VOID *pool, UINT32 size, UINT32 attr);

/**
 * @ingroup los_memory
 * @brief Initialize dynamic memory.
 *
 * @par Description:
 * This API is used to initialize the dynamic memory of a doubly linked list.
 * @attention
 * <ul>
 * <li>The size parameter value should match the following two conditions :
 * 1) Be less than or equal to the Memory pool size;
 * 2) Be greater than the size of OS_MEM_MIN_POOL_SIZE.</li>
 * <li>Call this API when dynamic memory needs to be initialized during the startup of LiteOS.</li>
 * <li>The parameter input must be OS_MEM_ALIGN_SIZE byte-aligned.</li>
 * <li>The init area [pool, pool + size] should not conflict with other pools.</li>
 * </ul>
 *
 * @param pool         [IN] Starting address of memory.
 * @param size         [IN] Memory size.
 *
 * @retval #LOS_NOK    The dynamic memory fails to be initialized.
 * @retval #LOS_OK     The dynamic memory is successfully initialized.
 * @par Dependency:
 * <ul>
 * <li>los_memory.h: the header file that contains the API declaration.</li>
 * </ul>
 */
extern UINT32 LOS_MemInit(VOID *pool, UINT32 size);

/**
 * @ingroup los_memory
 * @brief Allocate dynamic memory.
 *
 * @par Description:
 * This API is used to allocate a memory block of which the size is specified.
 * @attention
 * <ul>
 * <li>The input pool parameter must be initialized via func LOS_MemInit.</li>
 * <li>The size of the input parameter size can not be greater than the memory pool size that specified at the second
 * input parameter of LOS_MemInit.</li>
 * <li>The size of the input parameter size must be four byte-aligned.</li>
 * </ul>
 *
 * @param  pool    [IN] Pointer to the memory pool that contains the memory block to be allocated.
 * @param  size    [IN] Size of the memory block to be allocated (unit: byte).
 *
 * @retval #NULL          The memory fails to be allocated.
 * @retval #VOID*         The memory is successfully allocated, and the API returns the pointer to
 *                        the allocated memory block.
 * @par Dependency:
 * <ul><li>los_memory.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_MemRealloc | LOS_MemAllocAlign | LOS_MemFree
 */
extern VOID *LOS_MemAlloc(VOID *pool, UINT32 size);

/**
 * @ingroup los_memory
 * @brief Free dynamic memory.
 *
 * @par Description:
 * This API is used to free specified dynamic memory that has been allocated.
 * @attention
 * <ul>
 * <li>The input pool parameter must be initialized via func LOS_MemInit.</li>
 * <li>The input ptr parameter must be allocated by LOS_MemAlloc or LOS_MemAllocAlign or LOS_MemRealloc.</li>
 * </ul>
 *
 * @param  pool  [IN] Pointer to the memory pool that contains the dynamic memory block to be freed.
 * @param  ptr   [IN] Starting address of the memory block to be freed.
 *
 * @retval #LOS_NOK      The memory block fails to be freed because the starting address of the memory block is
 *                       invalid, or the memory overwriting occurs.
 * @retval #LOS_OK       The memory block is successfully freed.
 * @par Dependency:
 * <ul><li>los_memory.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_MemAlloc | LOS_MemRealloc | LOS_MemAllocAlign
 */
extern UINT32 LOS_MemFree(const VOID *pool, const VOID *ptr);

/**
 * @ingroup los_memory
 * @brief Re-allocate a memory block.
 *
 * @par Description:
 * This API is used to allocate a new memory block of which the size is specified by size if the original memory
 * block size is insufficient. The new memory block will copy the data in the original memory block of which the
 * address is specified by ptr. The size of the new memory block determines the maximum size of data to be copied.
 * After the new memory block is created, the original one is freed.
 * @attention
 * <ul>
 * <li>The input pool parameter must be initialized via func LOS_MemInit.</li>
 * <li>The input ptr parameter must be allocated by LOS_MemAlloc; if the ptr is allocated by LOS_MemAllocAlign,
 * the alignment of the newly allocated memory address cannot be guaranteed to be consistent with the original
 * memory address.</li>
 * <li>The input size parameter can not be greater than the memory pool size that specified at the second
 * input parameter of LOS_MemInit.</li>
 * <li>The input size parameter must be aligned as follows: 1) if the ptr is allocated by LOS_MemAlloc,
 * it must be four byte-aligned; 2) if the ptr is allocated by LOS_MemAllocAlign, it must be aligned with the size of
 * the input parameter boundary of LOS_MemAllocAlign.</li>
 * <li> If the user has special requirements for address alignment, it is not recommended to use this realloc function
 *  for address allocated by LOS_MemAllocAlign. For example, on a 32-bit system, LiteOS's default address alignment
 *  is 4 bytes, and if the boundary specified in LOS_MemAllocAlign is 8 bytes, it is cannot be ensured that the
 *  reallocated address is 8 bytes aligned.</li>
 * </ul>
 *
 * @param  pool     [IN] Pointer to the memory pool that contains the original and new memory blocks.
 * @param  ptr      [IN] Address of the original memory block.
 * @param  size     [IN] Size of the new memory block.
 *
 * @retval #NULL     The memory fails to be re-allocated.
 * @retval #VOID*    The memory is successfully re-allocated, and the API returns the pointer to the new memory block.
 * @par Dependency:
 * <ul><li>los_memory.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_MemAlloc | LOS_MemAllocAlign | LOS_MemFree
 * @since Huawei LiteOS V100R001C00
 */
extern VOID *LOS_MemRealloc(VOID *pool, VOID *ptr, UINT32 size);

/**
 * @ingroup los_memory
 * @brief Allocate aligned memory.
 *
 * @par Description:
 * This API is used to allocate memory blocks of specified size and of which the starting addresses are aligned on
 * a specified boundary.
 * @attention
 * <ul>
 * <li>The input pool parameter must be initialized via func LOS_MemInit.</li>
 * <li>The size of the input parameter size can not be greater than the memory pool size that specified at the second
 * input parameter of LOS_MemInit.</li>
 * <li>The alignment parameter value must be a power of 2 with the minimum value being 4.</li>
 * </ul>
 *
 * @param  pool      [IN] Pointer to the memory pool that contains the memory blocks to be allocated.
 * @param  size      [IN] Size of the memory to be allocated.
 * @param  boundary  [IN] Boundary on which the memory is aligned.
 *
 * @retval #NULL    The memory fails to be allocated.
 * @retval #VOID*   The memory is successfully allocated, and the API returns the pointer to the allocated memory.
 * @par Dependency:
 * <ul><li>los_memory.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_MemAlloc | LOS_MemRealloc | LOS_MemFree
 */
extern VOID *LOS_MemAllocAlign(VOID *pool, UINT32 size, UINT32 boundary);

/**
 * @ingroup los_memory
 * @brief Get the size of memory pool's size.
 *
 * @par Description:
 * This API is used to get the size of memory pool' total size.
 * @attention
 * The input pool parameter must be initialized via func LOS_MemInit.
 *
 * @param  pool           [IN] A pointer pointed to the memory pool.
 *
 * @retval #LOS_NOK        The input parameter pool is NULL.
 * @retval #UINT32         The size of the memory pool.
 * @par Dependency:
 * <ul><li>los_memory.h: the header file that contains the API declaration.</li></ul>
 */
extern UINT32 LOS_MemPoolSizeGet(const VOID *pool);

/**
 * @ingroup los_memory
 * @brief Get the size of memory totally used.
 *
 * @par Description:
 * This API is used to get the size of memory totally used in memory pool.
 * @attention
 * The input pool parameter must be initialized via func LOS_MemInit.
 *
 * @param  pool           [IN] A pointer pointed to the memory pool.
 *
 * @retval #LOS_NOK        The input parameter pool is NULL.
 * @retval #UINT32         The size of the used memory pool.
 * @par Dependency:
 * <ul><li>los_memory.h: the header file that contains the API declaration.</li></ul>
 */
extern UINT32 LOS_MemTotalUsedGet(const VOID *pool);

/**
 * @ingroup los_memory
 * @brief Get the number of free memory nodes.
 *
 * @par Description:
 * This API is used to get the number of free memory nodes in memory pool.
 * @attention
 * The input pool parameter must be initialized via func LOS_MemInit.
 *
 * @param  pool           [IN] A pointer pointed to the memory pool.
 *
 * @retval #LOS_NOK        The input parameter pool is NULL.
 * @retval #UINT32         The number of free memory nodes.
 * @par Dependency:
 * <ul><li>los_memory.h: the header file that contains the API declaration.</li></ul>
 */
extern UINT32 LOS_MemFreeBlksGet(VOID *pool);

/**
 * @ingroup los_memory
 * @brief Get the number of used memory nodes.
 *
 * @par Description:
 * This API is used to get the number of used memory nodes in memory pool.
 * @attention
 * The input pool parameter must be initialized via func LOS_MemInit.
 *
 * @param  pool           [IN] A pointer pointed to the memory pool.
 *
 * @retval #LOS_NOK        The input parameter pool is NULL.
 * @retval #UINT32         The number of used memory nodes.
 * @par Dependency:
 * <ul><li>los_memory.h: the header file that contains the API declaration.</li></ul>
 */
extern UINT32 LOS_MemUsedBlksGet(const VOID *pool);

/**
 * @ingroup los_memory
 * @brief Get the task ID of a used memory node.
 *
 * @par Description:
 * This API is used to get the task ID of a used memory node.
 * @attention
 * <ul>
 * <li>The input ptr parameter must be allocated by LOS_MemAlloc or LOS_MemAllocAlign.</li>
 * <li>This interface only support obtain the task ID of a used memory node which is allocated from the system memory
 * pool (OS_SYS_MEM_ADDR) at present.</li>
 * <li>This API can be enabled when you set the macro value of LOSCFG_MEM_DEBUG or LOSCFG_MEM_TASK_STAT is defined
 * in LiteOS.</li>
 * </ul>
 *
 * @param  ptr               [IN] A used memory node.
 *
 * @retval #OS_INVALID        The input parameter ptr is illegal.
 * @retval #UINT32            The task ID of used memory node ptr.
 * @par Dependency:
 * <ul><li>los_memory.h: the header file that contains the API declaration.</li></ul>
 */
extern UINT32 LOS_MemTaskIdGet(const VOID *ptr);

/**
 * @ingroup los_memory
 * @brief Get the address of the next byte of the last used byte.
 *
 * @par Description:
 * This API is used to get the address of the next byte of the last used byte.
 * @attention
 * <ul>
 * <li>The input pool parameter must be initialized via func LOS_MemInit.</li>
 * <li>The last used byte may the last byte of an used node or the last byte of an unused node head</li>
 * </ul>
 *
 * @param  pool               [IN] A pointer pointed to the memory pool.
 *
 * @retval #LOS_NOK           The input parameter pool is NULL.
 * @retval #UINTPTR           The pointer to the next byte of the last used byte.
 * @par Dependency:
 * <ul><li>los_memory.h: the header file that contains the API declaration.</li></ul>
 */
extern UINTPTR LOS_MemLastUsedGet(const VOID *pool);

/**
 * @ingroup los_memory
 * @brief Get the information of memory pool.
 *
 * @par Description:
 * This API is used to get the information of memory pool.
 * @attention
 * The input pool parameter must be initialized via func LOS_MemInit.
 *
 * @param  pool                 [IN] A pointer pointed to the memory pool.
 * @param  poolStatus           [OUT] A pointer for storage the pool status
 *
 * @retval #LOS_NOK           The input parameter pool is NULL or invalid.
 * @retval #LOS_OK            Get memory information successfully.
 * @par Dependency:
 * <ul><li>los_memory.h: the header file that contains the API declaration.</li></ul>
 */
extern UINT32 LOS_MemInfoGet(const VOID *pool, LOS_MEM_POOL_STATUS *poolStatus);

/**
 * @ingroup los_memory
 * @brief Get the number of free node in every size.
 *
 * @par Description:
 * This API is used to print the number of free node in every size.
 * @attention
 * The input pool parameter must be initialized via func LOS_MemInit.
 *
 * @param  pool               [IN] A pointer pointed to the memory pool.
 *
 * @retval #LOS_NOK           The input parameter pool is NULL.
 * @retval #LOS_OK            Print the number of free node in every size successfully.
 * @par Dependency:
 * <ul><li>los_memory.h: the header file that contains the API declaration.</li></ul>
 */
extern UINT32 LOS_MemFreeNodeShow(const VOID *pool);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LOS_MEMORY_H */
