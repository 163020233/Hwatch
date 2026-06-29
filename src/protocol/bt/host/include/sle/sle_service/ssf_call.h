
/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 *
 * Description: SLE Service Framework (SSF) Functions about Call API File.
 */

#ifndef SSF_CALL_H
#define SSF_CALL_H
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "bts_def.h"
#include "errcode.h"
#include "ssf_device.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief enumeration definition for call control operation code.
 * @else
 * @brief 通话控制操作码。
 * @endif
 */
typedef enum {
    SLE_CALL_IN,                /*!< @if Eng call in
                                    @else 呼入通话 @endif */
    SLE_CALL_OUT,               /*!< @if Eng call out
                                    @else 呼出通话 @endif */
    SLE_CALL_ACCEPT,            /*!< @if Eng accept call
                                    @else 接听通话 @endif */
    SLE_CALL_TERMINATE,         /*!< @if Eng terminate call
                                    @else 挂断通话 @endif */
    SLE_CALL_HOLD,              /*!< @if Eng hold call
                                    @else 保持通话 @endif */
    SLE_CALL_RETRIEVE,          /*!< @if Eng retrieve call
                                    @else 恢复通话 @endif */
    SLE_CALL_MERGE,             /*!< @if Eng merge call
                                    @else 合并通话 @endif */
    SLE_CALL_MAX,
} bs_sle_call_oper_type_t;

/**
 * @if Eng
 * @brief enumeration definition for call state.
 * @else
 * @brief 通话状态。
 * @endif
 */
typedef enum {
    SLE_CALL_STATE_INCOMING =                                 0x01,     /*!< @if call incoming state
                                                                                 @else 来电状态 @endif */
    SLE_CALL_STATE_OUTGOING =                                 0x02,     /*!< @if call outgoing state
                                                                                 @else 呼出状态 @endif */
    SLE_CALL_STATE_RINGING =                                  0x03,     /*!< @if call ringing state
                                                                                 @else 振铃状态 @endif */
    SLE_CALL_STATE_ACTIVE =                                   0x04,     /*!< @if call active state
                                                                                 @else 通话状态 @endif */
    SLE_CALL_STATE_LOCALLY_HELD =                             0x05,     /*!< @if call local hold state
                                                                                 @else 本地保持状态 @endif */
    SLE_CALL_STATE_REMOTELY_HELD =                            0x06,     /*!< @if call remote hold state
                                                                                 @else 远端保持状态 @endif */
    SLE_CALL_STATE_LOCALLY_AND_REMOTELY_HELD =                0x07,     /*!< @if call local and remote hold state
                                                                                 @else 本地和远端保持状态 @endif */
    SLE_CALL_STATE_RFU
} bs_sle_call_state_t;

/**
 * @if Eng
 * @brief enumeration definition for call terminate reason.
 * @else
 * @brief 通话结束原因。
 * @endif
 */
typedef enum {
    SLE_CALL_TERMINATE_REASON_CLIENT_END_THE_CALL       = 0x01,  /*!< @if Eng client terminated the call
                                                                          @else 客户端结束电话 @endif */
    SLE_CALL_TERMINATE_REASON_SERVER_END_THE_CALL       = 0x02,  /*!< @if Eng server terminated the call
                                                                          @else 服务端结束电话 @endif */
    SLE_CALL_TERMINATE_REASON_REMOTE_END_THE_CALL       = 0x03,  /*!< @if Eng remote terminated the call
                                                                          @else 远端结束电话 @endif */
    SLE_CALL_TERMINATE_REASON_LINE_BUSY                 = 0x04,  /*!< @if Eng line busy
                                                                          @else 线路忙 @endif */
    SLE_CALL_TERMINATE_REASON_NO_ANSWER                 = 0x05,  /*!< @if Eng no answer
                                                                          @else 无人接听 @endif */
    SLE_CALL_TERMINATE_REASON_NO_SERVICE                = 0x06,  /*!< @if Eng no network service
                                                                          @else 无网络服务 @endif */
    SLE_CALL_TERMINATE_REASON_RFU
} bs_sle_call_terminate_reason_t;

/**
 * @if Eng
 * @brief enumeration definition for voice assistant state.
 * @else
 * @brief 语音助手状态码定义。
 * @endif
 */
typedef enum {
    SLE_VAP_STATE_DISABLE    = 0x00,                /*!< @if Eng voice assistant disable.
                                                         @else 语音助手待机态 @endif */
    SLE_VAP_STATE_ENABLE     = 0x01,                /*!< @if Eng voice assistant enable.
                                                         @else 语音助手激活态 */
    SLE_VAP_VOICE_RFU                               /*!< @if Eng other.
                                                         @else 其他 */
} bs_sle_vap_state_t;

/**
 * @if Eng
 * @brief enumeration definition for call flags bitmap.
 * @else
 * @brief 通话控制服务通话标记位图定义。
 * @endif
 */
typedef enum {
    SLE_CALL_FLAGS_OUTGOING_CALL             = 0x01,  /*!< @if Eng the call is outgoing
                                                               @else 呼出通话标识位，0为呼入，1为呼出 @endif */
    SLE_CALL_FLAGS_INFO_WITHHELD_BY_NETWORK  = 0x02,  /*!< @if Eng call info withheld by network.
                                                               @else network 存储通话信息标识位，用户信息、用户别名是否被网络隐藏。 @endif */
    SLE_CALL_FLAGS_INFO_WITHHELD_BY_SERVER   = 0x04,  /*!< @if Eng call info withheld by server.
                                                               @else server 端存储通话信息标识位，用户信息、用户别名是否被服务端隐藏。 @endif */
    SLE_CALL_FLAGS_RFU
} bs_sle_call_flags_t;

/**
 * @if Eng
 * @brief call outgoing type.
 * @else
 * @brief 通话呼出类型枚举。
 * @endif
 */
typedef enum {
    SLE_CALL_OUTGOING_BY_URI = 0x01,                 /*!< @if Eng outgoing by uri.
                                                      @else 通过uri呼出通话 @endif */
    SLE_CALL_OUTGOING_BY_SPECIFIC_WAY = 0x02,        /*!< @if Eng outgoing by specific funcition.
                                                      @else 通过指定方式呼出通话 @endif */
} bs_sle_call_outgoing_type_t;
 
/**
 * @if Eng
 * @brief call outgoing ways.
 * @else
 * @brief 通话呼出方式枚举。
 * @endif
 */
typedef enum {
    SLE_CALL_OUTGOING_LAST_CALL = 0x01,              /*!< @if Eng outgoing last call.
                                                        @else 呼出最后一路通话 @endif */
} bs_sle_call_outgoing_way_t;

/**
 * @if Eng
 * @brief call status stru.
 * @else
 * @brief 通话状态标记结构。
 * @endif
 */
typedef struct {
    uint8_t call_id;            /*!< @if Eng the call ID
                                    @else 通话标识 @endif */
    uint8_t net_id;             /*!< @if Eng the network ID
                                    @else 网络标识 @endif */
    uint8_t call_state;         /*!< @if Eng call state. { @ref bs_sle_call_state_t }
                                    @else 通话状态 @endif */
    uint8_t call_flags;         /*!< @if Eng call flags. { @ref bs_sle_call_flags_t }
                                    @else 通话标记 @endif */
} bs_sle_call_status_t;

/**
 * @if Eng
 * @brief Server-side call incoming and outgoing scenarios, the stru of call information.
 * @else
 * @brief 服务端通话呼入呼出场景，传入的通话信息结构。
 * @endif
 */
typedef struct {
    uint8_t *call_id_out;       /*!< @if Eng out call ID pointer. As output, return generated call identifier.
                                    @else 通话标识指针，作为出参，返回生成的通话标识 @endif */
    uint8_t net_id;             /*!< @if Eng the network ID
                                     @else 网络标识，用于关联网络信息服务,包含网络制式、运营商名称等.非必选 @endif */
    uint8_t call_flags;         /*!< @if Eng the call flags. { @ref bs_sle_call_flags_t }
                                    @else 通话标志，非必选 @endif */
    bg_string_t remote_uri;     /*!< @if Eng the call remote uri
                                    @else 通话远端URI。格式为 "tel:'电话号码'"，非必选 @endif */
    bg_string_t friend_name;    /*!< @if Eng the call friend mname
                                    @else 通话用户别名，非必选 @endif */
} bs_sle_call_info_t;

/**
 * @if Eng
 * @brief call role stru.
 * @else
 * @brief 通话角色结构。
 * @endif
 */
typedef struct {
    uint8_t call_id;            /*!< @if Eng the call ID
                                    @else 通话标识 @endif */
    uint8_t role;                /*!< @if Eng role for terminate the call, remote or local
                                    @else 保持通话的角色，远端设备还是本端设备 @endif */
} bs_sle_call_role_t;

/**
 * @if Eng
 * @brief call terminate stru.
 * @else
 * @brief 通话终止原因。
 * @endif
 */
typedef struct {
    uint8_t call_id;            /*!< @if Eng the call ID
                                    @else 通话标识 @endif */
    uint8_t reason;             /*!< @if Eng reason for terminate the call. { @ref bs_sle_call_terminate_reason_t }
                                    @else 通话结束的原因 @endif */
} bs_sle_call_reason_t;

/**
 * @if Eng
 * @brief call uri info stru.
 * @else
 * @brief 通话uri信息。
 * @endif
 */
typedef struct {
    uint16_t len;                   /*!< @if Eng size of the value, exclude the end '\0'.
                                    @else 字符个数，不包含字符串结束符 @endif */
    uint8_t* value;                 /*!< @if Eng ptr to the uri value
                                    @else uri字符串指针 @endif */
} bs_sle_call_uri_info_t;

/**
 * @if Eng
 * @brief call outgoing structure.(input param stru when client call out)
 * @else
 * @brief 通话呼出结构定义。（客户端呼出时的入参结构体）
 * @endif
 */
typedef struct {
    uint8_t outgoing_type;          /*!< @if Eng outgoing type, { @ref bs_sle_call_outgoing_type_t }.
                                     @else 通话呼出类型，{ @ref bs_sle_call_outgoing_type_t } @endif */
    union {
        bs_sle_call_uri_info_t uri;    /*!< @if remote uri infomation, when outgoing type is SLE_CALL_OUTGOING_BY_URI
                                     @else 呼出通话的uri信息，当呼出类型取值为 SLE_CALL_OUTGOING_BY_URI 时携带 @endif */
        uint8_t outgoing_way;       /*!< @if outgong way, when outgoing type is SLE_CALL_OUTGOING_BY_SPECIFIC_WAY,
                                     { @ref bs_sle_call_outgoing_way_t },
                                     @else 通话呼出方式，当呼出类型取值为 SLE_CALL_OUTGOING_BY_SPECIFIC_WAY 时携带
                                     { @ref bs_sle_call_outgoing_way_t }, @endif */
    };
} bs_sle_call_outgoing_param_t;

/**
 * @if Eng
 * @brief  callback function for client receive the server's voice assistant status notify.
 * @par    When registered, this callback notifies the upper layer
 *         when client receive the server's voice assistant status notify.
 * @param [in] obj              device object.
 * @param [in] state            state of voice assistant.
 * @par Dependency:
 * @see bs_sle_call_callbacks_t
 * @else
 * @brief  客户端收到服务端语音助手状态通知的回调函数。
 * @par    注册该回调函数之后，收到服务端语音助手状态通知后该回调函数会上报上层。
 * @param [in] obj              设备对象。
 * @param [in] state            语音助手状态。
 * @par 依赖:
 * @see bs_sle_call_callbacks_t
 * @endif
 */
typedef void (*bs_sle_voice_assistant_status_notify_cbk)(sle_obj_t obj, uint8_t state);

/**
 * @if Eng
 * @brief  callback function for server receive the client's request.
 * @par    When registered, this callback notifies the upper layer
 *         when server receive the client's voice assistant operation request.
 * @param [in] obj              device object.
 * @param [in] state            state of voice assistant.
 * @par Dependency:
 * @see bs_sle_call_callbacks_t
 * @else
 * @brief  服务端收到客户端语音助手状态操作请求的回调函数。
 * @par    注册该回调函数之后，收到客户端操作请求后该回调函数会上报上层。
 * @param [in] obj              设备对象。
 * @param [in] state            语音助手状态。
 * @par 依赖:
 * @see bs_sle_call_callbacks_t
 * @endif
 */
typedef void (*bs_sle_voice_assistant_operate_request_cbk)(sle_obj_t obj, uint8_t state);

/**
 * @if Eng
 * @brief  callback function for client receive the server's call list status notify.
 * @par    When registered, this callback notifies the upper layer
 *         when client receive the server's call list status notify.
 * @param [in] obj              device object.
 * @param [in] count            number of call list statuses.
 * @param [in] status           pointer to the array of call list statuses.
 * @par Dependency:
 * @see bs_sle_call_callbacks_t
 * @else
 * @brief  客户端收到服务端通话列表状态通知的回调函数。
 * @par    注册该回调函数之后，收到服务端通话列表状态通知后该回调函数会上报上层。
 * @param [in] obj              设备对象。
 * @param [in] count            通话列表状态数量。
 * @param [in] status           通话列表状态数组指针。
 * @par 依赖:
 * @see bs_sle_call_callbacks_t
 * @endif
 */
typedef void (*bs_sle_call_status_notify_cbk)(sle_obj_t obj, uint8_t count, bs_sle_call_status_t *status);

/**
 * @if Eng
 * @brief Callback invoked when receive vendor at command.
 * @par Description: Callback invoked when receive vendor at command.
 * @param  [in] obj             device object.
 * @param  [in] is_band_in      true in band，false out band.
 * @retval #void  No return value.
 * @par Dependency:
 * @else
 * @brief 来电响铃上报。
 * @par 说明: 来电响铃上报。
 * @param  [in] obj             设备对象。
 * @param  [in] is_band_in      true 带内铃声，false 带外铃声。
 * @retval #void  无返回值。
 * @par 依赖:
 * @endif
 */
typedef void (*bs_sle_call_band_ring_tone_changed_cbk)(sle_obj_t obj, bool is_band_in);

/**
 * @if Eng
 * @brief  callback function for server receive the client's request.
 * @par    When registered, this callback notifies the upper layer
 *         when server receive the client's operation request.
 * @param [in] obj              device object.
 * @param [in] type             operation type. { @ref bs_sle_call_oper_type_t }.
 * @param [in] param            The operation parameters.
 * @param [in] param_len        The length of the operation parameters.
 * @par Dependency:
 * @see bs_sle_call_callbacks_t
 * @else
 * @brief  服务端收到客户端操作请求的回调函数。
 * @par    注册该回调函数之后，收到客户端操作请求后该回调函数会上报上层。
 * @param [in] obj              设备对象。
 * @param [in] type             操作类型。{ @ref bs_sle_call_oper_type_t }。
 * @param [in] param            操作参数。
 * @param [in] param_len        操作参数长度。
 * @par 依赖:
 * @see bs_sle_call_callbacks_t
 * @endif
 */
typedef void (*bs_sle_call_operate_request_cbk)(sle_obj_t obj, bs_sle_call_oper_type_t type,
    uint8_t *param, uint16_t param_len);

/**
 * @if Eng
 * @brief  callback function for client receive the server's call terminate notify.
 * @par    When registered, this callback notifies the upper layer
 *         when client receive the server's call terminate notify.
 * @param [in] obj              device object.
 * @param [in] call_id          the call ID
 * @param [in] reason           reason for terminate the call
 * @par Dependency:
 * @see bs_sle_call_callbacks_t
 * @else
 * @brief  客户端收到服务端通话终止通知的回调函数。
 * @par    注册该回调函数之后，收到服务端通话终止通知后该回调函数会上报上层。
 * @param [in] obj              设备对象。
 * @param [in] call_id          通话标识。
 * @param [in] reason           通话终止原因。
 * @par 依赖:
 * @see bs_sle_call_callbacks_t
 * @endif
 */
typedef void (*bs_sle_call_terminate_notify_cbk)(sle_obj_t obj, uint8_t call_id, uint8_t reason);

/**
 * @if Eng
 * @brief Struct of call manage callback function.
 * @else
 * @brief 通话控制回调函数接口定义。
 * @endif
 */
typedef struct {
    bs_sle_voice_assistant_status_notify_cbk    voice_assistant_status_notify_cbk;
    bs_sle_voice_assistant_operate_request_cbk  voice_assistant_operate_request_cbk;
    bs_sle_call_status_notify_cbk               call_status_notify_cbk;
    bs_sle_call_band_ring_tone_changed_cbk      band_ring_tone_changed_cbk;
    bs_sle_call_operate_request_cbk             call_operate_request_cbk;
    bs_sle_call_terminate_notify_cbk            call_terminate_notify_cbk;
} bs_sle_call_callbacks_t;

/**
 * @if Eng
 * @brief client request to server.
 * @par Description: client initiates an operation request to server.
 * @param [in] obj              device object.
 * @param [in] type             operation type. { @ref bs_sle_call_oper_type_t }.
 * @param [in] param            The operation parameters.
 * @param [in] param_len        The length of the operation parameters.
 * @retval errcode { @ref errcode_t }.
 * @par Depends:
 * @li
 * @else
 * @brief  客户端向服务端发起操作请求。
 * @par Description: 客户端向服务端发起操作请求。
 * @param [in] obj              设备对象。
 * @param [in] type             操作类型。{ @ref bs_sle_call_oper_type_t }。
 * @param [in] param            操作参数。
 * @param [in] param_len        操作参数长度。
 * @retval 执行结果错误码。
 * @par 依赖：
 * @li
 * @endif
 */
errcode_t bs_sle_call_operate_request(sle_obj_t obj, bs_sle_call_oper_type_t type, uint8_t *param, uint16_t param_len);

/**
 * @if Eng
 * @brief server notify client.
 * @par Description: server notify client the operation status.
 * @param [in] obj              device object.
 * @param [in] type             operation type. { @ref bs_sle_call_oper_type_t }.
 * @param [in] param            The operation parameters.
 * @param [in] param_len        The length of the operation parameters.
 * @retval errcode { @ref errcode_t }.
 * @par Depends:
 * @li
 * @else
 * @brief  服务端通知客户端操作状态。
 * @par Description: 服务端通知客户端操作状态。
 * @param [in] obj              设备对象。
 * @param [in] type             操作类型。{ @ref bs_sle_call_oper_type_t }。
 * @param [in] param            操作参数。
 * @param [in] param_len        操作参数长度。
 * @retval 执行结果错误码。
 * @par 依赖：
 * @li
 * @endif
 */
errcode_t bs_sle_call_status_notify(sle_obj_t obj, bs_sle_call_oper_type_t type, uint8_t *param, uint16_t param_len);

/**
 * @if Eng
 * @brief set voice assistant state.
 * @par Description: set voice assistant state.
 * @param  [in] obj             device object.
 * @param  [in] state           state of voice assistant.
 * @retval errcode { @ref errcode_t }.
 * @par Dependency:
 * @else
 * @brief  设置语音助手状态。
 * @par Description: 设置语音助手状态。
 * @param  [in] obj             设备对象。
 * @param  [in] state           语音助手状态。
 * @retval 执行结果错误码。
 * @par 依赖:
 * @endif
 */
errcode_t bs_sle_set_voice_assistant_state(sle_obj_t obj, uint8_t state);

/**
 * @if Eng
 * @brief notify voice assistant state.
 * @par Description: notify voice assistant state.
 * @param  [in] state           state of voice assistant.
 * @retval errcode { @ref errcode_t }.
 * @else
 * @brief  通知语音助手状态。
 * @par Description: 通知语音助手状态。
 * @param  [in] state           语音助手状态。
 * @retval 执行结果错误码。
 * @endif
 */
errcode_t bs_sle_notify_voice_assistant_state(uint8_t state);

/**
 * @if Eng
 * @brief register the callback function for call control.
 * @par Description: register the callback function for call control.
 * @param [in] cb               callback function.
 * @retval errcode { @ref errcode_t }.
 * @par Depends:
 * @li
 * @else
 * @brief  注册通话控制回调函数。
 * @par Description: 注册通话控制回调函数。
 * @param [in] cb               回调函数。
 * @retval 执行结果错误码。
 * @par 依赖：
 * @li
 * @endif
 */
errcode_t bs_sle_call_register_callbacks(uint32_t app_id, bs_sle_call_callbacks_t *cb);

/**
 * @if Eng
 * @brief unregister the callback function for call control.
 * @par Description: unregister the callback function for call control.
 * @param [in] None
 * @retval errcode { @ref errcode_t }.
 * @par Depends:
 * @li
 * @else
 * @brief  注销通话控制回调函数。
 * @par Description: 注销通话控制回调函数。
 * @param [in] 无
 * @retval 执行结果错误码。
 * @par 依赖：
 * @li
 * @endif
 */
errcode_t bs_sle_call_unregister_callbacks(uint32_t app_id);

#ifdef __cplusplus
}
#endif
#endif /* SSF_CALL_H */