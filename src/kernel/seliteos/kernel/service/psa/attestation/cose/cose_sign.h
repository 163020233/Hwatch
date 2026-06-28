/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: cose sign header. \n
 *
 * History: \n
 * 2025-03-14, Create file. \n
 */

#ifndef COSE_SIGN_H
#define COSE_SIGN_H

#include "attes_common.h"

int32_t cose_sign_encode_parameters(struct cose_sign_ctx *me, QCBOREncodeContext *cbor_encode_ctx);

int32_t cose_sign_encode_signature(struct cose_sign_ctx *me, QCBOREncodeContext *cbor_encode_ctx,
    useful_bufc *signed_payload);

#endif