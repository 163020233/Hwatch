/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Common data types of the system.
 * Author: AuthorNameMagicTag
 * Create: 2025-07-21
 */

#ifndef __DRV_MCP_TYPE_H__
#define __DRV_MCP_TYPE_H__

#include "td_type.h"
#include "soc_errno.h"

#ifdef __cplusplus
#if __cplusplus
    extern "C" {
#endif
#endif /* __cplusplus */

typedef td_u32                  ext_chip_name_id;

#define TD_SUCCESS EXT_ERR_SUCCESS
#define TD_FAILURE EXT_ERR_FAILURE
#define TD_INVALID_HANDLE 0xFFFFFFFF
#define TD_UNUSED(x) ((x) = (x))
#define MCP_NULL  0

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __DRV_MCP_TYPE_H__ */
