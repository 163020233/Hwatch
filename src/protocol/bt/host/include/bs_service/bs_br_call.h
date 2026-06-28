/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: BR call service
 */
#ifndef BS_BR_CALL_H
#define BS_BR_CALL_H

#include <stdint.h>
#include <stdbool.h>
#include "errcode.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief Enum of br call hfp service role.
 * @else
 * @brief 蓝牙通话hfp服务角色
 * @endif
 */
typedef enum {
    BR_CALL_HFP_HF = 0x01,
    BR_CALL_HFP_AG = 0x02,
} bs_br_call_hfp_role_t;

/**
 * @if Eng
 * @brief  Enum of br call volume type.
 * @else
 * @brief  蓝牙通话音量调整类型。
 * @endif
 */
typedef enum {
    BR_CALL_VOLUME_MIC = 0x00, /*!< @if Eng volume type of microphone. @else 调整microphone增益。 @endif */
    BR_CALL_VOLUME_SPEAKER     /*!< @if Eng volume type of speaker. @else 调整speaker增益。 @endif */
} bs_br_call_volume_type_t;

/**
 * @if Eng
 * @brief  Enum of br call operate type.
 * @else
 * @brief  蓝牙通话操作请求类型。
 * @endif
 */
typedef enum {
    BR_CALL_REQUEST_OUT = 0x01,  /*!< @if Eng call out request, param is phone number,
                                               param_len is the length of phone number
                                      @else 呼出请求，入参param为手机号,param_len为手机号长度 @endif */
    BR_CALL_REQUEST_ACCEPT = 0x02,  /*!< @if Eng call accept request, param is NULL, param_len is 0
                                         @else 接听请求，入参param为空，param_len为0 @endif */
    BR_CALL_REQUEST_FINISH = 0x03,  /*!< @if Eng call finish request,param is NULL, param_len is 0.
                                         @else 挂断请求，入参param为空，param_len为0 @endif */
    BR_CALL_REQUEST_REJECT = 0x04,  /*!< @if Eng call reject request, param is NULL, param_len is 0
                                         @else 拒接请求，入参param为空,param_len为0 @endif */
    BR_CALL_REQUEST_HOLD = 0x05,    /*!< @if Eng call hold active call request, param is NULL, param_len is 0
                                         @else 保持当前通话请求，入参param为空，param_len为0 @endif */
    BR_CALL_REQUEST_CALL_INFO = 0x06,  /*!< @if Eng call info request, param is NULL, param_len is 0
                                         @else 通话信息请求，入参param为空,param_len为0 @endif */
} bs_br_call_opt_type_t;

/**
 * @if Eng
 * @brief  Enum of br call operate type.
 * @else
 * @brief  蓝牙通话呼叫状态类型。
 * @endif
 */
typedef enum {
    BR_CALL_STATE_ACTIVE = 0, /*!< @if Eng HFP phone calling state of active @else 正在通话 @endif */
    BR_CALL_STATE_HELD,       /*!< @if Eng HFP phone calling state of hold @else 呼叫保持通话 @endif */
    BR_CALL_STATE_DIALING,    /*!< @if Eng HFP phone calling state of dialing @else 呼出拨号中 @endif */
    BR_CALL_STATE_ALERTING,   /*!< @if Eng HFP phone calling state of alerting @else 呼出响铃中 @endif */
    BR_CALL_STATE_INCOMING,   /*!< @if Eng HFP phone calling state of incoming @else 来电中 @endif */
    BR_CALL_STATE_WAITING,    /*!< @if Eng HFP phone calling state of waiting @else 呼叫等待 @endif */
    BR_CALL_STATE_RESPONSE_HELD, /*!< @if Eng HFP phone calling state of respond hold
                                          @else 呼叫响应保持 @endif */
    BR_CALL_STATE_FINISHED       /*!< @if Eng HFP phone calling state of finshed @else 通话结束 @endif */
} bs_br_call_state_t;

/**
 * @if Eng
 * @brief Max length of phone number.
 * @else
 * @brief 最大手机号码长度。
 * @endif
 */
#define BR_CALL_PHONENUM_MAX_DIGITS 32

/**
 * @if Eng
 * @brief  Struct of br call info.
 * @else
 * @brief  蓝牙通话信息。
 * @endif
 */
typedef struct {
    uint8_t id;                   /*!< @if Eng Call index @else 编号 @endif */
    uint8_t state;            /*!< @if Eng Call state see bs_br_call_state_t
                                   @else  电话呼叫状态 参考 @ref bs_br_call_state_t @endif */
    bool multi_party;         /*!< @if Eng Is multiparty flag @else 是否多方电话 @endif */
    bool outgoing;            /*!< @if Eng Incoming/outing flag @else 呼入/呼出 @endif */
    uint8_t number_len;       /*!< @if Eng Call number length @else  电话号码位数 @endif */
    uint8_t number[BR_CALL_PHONENUM_MAX_DIGITS];             /*!< @if Eng Call number @else 电话号码 @endif */
} bs_br_call_info_t;

/**
 * @if Eng
 * @brief  Enum of br call accept type.
 * @else
 * @brief  蓝牙通话接听类型。
 * @endif
 */
typedef enum {
    BR_CALL_ACCEPT_INCOMING_CALL = 0x00,          /*!< @if Eng accept incoming call
                                                       @else 来电接听 @endif */
    BR_CALL_ACCEPT_OUTGOING_CALL_BY_REMOTE = 0x01 /*!< @if Eng outgoing call is accepted by remote.
                                                       @else 呼出电话被对端接听 @endif */
} bs_br_call_accept_type_t;

/**
 * @if Eng
 * @brief  Enum of br call notify type
 * @else
 * @brief  蓝牙通话通知类型。
 * @endif
 */
typedef enum {
    BR_CALL_NOTIFY_OUT = 0x01,      /*!< @if Eng call out notify, param is NULL, param_len is 0
                                         @else 电话呼出通知，入参param为空,param_len为0 @endif */
    BR_CALL_NOTIFY_ACCEPT = 0x02,   /*!< @if Eng call accept notify,
                                                 param is the enum of accept type @ref bs_br_call_accept_type_t,
                                                 param_len is 1.
                                         @else 接听请求，入参param为接听类型枚举值，参考 @ref bs_br_call_accept_type_t，
                                                param_len为1 @endif */
    BR_CALL_NOTIFY_FINISH = 0x03,   /*!< @if Eng call finish notify, param is NULL, param_len is 0.
                                         @else 挂断通知，入参param为空,param_len为0 @endif */
    BR_CALL_NOTIFY_REJECT = 0x04,      /*!< @if Eng reject call in notify, param is NULL, param_len is 0
                                         @else 来电拒接通知，入参param为空,param_len为0 @endif */
    BR_CALL_NOTIFY_INCOMING = 0x05,  /*!< @if Eng call incoming notify, param is phone number,
                                               param_len is the length of phone number
                                          @else 来电通知，入参param为手机号,param_len为手机号长度 @endif */
    BR_CALL_NOTIFY_CALL_INFO = 0x06,  /*!< @if Eng call info notify,
                                                   param is the point of call info @ref bs_br_call_info_t,
                                                   param_len is the length of the struct of bs_br_call_info_t.
                                           @else 通话信息通知，入参param为通话信息结构体指针，
                                                param_len为通话信息结构体长度 参考 @ref bs_br_call_info_t @endif */
    BR_CALL_NOTIFY_OUT_ALERTING = 0x07,   /*!< @if Eng call out alerting notify, param is NULL, param_len is 0
                                               @else 电话呼出响铃对端通知，入参param为空,param_len为0 @endif */
} bs_br_call_notify_type_t;

/**
 * @if Eng
 * @brief  Enum of br call ring status
 * @else
 * @brief  蓝牙通话来电响铃状态。
 * @endif
 */
typedef enum {
    BR_CALL_INCOMING_RING_GOING = 1, /*!< @if Eng incoming ring going. @else 来电响铃中 @endif */
    BR_CALL_INCOMING_RING_TERMINATE = 2, /*!< @if Eng incoming ring terminate. @else 来电响铃结束 @endif */
} bs_br_call_ring_status_t;

/**
 * @if Eng
 * @brief Use this function to register hfp service.
 * @par Description: Use this function to register hfp service.
 * @param  [in]  hfp_role hfp profile role。See @ref bs_br_call_hfp_role_t
 * @retval #ERRCODE_BT_SUCCESS   Execute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @else
 * @brief 注册蓝牙hfp服务。
 * @par 说明: 注册蓝牙hfp服务。
 * @param  [in]  hfp_role 服务角色。参考 @ref bs_br_call_hfp_role_t
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @endif
 */
errcode_t bs_br_call_hfp_register(uint8_t hfp_role);

/**
 * @if Eng
 * @brief Use this function to unregister hfp service.
 * @par Description: Use this function to unregister hfp service.
 * @param  [in]  hfp_role hfp profile role。See @ref bs_br_call_hfp_role_t
 * @retval #ERRCODE_BT_SUCCESS   Execute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @else
 * @brief 去注册蓝牙hfp服务。
 * @par 说明: 去注册蓝牙hfp服务。
 * @param  [in]  hfp_role 服务角色。参考 @ref bs_br_call_hfp_role_t
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @endif
 */
errcode_t bs_br_call_hfp_unregister(uint8_t hfp_role);

/**
 * @if Eng
 * @brief Use this function to create hfp connection.
 * @par Description: Use this function to create hfp connection.
 * @param  [in]  dev_id The id of one connected device.
 * @param  [in]  hfp_role hfp profile role。See @ref bs_br_call_hfp_role_t
 * @retval #ERRCODE_BT_SUCCESS   Execute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @else
 * @brief 建立蓝牙通话服务连接。
 * @par 说明: 建立蓝牙通话服务连接。
 * @param  [in]  dev_id 对端设备编号。
 * @param  [in]  hfp_role 服务角色。参考 @ref bs_br_call_hfp_role_t
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @endif
 */
errcode_t bs_br_call_hfp_connect(uint16_t dev_id, uint8_t hfp_role);

/**
 * @if Eng
 * @brief Use this function to release hfp connection.
 * @par Description: Use this function to release hfp connection.
 * @param  [in]  dev_id The id of one connected device.
 * @param  [in]  hfp_role hfp profile role。See @ref bs_br_call_hfp_role_t
 * @retval #ERRCODE_BT_SUCCESS   Execute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @else
 * @brief 断开蓝牙通话服务连接。
 * @par 说明: 断开蓝牙通话服务连接。
 * @param  [in]  dev_id 对端设备编号。
 * @param  [in]  hfp_role 服务角色。参考 @ref bs_br_call_hfp_role_t
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @endif
 */
errcode_t bs_br_call_hfp_disconnect(uint16_t dev_id, uint8_t hfp_role);

/**
 * @if Eng
 * @brief Use this function to send call operate request.
 * @par Description: Use this function to send call operate request.
 * @param  [in]  dev_id The id of one connected device.
 * @param  [in]  opt_type call operate type.See @ref bs_br_call_opt_type_t
 * @param  [in]  param    call operate param pointer.See @ref bs_br_call_opt_type_t
 * @param  [in]  param_len call operate param length.See @ref bs_br_call_opt_type_t
 * @retval #ERRCODE_BT_SUCCESS   Execute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @else
 * @brief 通话操作请求。
 * @par 说明: 通话操作请求。
 * @param  [in]  dev_id 对端设备编号。
 * @param  [in]  opt_type 通话操作类型。参考 @ref bs_br_call_opt_type_t
 * @param  [in]  param    通话操作参数指针。参考 @ref bs_br_call_opt_type_t
 * @param  [in]  param_len 通话操作参数长度。参考 @ref bs_br_call_opt_type_t
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @endif
 */
errcode_t bs_br_call_operate_request(uint16_t dev_id, uint8_t opt_type, uint8_t *param, uint16_t param_len);

/**
 * @if Eng
 * @brief Use this function to voice recognition request.
 * @par Description: Use this function to voice recognition request.
 * @param  [in]  dev_id The id of one connected device.
 * @param  [in]  is_open true open,false close.
 * @retval #ERRCODE_BT_SUCCESS   Execute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @else
 * @brief 语音识别请求。
 * @par 说明: 语音识别请求。
 * @param  [in]  dev_id 对端设备编号。
 * @param  [in]  is_open true 打开，false 关闭。
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @endif
 */
errcode_t bs_br_call_voice_recognition_request(uint16_t dev_id, bool is_open);

/**
 * @if Eng
 * @brief Use this function to set volume request.
 * @par Description: Use this function to set volume request.
 * @param  [in]  dev_id The id of one connected device.
 * @param  [in]  volume_type volume type。See @ref bs_br_call_volume_type_t
 * @param  [in]  volume volume value，region is 0-15。
 * @retval #ERRCODE_BT_SUCCESS   Execute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @else
 * @brief 通话音量配置请求。
 * @par 说明: 通话音量配置请求。
 * @param  [in]  dev_id 对端设备编号。
 * @param  [in]  volume_type 音量类型。参考 @ref bs_br_call_volume_type_t
 * @param  [in]  volume 音量值，值范围0-15。
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @endif
 */
errcode_t bs_br_call_set_volume_request(uint16_t dev_id, uint8_t volume_type, uint8_t volume);

/**
 * @if Eng
 * @brief Use this function to send dtmf code request.
 * @par Description: Use this function to send dtmf code.
 * @param  [in]  dev_id The id of one connected device.
 * @param  [in]  code DTMF code, the region is the ascci code of [0-9][A-D][*#].
 * @retval #ERRCODE_BT_SUCCESS   Execute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @else
 * @brief 发送DTMF识别码请求。
 * @par 说明: 发送DTMF识别码请求。
 * @param  [in]  dev_id 对端设备编号。
 * @param  [in]  code DTMF识别码。值范围为[0-9][A-D][*#]的ASCII码
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @endif
 */
errcode_t bs_br_call_send_dtmf_tone_request(uint16_t dev_id, uint8_t code);

/**
 * @if Eng
 * @brief Use this function to send call operate request.
 * @par Description: Use this function to send call operate request.
 * @param  [in]  dev_id The id of one connected device.
 * @param  [in]  notify_type call operate type.See @ref bs_br_call_opt_type_t
 * @param  [in]  param    call operate param pointer.See @ref bs_br_call_opt_type_t
 * @param  [in]  param_len call operate param length.See @ref bs_br_call_opt_type_t
 * @retval #ERRCODE_BT_SUCCESS   Execute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @else
 * @brief 通话操作请求。
 * @par 说明: 通话操作请求。
 * @param  [in]  dev_id 对端设备编号。
 * @param  [in]  notify_type 通知类型。参考 @ref bs_br_call_notify_type_t
 * @param  [in]  param    通知参数指针。参考 @ref bs_br_call_notify_type_t
 * @param  [in]  param_len 通知参数长度。参考 @ref bs_br_call_notify_type_t
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @endif
 */
errcode_t bs_br_call_status_notify(uint16_t dev_id, uint8_t notify_type, uint8_t *param, uint16_t param_len);

/**
 * @if Eng
 * @brief Use this function to send vendor at request.
 * @par Description: Use this function to send vendor at request.
 * @param  [in]  dev_id The id of one connected device.
 * @param  [in]  at_cmd AT command param.
 * @param  [in]  at_len AT command param length.
 * @retval #ERRCODE_BT_SUCCESS   Execute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @else
 * @brief 发送私有命令请求。
 * @par 说明: 发送私有命令请求。
 * @param  [in]  dev_id 对端设备编号。
 * @param  [in]  at_cmd AT命令参数。
 * @param  [in]  at_len AT命令参数长度。
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @endif
 */
errcode_t bs_br_call_send_vendor_at_request(uint16_t dev_id, uint8_t *at_cmd, uint16_t at_len);

/**
 * @if Eng
 * @brief Callback invoked when hfp connection status changed.
 * @par Description: Callback invoked when hfp connection status changed.
 * @param  [in]  dev_id  The id of one connected device.
 * @param  [in]  hfp_role local hfp profile role。See @ref bs_br_call_hfp_role_t
 * @param  [in]  conn_state hfp connection status。See @ref bs_br_profile_connect_state_t
 * @retval #void  No return value.
 * @par Dependency:
 * @else
 * @brief 服务连接状态上报。
 * @par 说明: 服务连接状态上报。
 * @param  [in]  dev_id  对端设备编号
 * @param  [in]  hfp_role 本端服务角色。参考 @ref bs_br_call_hfp_role_t
 * @param  [in]  conn_state 服务连接状态。See @ref bs_br_profile_connect_state_t
 * @retval #void  无返回值。
 * @par 依赖:
 * @endif
 */
typedef void (*bs_br_call_conn_state_cbk)(uint16_t dev_id, uint8_t hfp_role, uint8_t conn_state);

/**
 * @if Eng
 * @brief Callback invoked when voice recognition status changed.
 * @par Description: Callback invoked when voice recognition status changed.
 * @param  [in]  dev_id  The id of one connected device.
 * @param  [in]  is_open true open,false close.
 * @retval #void  No return value.
 * @par Dependency:
 * @else
 * @brief 语音识别状态上报。
 * @par 说明: 语音识别状态上报。
 * @param  [in]  dev_id  对端设备编号
 * @param  [in]  is_open true 打开，false 关闭。
 * @retval #void  无返回值。
 * @par 依赖:
 * @endif
 */
typedef void (*bs_br_call_voice_recognition_status_cbk)(uint16_t dev_id, bool is_open);

/**
 * @if Eng
 * @brief Callback invoked when receive set volume request.
 * @par Description: Callback invoked when receive set volume request.
 * @param  [in]  dev_id  The id of one connected device.
 * @param  [in]  volume_type volume type。See @ref bs_br_call_volume_type_t
 * @param  [in]  volume volume value，region is 0-15。
 * @retval #void  No return value.
 * @par Dependency:
 * @else
 * @brief 通话音量配置上报。
 * @par 说明: 通话音量配置上报。
 * @param  [in]  dev_id  对端设备编号
 * @param  [in]  volume_type 音量类型。参考 @ref bs_br_call_volume_type_t
 * @param  [in]  volume 音量值，值范围0-15。
 * @retval #void  无返回值。
 * @par 依赖:
 * @endif
 */
typedef void (*bs_br_call_set_volume_request_cbk)(uint16_t dev_id, uint8_t volume_type, uint8_t volume);

/**
 * @if Eng
 * @brief Callback invoked when receive call status info.
 * @par Description: Callback invoked when receive call info.
 * @param  [in]  dev_id  The id of one connected device.
 * @param  [in]  call_info the pointer of call status info。See @ref bs_br_call_info_t
 * @retval #void  No return value.
 * @par Dependency:
 * @else
 * @brief 通话状态信息上报。
 * @par 说明: 通话状态信息上报。
 * @param  [in]  dev_id  对端设备编号
 * @param  [in]  call_info 通话状态信息。参考 @ref bs_br_call_info_t
 * @retval #void  无返回值。
 * @par 依赖:
 * @endif
 */
typedef void (*bs_br_call_status_notify_cbk)(uint16_t dev_id, bs_br_call_info_t *call_info);

/**
 * @if Eng
 * @brief Callback invoked when receive vendor at command.
 * @par Description: Callback invoked when receive vendor at command.
 * @param  [in]  dev_id  The id of one connected device.
 * @param  [in]  is_band_in true in band，false out band.
 * @param  [in]  ring_status 来电响铃状态.See @ref bs_br_call_ring_status_t
 * @retval #void  No return value.
 * @par Dependency:
 * @else
 * @brief 来电响铃上报。
 * @par 说明: 来电响铃上报。
 * @param  [in]  dev_id  对端设备编号
 * @param  [in]  is_band_in true 带内铃声，false 带外铃声
 * @param  [in]  ring_status 来电响铃状态。参考 @ref bs_br_call_ring_status_t
 * @retval #void  无返回值。
 * @par 依赖:
 * @endif
 */
typedef void (*bs_br_call_band_ring_tone_changed_cbk)(uint16_t dev_id, bool is_band_in, uint8_t ring_status);

/**
 * @if Eng
 * @brief Callback invoked when receive call operate request.
 * @par Description: Callback invoked when  receive call operate request.
 * @param  [in]  dev_id  The id of one connected device.
 * @param  [in]  opt_type call operate type.See @ref bs_br_call_opt_type_t
 * @param  [in]  param    call operate param pointer.See @ref bs_br_call_opt_type_t
 * @param  [in]  param_len call operate param length.See @ref bs_br_call_opt_type_t
 * @retval #void  No return value.
 * @par Dependency:
 * @else
 * @brief 通话操作请求上报。
 * @par 说明: 通话操作请求上报。
 * @param  [in]  dev_id  对端设备编号
 * @param  [in]  opt_type 通话操作类型。参考 @ref bs_br_call_opt_type_t
 * @param  [in]  param    通话操作参数指针。参考 @ref bs_br_call_opt_type_t
 * @param  [in]  param_len 通话操作参数长度。参考 @ref bs_br_call_opt_type_t
 * @retval #void  无返回值。
 * @par 依赖:
 * @endif
 */
typedef void (*bs_br_call_operate_request_cbk)(uint16_t dev_id, uint8_t opt_type, uint8_t *param, uint16_t param_len);

/**
 * @if Eng
 * @brief Callback invoked when receive vendor at command.
 * @par Description: Callback invoked when receive vendor at command.
 * @param  [in]  dev_id  The id of one connected device.
 * @param  [in]  at_cmd AT command param.
 * @param  [in]  at_len AT command param length.
 * @retval #void  No return value.
 * @par Dependency:
 * @else
 * @brief 私有命令上报。
 * @par 说明: 私有命令上报。
 * @param  [in]  dev_id  对端设备编号
 * @param  [in]  at_cmd AT命令参数。
 * @param  [in]  at_len AT命令参数长度。
 * @retval #void  无返回值。
 * @par 依赖:
 * @endif
 */
typedef void (*bs_br_call_vendor_at_cbk)(uint16_t dev_id, uint8_t *at_cmd, uint16_t at_len);

/**
 * @if Eng
 * @brief br call callbacks
 * @else
 * @brief 通话回调函数
 * @endif
 */
typedef struct {
    bs_br_call_conn_state_cbk                connect_state_cbk;
    bs_br_call_voice_recognition_status_cbk  voice_reco_status_cbk;
    bs_br_call_set_volume_request_cbk        set_volume_request_cbk;
    bs_br_call_status_notify_cbk             call_status_notify_cbk;
    bs_br_call_band_ring_tone_changed_cbk    band_ring_tone_changed_cbk;
    bs_br_call_operate_request_cbk           call_opt_req_cbk;
    bs_br_call_vendor_at_cbk                 vendor_at_cbk;
} bs_br_call_callbacks_t;

/**
 * @if Eng
 * @brief Use this function to register hfp callbacks.
 * @par Description: Use this function to register hfp callbacks.
 * @param  [in]  app_id application id , default is 0.
 * @param  [in]  func the pointer of callbacks struct.
 * @retval #ERRCODE_BT_SUCCESS   Execute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @else
 * @brief 注册蓝牙通话回调函数。
 * @par 说明: 注册蓝牙通话回调函数。
 * @param  [in]  app_id 应用id 默认为0。
 * @param  [in]  func 回调函数。
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @endif
 */
errcode_t bs_br_call_register_callbacks(uint8_t app_id, bs_br_call_callbacks_t *func);

/**
 * @if Eng
 * @brief Use this function to unregister hfp callbacks.
 * @par Description: Use this function to unregister hfp callbacks.
 * @param  [in]  app_id application id , default is 0.
 * @retval #ERRCODE_BT_SUCCESS   Execute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @else
 * @brief 去注册蓝牙通话回调函数。
 * @par 说明: 去注册蓝牙通话回调函数。
 * @param  [in]  app_id 应用id 默认为0。
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @endif
 */
errcode_t bs_br_call_unregister_callbacks(uint8_t app_id);

#ifdef __cplusplus
}
#endif
#endif