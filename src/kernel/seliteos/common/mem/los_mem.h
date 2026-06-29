/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All
 * rights reserved. Description: seliteos memory management API header file.
 *
 * Create: 2024-07-23
 */

#ifndef LOS_MEM_H
#define LOS_MEM_H

#include "los_errno.h"
#include "los_typedef.h"
#include "asm/memmap_config.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @if Eng
 * @brief  Kernel memory pool initialization.
 *
 * @retval LOS_OK   The initialization is successful.
 * @retval Other    The initialization failed.
 * @else
 * @brief  内核内存池初始化
 *
 * @retval LOS_OK   初始化成功。
 * @retval Other    初始化失败。
 */
UINT32 los_kernel_mem_init(VOID);

/**
 * @if Eng
 * @brief  Kernel memory pool deinitialization.
 *
 * @retval LOS_OK   The deinitialization is successful.
 * @retval Other    The deinitialization failed.
 * @else
 * @brief  内核内存池去初始化
 *
 * @retval LOS_OK           去初始化成功。
 * @retval Other            去初始化失败。
 */
UINT32 los_kernel_mem_deinit(VOID);

/**
 * @if Eng
 * @brief  Allocate a kernelspace memory block.
 *
 * @param  [in]   size The size of the memory block to allocate.
 * @retval NULL The memory block is not successfully allocated.
 * @retval Other The address of the successfully allocated memory block.
 * @else
 * @brief  分配一个内核空间内存块。
 *
 * @param  [in]   size 需要分配的内存块的大小。
 * @retval NULL 内存块未成功分配。
 * @retval 其他 成功分配的内存块的地址。
 * @endif
 */
VOID *los_kmalloc(INT32 size);

/**
 * @if Eng
 * @brief  Free a kernelspace memory block.
 *
 * @param  [in]   ptr The pointer to the memory block to free.
 * @else
 * @brief  释放一个内核空间内存块。
 *
 * @param  [in]   ptr 指向需要释放的内存块的指针。
 */
VOID los_kfree(VOID *ptr);

/**
 * @if Eng
 * @brief  dump the kernel memory pool info.
 *
 * @else
 * @brief  打印内核内存信息。
 *
 */
VOID los_kmem_info_print(VOID);

/**
 * @if Eng
 * @brief  User space memory pool initialization.
 *
 * @param  [in]   mpool The user space memory pool start address.
 * @param  [in]   size The user space memory pool size.
 * @retval LOS_OK   The initialization is successful.
 * @retval Other    The initialization failed.
 * @else
 * @brief  用户空间内存池初始化。
 *
 * @param  [in]   mpool 用户空间内存池起始地址。
 * @param  [in]   size 用户空间内存池大小。
 * @retval LOS_OK   初始化成功。
 * @retval Other    初始化失败。
 */
UINT32 los_usr_mem_init(VOID *mpool, UINT32 size);

/**
 * @if Eng
 * @brief  User space memory pool deinitialization.
 *
 * @param  [in]   mpool The user space memory pool start address.
 * @retval LOS_OK   The deinitialization is successful.
 * @retval Other    The deinitialization failed.
 * @else
 * @brief  用户空间内存池去初始化。
 *
 * @param  [in]   mpool 用户空间内存池起始地址。
 * @retval LOS_OK           去初始化成功。
 * @retval Other            去初始化失败。
 */
UINT32 los_usr_mem_deinit(VOID *mpool);

/**
 * @if Eng
 * @brief  Allocate a memory block from the user space heap memory pool.
 *
 * @param  [in]   size The size of the memory block to allocate.
 * @retval NULL        Failure.
 * @retval Other       Success. The pointer to the allocated memory block.
 * @else
 * @brief  从用户空间堆内存池中分配一个内存块。
 *
 * @param  [in]   size 需要分配的内存块的大小。
 * @retval NULL        失败。
 * @retval Other       成功。指向分配的内存块的指针。
 */
VOID *los_usr_malloc(INT32 size);

/**
 * @if Eng
 * @brief  Free a memory block to the user space heap memory pool.
 *
 * @param  [in]   ptr The pointer to the memory block to free.
 * @else
 * @brief  将一个内存块释放回用户空间堆内存池。
 *
 * @param  [in]   ptr 指向需要释放的内存块的指针。
 */
VOID los_usr_free(VOID *ptr);

/**
 * @if Eng
 * @brief  dump the TA memory pool info.
 *
 * @else
 * @brief  打印当前TA内存信息。
 *
 */
VOID los_usr_mem_info_print(VOID);

/**
 *
 * @if Eng
 * @brief  Kernel memory pool initialization.
 *
 * @retval LOS_OK   The initialization is successful.
 * @retval Other    The initialization failed.
 * @else
 * @brief  drvbox内存池初始化
 *
 * @retval LOS_OK   初始化成功。
 * @retval Other    初始化失败。
 */
LITE_OS_SEC_DRVBOX_TEXT UINT32 los_drvbox_mem_init(VOID);

/**
 * @if Eng
 * @brief  Kernel memory pool deinitialization.
 *
 * @retval LOS_OK   The deinitialization is successful.
 * @retval Other    The deinitialization failed.
 * @else
 * @brief  drvbox内存池去初始化
 *
 * @retval LOS_OK           去初始化成功。
 * @retval Other            去初始化失败。
 */
LITE_OS_SEC_DRVBOX_TEXT UINT32 los_drvbox_mem_deinit(VOID);

/**
 * @if Eng
 * @brief  Allocate a drvboxspace memory block.
 *
 * @param  [in]   size The size of the memory block to allocate.
 * @retval NULL The memory block is not successfully allocated.
 * @retval Other The address of the successfully allocated memory block.
 * @else
 * @brief  分配一个drvbox空间内存块。
 *
 * @param  [in]   size 需要分配的内存块的大小。
 * @retval NULL 内存块未成功分配。
 * @retval 其他 成功分配的内存块的地址。
 * @endif
 */
LITE_OS_SEC_DRVBOX_TEXT VOID *los_drvbox_malloc(INT32 size);

/**
 * @if Eng
 * @brief  Free a drvboxspace memory block.
 *
 * @param  [in]   ptr The pointer to the memory block to free.
 * @else
 * @brief  释放一个drvbox空间内存块。
 *
 * @param  [in]   ptr 指向需要释放的内存块的指针。
 */
LITE_OS_SEC_DRVBOX_TEXT VOID los_drvbox_free(VOID *ptr);

/**
 * @if Eng
 * @brief  dump the drvbox memory pool info.
 *
 * @else
 * @brief  打印drvbox内存池信息。
 *
 */
LITE_OS_SEC_DRVBOX_TEXT VOID los_drvbox_mem_info_print(VOID);

/**
 * @if Eng
 * @brief  dump the memory pool info, according to the current env.
 *
 * @else
 * @brief  根据当前运行环境动态打印内存信息。
 *
 */
VOID los_mem_info_print(VOID);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
