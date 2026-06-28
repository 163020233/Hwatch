/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All
 * rights reserved.
 * Description: tee session header file.
 */

#ifndef _TEE_SESSION_H_
#define _TEE_SESSION_H_

#include "los_list.h"
#include "tee_common_def.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define MAX_SESSION_NUM 32
#ifndef UINT32_CUT_MASK
#define UINT32_CUT_MASK 0xFFFFFFFF
#endif

typedef struct tee_session {
    uint32_t sess_id;
    void *ta;
    TEE_Identity clint_id;
    LOS_DL_LIST list;
} tee_session_t;

/* Find the first bit clear in array_num. */
static inline uint32_t bit_find_first_clear(uint32_t array_num, uint32_t max_bit)
{
    uint32_t i;
    for (i = 0; i < max_bit; ++i) {
        if (((array_num >> i) & 0x1) == 0) {
            break;
        }
    }

    return i;
}

/* Set bit N of bit array_num. */
static inline void bit_set(uint32_t *array_num, int idx)
{
    *array_num |= (1U << idx);
}

/* clear bit N of bit array_num */
static inline void bit_clear(uint32_t *array_num, int idx)
{
    *array_num &= (~((1U << idx) & UINT32_CUT_MASK));
}

TEE_Result tee_create_session(TEE_UUID *uuid, tee_session_t **s);

TEE_Result tee_get_session(uint32_t sess_id, tee_session_t **session);

void tee_destroy_session(tee_session_t *sess);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _TEE_SESSION_H_ */
