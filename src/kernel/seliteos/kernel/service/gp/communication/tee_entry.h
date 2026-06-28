/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All
 * rights reserved.
 * Description: tee_entry header file.
 */

#ifndef _TEE_ENTRY_H_
#define _TEE_ENTRY_H_

#include "tee_common_def.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

TEE_Result tee_entry_std(struct tee_msg_arg *arg, uint32_t num_params);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _TEE_ENTRY_H_ */
