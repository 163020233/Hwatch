/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: LiteOS USB Driver KalList HeadFile
 * Author: Huawei LiteOS Team
 * Create: 2025-02-07
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

#ifndef KAL_LIST_H
#define KAL_LIST_H

#if defined(__LITEOS__)
#include <los_list.h>
#elif defined(__NONOS__)
#include <stdint.h>
#include "stdbool.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if defined(__LITEOS__)
#define KalList LOS_DL_LIST

#define KAL_LIST_FIRST_ENTRY(ptr, type, member) LOS_DL_LIST_ENTRY((ptr)->pstNext, type, member)
#define KAL_IS_LIST_LAST(list, head)            ((list)->pstNext == (head))
#define KAL_DL_LIST_FOR_EACH(item, list)        LOS_DL_LIST_FOR_EACH(item, list)
#define KAL_DL_LIST_ENTRY(item, type, member)   LOS_DL_LIST_ENTRY(item, type, member)

static inline void KAL_ListInit(KalList *list)
{
    LOS_ListInit((LOS_DL_LIST *)list);
}

static inline void KAL_ListDelInit(KalList *list)
{
    LOS_ListDelInit((LOS_DL_LIST *)list);
}

static inline bool KAL_ListEmpty(KalList *list)
{
    return (bool)LOS_ListEmpty((LOS_DL_LIST *)list);
}

static inline void KAL_ListTailInsert(KalList *list, KalList *node)
{
    LOS_ListTailInsert((LOS_DL_LIST *)list, (LOS_DL_LIST *)node);
}

#elif defined(__NONOS__)

typedef struct NONOS_DL_LIST {
    struct NONOS_DL_LIST *pstPrev; /**< Current node's pointer to the previous node */
    struct NONOS_DL_LIST *pstNext; /**< Current node's pointer to the next node */
} NONOS_DL_LIST;

static inline void LOS_ListAdd(NONOS_DL_LIST *list, NONOS_DL_LIST *node)
{
    node->pstNext = list->pstNext;
    node->pstPrev = list;
    list->pstNext->pstPrev = node;
    list->pstNext = node;
}

static inline void Nonos_ListTailInsert(NONOS_DL_LIST *list, NONOS_DL_LIST *node)
{
    LOS_ListAdd(list->pstPrev, node);
}

#define KalList NONOS_DL_LIST

#define NONOS_OFF_SET_OF(type, member) ((unsigned int)&((type *)0)->member)

#define NONOS_DL_LIST_ENTRY(item, type, member) \
    ((type *)(void *)((char *)(item) - NONOS_OFF_SET_OF(type, member)))

#define NONOS_DL_LIST_FOR_EACH(item, list) \
    for ((item) = (list)->pstNext;         \
         (item) != (list);               \
         (item) = (item)->pstNext)

#define KAL_LIST_FIRST_ENTRY(ptr, type, member) NONOS_DL_LIST_ENTRY((ptr)->pstNext, type, member)
#define KAL_IS_LIST_LAST(list, head)            ((list)->pstNext == (head))
#define KAL_DL_LIST_FOR_EACH(item, list)        NONOS_DL_LIST_FOR_EACH(item, list)
#define KAL_DL_LIST_ENTRY(item, type, member)   NONOS_DL_LIST_ENTRY(item, type, member)

static inline void KAL_ListInit(KalList *list)
{
    list->pstNext = list;
    list->pstPrev = list;
}

static inline void KAL_ListDelInit(KalList *list)
{
    list->pstNext->pstPrev = list->pstPrev;
    list->pstPrev->pstNext = list->pstNext;
    list->pstNext = list;
    list->pstPrev = list;
}

static inline bool KAL_ListEmpty(KalList *list)
{
    return (bool)(list->pstNext == list);
}

static inline void KAL_ListTailInsert(KalList *list, KalList *node)
{
    // NONOS_LIST_ADD(list->pstPrev, node);
    Nonos_ListTailInsert((NONOS_DL_LIST *)list, (NONOS_DL_LIST *)node);
}

#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* KAL_LIST_H */