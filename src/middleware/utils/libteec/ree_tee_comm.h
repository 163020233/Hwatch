/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: ree tee communication entry API defines.
 */

#ifndef __REE_TEE_COMM_H__
#define __REE_TEE_COMM_H__

#include <stdint.h>

uint32_t ree_tee_comm_unify_entry(uint32_t cmd, void *arg);

void ree_tee_shared_memory_init(void);

void* ree_tee_shared_memory_alloc(uint32_t size);

void ree_tee_shared_memory_free(void *buffer);

#endif /* __REE_TEE_COMM_H__ */
