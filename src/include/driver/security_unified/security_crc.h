/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: crc header file
 *
 * Create: 2025-05-07
 */

#ifndef SECURITY_CRC_H
#define SECURITY_CRC_H

#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
    extern "C" {
#endif
#endif

typedef uintptr_t crc_handle_t;

/* HASH(Single-Part). */
/**
 * @if Eng
 * @brief  CRC16 calculate. Single-Part API.
 * @param  [in]   data Mssage.
 * @param  [in]   data_len Message length.
 * @param  [out]  out_crc CRC result.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  CRC16 循环冗余校验。 单流程接口。
 * @param  [in]   data 消息。
 * @param  [in]   data_len 消息长度。
 * @param  [out]  out_crc 循环冗余校验结果。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_crc16_compute(const uint8_t *data, uint32_t data_len, uint16_t *out_crc);

/**
 * @if Eng
 * @brief  CRC32 calculate. Single-Part API.
 * @param  [in]   data Mssage.
 * @param  [in]   data_len Message length.
 * @param  [out]  out_crc CRC result.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  CRC32 循环冗余校验。 单流程接口。
 * @param  [in]   data 消息。
 * @param  [in]   data_len 消息长度。
 * @param  [out]  out_crc 循环冗余校验结果。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_crc32_compute(const uint8_t *data, uint32_t data_len, uint32_t *out_crc);

/* HASH(Multi-Part) */
/**
 * @if Eng
 * @brief  Create CRC16 handle.
 * @param  [out]  ctx CRC16 handle.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  创建 CRC16 句柄。
 * @param  [out]  ctx CRC16 句柄.
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_crc16_start(crc_handle_t *ctx);

/**
 * @if Eng
 * @brief  Update CRC16 message. Support multi-calling.
 * @param  [in]   ctx CRC16 handle.
 * @param  [in]   data Message.
 * @param  [in]   data_len Message length.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  更新 CRC16 消息内容。支持多次调用。
 * @param  [in]   ctx CRC16 句柄。
 * @param  [in]   data 消息。
 * @param  [in]   data_len 消息长度。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_crc16_update(crc_handle_t ctx, const uint8_t *data, uint32_t data_len);

/**
 * @if Eng
 * @brief  Finish CRC16 calculation and get CRC result.
 * @param  [in]   ctx CRC16 handle.
 * @param  [out]  out_crc CRC16 result.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  结束 CRC16 计算，并获取循环冗余校验结果。
 * @param  [in]   ctx CRC16 句柄。
 * @param  [out]  out_crc 循环冗余校验结果.
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_crc16_finish(crc_handle_t ctx, uint16_t *out_crc);

/**
 * @if Eng
 * @brief  Destroy CRC16 handle.
 * @param  [in]   ctx CRC16 handle.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  销毁 CRC16 句柄。
 * @param  [in]   ctx CRC16 句柄.
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_crc16_destroy(crc_handle_t ctx);

/**
 * @if Eng
 * @brief  Create CRC32 handle.
 * @param  [out]  ctx CRC32 handle.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  创建 CRC32 句柄。
 * @param  [out]  ctx CRC32 句柄.
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_crc32_start(crc_handle_t *ctx);

/**
 * @if Eng
 * @brief  Update CRC32 message. Support multi-calling.
 * @param  [in]   ctx CRC32 handle.
 * @param  [in]   data Message.
 * @param  [in]   data_len Message length.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  更新 CRC32 消息内容。支持多次调用。
 * @param  [in]   ctx CRC32 句柄。
 * @param  [in]   data 消息。
 * @param  [in]   data_len 消息长度。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_crc32_update(crc_handle_t ctx, const uint8_t *data, uint32_t data_len);

/**
 * @if Eng
 * @brief  Finish CRC32 calculation and get CRC result.
 * @param  [in]   ctx CRC32 handle.
 * @param  [out]  out_crc CRC32 result.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  结束 CRC32 计算，并获取循环冗余校验结果。
 * @param  [in]   ctx CRC32 句柄。
 * @param  [out]  out_crc 循环冗余校验结果.
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_crc32_finish(crc_handle_t ctx, uint32_t *out_crc);

/**
 * @if Eng
 * @brief  Destroy CRC32 handle.
 * @param  [in]   ctx CRC32 handle.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  销毁 CRC32 句柄。
 * @param  [in]   ctx CRC32 句柄.
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_crc32_destroy(crc_handle_t ctx);

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif  /* SECURITY_CRC_H */