
/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 *
 * Description: SLE Service Framework (SSF) Functions about Media API File.
 */

#ifndef SSF_MEDIA_H
#define SSF_MEDIA_H
#include <stdint.h>
#include "errcode.h"
#include "ssf_device.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief  Enum of media play status.
 * @else
 * @brief  媒体播放状态。
 * @endif
 */
typedef enum {
    SLE_MEDIA_PLAY_STATUS_STOPPED = 0x00, /*!< @if Eng Playing status of stop @else 星闪媒体停止状态 @endif */
    SLE_MEDIA_PLAY_STATUS_PLAYING = 0x01, /*!< @if Eng Playing status of play @else 星闪媒体播放状态 @endif */
    SLE_MEDIA_PLAY_STATUS_PAUSED = 0x02,  /*!< @if Eng Playing status of pause @else 星闪媒体暂停状态 @endif */
    SLE_MEDIA_PLAY_STATUS_FWD_SEEK = 0x03, /*!< @if Eng Playing status of fwd_seeking @else 星闪媒体快进状态 @endif */
    SLE_MEDIA_PLAY_STATUS_REV_SEEK = 0x04, /*!< @if Eng Playing status of rev_seeking @else 星闪媒体快退状态 @endif */
} bs_sle_media_play_status_t;

/**
 * @if Eng
 * @brief  Enum of media info.
 * @else
 * @brief  媒体信息类型。
 * @endif
 */
typedef enum {
    SLE_MEDIA_INFO_PLAY_STATUS = 0x01,  /*!< @if Eng media info play status, param is {bs_sle_media_play_status_t},
                                                 param_len is 1.
                                             @else 媒体信息-播放状态, 入参是枚举{bs_sle_media_play_status_t},
                                                   入参长度是1. @endif */
    SLE_MEDIA_INFO_ELEMENT_ATTR = 0x02, /* bs_sle_media_element_attribute_t NOT SUPPORT NOW!! */
} bs_sle_media_info_type_t;

typedef enum {
    SLE_MEDIA_OPT_PLAY = 0x01,          /*!< @if Eng play operation, param is NULL, param_len is 0.
                                             @else play操作,  入参是空, 入参长度是0. @endif */
    SLE_MEDIA_OPT_STOP = 0x02,          /*!< @if Eng stop operation, param is NULL, param_len is 0.
                                             @else stop操作,  入参是空, 入参长度是0.@endif */
    SLE_MEDIA_OPT_PAUSE = 0x03,         /*!< @if Eng pause operation, param is NULL, param_len is 0.
                                             @else pause操作,  入参是空, 入参长度是0. @endif */
    SLE_MEDIA_OPT_FAST_FORWARD = 0x04,  /*!< @if Eng Fast forward, param is NULL, param_len is 0.
                                             @else 设置媒体快进, 入参是空, 入参长度是0. @endif */
    SLE_MEDIA_OPT_FAST_REWIND = 0x05,   /*!< @if Eng Fast forward, param is NULL, param_len is 0.
                                             @else 设置媒体快退, 入参是空, 入参长度是0. @endif */
    SLE_MEDIA_OPT_MOVE_PREVIOUS = 0x06, /*!< @if Eng Switches to the backward one, param is NULL, param_len is 0.
                                             @else 上一首,  入参是空, 入参长度是0. @endif */
    SLE_MEDIA_OPT_MOVE_NEXT = 0x07,     /*!< @if Eng Switches to the forward one, param is NULL, param_len is 0.
                                             @else 下一首,  入参是空, 入参长度是0. @endif */
    SLE_MEDIA_OPT_MAX,
} bs_sle_media_operate_type_t;

/**
 * @if Eng
 * @brief Use this funtion to send media operate request.
 * @par Description: Use this funtion to send media operate request.
 * @param  [in]  obj The id of one connected sle obj.
 * @param  [in]  opt_type media operate type.See @ref bs_sle_media_operate_type_t
 * @param  [in]  param    media operate param pointer.See @ref bs_sle_media_operate_type_t
 * @param  [in]  param_len media operate param length.See @ref bs_sle_media_operate_type_t
 * @retval #ERRCODE_SLE_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_sle_t
 * @par Dependency:
 * @else
 * @brief 星闪媒体操作请求。
 * @par 说明: 星闪媒体操作请求。
 * @param  [in]  obj 对端星闪操作对象编号。
 * @param  [in]  opt_type 媒体操作类型。参考 @ref bs_sle_media_operate_type_t
 * @param  [in]  param    媒体操作参数指针。参考 @ref bs_sle_media_operate_type_t
 * @param  [in]  param_len 媒体操作参数长度。参考 @ref bs_sle_media_operate_type_t
 * @retval #ERRCODE_SLE_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_sle_t
 * @par 依赖:
 * @endif
 */
errcode_t bs_sle_media_operate_request(sle_obj_t obj, uint8_t opt_type, uint8_t *param, uint16_t param_len);

/**
 * @if Eng
 * @brief Use this funtion to update media info.
 * @par Description: Use this funtion to update media info.
 * @param  [in]  obj The id of one sle obj.
 * @param  [in]  type    media info type.See @ref bs_sle_media_info_type_t
 * @param  [in]  param    media info param pointer.See @ref bs_sle_media_info_type_t
 * @param  [in]  param_len media info param length.See @ref bs_sle_media_info_type_t
 * @retval #ERRCODE_SLE_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_sle_t
 * @par Dependency:
 * @else
 * @brief 星闪媒体信息更新。
 * @par 说明: 星闪媒体信息更新。
 * @param  [in]  obj 对端星闪操作对象编号。
 * @param  [in]  type 媒体信息类型。参考 @ref bs_sle_media_info_type_t
 * @param  [in]  param    媒体信息参数指针。参考 @ref bs_sle_media_info_type_t
 * @param  [in]  param_len 媒体信息参数长度。参考 @ref bs_sle_media_info_type_t
 * @retval #ERRCODE_SLE_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_sle_t
 * @par 依赖:
 * @endif
 */
errcode_t bs_sle_media_update_info(sle_obj_t obj, uint8_t type, uint8_t *param, uint16_t param_len);

/**
 * @if Eng
 * @brief  Callback invoked when receive media operate request.
 * @par Description:
 *           Callback invoked when receive media operate request.
 * @param  [in]  obj The id of one sle obj.
 * @param  [in]  opt_type media operate type.See @ref bs_sle_media_operate_type_t
 * @param  [in]  param    media operate param pointer.See @ref bs_sle_media_operate_type_t
 * @param  [in]  param_len media operate param length.See @ref bs_sle_media_operate_type_t
 * @retval #void  no return value.
 * @par Dependency:
 * @li
 * @see  bs_sle_media_callbacks_t
 * @else
 * @brief  媒体操作请求上报。
 * @par 说明:媒体操作请求上报。
 * @param  [in]  obj 对端星闪操作对象编号。
 * @param  [in]  opt_type 媒体操作类型。参考 @ref bs_sle_media_operate_type_t
 * @param  [in]  param    媒体操作参数指针。参考 @ref bs_sle_media_operate_type_t
 * @param  [in]  param_len 媒体操作参数长度。参考 @ref bs_sle_media_operate_type_t
 * @retval #void  无返回值。
 * @par 依赖:
 * @li
 * @see  bs_sle_media_callbacks_t
 * @endif
 */
typedef void (*bs_sle_media_operate_request_cbk)(sle_obj_t obj, uint8_t opt_type, uint8_t *param, uint16_t param_len);

/**
 * @if Eng
 * @brief  Callback invoked When receive media info update.
 * @par Description:Callback invoked When receive media info update.
 * @param  [in]  obj The id of one sle obj.
 * @param  [in]  type    media info type.See @ref bs_sle_media_info_type_t
 * @param  [in]  param    media info param pointer.See @ref bs_sle_media_info_type_t
 * @param  [in]  param_len media info param length.See @ref bs_sle_media_info_type_t
 * @retval #void  no return value.
 * @par Dependency:
 * @li
 * @see  bs_sle_media_callbacks_t
 * @else
 * @brief  媒体信息更新上报。
 * @par 说明:媒体信息更新上报。
 * @param  [in]  obj 对端星闪操作对象编号。
 * @param  [in]  type 媒体信息类型。参考 @ref bs_sle_media_info_type_t
 * @param  [in]  param    媒体信息参数指针。参考 @ref bs_sle_media_play_status_t
 * @param  [in]  param_len 媒体信息参数长度。参考 @ref bs_sle_media_play_status_t
 * @retval #void 无返回值。
 * @par 依赖:
 * @li
 * @see  bs_sle_media_callbacks_t
 * @endif
 */
typedef void (*bs_sle_media_info_update_cbk)(sle_obj_t obj, uint8_t type, uint8_t *param, uint16_t param_len);

/**
 * @if Eng
 * @brief  Struct of sle media callback function.
 * @else
 * @brief  SLE media回调函数接口定义。
 * @endif
 */
typedef struct {
    bs_sle_media_operate_request_cbk media_opt_req_cbk;  // 服务端回调
    bs_sle_media_info_update_cbk media_update_cbk;       // 客户端回调
} bs_sle_media_callbacks_t;

/**
 * @if Eng
 * @brief  Use this funtion to register callback function of framework.
 * @par Description:
 *           Use this funtion to register callback function of framework.
 * @param  [in]  app_id application id , default is 0.
 * @param  [in]  func   A poniter of the callback function. See @ref bs_sle_media_callbacks_t
 * @retval #ERRCODE_SLE_SUCCESS   Success.
 * @retval Other                Failure. For details, see @ref errcode_sle_t
 * @else
 * @brief  注册上层应用的回调。
 * @par 说明:
 *           注册上层应用的回调。
 * @param  [in]  app_id 应用id 默认为0。
 * @param  [in]  func   回调函数指针。参考 @ref bs_sle_media_callbacks_t
 * @retval #ERRCODE_SLE_SUCCESS   成功。
 * @retval Other                失败，参考 @ref errcode_sle_t
 * @endif
 * @par Dependency:
 * @li
 */
errcode_t bs_sle_media_register_callbacks(uint8_t app_id, bs_sle_media_callbacks_t *func);

/**
 * @if Eng
 * @brief  Use this funtion to deregister callback function of framework.
 * @par Description:
 *           Use this funtion to deregister callback function of framework.
 * @param  [in]  app_id application id , default is 0.
 * @retval #ERRCODE_SLE_SUCCESS   Success.
 * @retval Other                Failure. For details, see @ref errcode_sle_t
 * @else
 * @brief  去注册上层应用的回调。
 * @par 说明:
 *           去注册上层应用的回调。
 * @param  [in]  app_id 应用id 默认为0。
 * @retval #ERRCODE_SLE_SUCCESS   成功。
 * @retval Other                失败，参考 @ref errcode_sle_t
 * @endif
 * @par Dependency:
 * @li
 */
errcode_t bs_sle_media_unregister_callbacks(uint8_t app_id);

/**
 * @if Eng
 * @brief Use this funtion to send start/stop request decided by media state.
 * @par Description:
 *          If media state in SLE_MEDIA_STATE_PLAYING or SLE_MEDIA_STATE_PLAY_START, then send stop msg.
 *          If media state in SLE_MEDIA_STATE_STOPPED or SLE_MEDIA_STATE_STOP_START state,
 *          then send start msg.
 * @param  [in]  obj The id of one sle obj.
 * @retval #ERRCODE_SLE_SUCCESS   Success.
 * @retval Other                Failure. For details, see @ref errcode_sle_t
 * @else
 * @brief  根据媒体状态去发送播放/暂停请求
 * @par 说明:
 *         如果目前媒体状态是开始播放或播放中发送暂停请求，如果目前媒体状态是暂停或暂停中则发送播放请求。
 * @param  [in]  obj 对端星闪操作对象编号。
 * @retval #ERRCODE_SLE_SUCCESS   成功。
 * @retval Other                失败，参考 @ref errcode_sle_t
 * @endif
 * @par Dependency:
 * @li
 */
errcode_t bs_sle_music_play_pause_switch_msg_handler(sle_obj_t obj);

#ifdef __cplusplus
}
#endif
#endif /* SSF_MEDIA_H */