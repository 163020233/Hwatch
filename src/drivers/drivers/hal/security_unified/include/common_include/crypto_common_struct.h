/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: crypto common struct header. \n
 *
 * History: \n
 * 2023-03-22, Create file. \n
 */
#ifndef CRYPTO_COMMON_STRUCT_H
#define CRYPTO_COMMON_STRUCT_H

#include "crypto_type.h"

typedef enum {
    CRYPTO_BUF_SECURE,
    CRYPTO_BUF_NONSECURE,
} crypto_buffer_secure;

typedef struct {
    unsigned long long uapi_mem_handle;     /* Handle to buffer header address */
    unsigned long long addr_offset;         /* buffer offset, unused. */
    td_void *kapi_mem_handle;

    unsigned long phys_addr;
    void *virt_addr;
    crypto_buffer_secure buf_sec;           /* NONSECURE or SECURE */
} crypto_buf_attr;

typedef enum {
    CPU_ID_AIDSP = 0x00000035,
    CPU_ID_PCPU = 0x0000006a,
    CPU_ID_SCPU = 0x000000a5,
    CPU_ID_ACPU = 0x000000aa,
    CPU_ID_INVALID = 0xffffffff
} ca_misc_cpu_id;

typedef enum {
    CRYPTO_CPU_TYPE_SCPU,
    CRYPTO_CPU_TYPE_ACPU,
    CRYPTO_CPU_TYPE_HPPCPU,
    CRYPTO_CPU_TYPE_PCPU,
    CRYPTO_CPU_TYPE_AIDSP,
    CRYPTO_CPU_TYPE_INVALID
} crypto_cpu_type;

typedef enum {
    IN_NODE_TYPE_FIRST = 1 << 0,
    IN_NODE_TYPE_NORMAL = 1 << 1,
    IN_NODE_TYPE_LAST = 1 << 2,
    /* CCM. */
    IN_NODE_TYPE_CCM_N = 1 << 3,
    IN_NODE_TYPE_CCM_AAD = 1 << 4,
    IN_NODE_TYPE_CCM_P = 1 << 5,
    IN_NODE_TYPE_CCM_LAST = 1 << 6,
    /* GCM. */
    IN_NODE_TYPE_GCM_FIRST = 1 << 7,
    IN_NODE_TYPE_GCM_A = 1 << 8,
    IN_NODE_TYPE_GCM_P = 1 << 9,
    IN_NODE_TYPE_GCM_LEN = 1 << 10,
    IN_NODE_TYPE_GCM_GHASH = 1 << 11,
    IN_NODE_TYPE_GCM_IV = 1 << 12,
} in_node_type_e;

#define CRYPTO_INVALID_KEYSLOT_HANDLE       (0xFFFFFFFF)

#endif