/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Hash Table Implementation
 * Author: Huawei LiteOS Team
 * Create: 2021-7-15
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

#include "stdlib.h"
#include "hash.h"

typedef struct {
    LOS_DL_LIST lruListNode;
    LOS_DL_LIST hashListNode;
} HashEntry;

static BOOL HashIndexVaild(const HashTable *table, UINT32 index)
{
    UINT32 entrySize = 1 << table->tableShift;
    return (index >= entrySize) ? FALSE : TRUE;
}

static inline VOID *HashEntry2Obj(const HashTable *table, const HashEntry *entry)
{
    return (VOID *)((UINTPTR)entry - table->entrySize);
}

static inline HashEntry *HashObj2Entry(const HashTable *table, const VOID *obj)
{
    return (HashEntry *)((UINTPTR)obj + table->entrySize);
}

INT32 HashIterate(const HashTable *table, ObjHookFun hook)
{
    INT32 ret;
    HashEntry *entry = NULL;
    LOS_DL_LIST *node = NULL;

    if (table == NULL || hook == NULL) {
        return LOS_NOK;
    }

    node = table->lruListHead.pstNext;
    while (node != &table->lruListHead) {
        entry = LOS_DL_LIST_ENTRY(node, HashEntry, lruListNode);
        ret = hook(HashEntry2Obj(table, entry));
        if (ret != LOS_OK) {
            return ret;
        }
        node = node->pstNext;
    }

    return LOS_OK;
}

HashTable *HashInit(const HashParam *param)
{
    UINT32 i, objSize;
    HashEntry *entry = NULL;
    HashTable *table = NULL;

    if (param == NULL) {
        return NULL;
    }

    table = zalloc(sizeof(HashTable));
    if (table == NULL) {
        return NULL;
    }

    objSize = param->entrySize + sizeof(HashEntry);
    table->entrySize = param->entrySize;
    table->tableShift = param->tableShift;
    table->hashCompare = param->hashCompare;
    table->hashCaculate = param->hashCaculate;

    table->hashPool = zalloc(objSize << param->entryShift);
    if (table->hashPool == NULL) {
        free(table);
        return NULL;
    }

    table->hashListHead = zalloc(sizeof(LOS_DL_LIST) << table->tableShift);
    if (table->hashListHead == NULL) {
        free(table->hashPool);
        free(table);
        return NULL;
    }

    LOS_ListInit(&table->lruListHead);
    LOS_ListInit(&table->freeListHead);

    for (i = 0; i < (UINT32)(1 << table->tableShift); i++) {
        LOS_ListInit(&table->hashListHead[i]);
    }

    /* Entry point to first HashEntry */
    entry = HashObj2Entry(table, table->hashPool);
    for (i = 0; i < (UINT32)(1 << param->entryShift); i++) {
        LOS_ListTailInsert(&table->freeListHead, &entry->lruListNode);
        entry = (HashEntry *)((UINTPTR)(entry) + objSize);
    }

    return table;
}

VOID HashDeinit(HashTable *table)
{
    if (table == NULL) {
        return;
    }

    free((VOID *)table->hashPool);
    table->hashPool = NULL;
    free(table->hashListHead);
    table->hashListHead = NULL;
    free(table);
}

VOID *HashLookup(const HashTable *table, const VOID *data)
{
    VOID *dst = NULL;
    HashEntry *entry = NULL;
    LOS_DL_LIST *node = NULL;
    UINT32 index;

    if ((table == NULL) || (table->hashCaculate == NULL) || (table->hashCompare == NULL) || (data == NULL)) {
        return NULL;
    }

    index = table->hashCaculate(data);
    if (HashIndexVaild(table, index) == FALSE) {
        return NULL;
    }

    node = table->hashListHead[index].pstNext;
    while (node != &table->hashListHead[index]) {
        entry = LOS_DL_LIST_ENTRY(node, HashEntry, hashListNode);
        dst = HashEntry2Obj(table, entry);
        if (table->hashCompare(data, dst) == TRUE) {
            return (VOID *)dst;
        }

        node = node->pstNext;
    }

    return NULL;
}

INT32 HashAdd(HashTable *table, const VOID *data)
{
    UINT32 index;
    HashEntry *entry = NULL;

    if ((table == NULL) || (table->hashCaculate == NULL) || (data == NULL)) {
        return LOS_NOK;
    }

    index = table->hashCaculate(data);
    if (HashIndexVaild(table, index) == FALSE) {
        return LOS_NOK;
    }

    entry = HashObj2Entry(table, data);
    LOS_ListAdd(&table->hashListHead[index], &entry->hashListNode);

    /* Move from freeList to lruList */
    LOS_ListDelete(&entry->lruListNode);
    LOS_ListAdd(&table->lruListHead, &entry->lruListNode);

    return LOS_OK;
}

VOID HashDelete(HashTable *table, const VOID *data)
{
    HashEntry *entry = NULL;

    if ((table == NULL) || (data == NULL)) {
        return;
    }

    entry = HashObj2Entry(table, data);
    LOS_ListDelete(&entry->hashListNode);

    /* Move from lruList to freeList */
    LOS_ListDelete(&entry->lruListNode);
    LOS_ListAdd(&table->freeListHead, &entry->lruListNode);
}

VOID *HashAlloc(HashTable *table)
{
    HashEntry *entry = NULL;

    if (table == NULL) {
        return NULL;
    }

    if (LOS_ListEmpty(&table->freeListHead) == TRUE) {
        return NULL;
    }

    entry = LOS_DL_LIST_ENTRY(table->freeListHead.pstNext, HashEntry, lruListNode);

    return HashEntry2Obj(table, entry);
}
