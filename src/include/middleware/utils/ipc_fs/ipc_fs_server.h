/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: IPC file system server. \n
 *
 * History: \n
 * 2025-03-14, Create file. \n
 */
#ifndef IPC_FS_SERVER_H
#define IPC_FS_SERVER_H

#include <stdint.h>
#include "ipc_fs_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef struct {
    uintptr_t start;
    uintptr_t end;
    uintptr_t offset;
} ipc_core_memmap;

typedef struct {
    int32_t (* ipc_ack_func)(uint32_t src_core, ipc_fs_ack *ack);
    uintptr_t (* memmap)(uintptr_t addr, uint32_t len);
    void (* mem_request)(uintptr_t addr);
    void (* mem_release)(uintptr_t addr);
} ipc_fs_server_ops;

int32_t ipc_fs_server_register_ops(const ipc_fs_server_ops *ops);
int32_t ipc_fs_server_handle(uint32_t src_core, uint32_t msg_id, uint8_t *payload, uint32_t payload_len);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif