/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Cortex-M Secure heap Implementation
 * Author: Huawei LiteOS Team
 * Create: 2022-01-07
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

#include "arch/secure/secure_heap.h"
#include "arch/secure/secure_pri.h"

typedef struct SecureMemoryNodeStr {
    struct SecureMemoryNodeStr *next;
    UINT32 size;
} SecureMemoryNode;

extern CHAR __heap_addr_start, __heap_size;

STATIC VOID * const g_secureHeap = (VOID *)(&__heap_addr_start);
#define SECURE_MEM_SIZE         ((UINTPTR)&__heap_size)

#define MEMORY_CAST(t, exp)     ((t)(exp))
#define MEMORY_ALIGN_SIZE       4
#define MEMORY_ALIGNE(sz)       (((sz) + (MEMORY_ALIGN_SIZE - 1)) & ~(MEMORY_ALIGN_SIZE - 1))
#define MALLOC_MAXSIZE          ((0xFFFFFFFF - sizeof(SecureMemoryNode)) & ~(MEMORY_ALIGN_SIZE - 1))

UINT32 ArchSecureMemInit(VOID)
{
    SecureMemoryNode *headNode = NULL;
    SecureMemoryNode *tailNode = NULL;

    if ((g_secureHeap == NULL) || (SECURE_MEM_SIZE < (sizeof(SecureMemoryNode)))) {
        return LOS_NOK;
    }

    headNode = MEMORY_CAST(SecureMemoryNode *, g_secureHeap);
    headNode->size = 0;
    tailNode = MEMORY_CAST(SecureMemoryNode *,
                           ((UINT32)(UINTPTR)g_secureHeap + SECURE_MEM_SIZE - sizeof(SecureMemoryNode)));
    headNode->next = tailNode;
    tailNode->next = NULL;
    tailNode->size = 0;

    return LOS_OK;
}

VOID *ArchSecureMemAlloc(UINT32 size)
{
    SecureMemoryNode *node = NULL;
    SecureMemoryNode *realNewNode = NULL;
    SecureMemoryNode *newNode = NULL;
    UINT32 matchSize;
    UINT32 intSave;
    UINT32 alignSize;

    if ((g_secureHeap == NULL) || (size == 0) || (size > MALLOC_MAXSIZE)) {
        return NULL;
    }

    alignSize = MEMORY_ALIGNE(size + sizeof(SecureMemoryNode));
    node = MEMORY_CAST(SecureMemoryNode *, g_secureHeap);

    intSave = ArchSecureIntLock();
    while (node->next != NULL) {
        matchSize = ((UINT32)(UINTPTR)node->next - (UINT32)(UINTPTR)node) - node->size;
        if (matchSize >= alignSize) {
            break;
        }
        node = node->next;
    }

    if (node->next == NULL) {
        ArchSecureIntRestore(intSave);
        return NULL;
    }

    if (node->size == 0) {
        newNode = MEMORY_CAST(SecureMemoryNode *, ((UINT32)(UINTPTR)node + sizeof(SecureMemoryNode)));
        node->size = alignSize;
    } else {
        realNewNode = MEMORY_CAST(SecureMemoryNode *, ((UINT32)(UINTPTR)node + node->size));
        realNewNode->size = alignSize;
        realNewNode->next = node->next;
        node->next = realNewNode;
        newNode = MEMORY_CAST(SecureMemoryNode *, ((UINT32)(UINTPTR)realNewNode + sizeof(SecureMemoryNode)));
    }
    ArchSecureIntRestore(intSave);
    return newNode;
}

UINT32 ArchSecureMemFree(VOID *ptr)
{
    SecureMemoryNode *node = NULL;
    SecureMemoryNode *freeNode = NULL;
    SecureMemoryNode *prevNode = NULL;
    UINT32 intSave;

    if ((g_secureHeap == NULL) || (ptr == NULL)) {
        return LOS_NOK;
    }

    node = MEMORY_CAST(SecureMemoryNode *, g_secureHeap);
    freeNode = MEMORY_CAST(SecureMemoryNode *, ((UINT32)(UINTPTR)ptr - sizeof(SecureMemoryNode)));

    intSave = ArchSecureIntLock();
    while (node != freeNode) {
        prevNode = node;
        node = node->next;
        if (node->next == NULL) {
            ArchSecureIntRestore(intSave);
            return LOS_NOK;
        }
    }

    if (prevNode == NULL) {
        node->size = 0;
    } else {
        prevNode->next = node->next;
    }
    ArchSecureIntRestore(intSave);

    return LOS_OK;
}

