/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2021. All rights reserved.
 * Description: overlay information
 */

#ifndef __CORE_OVERLAY_INFO_H__
#define __CORE_OVERLAY_INFO_H__

#include "audio_type.h"
#include "audio_struct.h"
#include "mem_addr.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define HAID_BIN_SIZE 0x00020000 /* 128KB */
#define HAID_ENTRY_ADDR 0xC818C000

#define WDRC_BIN_SIZE 0x00020000 /* 128KB */
#define WDRC_ENTRY_ADDR 0xC81AC000

/* overlay module id */
typedef enum {
    OVERLAY_MAIN = 0,
    OVERLAY_ROM,
    OVERLAY_CODEC,
    OVERLAY_SEA,
    OVERLAY_AAI,
    OVERLAY_HAID,
    OVERLAY_AEF,
    OVERLAY_AEF_PRE,

    OVERLAY_MAX,
} core_overlay_id;

td_s32 overlay_get_section_attr(core_overlay_id overlay_id, td_u32 *addr, td_u32 *size);

td_void *find_component(td_u32 component_id, td_u32 sub_id,
    td_bool (* verify)(const td_void *entry, td_u32 sub_id));

td_bool check_component_in_main(td_u32 component_id, td_u32 sub_id,
    td_bool (* verify)(const td_void *entry, td_u32 sub_id));

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __CORE_OVERLAY_INFO_H__ */
