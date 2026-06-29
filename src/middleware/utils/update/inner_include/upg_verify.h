/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: UPG verify header file.
 */

#ifndef UPG_VERIFY_H
#define UPG_VERIFY_H

#include "upg.h"
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

errcode_t verify_hash_cmp(const uint8_t *hash, const uint8_t *hash_res, uint32_t hash_len);

errcode_t uapi_upg_check_head_integrity(const upg_package_header_t *pkg_header);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* UPG_VERIFY_H */
