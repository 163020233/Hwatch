/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: BR media service, media controller module
 */

#ifndef BS_BR_MEDIA_CONTROL_H
#define BS_BR_MEDIA_CONTROL_H

#include <stdint.h>
#include <stdbool.h>
#include "errcode.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief Enum of br media avrcp service role.
 * @else
 * @brief 蓝牙媒体avrcp服务角色
 * @endif
 */
typedef enum {
    BR_MEDIA_AVRCP_CT = 0x01,
    BR_MEDIA_AVRCP_TG = 0x02,
} bs_br_media_avrcp_role_t;

/**
 * @if Eng
 * @brief  Enum of meida element attribute ID.
 * @else
 * @brief  媒体元素属性ID
 * @endif
 */
typedef enum {
    BR_MEDIA_ELEMENT_ATTR_TITLE = 0x01,       /*!< @if Eng Title, song name or content description
                                                   @else 标题，歌曲名或者内容描述 @endif */
    BR_MEDIA_ELEMENT_ATTR_ARTIST_NAME = 0x02, /*!< @if Eng Artist(s), performer(s) or group
                                                   @else 艺人、表演者或团体 @endif */
    BR_MEDIA_ELEMENT_ATTR_ALBUM_NAME = 0x03,   /*!< @if Eng Album @else 唱片，专辑 @endif */
    BR_MEDIA_ELEMENT_ATTR_TRACK_NUMBER = 0x04, /*!< @if Eng The order number of the audio-file on its original recording
                                                    @else 音频文件的原始编号 @endif */
    BR_MEDIA_ELEMENT_ATTR_TOTAL_NUMBER_OF_TRACKS = 0x05, /*!< @if Eng The total number of tracks/elements on the
                                                        original recording @else tracks/elements的总个数 @endif */
    BR_MEDIA_ELEMENT_ATTR_TRACK_GENRE = 0x06,            /*!< @if Eng Genre @else 风格，流派 @endif */
    BR_MEDIA_ELEMENT_ATTR_TRACK_PLAYING_TIME = 0x07      /*!< @if Eng The length of the audio file in milliseconds
                                                              @else 音频文件的播放时间 @endif */
} bs_br_media_element_attr_id_t;

/**
 * @if Eng
 * @brief  Enum of media operation.
 * @else
 * @brief  媒体操作类型。
 * @endif
 */
typedef enum {
    BR_MEDIA_OPT_PLAY = 0x01,         /*!< @if Eng play operation, param is NULL, param_len is 0.
                                           @else play操作,  入参是空, 入参长度是0. @endif */
    BR_MEDIA_OPT_STOP = 0x02,         /*!< @if Eng stop operation, param is NULL, param_len is 0.
                                           @else stop操作,  入参是空, 入参长度是0.@endif */
    BR_MEDIA_OPT_PAUSE = 0x03,        /*!< @if Eng pause operation, param is NULL, param_len is 0.
                                           @else pause操作,  入参是空, 入参长度是0. @endif */
    BR_MEDIA_OPT_FORWARD = 0x04,      /*!< @if Eng Switches to the forward one, param is NULL, param_len is 0.
                                           @else 下一首,  入参是空, 入参长度是0. @endif */
    BR_MEDIA_OPT_BACKWARD = 0x05,     /*!< @if Eng Switches to the backward one, param is NULL, param_len is 0.
                                           @else 上一首,  入参是空, 入参长度是0. @endif */
    BR_MEDIA_OPT_REWIND = 0x06,       /*!< @if Eng Moves the position toward the beginning of the medium,
                                                   param is NULL, param_len is 0.
                                             @else 将媒体设置为从起始位置开始, param is NULL, param_len is 0. @endif */
    BR_MEDIA_OPT_FAST_FORWARD = 0x07, /*!< @if Eng Fast forward, param is NULL, param_len is 0.
                                             @else 设置媒体快进, 入参是空, 入参长度是0. @endif */
    BR_MEDIA_OPT_REGISTER_ATTRIBUTE_NOTIFICATION = 0x08, /*!< @if Eng Register media attribute notification,
                                                                      param is NULL, param_len is 0,
                                                              @else 注册歌词属性通知,入参是空, 入参长度是0. @endif */
    BR_MEDIA_OPT_UNREGISTER_ATTRIBUTE_NOTIFICATION = 0x09, /*!< @if Eng Unregister media attribute notification,
                                                                      param is NULL, param_len is 0,
                                                              @else 去注册歌词属性通知,入参是空, 入参长度是0. @endif */
    BR_MEDIA_OPT_REGISTER_POSITION_NOTIFICATION = 0x0A, /*!< @if Eng Register playback position notification,
                                                                      param is interval, param_len is 4 byte,
                                                              @else 注册已播放时长变化通知,入参是通知间隔,
                                                                    入参长度是4字节. @endif */
    BR_MEDIA_OPT_UNREGISTER_POSITION_NOTIFICATION = 0x0B, /*!< @if Eng Unregister playback position notification,
                                                                      param is NULL, param_len is 0,
                                                              @else 去注册已播放时长变化通知,入参是空,入参长度是0. @endif */
} bs_br_media_operate_type_t;

/**
 * @if Eng
 * @brief  Struct of attribute value of br media element.
 * @else
 * @brief  TG返回的attribute值,参考Audio/Video Remote Control 1.6.2 Section 6.6.1 Table 6.27。
 * @endif
 */
typedef struct {
    uint32_t attr_id;            /*!< @if Eng Specifies the attribute ID to be written,
                                          See @ref {bs_br_media_element_attr_id_t}
                                      @else 属性ID 参考 {bs_br_media_element_attr_id_t} @endif */
    uint16_t character_set_id; /*!< @if Eng Specifies the character set ID to be displayed on CT
                                    @else 字符集ID @endif */
    uint16_t value_len; /*!< @if Eng Length of the value of the attribute. 0-65535 (0, if no name is provided)
                             @else 属性名称的长度, 缺省默认0值 @endif */
    uint8_t *value;            /*!< @if Eng Name of the value of the attribute
                                    @else 属性名称 @endif */
} bs_br_media_element_attr_value_t;

/**
 * @if Eng
 * @brief  Struct of br media element attribute.
 * @else
 * @brief  TG返回的GetElementAttributes响应参数,在接口 {@link AvrcpCtGetElementAttributeCallback} 中使用。
 * @endif
 */
typedef struct {
    uint8_t attr_num;                       /*!< @if Eng Specifies the attribute number, in range(1, 255)
                                                       @else TG返回的attribute数量，1-255 @endif */
    bs_br_media_element_attr_value_t *attr_value; /*!< @if Eng Specifies the attribute msg
                                                       @else 属性信息，根据attrNum大小动态分配空间 @endif */
} bs_br_media_element_attribute_t;

/**
 * @if Eng
 * @brief  Enum of media play status.
 * @else
 * @brief  媒体播放状态。
 * @endif
 */
typedef enum {
    BR_MEDIA_PLAY_STATUS_STOPPED = 0x00,  /*!< @if Eng Playing status of stop @else AVRCP媒体stopped状态 @endif */
    BR_MEDIA_PLAY_STATUS_PLAYING = 0x01,  /*!< @if Eng Playing status of playing @else AVRCP媒体playing状态 @endif */
    BR_MEDIA_PLAY_STATUS_PAUSED = 0x02,   /*!< @if Eng Playing status of pause @else AVRCP媒体Paused状态 @endif */
    BR_MEDIA_PLAY_STATUS_FWD_SEEK = 0x03, /*!< @if Eng Playing status of FWD_SEEK @else AVRCP媒体快进状态 @endif */
    BR_MEDIA_PLAY_STATUS_REV_SEEK = 0x04, /*!< @if Eng Playing status of REV_SEEK @else AVRCP媒体快退状态 @endif */
} br_media_play_status_t;

/**
 * @if Eng
 * @brief  Enum of media info.
 * @else
 * @brief  媒体信息类型。
 * @endif
 */
typedef enum {
    BR_MEDIA_INFO_PLAY_STATUS = 0x01, /*!< @if Eng media info play status, param is {br_media_play_status_t},
                                               param_len is 1.
                                           @else 媒体信息-播放状态, 入参是枚举{br_media_play_status_t},
                                                 入参长度是1. @endif */
    BR_MEDIA_INFO_ELEMENT_ATTR = 0x02, /*!< @if Eng media info element attribut,
                                                param is the pointer of {bs_br_media_element_attribute_t},
                                                param_len is the length of struct {bs_br_media_element_attribute_t}.
                                            @else 媒体信息-媒体元素属性, 入参是结构体指针{bs_br_media_element_attribute_t},
                                                 入参长度是结构体{bs_br_media_element_attribute_t}的长度. @endif */
    BR_MEDIA_INFO_PLAYBACK_POSITION = 0x03, /*!< @if Eng Current playback position in millisecond,param_len is 4.
                                                 @else 媒体信息-当前播放时长,单位是毫秒,入参长度是4. @endif */
} br_media_info_type_t;

/**
 * @if Eng
 * @brief  Enum of media info.
 * @else
 * @brief  媒体信息类型。
 * @endif
 */
typedef enum {
    BR_MEDIA_VOLUME_OPT_UP = 0x01, /*!< @if Eng Turn up the volume. @else 调高音量 @endif */
    BR_MEDIA_VOLUME_OPT_DOWN = 0x02, /*!< @if Eng Turn down the volume. @else 调低音量 @endif */
    BR_MEDIA_VOLUME_OPT_MUTE = 0x03, /*!< @if Eng Mute the volume. @else 静音 @endif */
    BR_MEDIA_VOLUME_OPT_SET_ABSOLUTE = 0x04, /*!< @if Eng Set the absolute volume,volume value is 0x00-0x7F
                                                  @else 配置绝对音量,音量值范围是0x00-0x7F @endif */
} bs_br_media_volume_opt_type_t;

/**
 * @if Eng
 * @brief Use this funtion to register avrcp service.
 * @par Description: Use this funtion to register avrcp service.
 * @param  [in]  avrcp_role local avrcp profile role。See @ref bs_br_media_avrcp_role_t
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @else
 * @brief 注册蓝牙avrcp服务。
 * @par 说明: 注册蓝牙avrcp服务。
 * @param  [in]  avrcp_role 本地服务角色。参考 @ref bs_br_media_avrcp_role_t
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @endif
 */
errcode_t bs_br_media_avrcp_register(uint8_t avrcp_role);

/**
 * @if Eng
 * @brief Use this funtion to unregister avrcp service.
 * @par Description: Use this funtion to unregister avrcp service.
 * @param  [in]  avrcp_role local avrcp profile role。See @ref bs_br_media_avrcp_role_t
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @else
 * @brief 去注册蓝牙avrcp服务。
 * @par 说明: 去注册蓝牙avrcp服务。
 * @param  [in]  avrcp_role 本地服务角色。参考 @ref bs_br_media_avrcp_role_t
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @endif
 */
errcode_t bs_br_media_avrcp_unregister(uint8_t avrcp_role);

/**
 * @if Eng
 * @brief Use this funtion to create avrcp connection.
 * @par Description: Use this funtion to create avrcp connection.
 * @param  [in]  dev_id The id of one connected device.
 * @param  [in]  avrcp_role local avrcp profile role。See @ref bs_br_media_avrcp_role_t
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @else
 * @brief 建立蓝牙媒体avrcp连接。
 * @par 说明: 建立蓝牙媒体avrcp连接。
 * @param  [in]  dev_id 对端设备编号。
 * @param  [in]  avrcp_role 本端服务角色。参考 @ref bs_br_media_avrcp_role_t
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @endif
 */
errcode_t bs_br_media_avrcp_connect(uint16_t dev_id, uint8_t avrcp_role);

/**
 * @if Eng
 * @brief Use this funtion to release avrcp connection.
 * @par Description: Use this funtion to release avrcp connection.
 * @param  [in]  dev_id The id of one connected device.
 * @param  [in]  avrcp_role local avrcp profile role。See @ref bs_br_media_avrcp_role_t
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @else
 * @brief 断开蓝牙媒体avrcp连接。
 * @par 说明: 断开蓝牙媒体avrcp连接。
 * @param  [in]  dev_id 对端设备编号。
 * @param  [in]  avrcp_role 本端服务角色。参考 @ref bs_br_media_avrcp_role_t
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @endif
 */
errcode_t bs_br_media_avrcp_disconnect(uint16_t dev_id, uint8_t avrcp_role);

/**
 * @if Eng
 * @brief Use this funtion to send media operate request.
 * @par Description: Use this funtion to send media operate request.
 * @param  [in]  dev_id The id of one connected device.
 * @param  [in]  opt_type media operate type.See @ref bs_br_media_operate_type_t
 * @param  [in]  param    media operate param pointer.See @ref bs_br_media_operate_type_t
 * @param  [in]  param_len media operate param length.See @ref bs_br_media_operate_type_t
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @else
 * @brief 蓝牙媒体操作请求。
 * @par 说明: 蓝牙媒体操作请求。
 * @param  [in]  dev_id 对端设备编号。
 * @param  [in]  opt_type 媒体操作类型。参考 @ref bs_br_media_operate_type_t
 * @param  [in]  param    媒体操作参数指针。参考 @ref bs_br_media_operate_type_t
 * @param  [in]  param_len 媒体操作参数长度。参考 @ref bs_br_media_operate_type_t
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @endif
 */
errcode_t bs_br_media_operate_request(uint16_t dev_id, uint8_t opt_type, uint8_t *param, uint16_t param_len);

/**
 * @if Eng
 * @brief Use this funtion to update media info.
 * @par Description: Use this funtion to update media info.
 * @param  [in]  dev_id The id of one connected device.
 * @param  [in]  type    media info type.See @ref br_media_info_type_t
 * @param  [in]  param    media info param pointer.See @ref br_media_info_type_t
 * @param  [in]  param_len media info param length.See @ref br_media_info_type_t
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @else
 * @brief 蓝牙媒体信息更新。
 * @par 说明: 蓝牙媒体信息更新。
 * @param  [in]  dev_id 对端设备编号。
 * @param  [in]  type 媒体信息类型。参考 @ref br_media_info_type_t
 * @param  [in]  param    媒体信息参数指针。参考 @ref br_media_info_type_t
 * @param  [in]  param_len 媒体信息参数长度。参考 @ref br_media_info_type_t
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @endif
 */
errcode_t bs_br_media_update_info(uint16_t dev_id, uint8_t type, uint8_t *param, uint16_t param_len);

/**
 * @if Eng
 * @brief Use this funtion to media volume operate.
 * @par Description: Use this funtion to media volume operate.
 * @param  [in]  dev_id The id of one connected device.
 * @param  [in]  type   operate type。参考 @ref bs_br_media_volume_opt_type_t
 * @param  [in]  volume media absolute volume notify, the region is 0x00-0x7F,
                        when operate type is {BR_MEDIA_VOLUME_OPT_SET_ABSOLUTE}, this must be set.
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @else
 * @brief 蓝牙媒体音量操作。
 * @par 说明: 蓝牙媒体音量操作。
 * @param  [in]  dev_id 对端设备编号。
 * @param  [in]  type   操作类型。参考 @ref bs_br_media_volume_opt_type_t
 * @param  [in]  volume 绝对音量值。值范围0x00-0x7F，操作类型为配置绝对音量时的入参。
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @endif
 */
errcode_t bs_br_media_volume_operate(uint16_t dev_id, uint8_t type, uint8_t volume);

/**
 * @if Eng
 * @brief Use this funtion to notify media absolume volume.
 * @par Description: Use this funtion to notify media absolume volume.
 * @param  [in]  dev_id The id of one connected device.
 * @param  [in]  volume media absolute volume notify, the region is 0x00-0x7F.
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @else
 * @brief 蓝牙媒体音量通知。
 * @par 说明: 蓝牙媒体音量通知。
 * @param  [in]  dev_id 对端设备编号。
 * @param  [in]  volume 音量值。值范围0x00-0x7F
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @endif
 */
errcode_t bs_br_media_volume_notify(uint16_t dev_id, uint8_t volume);

/**
 * @if Eng
 * @brief Callback invoked when avrcp connection status changed.
 * @par Description: Callback invoked when avrcp connection status changed.
 * @param  [in]  dev_id  The id of one connected device.
 * @param  [in]  avrcp_role local avrcp profile role。See @ref bs_br_media_avrcp_role_t
 * @param  [in]  conn_state hfp connection status。See @ref bs_br_profile_connect_state_t
 * @retval #void  No return value.
 * @par Dependency:
 * @else
 * @brief 服务连接状态上报。
 * @par 说明: 服务连接状态上报。
 * @param  [in]  dev_id  对端设备编号
 * @param  [in]  avrcp_role 本端服务角色。参考 @ref bs_br_media_avrcp_role_t
 * @param  [in]  conn_state 服务连接状态。See @ref bs_br_profile_connect_state_t
 * @retval #void  无返回值。
 * @par 依赖:
 * @endif
 */
typedef void (*bs_br_media_avrcp_conn_state_cbk)(uint16_t dev_id, uint8_t avrcp_role, uint8_t conn_state);

/**
 * @if Eng
 * @brief  Callback invoked when receive media operate request.
 * @par Description:
 *           Callback invoked when receive media operate request.
 * @param  [in]  dev_id The id of one connected device.
 * @param  [in]  opt_type media operate type.See @ref bs_br_media_operate_type_t
 * @param  [in]  param    media operate param pointer.See @ref bs_br_media_operate_type_t
 * @param  [in]  param_len media operate param length.See @ref bs_br_media_operate_type_t
 * @retval #void  no return value.
 * @par Dependency:
 * @li
 * @see  bs_br_media_callbacks_t
 * @else
 * @brief  媒体操作请求上报。
 * @par 说明:媒体操作请求上报。
 * @param  [in]  dev_id 对端设备编号。
 * @param  [in]  opt_type 媒体操作类型。参考 @ref bs_br_media_operate_type_t
 * @param  [in]  param    媒体操作参数指针。参考 @ref bs_br_media_operate_type_t
 * @param  [in]  param_len 媒体操作参数长度。参考 @ref bs_br_media_operate_type_t
 * @retval #void  无返回值。
 * @par 依赖:
 * @li
 * @see  bs_br_media_callbacks_t
 * @endif
 */
typedef void (*bs_br_media_operate_request_cbk)(uint16_t dev_id, uint8_t opt_type, uint8_t *param, uint16_t param_len);

/**
 * @if Eng
 * @brief  Callback invoked When receive media info update.
 * @par Description:Callback invoked When receive media info update.
 * @param  [in]  dev_id The id of one connected device.
 * @param  [in]  type    media info type.See @ref br_media_info_type_t
 * @param  [in]  param    media info param pointer.See @ref br_media_info_type_t
 * @param  [in]  param_len media info param length.See @ref br_media_info_type_t
 * @retval #void  no return value.
 * @par Dependency:
 * @li
 * @see  bs_br_media_callbacks_t
 * @else
 * @brief  媒体信息更新上报。
 * @par 说明:媒体信息更新上报。
 * @param  [in]  dev_id 对端设备编号。
 * @param  [in]  type 媒体信息类型。参考 @ref br_media_info_type_t
 * @param  [in]  param    媒体信息参数指针。参考 @ref br_media_info_type_t
 * @param  [in]  param_len 媒体信息参数长度。参考 @ref br_media_info_type_t
 * @retval #void 无返回值。
 * @par 依赖:
 * @li
 * @see  bs_br_media_callbacks_t
 * @endif
 */
typedef void (*bs_br_media_info_update_cbk)(uint16_t dev_id, uint8_t type, uint8_t *param, uint16_t param_len);

/**
 * @if Eng
 * @brief  Callback invoked when receive volume operate request.
 * @par Description:
 *           Callback invoked when receive volume operate request.
 * @param  [in]  dev_id The id of one connected device.
 * @param  [in]  type   operate type。参考 @ref bs_br_media_volume_opt_type_t
 * @param  [in]  volume media absolute volume notify, the region is 0x00-0x7F,
                        when operate type is {BR_MEDIA_VOLUME_OPT_SET_ABSOLUTE}, this must be set.
 * @retval #void  no return value.
 * @par Dependency:
 * @li
 * @see  bs_br_media_callbacks_t
 * @else
 * @brief  音量操作上报。
 * @par 说明:音量操作上报。
 * @param  [in]  dev_id 对端设备编号。
 * @param  [in]  type   操作类型。参考 @ref bs_br_media_volume_opt_type_t
 * @param  [in]  volume 绝对音量值。值范围0x00-0x7F，操作类型为配置绝对音量时的入参。
 * @retval #void  无返回值。
 * @par 依赖:
 * @li
 * @see  bs_br_media_callbacks_t
 * @endif
 */
typedef void (*bs_br_media_volume_operate_cbk)(uint16_t dev_id, uint8_t type, uint8_t volume);

/**
 * @if Eng
 * @brief  Callback invoked when receive media absolume volume notify.
 * @par Description:
 *           Callback invoked when receive media absolume volume notify.
 * @param  [in]  dev_id The id of one connected device.
 * @param  [in]  volume  the volume value, region is 0x0-0x7F.
 * @retval #void  no return value.
 * @par Dependency:
 * @li
 * @see  bs_br_media_callbacks_t
 * @else
 * @brief  音量通知上报。
 * @par 说明:音量通知上报。
 * @param  [in]  dev_id 设备编号
 * @param  [in]  volume  音量值，值范围0x0-0x7F.
 * @retval #void  无返回值。
 * @par 依赖:
 * @li
 * @see  bs_br_media_callbacks_t
 * @endif
 */
typedef void (*bs_br_media_volume_info_report_cbk)(uint16_t dev_id, uint8_t volume);

/**
 * @if Eng
 * @brief  Struct of br media callback function.
 * @else
 * @brief  BR media回调函数接口定义。
 * @endif
 */
typedef struct {
    bs_br_media_avrcp_conn_state_cbk connect_state_cbk;
    bs_br_media_operate_request_cbk media_opt_req_cbk;
    bs_br_media_info_update_cbk media_update_cbk;
    bs_br_media_volume_operate_cbk media_volume_operate_cbk;
    bs_br_media_volume_info_report_cbk media_volume_report_cbk;
} bs_br_media_callbacks_t;

/**
 * @if Eng
 * @brief  Use this funtion to register callback function of framework.
 * @par Description:
 *           Use this funtion to register callback function of framework.
 * @param  [in]  app_id application id , default is 0.
 * @param  [in]  func   A poniter of the callback function. See @ref bs_br_media_callbacks_t
 * @retval #ERRCODE_BT_SUCCESS   Success.
 * @retval Other                Failure. For details, see @ref errcode_bt_t
 * @else
 * @brief  注册上层应用的回调。
 * @par 说明:
 *           注册上层应用的回调。
 * @param  [in]  app_id 应用id 默认为0。
 * @param  [in]  func   回调函数指针。参考 @ref bs_br_media_callbacks_t
 * @retval #ERRCODE_BT_SUCCESS   成功。
 * @retval Other                失败，参考 @ref errcode_bt_t
 * @endif
 * @par Dependency:
 * @li
 */
errcode_t bs_br_media_register_callbacks(uint8_t app_id, bs_br_media_callbacks_t *func);

/**
 * @if Eng
 * @brief  Use this funtion to deregister callback function of framework.
 * @par Description:
 *           Use this funtion to deregister callback function of framework.
 * @param  [in]  app_id application id , default is 0.
 * @retval #ERRCODE_BT_SUCCESS   Success.
 * @retval Other                Failure. For details, see @ref errcode_bt_t
 * @else
 * @brief  去注册上层应用的回调。
 * @par 说明:
 *           去注册上层应用的回调。
 * @param  [in]  app_id 应用id 默认为0。
 * @retval #ERRCODE_BT_SUCCESS   成功。
 * @retval Other                失败，参考 @ref errcode_bt_t
 * @endif
 * @par Dependency:
 * @li bts_a2dp_snk.h
 */
errcode_t bs_br_media_unregister_callbacks(uint8_t app_id);

#ifdef __cplusplus
}
#endif
#endif