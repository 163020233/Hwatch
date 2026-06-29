/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: hifi component manager
 * Author: audio
 */

#include "component.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

extern component _component_start[];
extern component _component_end[];

/* 公共组建结构体 */
typedef struct {
    const td_char *name; /* 第一个成员为指针类型 */
    td_u32 sub_id;       /* 第二个成员为uint32类型，为组建的标识 */
} component_entry;

/* 所有以component_entry结构体开头的组建都可以使用公共校验函数 */
td_bool component_verify(const td_void *entry, td_u32 sub_id)
{
    return (entry != TD_NULL) && (((component_entry *)entry)->sub_id == sub_id);
}

td_void *get_component(td_u32 component_id, td_u32 sub_id,
    td_bool (* verify)(const td_void *entry, td_u32 sub_id))
{
    const component *node = NULL;

    for (node = _component_start; node < _component_end; node++) {
        if (node->id != component_id) {
            continue;
        }

        if (node->entry == TD_NULL) {
            continue;
        }

        /* verify函数设置为空指针时，查找第一个可用组建 */
        if (verify == TD_NULL) {
            return node->entry;
        }

        if (verify(node->entry, sub_id) == TD_TRUE) {
            return node->entry;
        }
    }

    return TD_NULL;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

