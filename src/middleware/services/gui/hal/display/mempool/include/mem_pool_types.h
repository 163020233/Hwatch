/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: memory pool type
 * Author: Hisi Graphic Team
 * Created: 2025-10
 */

#ifndef MEM_POOL_TYPES_H
#define MEM_POOL_TYPES_H
#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define CHECK_NULL_POINTER_RETURN_VALUE(pointer, ret) do { \
    if ((pointer) == NULL) { \
        return (ret); \
    } \
} while (0)

#define CHECK_NULL_POINTER_RETURN(pointer) do { \
    if ((pointer) == NULL) { \
        return; \
    } \
} while (0)

typedef struct _MemEntryTable {
    uint8_t *memAddr;
} MemEntryTable;

#define MEMORY_ID_INVALID 0xFFFF      /* invalid memory ID */
#define MEMORY_POOL_ID_INVALID NULL   /* invalid memory ID */
#define MIN_MEMORY_ID 1
#define MEM_ALIGN 16
#define MEM_NODE_USED_FLAG 0x80000000
#define MEM_NODE_USED 1
#define MEM_NODE_UNUSED 0
#define MEM_NODE_GET_SIZE(uwSizeAndFlag) ((uwSizeAndFlag) & (~MEM_NODE_USED_FLAG))

#define MEM_OK 0
#define MEM_NOK 1

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#endif /* MEMORY_POOL_COMMON_H */
