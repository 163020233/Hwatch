/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: BR phone book service
 */

#ifndef BS_BR_PHONE_BOOK_H
#define BS_BR_PHONE_BOOK_H

#include <stdint.h>
#include <stdbool.h>
#include "errcode.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief Enum of phone book vCard version.
 * @else
 * @brief 电话薄vCard版本。
 * @endif
 */
typedef enum {
    BR_PHONE_BOOK_VCARD21 = 0,              /*!< @if Eng vCard version is 2.1
                                                @else vCard 2.1 版本 @endif */
    BR_PHONE_BOOK_VCARD30,                  /*!< @if Eng vCard version is 3.0
                                                @else vCard 3.0 版本 @endif */
} bs_br_phone_book_vcard_version_t;

/**
 * @if Eng
 * @brief Enum of call log type.
 * @else
 * @brief 通话记录类型。
 * @endif
 */
typedef enum {
    BR_CALL_LOG_ICH = 0x1,          /*!< @if Eng incoming call log
                                        @else 来电通话记录 @endif */
    BR_CALL_LOG_OCH,                /*!< @if Eng outgoing call log
                                        @else 去电通话记录 @endif */
    BR_CALL_LOG_MCH,                /*!< @if Eng missed call log
                                        @else 未接来电通话记录 @endif */
    BR_CALL_LOG_CCH,                /*!< @if Eng all call log
                                        @else 所有的通话记录 @endif */
} bs_br_call_log_type_t;

/**
 * @if Eng
 * @brief  Connect to the phone book.
 * @par    Connect to the phone book.
 * @param [in] dev_id            The id of one connected device.
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other                Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li
 * @else
 * @brief  连接电话薄。
 * @par    连接电话薄。
 * @param [in] dev_id            远端设备ID。
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other                错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li
 * @endif
 */
errcode_t bs_br_phone_book_connect(uint16_t dev_id);

/**
 * @if Eng
 * @brief  Disconnect to the phone book.
 * @par    Disconnect to the phone book.
 * @param [in] dev_id            The id of one connected device.
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other                Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li
 * @else
 * @brief  断连电话薄。
 * @par    断连电话薄。
 * @param [in] dev_id            远端设备ID。
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other                错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li
 * @endif
 */
errcode_t bs_br_phone_book_disconnect(uint16_t dev_id);

/**
 * @if Eng
 * @brief  Download phone book.
 * @par    Download phone book.
 * @param [in] dev_id            The id of one connected device.
 * @param [in] version           vCard Version.See @ref bs_br_phone_book_vcard_version_t.
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other                Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li
 * @else
 * @brief  下载电话薄。
 * @par    下载电话薄。
 * @param [in] dev_id            远端设备ID。
 * @param [in] version           vCard版本。参考 @ref bs_br_phone_book_vcard_version_t.
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other                错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li
 * @endif
 */
errcode_t bs_br_pull_phone_book(uint16_t dev_id, bs_br_phone_book_vcard_version_t version);

/**
 * @if Eng
 * @brief  Download call log.
 * @par    Download call log.
 * @param [in] dev_id            The id of one connected device.
 * @param [in] type              call log type.See @ref bs_br_call_log_type_t.
 * @param [in] version           vCard Version.See @ref bs_br_phone_book_vcard_version_t.
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other                Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li
 * @else
 * @brief  下载通话记录。
 * @par    下载通话记录。
 * @param [in] dev_id            远端设备ID。
 * @param [in] type              通话记录类型。参考 @ref bs_br_call_log_type_t.
 * @param [in] version           vCard版本。参考 @ref bs_br_phone_book_vcard_version_t.
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other                错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li
 * @endif
 */
errcode_t bs_br_pull_call_log(uint16_t dev_id, bs_br_call_log_type_t type, bs_br_phone_book_vcard_version_t version);

/**
 * @if Eng
 * @brief  Callback invoked when the phonebook connection state has changed.
 * @par    When registered, this callback reports connection state to upper layer application.
 * @param  [in]  dev_id     The id of one connected device.
 * @param  [in]  conn_state The phone book connection status。See @ref bs_br_profile_connect_state_t
 * @par Dependency:
 * @li
 * @see  bs_br_phone_book_callbacks_t
 * @else
 * @brief  电话薄连接状态的回调函数。
 * @par    注册该回调函数之后，调用该回调函数将连接状态上报给应用。
 * @param  [in]  dev_id     设备id。
 * @param  [in]  conn_state 电话薄连接状态。参考 @ref bs_br_profile_connect_state_t
 * @par 依赖:
 * @li
 * @see  bs_br_phone_book_callbacks_t
 * @endif
 */
typedef void (*bs_br_phone_book_conn_state_changed_cbk)(uint16_t dev_id, uint8_t conn_state);

/**
 * @if Eng
 * @brief  Callback invoked when the phone book is downloaded.
 * @par    When registered, this callback reports received data to upper layer application.
 * @param  [in]  dev_id     The id of one connected device.
 * @param  [in]  data       Received data.
 * @param  [in]  len        Length of data.
 * @par Dependency:
 * @li
 * @see  bs_br_phone_book_callbacks_t
 * @else
 * @brief  拉取电话薄数据的回调函数。
 * @par    注册该回调函数之后，调用该回调函数将接收到的数据上报给应用。
 * @param  [in]  dev_id     设备id。
 * @param  [in]  data       接收到的数据。
 * @param  [in]  len        数据长度。
 * @par 依赖:
 * @li
 * @see  bs_br_phone_book_callbacks_t
 * @endif
 */
typedef void (*bs_br_phone_book_recevice_cbk)(uint16_t dev_id, const uint8_t *data, uint16_t len);

/**
 * @if Eng
 * @brief  Callback invoked when the phone book is downloaded.
 * @par    When registered, this callback notifies upper layer of the
 *         result when the phone book is downloaded.
 * @param  [in]  dev_id     The id of one connected device.
 * @param  [in]  ret        result.See @ref errcode_bt_t
 * @par Dependency:
 * @li
 * @see  bs_br_phone_book_callbacks_t
 * @else
 * @brief  拉出电话薄结果的回调函数。
 * @par    注册该回调函数之后，拉取电话薄数据完成后会上报结果。
 * @param  [in]  dev_id     设备id。
 * @param  [in]  ret        结果。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li
 * @see  bs_br_phone_book_callbacks_t
 * @endif
 */
typedef void (*bs_br_phone_book_result_cbk)(uint16_t dev_id, errcode_t ret);

/**
 * @if Eng
 * @brief  Callback invoked when the call log is downloaded.
 * @par    When registered, this callback reports received data to upper layer application.
 * @param  [in]  dev_id     The id of one connected device.
 * @param  [in]  data       Received data.
 * @param  [in]  len        Length of data.
 * @par Dependency:
 * @li
 * @see  bs_br_phone_book_callbacks_t
 * @else
 * @brief  拉取通话记录数据的回调函数。
 * @par    注册该回调函数之后，调用该回调函数将接收到的数据上报给应用。
 * @param  [in]  dev_id     设备id。
 * @param  [in]  data       接收到的数据。
 * @param  [in]  len        数据长度。
 * @par 依赖:
 * @li
 * @see  bs_br_phone_book_callbacks_t
 * @endif
 */
typedef void (*bs_br_call_log_recevice_cbk)(uint16_t dev_id, const uint8_t *data, uint16_t len);

/**
 * @if Eng
 * @brief  Callback invoked when the phone book is downloaded.
 * @par    When registered, this callback notifies upper layer of the
 *         result when the phone book is downloaded.
 * @param  [in]  dev_id     The id of one connected device.
 * @param  [in]  ret        result.See @ref errcode_bt_t
 * @par Dependency:
 * @li
 * @see  bs_br_phone_book_callbacks_t
 * @else
 * @brief  拉取通话记录数据结果的回调函数。
 * @par    注册该回调函数之后，拉取通话记录数据完成后会上报结果。
 * @param  [in]  dev_id     设备id。
 * @param  [in]  ret        结果。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li
 * @see  bs_br_phone_book_callbacks_t
 * @endif
 */
typedef void (*bs_br_call_log_result_cbk)(uint16_t dev_id, errcode_t ret);

/**
 * @if Eng
 * @brief Struct of phone book callback function.
 * @else
 * @brief 电话薄回调函数接口定义。
 * @endif
 */
typedef struct {
    bs_br_phone_book_conn_state_changed_cbk connect_state_cbk;          /*!< @if Eng Connection state callback.
                                                                             @else 连接状态上报回调函数。 @endif */
    bs_br_phone_book_recevice_cbk           recevice_phone_book_cbk;    /*!< @if Eng Pull phone book data callback.
                                                                             @else 拉取电话薄数据的回调函数。 @endif */
    bs_br_phone_book_result_cbk             phone_book_result_cbk;      /*!< @if Eng Pull phone book result callback.
                                                                             @else 拉取电话薄结果的回调函数。 @endif */
    bs_br_call_log_recevice_cbk             recevice_call_log_cbk;      /*!< @if Eng Pull call log data callback.
                                                                             @else 拉取通话记录数据的回调函数。 @endif */
    bs_br_call_log_result_cbk               call_log_result_cbk;        /*!< @if Eng Pull call log result callback.
                                                                             @else 拉取通话记录结果的回调函数。 @endif */
} bs_br_phone_book_callbacks_t;

/**
 * @if Eng
 * @brief Use this function to register the callback functions for the phone book.
 * @par Use this function to register the callback functions for the phone book.
 * @param [in] app_id   app_id application id , default is 0.
 * @param [in] cbks     callback functions {@ref bs_br_phone_book_callbacks_t}.
 * @retval #ERRCODE_BT_SUCCESS   Execute successfully.
 * @retval #Other                Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li
 * @else
 * @brief  注册电话薄回调。
 * @par    注册电话薄回调。
 * @param [in] app_id   app_id 应用id 默认为0。
 * @param [in] cbks     回调结构体 {@ref bs_br_phone_book_callbacks_t}。
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other                错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li
 * @endif
 */
errcode_t bs_br_phone_book_register_callbacks(uint8_t app_id, const bs_br_phone_book_callbacks_t *funs);

/**
 * @if Eng
 * @brief Use this function to unregister phone book callbacks.
 * @par Description: Use this function to unregister phone book callbacks.
 * @param  [in] app_id   The id of one connected device.
 * @retval #ERRCODE_BT_SUCCESS   Execute successfully.
 * @retval #Other                Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @else
 * @brief 去注册电话薄回调。
 * @par 说明: 去注册电话薄回调。
 * @param  [in] app_id   app_id 应用id 默认为0。
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other                错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @endif
 */
errcode_t bs_br_phone_book_unregister_callbacks(uint8_t app_id);

#ifdef __cplusplus
}
#endif
#endif