/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: tesvec call define for LiteOS.
 */

#ifndef TESVEC_CALLS_DEF_H
#define TESVEC_CALLS_DEF_H

#include "tesvec_calls_macro.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef TESVEC_CALL_DEF
#error TESVEC_CALL_DEF not defined
#endif

TESVEC_CALL_DEF(ree_tee_comm_entry, REE_TEE_COMM_ENTRY)
TESVEC_CALL_DEF(lowpower_entry, LOWPOWER_ENTRY)

#undef TESVEC_CALL_DEF

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* TESVEC_CALLS_DEF_H */
