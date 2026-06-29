/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024. All rights reserved.
 *
 * Description: SLE Trans Manager module.
 */

/**
 * @defgroup sle_trans_manager trans manager API
 * @ingroup  SLE
 * @{
 */

#ifndef SLE_TM_SIGNAL_H
#define SLE_TM_SIGNAL_H

#include <stdbool.h>
#include <stdint.h>
#include "errcode.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief Enum of sle link qos state.
 * @else
 * @brief 星闪链路忙闲状态。
 * @endif
 */
typedef enum {
    SLE_QOS_IDLE       = 0x00,    /*!< @if Eng Link state of idle
                                       @else   空闲状态 @endif */
    SLE_QOS_FLOWCTRL   = 0x01,    /*!< @if Eng Link state of flowctrl
                                       @else   流控状态 @endif */
    SLE_QOS_BUSY       = 0x02,    /*!< @if Eng Link state of busy
                                       @else   繁忙状态 @endif */
} sle_link_qos_state_t;

/**
 * @if Eng
 * @brief Enum of sle link transmission channel transmission mode.
 * @else
 * @brief 星闪传输通道传输模式。
 * @endif
 */
typedef enum {
    SLE_CHANNEL_BASIC_MODE = 0,     /*!< @if Eng Basic mode.
                                        @else   基础模式。 @endif */
    SLE_CHANNEL_STREAM_MODE,        /*!< @if Eng Streaming mode.
                                        @else   流模式。 @endif */
    SLE_CHANNEL_TRANSPARENT_MODE,   /*!< @if Eng Transparent transmission mode.
                                        @else   透传模式。 @endif */
    SLE_CHANNEL_RELIABLE_MODE,      /*!< @if Eng Reliable mode.
                                        @else   可靠模式。 @endif */
} sle_channel_trans_mode_t;

/**
 * @if Eng
 * @brief Enum of dynamic channel data transmission status.
 * @else
 * @brief 星闪动态通道数据传输状态。
 * @endif
 */
typedef enum {
    SLE_DATA_SEND_SUCC = 0,     /*!< @if Eng Data queue is full
                                    @else   数据发送成功 @endif */
    SLE_DATA_QUEUE_FULL,        /*!< @if Eng Data queue is full
                                    @else   数据队列已满 @endif */
    SLE_DATA_OVER_MTU,          /*!< @if Eng Data exceeds MTU
                                    @else   数据超过MTU @endif */
    SLE_DATA_RETRANS_TIMEOUT,   /*!< @if Eng Data retransmission timeout
                                    @else   数据重传超时 @endif */
    SLE_DATA_REORDER_TIMEOUT,   /*!< @if Eng Data reordering timeout
                                    @else   数据重排序超时 @endif */
} sle_tm_trans_data_status_t;

/**
 * @brief  GLE传输模式
 */
typedef enum {
    SLE_DEFAULT_UNICAST_DATA_TRANSMISSION_CHANNEL = 0,     /*!< @if Eng SLE Default Unicast Data Transmission Channel.
                                                                @else   SLE默认单播数据传输通道。 @endif */
} sle_transmission_channel_type;

/**
 * @if Eng
 * @brief Sle transmission channel callback result.
 * @else
 * @brief 星闪传输通道回调结果。
 * @endif
 */
typedef struct {
    uint8_t tc_handle;             /*!< @if Eng channel ID.
                                        @else   通道标识。 @endif */
    uint8_t is_exclusive;          /*!< @if Eng Mark whether the channel is exclusive, 0: shared, 1: exclusive
                                        @else   独占/共享标记, 1：表示独占， 0：表示共享。 @endif */
    uint8_t status;                /*!< @if Eng channel status.
                                        @else   通道状态。 @endif */
    uint8_t tc_mode;               /*!< @if Eng Channel transmission mode.
                                        @else   通道传输模式。 @endif */
    uint8_t tx_window;             /*!< @if Eng Send queue length.
                                        @else   发送队列的长度, 可靠模式有效，非可靠模式缺省为0。 @endif */
    uint16_t src_port;             /*!< @if Eng Source port.
                                        @else   源端口。 @endif */
    uint16_t dst_port;             /*!< @if Eng Destination port.
                                        @else   目的端口。 @endif */
    uint16_t mtu;                  /*!< @if Eng maximum transmission unit.
                                        @else   最大传输单元。 @endif */
} sle_tm_channel_result_t;

/**
 * @brief  连接管理查询能力比特位参数
 */
typedef struct {
    uint32_t relay_capability : 1;         /*!< @if Eng relay capability
                                                @else   中继能力 @endif */
    uint32_t trans_mode : 1;               /*!< @if Eng trans mode
                                                @else   传输模式 @endif */
    uint32_t measurement_capability : 1;   /*!< @if Eng measurement capability
                                                @else   测量能力 @endif */
    uint32_t access_sle : 1;               /*!< @if Eng access sle
                                                @else   sle接入 @endif */
    uint32_t access_slb : 1;               /*!< @if Eng access slb
                                                @else   slb接入 @endif */
    uint32_t mtu : 1;                      /*!< @if Eng max mtu
                                                @else   最大支持mtu @endif */
    uint32_t mps : 1;                      /*!< @if Eng max mps
                                                @else   最大支持mps @endif */
    uint32_t version : 1;                  /*!< @if Eng device version number
                                                @else   支持设备版本号 @endif */
    uint32_t tx_window : 1;                /*!< @if Eng data transmission queue length
                                                @else   数据发送队列长度 @endif */
    uint32_t reverse : 23;                 /*!< @if Eng reverse
                                                @else   保留比特位 @endif */
} sle_transmission_signal_capability_bit_t;

/**
 * @if Eng
 * @brief Sle capability query callback result.
 * @else
 * @brief 星闪能力查询回调结果。
 * @endif
 */
typedef struct {
    bool support_relay;
    bool support_measure;
    bool support_reliable_mode;
    bool support_stream_mode;
    bool support_transparent_mode;
    uint8_t tx_window;
    uint8_t sle_addr[SLE_ADDR_LEN];
    uint8_t slb_addr[SLE_ADDR_LEN];
    uint16_t mtu;
    uint16_t mps;
    uint16_t version;
} sle_tm_capabiliaty_result_t;

/**
 * @if Eng
 * @brief Sle data dynamic transmission status return result.
 * @else
 * @brief 星闪数据动态传输状态返回结果。
 * @endif
 */
typedef struct {
    uint16_t src_port;          /*!< @if Eng Source port.
                                    @else   源端口。 @endif */
    uint16_t dst_port;          /*!< @if Eng Destination port.
                                    @else   目的端口。 @endif */
    uint8_t status;             /*!< @if Eng Data transmission status {@ref sle_tm_trans_data_status_t}.
                                    @else   数据传输状态。 参考枚举 sle_tm_trans_data_status_t @endif */
    uint8_t tc_handle;          /*!< @if Eng Transmission channel identification.
                                    @else   传输通道标识。 @endif */
    uint8_t send_succ_num;      /*!< @if Eng Number of packets sent successfully.
                                    @else   发送成功包数量。 @endif */
} sle_tm_dynamic_data_trans_result_t;

/**
 * @if Eng
 * @brief  Callback invoked when trans data busy.
 * @par Callback invoked when trans data busy.
 * @param [in] conn_id       connection ID.
 * @param [in] link_state    link state.
 * @par Depends:
 * @see  sle_transmission_callbacks_t
 * @else
 * @brief  发送数据繁忙回调函数。
 * @par Description: 发送数据繁忙回调函数。
 * @param [in] conn_id       发送数据繁忙回调信息.
 * @param [in] link_state    链路状态。
 * @par 依赖：
 * @see  sle_transmission_callbacks_t
 * @endif
 */
typedef void (*sle_trans_data_busy_callback)(uint16_t conn_id, sle_link_qos_state_t link_state);

/**
 * @if Eng
 * @brief  Callback transmission channel creation/release result.
 * @par Callback transmission channel creation/release result.
 * @param [in] conn_id       connection ID.
 * @param [in] channel_result    transmission channel callback return information.
 * @par Depends:
 * @see  sle_transmission_callbacks_t
 * @else
 * @brief  传输通道 创建/释放 结果回调函数。
 * @par Description: 传输通道 创建/释放 结果回调函数。
 * @param [in] conn_id       链路标识。
 * @param [in] channel_result   传输通道回调返回信息。
 * @par 依赖：
 * @see  sle_transmission_callbacks_t
 * @endif
 */
typedef void (*sle_dynamic_channel_result_callback)(uint16_t conn_id, sle_tm_channel_result_t *channel_result);

/**
 * @if Eng
 * @brief  Callback Transmission channel data transmission status callback function.
 * @par Callback Transmission channel data transmission status callback function.
 * @param [in] conn_id       connection ID.
 * @param [in] trans_result Data transfer status return information.
 * @par Depends:
 * @see  sle_transmission_callbacks_t
 * @else
 * @brief  传输通道数据传输状态回调函数。
 * @par Description: 传输通道数据传输状态回调函数。
 * @param [in] conn_id       链路标识。
 * @param [in] trans_result  数据传输状态返回信息。
 * @par 依赖：
 * @see  sle_transmission_callbacks_t
 * @endif
 */
typedef void (*sle_dynamic_trans_status_callback)(uint16_t conn_id, sle_tm_dynamic_data_trans_result_t *trans_result);

/**
 * @if Eng
 * @brief  Capability query result callback function.
 * @par Capability query result callback function.
 * @param [in] conn_id       connection ID.
 * @param [in] capability_result    capability query callback return information.
 * @par Depends:
 * @see  sle_transmission_callbacks_t
 * @else
 * @brief  能力查询结果回调函数。
 * @par Description: 能力查询结果回调函数。
 * @param [in] conn_id       链路标识。
 * @param [in] capability_result   能力查询回调返回信息。
 * @par 依赖：
 * @see  sle_transmission_callbacks_t
 * @endif
 */
typedef void (*sle_tm_capability_query_callback)(uint16_t conn_id, sle_tm_capabiliaty_result_t *capability_result);

/**
 * @if Eng
 * @brief Struct of SLE transmission manager callback function.
 * @else
 * @brief SLE传输管理回调函数接口定义。
 * @endif
 */
typedef struct {
    sle_trans_data_busy_callback send_data_cb;               /*!< @if Eng Trans data busy callback.
                                                                @else   传输数据繁忙回调函数。 @endif */
    sle_dynamic_channel_result_callback channel_result_cbk;  /*!< @if Eng Transmission channel result callback function.
                                                                @else   传输通道结果回调函数。 @endif */
    sle_dynamic_trans_status_callback trans_status_cbk;      /*!< @if Eng Data transfer status callback function.
                                                                @else   传输状态回调函数。 @endif */
    sle_tm_capability_query_callback capability_result_cbk;  /*!< @if Eng Capability query callback function.
                                                                @else   能力查询回调函数。 @endif */
} sle_transmission_callbacks_t;

/**
 * @if Eng
 * @brief Sle transmission channel reliable mode parameter configuration.
 * @else
 * @brief 星闪传输通道可靠模式参数配置。
 * @endif
 */
typedef struct {
    uint16_t reorder_timeout;           /*!< @if Eng Reordering timer timeout.
                                            @else   重排序定时器超时时间。 @endif */
    uint16_t retransmission_timeout;    /*!< @if Eng Retransmission timer timeout.
                                            @else   重传定时器超时时间。 @endif */
    uint16_t response_timeout;          /*!< @if Eng Answer timer timeout.
                                            @else   应答定时器超时时间。 @endif */
    uint8_t tx_window;                  /*!< @if Eng Send queue length.
                                            @else   发送队列的长度。 @endif */
    uint8_t max_tx_threshold;           /*!< @if Eng Maximum number of retransmissions.
                                            @else   最大重传次数。 @endif */
} sle_transmission_reliable_mode_param_t;

/**
 * @if Eng
 * @brief Sle transmission channel transmission mode parameter configuration.
 * @else
 * @brief 星闪传输通道传输模式参数配置。
 * @endif
 */
typedef struct {
    uint8_t tc_mode;                                /*!< @if Eng Transmission mode {@ref sle_channel_trans_mode_t}.
                                                        @else   传输模式，参考枚举sle_channel_trans_mode_t。 @endif */
    uint16_t mtu;                                   /*!< @if Eng maximum transmission unit.
                                                        @else   最大传输单元。 @endif */
    uint16_t mps;                                   /*!< @if Eng Maximum payload size.
                                                        @else   最大载荷大小。 @endif */
    union {
        sle_transmission_reliable_mode_param_t *reliable_mode;  /*!< @if Eng Reliable mode parameter configuration.
                                                                    @else   可靠模式参数配置。 @endif */
    } ext_params;
} sle_transmission_trans_mode_t;

/**
 * @if Eng
 * @brief Sle transmission channel creation extended field port information.
 * @else
 * @brief 星闪传输通道创建扩展字段端口信息。
 * @endif
 */
typedef struct {
    uint16_t src_port;                  /*!< @if The local port negotiated by the basic application layer.
                                            @else   基础应用层协商的本端端口。 @endif */
    uint16_t dst_port;                  /*!< @if The remote port negotiated by the basic application layer.
                                            @else   基础应用层协商的对端端口。 @endif */
} sle_tm_port_config_param_t;

/**
 * @if Eng
 * @brief Sle create a transport channel extension field bitmap.
 * @else
 * @brief 星闪创建传输通道扩展字段位图。
 * @endif
 */
typedef struct {
    uint8_t pc_flag;                /*!< @if Extended field portConfig identifier, {0: no such field, 1: such field}.
                                        @else  扩展字段portConfig标识，{0：无此字段，1：有此字段}。 @endif */
    uint8_t mc_flag;                /*!< @if Extended field multiLcConfig identifier, {0: no such field, 1: such field}.
                                        @else  扩展字段multiLcConfig标识，{0：无此字段，1：有此字段}。 @endif */
    uint8_t lc_flag;                /*!< @if Extended field lcConfig identifier, {0: no such field, 1: such field}.
                                        @else  扩展字段lcConfig标识，{0：无此字段，1：有此字段}。 @endif */
} sle_tm_create_channel_expand_map_t;

/**
 * @if Eng
 * @brief Sle transport channel connection establishment request parameters.
 * @else
 * @brief 星闪传输通道连接建立请求参数。
 * @endif
 */
typedef struct {
    uint8_t is_exclusive;                       /*!< @if Eng whether the channel is exclusive, 0: shared, 1: exclusive.
                                                    @else  传输通道独占/共享标记位，1：独占，0：共享。 @endif */
    sle_tm_port_config_param_t port_info;               /*!< @if Eng Local/Peer Port.
                                                            @else  本地 / 对端port。 @endif */
    sle_tm_create_channel_expand_map_t extend_map;      /*!< @if Eng Create channel signaling extension field map.
                                                            @else  创建通道信令扩展字段位图。 @endif */
    sle_transmission_trans_mode_t channel_trans_param;  /*!< @if Eng Create a channel transfer mode configuration.
                                                            @else  创建通道传输模式配置。 @endif */
} sle_tm_create_channel_param_t;

/**
 * @if Eng
 * @brief  Create transmission channel request.
 * @par Description: Create transmission channel request.
 * @param [in] conn_id   Connection id.
 * @param [in] param     Transmission channel creation parameter information.
 * @retval error code.
 * @par Depends:
 * @li  sle_common.h
 * @else
 * @brief  创建传输通道。
 * @par Description: 创建传输通道。
 * @param [in] conn_id   链路 ID。
 * @param [in] param     传输通道创建参数信息。
 * @retval 执行结果错误码。
 * @par 依赖：
 * @li  sle_common.h
 * @endif
 */
errcode_t sle_transmission_create_dynamic_channel(uint16_t conn_id, sle_tm_create_channel_param_t *param);

/**
 * @if Eng
 * @brief  Release transmission channel request.
 * @par Description: Release transmission channel request.
 * @param [in] conn_id   Connection id.
 * @param [in] tc_handle   The ID of the released transmission channel.
 * @retval error code.
 * @par Depends:
 * @li  sle_common.h
 * @else
 * @brief  释放传输通道。
 * @par Description: 释放传输通道。
 * @param [in] conn_id   链路 ID。
 * @param [in] tc_handle     释放的传输通道标识。
 * @retval 执行结果错误码。
 * @par 依赖：
 * @li  sle_common.h
 * @endif
 */
errcode_t sle_transmission_release_dynamic_channel(uint16_t conn_id, uint8_t tc_handle);

/**
 * @if Eng
 * @brief  Send signal capability request.
 * @par Description: Send signal capability request.
 * @param [in] conn_id   Connection id.
 * @param [in] param     Capability info.
 * @retval error code
 * @par Depends:
 * @li  sle_common.h
 * @else
 * @brief  发送连接管理能力查询请求。
 * @par Description: 发送连接管理能力查询请求。
 * @param [in] conn_id   连接 ID。
 * @param [in] param     查询的能力信息。
 * @retval 执行结果错误码。
 * @par 依赖：
 * @li  sle_common.h
 * @endif
 */
errcode_t sle_transmission_signal_capability_req(uint16_t conn_id, sle_transmission_signal_capability_bit_t *param);

/**
 * @if Eng
 * @brief  Register SLE transmission manager callbacks.
 * @par Description: Register SLE transmission manager callbacks.
 * @param  [in]  func Callback function.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  注册SLE传输管理回调函数。
 * @par Description: 注册SLE传输管理回调函数。
 * @param  [in]  func 回调函数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @endif
 */
errcode_t sle_transmission_register_callbacks(sle_transmission_callbacks_t *func);

/**
 * @if Eng
 * @brief  Create SLE default transmission channel.
 * @par Description: Create SLE default transmission channel.
 * @param  [in]  channel_type   channel type. see @ref sle_transmission_channel_type
 * @param  [out] tc_handle  transmission channel handle.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  创建SLE缺省传输通道。
 * @par Description: 创建SLE缺省传输通道。
 * @param  [in]  channel_type   传输通道类型。参考 @ref sle_transmission_channel_type
 * @param  [out] tc_handle  传输通道句柄。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @endif
 */
errcode_t sle_transmission_create_default_trans_channel(uint8_t channel_type, uint8_t *tc_handle);
 
/**
 * @if Eng
 * @brief Use this function to set ssap flow control wait time.
 * @par Description: Use this function to set ssap flow control wait time，no wait if set 0xFFFF.
 * @param [in] milliseconds time.
 * @retval void.
 * @else
 * @brief  设置ssap流控等待时间。
 * @par Description: 设置ssap流控等待时间,设置0xFFFF不等待。
 * @param [in] milliseconds 时间毫秒
 * @retval 无返回值。
 * @endif
 */
void sle_set_ssap_wait_time(uint16_t milliseconds);

/**
 * @if Eng
 * @brief Use this function to set Qos max buffer size.
 * @par Description: Use this function to set Qos max buffer size，the value is 0~100.
 * @param [in] buff_size max buffer size.
 * @retval void.
 * @else
 * @brief  设置ssap Qos缓冲区最大数量。
 * @par Description: 设置ssap Qos缓冲区最大数量，取值0~100.
 * @param [in] buff_size 最大缓冲区大小
 * @retval 无返回值。
 * @endif
 */
void sle_set_ssap_max_buff_size(uint8_t buff_size);
#ifdef __cplusplus
}
#endif
#endif /* SLE_TM_SIGNAL */
/**
 * @}
 */
