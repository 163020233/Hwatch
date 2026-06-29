/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 * Description: ta manager header file.
 */

#ifndef _TA_MANAGER_H_
#define _TA_MANAGER_H_

#include "tee_session.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ta entry operation */
struct ta_ops {
    TEE_Result (*create_entry_point)(void);
    void (*destroy_entry_point)(void);
    TEE_Result (*open_session_entry_point)(uint32_t param_types,
                                           TEE_Param params[TEE_NUM_PARAMS],
                                           void **session);
    void (*close_session_entry_point)(void *session);
    TEE_Result (*invoke_command_entry_point)(void *session, uint32_t command,
                                             uint32_t param_types,
                                             TEE_Param params[TEE_NUM_PARAMS]);
};

/* ta entry header contain user param  */
struct ta_header {
    const char *name;
    TEE_UUID uuid;
    uint32_t data_addr;
    uint32_t heap_size;
    uint32_t data_size;
    uint32_t stack_addr;
    uint32_t stack_size;
    uint32_t text_rodata_addr;
    uint32_t text_rodata_size;
    struct ta_ops *ops;
};

#define LOS_CFG_MAX_TA_NUM 2
#define SECTION(name)   __attribute__((section(name)))
#define USED            __attribute__((used))

#define TA_ARRAY_BEGIN                                              \
    ({                                                              \
        static const struct ta_header __ta_header_begin[0]          \
            __attribute__((unused)) SECTION(".ta_array_ta_head_0"); \
        __ta_header_begin;                                          \
    })

#define TA_ARRAY_END                                                \
    ({                                                              \
        static const struct ta_header __ta_header_end[0]            \
            __attribute__((unused)) SECTION(".ta_array_ta_head_2"); \
        __ta_header_end;                                            \
    })

#define TA_ARRAY_FOREACH(cur) \
    for ((cur) = TA_ARRAY_BEGIN; (cur) < TA_ARRAY_END; ++(cur))

#define TA_ARRAY_ITEM1(count)                                    \
    static const struct ta_header ta_array_##count;              \
    static const struct ta_header ta_array_##count USED SECTION( \
        ".ta_array_ta_head_1")

#define TA_USER_PARAM_ITEM1(name)                                      \
    ({                                                                 \
        (void)(name);                                                  \
        static TEE_Param ta_user_param_##name[TEE_NUM_PARAMS] SECTION( \
            ".usr.data" #name) = {0};                                  \
        ta_user_param_##name;                                          \
    })

#define TA_ARRAY_ITEM            TA_ARRAY_ITEM1(__COUNTER__)
#define TA_USER_PARAM_ITEM(name) TA_USER_PARAM_ITEM1(name)

#define ta_register(...) TA_ARRAY_ITEM = {__VA_ARGS__}

TEE_Result register_ta(void);

TEE_Result get_current_ta_uuid(TEE_UUID *uuid);

TEE_Result usr_get_cur_ta(struct ta_header **ta);

TEE_Result tee_ta_open_session(struct tee_msg_param *params, TEE_UUID *uuid, TEE_Identity *clint_id,
    tee_session_t **session, uint32_t *ret_origin);

TEE_Result tee_ta_invoke_command(struct tee_msg_param *params, TEE_Identity *clint_id, tee_session_t *sess,
    uint32_t cmd, uint32_t *ret_origin);

TEE_Result tee_ta_close_session(TEE_Identity *clint_id, tee_session_t *sess, uint32_t *ret_origin);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  // _TA_MANAGER_H_
