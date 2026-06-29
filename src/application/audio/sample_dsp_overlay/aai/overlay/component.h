/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: hifi component define
 * Author: audio
 */

#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "td_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/* component id, used by define_component */
typedef enum {
    ADEC,
    AENC,
    SEA,
    AAI,
    AEF,
    HAID,
    DPM,
    ASRC,

    COMPONENT_ID_MAX,
} component_id;

typedef struct {
    td_u32 id; /* component id */
    td_void *entry; /* component entry */
} component;

/* define a component and put it in section ".component" */
#define define_component(_id, _entry) \
    static component __component_entry_##_entry \
    __attribute__((__used__)) \
    __attribute__((__section__(".component"))) = { \
        .id = (_id), \
        .entry = &(_entry), \
    };

/*
 * component.c提供的校验函数
 * 只支持组建结构体以component_entry结构体开头的组建
 */
td_bool component_verify(td_void *entry, td_u32 sub_id);

/*
 * 遍历所有组件
 * 第一步，查找component_id的组建
 * 第二步，使用verify函数校验entry
 *
 * sud_id为entry的标识
 */
td_void *get_component(td_u32 component_id, td_u32 sub_id,
    td_bool (* verify)(td_void *entry, td_u32 sub_id));

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* __COMPONENT_H__ */
