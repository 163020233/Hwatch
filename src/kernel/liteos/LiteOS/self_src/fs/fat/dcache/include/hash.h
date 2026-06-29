/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Hash Table HeadFile
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

#ifndef _DCACHE_HASH_H
#define _DCACHE_HASH_H

#include "los_list.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef INT32 (*ObjHookFun)(VOID *data);
typedef BOOL (*HashCmpFun)(const VOID *srcData, const VOID *dstData);
typedef UINT32 (*HashCaclFun)(const VOID *data);

typedef struct {
    UINT32 entrySize;
    UINT32 tableShift;
    UINT32 entryShift;
    HashCmpFun hashCompare;
    HashCaclFun hashCaculate;
} HashParam;

typedef struct {
    UINT32 tableShift;
    UINT32 entrySize;
    HashCaclFun hashCaculate;
    HashCmpFun hashCompare;
    LOS_DL_LIST *hashListHead;
    LOS_DL_LIST lruListHead;
    LOS_DL_LIST freeListHead;
    VOID *hashPool;
} HashTable;

extern HashTable *HashInit(const HashParam *param);
extern VOID HashDeinit(HashTable *table);

extern INT32 HashIterate(const HashTable *table, ObjHookFun hook);
extern VOID *HashAlloc(HashTable *table);
extern VOID *HashLookup(const HashTable *table, const VOID *data);
extern INT32 HashAdd(HashTable *table, const VOID *data);
extern VOID HashDelete(HashTable *table, const VOID *data);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _DCACHE_HASH_H */
